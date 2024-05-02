#ifndef FORCE_DIRECTED_DRAWING_H_
#define FORCE_DIRECTED_DRAWING_H_

#include <cmath>

#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/random.h"

using real = float;
using edge = std::pair<int, int>;
using coord = std::pair<real, real>;

auto norm(real x, real y)
{
    return std::sqrt(x * x + y * y);
}

// Node of a Barnes-Hut tree. Square bounding box can be computed traversing down tree
struct bh_tree
{
    bh_tree *ne, *nw, *se, *sw;
    uint count; // Always >= 1
    coord center_of_mass;
    real width;
};
/* Possible improvement: use separate structs for internal and
 * external nodes for memory saving. count >= 1 can be a tag
 */

parlay::type_allocator<bh_tree> bh_tree_allocator;

// Recursively create quadtree for Barnes-Hut
bh_tree *make_bh_tree(parlay::slice<coord *, coord *> nodes, real lo_x = 0.0, real hi_x = 1.0, real lo_y = 0.0, real hi_y = 1.0)
{
    if (nodes.size() == 0)
    {
        return nullptr;
    }

    bh_tree *node = bh_tree_allocator.alloc();
    node->count = nodes.size();

    // TODO do in recursive step?
    node->center_of_mass.first = parlay::reduce(parlay::delayed_map(nodes, [](coord x) -> real
                                                                    { return x.first; })) /
                                 nodes.size();
    node->center_of_mass.second = parlay::reduce(parlay::delayed_map(nodes, [](coord x) -> real
                                                                     { return x.second; })) /
                                  nodes.size();
    node->width = hi_x - lo_x;
    node->se = nullptr;
    node->sw = nullptr;
    node->ne = nullptr;
    node->nw = nullptr;

    if (nodes.size() == 1)
    {
        return node;
    }

    auto mid_x = (lo_x + hi_x) / 2;
    auto mid_y = (lo_y + hi_y) / 2;
    auto get_key = [&](coord &c) -> size_t
    { return 2 * (c.first <= mid_x) + (c.second <= mid_y); };

    // TODO in place?
    auto [sorted, counts] = parlay::counting_sort(nodes, 4, get_key);

    // Construct subtrees (in parallel if sufficiently large)
    parlay::par_do_if(counts[4] > 50, [&]()
                      { parlay::par_do_if(counts[2] > 50, [&]()
                                          { node->se = make_bh_tree(sorted.cut(counts[0], counts[1]), mid_x, hi_x, mid_y, hi_y); }, [&]()
                                          { node->ne = make_bh_tree(sorted.cut(counts[1], counts[2]), mid_x, hi_x, lo_y, mid_y); }); }, [&]()
                      { parlay::par_do_if(counts[4] - counts[2] > 50, [&]()
                                          { node->sw = make_bh_tree(sorted.cut(counts[2], counts[3]), lo_x, mid_x, mid_y, hi_y); }, [&]()
                                          { node->nw = make_bh_tree(sorted.cut(counts[3], counts[4]), lo_x, mid_x, lo_y, mid_y); }); });
    return node;
}

real theta = 0.5;

coord get_repulsive_force(bh_tree *tree, const coord &curr, real ideal_length)
{
    // Empty Node
    if (tree == nullptr)
    {
        return {0.0, 0.0};
    }

    real dist_x = curr.first - tree->center_of_mass.first;
    real dist_y = curr.second - tree->center_of_mass.second;
    real dist = norm(dist_x, dist_y);

    // Ignore queried node and any nodes coincidentally at the same point
    if (dist == 0)
    {
        return {0.0, 0.0};
    }

    // If external node or far enough internal node, calculate force using center of mass
    if (tree->count == 1 || tree->width / dist <= 0.5)
    {
        real for_x = dist_x * tree->count * (ideal_length / dist) * (ideal_length / dist);
        real for_y = dist_y * tree->count * (ideal_length / dist) * (ideal_length / dist);
        return {for_x, for_y};
    }

    // Otherwise, calculate total force from children
    // Not parallelised as queries will already be done in parallel
    auto [sex, sey] = get_repulsive_force(tree->se, curr, ideal_length);
    auto [nex, ney] = get_repulsive_force(tree->ne, curr, ideal_length);
    auto [nwx, nwy] = get_repulsive_force(tree->nw, curr, ideal_length);
    auto [swx, swy] = get_repulsive_force(tree->sw, curr, ideal_length);
    return {sex + nex + nwx + swx, sey + ney + nwy + swy};
}

