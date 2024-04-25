
#include <iostream>
#include <string>

#include "parlaylib/examples/helper/graph_utils.h"
#include "parlaylib/include/parlay/internal/get_time.h"
#include "parlaylib/include/parlay/primitives.h"
#include "parlaylib/include/parlay/sequence.h"
#include "parlaylib/include/parlay/random.h"


// **************************************************************
// Driver
// **************************************************************

using edge = std::pair<int, int>;
using coord = std::pair<double, double>;

auto calculate_pos(const parlay::sequence<edge>& E, uint n) {

    auto sol = parlay::random();


    return sol;
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
    if (tokens.size() != m * 3 + 2) {
        std::cout << "bad file format" << std::endl;
        exit(1);
    }
    parlay::sequence<edge> E;

    for (int i = 0; i < m; i++) {
        int u = parlay::chars_to_int(tokens[3 * i + 2]);
        int v = parlay::chars_to_int(tokens[3 * i + 3]);
        E.push_back(std::make_tuple(u, v));
    }

    auto positions = calculate_pos(E, n);
    for (coord& c: positions) {
        std::cout << std::get<0>(c) << " " << std::get<1>(c);
    }
}