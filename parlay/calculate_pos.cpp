
#include <iostream>
#include <string>

#include "graph_utils.h"
#include "parlay/internal/get_time.h"
#include "parlay/primitives.h"
#include "parlay/sequence.h"
#include "parlay/random.h"


// **************************************************************
// Driver
// **************************************************************

using real = float;
using edge = std::pair<int, int>;
using coord = std::pair<real, real>;

auto norm(real x, real y) {
    return std::sqrt(x * x + y * y);
}

auto update_pos(const parlay::sequence<edge>& E, parlay::sequence<coord>& coords, uint n, real temp) {
    parlay::sequence<coord> displacement = parlay::tabulate(n, [&] (int i) {return std::make_pair((real) 0, (real) 0);});
    real k = std::sqrt(((real) 1) / n); 

    parlay::parallel_for(0, n, [&] (size_t i) {
        parlay::parallel_for(i + 1, n, [&] (size_t j) {
            auto dx = coords[j].first - coords[i].first;
            auto dy = coords[j].second - coords[i].second;
            auto dist = norm(dx, dy);
            if (dist > 0) {
                displacement[i].first -= dx / dist * (k * k / dist);
                displacement[i].second -= dy / dist * (k * k / dist);
                displacement[j].first += dx / dist * (k * k / dist);
                displacement[j].second += dy / dist * (k * k / dist);
            }
        });
    });

    parlay::parallel_for(0, E.size(), [&] (size_t curr_e) {
        auto i = E[curr_e].first;
        auto j = E[curr_e].second;
        auto dx = coords[j].first - coords[i].first;
        auto dy = coords[j].second - coords[i].second;
        auto dist = norm(dx, dy);
        if (dist > 0) {
            displacement[i].first += dx * (dist / k);
            displacement[i].second += dy * (dist / k);
            displacement[j].first -= dx * (dist / k);
            displacement[j].second -= dy * (dist / k);
        }
    });

    parlay::parallel_for(0, n, [&] (size_t i) {
        auto curr_disp = displacement[i];
        auto len = norm(curr_disp.first, curr_disp.second);
        coords[i].first += curr_disp.first * std::min((real) 1, temp / len);
        coords[i].first = std::clamp(coords[i].first, (real) 0.0, (real) 1.0);
        coords[i].second += curr_disp.second * std::min((real) 1, temp / len);
        coords[i].second = std::clamp(coords[i].second, (real) 0.0, (real) 1.0);

    });
}

auto calculate_pos(const parlay::sequence<edge>& E, uint n, uint iters, real temp_init = 1, real cooling_rate = 0.99) {

    parlay::random_generator gen(0);
    std::uniform_real_distribution<real> dis(0.0,1.0);

    // generate n random points in a unit square
    auto coords = parlay::tabulate(n, [&] (int i) {
      auto r = gen[i];
      return std::make_pair(dis(r), dis(r));
    });

    real temp = temp_init;
    for (uint i = 0; i < iters; i++) {
        update_pos(E, coords, n, temp);
        temp *= cooling_rate;
    }
    
    return coords;
}


int main(int argc, char* argv[]) {
    auto usage = "Usage: calculate_pos <filename>";
    if (argc != 2) {
        std::cout << usage << std::endl;
        exit(1);
    }

    auto str = parlay::file_map(argv[1]);
    auto tokens = parlay::tokens(str);
    uint n = parlay::chars_to_uint(tokens[0]);
    uint m = parlay::chars_to_uint(tokens[1]);
    if (tokens.size() != m * 2 + 2) {
        std::cout << "bad file format" << std::endl;
        exit(1);
    }
    parlay::sequence<edge> E;

    for (uint i = 0; i < m; i++) {
        int u = parlay::chars_to_int(tokens[2 * i + 2]);
        int v = parlay::chars_to_int(tokens[2 * i + 3]);
        E.push_back(std::make_pair(u, v));
    }

    auto positions = calculate_pos(E, n, 1000);
    for (coord& c: positions) {
        std::cout << std::get<0>(c) << " " << std::get<1>(c) << std::endl;
    }
}