void free_bh_tree(bh_tree *tree)
{
    if (tree == nullptr)
    {
        return;
    }

    parlay::par_do_if(tree->count > 200, [&]()
                      { parlay::par_do([&]()
                                       { free_bh_tree(tree->ne); }, [&]()
                                       { free_bh_tree(tree->nw); }); }, [&]()
                      { parlay::par_do([&]()
                                       { free_bh_tree(tree->se); }, [&]()
                                       { free_bh_tree(tree->sw); }); });

    bh_tree_allocator.retire(tree);
}

auto update_pos(const parlay::sequence<edge> &E, parlay::sequence<coord> &coords, uint n, real temp, real ideal_length)
{
    bh_tree *tree = make_bh_tree(coords.cut(0, n));

    parlay::sequence<coord> displacement = parlay::map(coords, [&](coord &c)
                                                       { return get_repulsive_force(tree, c, ideal_length); });
    free_bh_tree(tree);

    parlay::parallel_for(0, E.size(), [&](size_t curr_e)
                         {
        auto i = E[curr_e].first;
        auto j = E[curr_e].second;
        auto dx = coords[j].first - coords[i].first;
        auto dy = coords[j].second - coords[i].second;
        auto dist = norm(dx, dy);
        if (dist > 0) {
            displacement[i].first += dx * (dist / ideal_length);
            displacement[i].second += dy * (dist / ideal_length);
            displacement[j].first -= dx * (dist / ideal_length);
            displacement[j].second -= dy * (dist / ideal_length);
        } });

    parlay::parallel_for(0, n, [&](size_t i)
                         {
                             auto curr_disp = displacement[i];
                             auto len = norm(curr_disp.first, curr_disp.second);
                             coords[i].first += curr_disp.first * std::min((real)1, temp / len);
                             coords[i].first = 1 - std::abs(1 - std::abs(coords[i].first));
                             coords[i].second += curr_disp.second * std::min((real)1, temp / len);
                             coords[i].second = 1 - std::abs(1 - std::abs(coords[i].second)); });
}

auto force_directed_drawing(const parlay::sequence<edge> &E, uint n, uint max_iters = 0, real temp = 1.0, real cooling_rate = -1)
{
    if (max_iters == 0)
        max_iters = 5 * n;

    // 0.1 chosen empirically to work for sparse graphs
    real ideal_length = 0.1 * std::sqrt(((real)1) / n);

    // temperature @ max_iters = temperature * (cooling_rate)^max_iters = 0.01
    cooling_rate = std::pow((0.1 * ideal_length) / temp, 1.0 / max_iters);

    // generate n random points in a unit square
    parlay::random_generator gen(0);
    std::uniform_real_distribution<real> dis(0.0, 1.0);
    auto coords = parlay::tabulate(n, [&](int i)
                                   {
      auto r = gen[i];
      return std::make_pair(dis(r), dis(r)); });

    for (uint i = 0; i < max_iters; i++)
    {
        update_pos(E, coords, n, temp, ideal_length);
        real eps = 0.01;
        int border_count = parlay::reduce(parlay::delayed_map(coords, [&] (coord c) -> int {
            return (c.first > 1 - eps || c.first < eps || c.second > 1 - eps || c.second < eps) ? 1 : 0;
        }));

        // If there are more than twice the expected number of points at the border, shrink ideal length to make space
        if ((real) border_count > n * (2 * 4 * eps)){
            ideal_length *= 0.95;
        }

        temp *= cooling_rate;
    }

    return coords;
}

#endif