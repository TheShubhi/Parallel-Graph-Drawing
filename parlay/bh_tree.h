#ifndef BH_TREE_H_
#define BH_TREE_H_

#include "parlay/sequence.h"
#include "parlay/primitives.h"
#include "defs.h"




struct bh_tree
{
    bh_tree *ne, *nw, *se, *sw;
    int count;
    coord com;
    real width;
};

parlay::type_allocator<bh_tree> bh_tree_allocator;

bh_tree* make_bh_tree(parlay::slice<coord *, coord *> nodes, real lo_x = 0.0, real hi_x = 1.0, real lo_y = 0.0, real hi_y = 1.0) {
    if (nodes.size() == 0) {
        return nullptr;
    }

    bh_tree* node = bh_tree_allocator.alloc();
    node->count = nodes.size();

    // TODO do in recursive step?
    node->com.first = parlay::reduce(parlay::delayed_map(nodes, [] (coord x) -> real {return x.first;})) / nodes.size();
    node->com.second = parlay::reduce(parlay::delayed_map(nodes, [] (coord x) -> real {return x.second;})) / nodes.size();
    node->width = hi_x - lo_x;
    node->se = nullptr;
    node->sw = nullptr;
    node->ne = nullptr;
    node->nw = nullptr;

    if (nodes.size() == 1) {
        return node;
    }

    auto mid_x = (lo_x + hi_x) / 2;
    auto mid_y = (lo_y + hi_y) / 2;
    auto get_key = [&](coord& c) -> size_t { return 2 * (c.first <= mid_x) + (c.second <= mid_y);};
    auto [sorted, counts] = parlay::counting_sort(nodes, 4, get_key);

    // par_do_if
    node->se = make_bh_tree(sorted.cut(counts[0], counts[1]), mid_x, hi_x, mid_y, hi_y);
    node->ne = make_bh_tree(sorted.cut(counts[1], counts[2]), mid_x, hi_x, lo_y, mid_y);
    node->sw = make_bh_tree(sorted.cut(counts[2], counts[3]), lo_x, mid_x, mid_y, hi_y);
    node->nw = make_bh_tree(sorted.cut(counts[3], counts[4]), lo_x, mid_x, lo_y, mid_y);
    return node;
    
}

real theta = 0.5;

coord query(bh_tree * tree, coord curr, real k) {
    if (tree == nullptr) {
        return {0.0, 0.0};
    }

    real dist_x = curr.first - tree->com.first;
    real dist_y = curr.second - tree->com.second;
    real dist = norm(dist_x, dist_y);

    // What if multiple nodes have a com same as query points?
    if (dist == 0) {
        return {0.0, 0.0};
    }

    if (tree->count == 1 || tree->width / dist <= 0.5) {
        real for_x = dist_x * tree->count * (k / dist) * (k / dist);
        real for_y = dist_y * tree->count * (k / dist) * (k / dist);
        return {for_x, for_y};
    }

    // par_do?
    auto [sex, sey] = query(tree->se, curr, k);
    auto [nex, ney] = query(tree->ne, curr, k);
    auto [nwx, nwy] = query(tree->nw, curr, k);
    auto [swx, swy] = query(tree->sw, curr, k);
    return {sex + nex + nwx + swx, sey + ney + nwy + swy};
}


void free_bh_tree(bh_tree * tree) {
    if (tree == nullptr) {
        return;
    }

    // par_do?
    free_bh_tree(tree->ne);
    free_bh_tree(tree->nw);
    free_bh_tree(tree->sw);
    free_bh_tree(tree->se);
    bh_tree_allocator.retire(tree);
}

#endif