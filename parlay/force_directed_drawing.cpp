
#include <iostream>
#include <string>

#include "graph_utils.h"
#include "parlay/internal/get_time.h"
#include "parlay/primitives.h"

#include "force_directed_drawing.h"

int main(int argc, char *argv[])
{
    auto usage = "Usage: force_directed_drawing <filename> [-t]";
    if (argc < 2 || argc > 3)
    {
        std::cerr << usage << std::endl;
        exit(1);
    }

    bool time = false;
    if (argc == 3) {
        if (std::string(argv[2]).compare("-t") != 0) {
            std::cerr << usage << std::endl;
            exit(1);
        }
        time = true;
    }

    auto str = parlay::file_map(argv[1]);
    auto tokens = parlay::tokens(str);
    uint n = parlay::chars_to_uint(tokens[0]);
    uint m = parlay::chars_to_uint(tokens[1]);
    if (tokens.size() != m * 2 + 2)
    {
        std::cout << "bad file format" << std::endl;
        exit(1);
    }
    parlay::sequence<edge> E;

    for (uint i = 0; i < m; i++)
    {
        int u = parlay::chars_to_int(tokens[2 * i + 2]);
        int v = parlay::chars_to_int(tokens[2 * i + 3]);
        E.push_back(std::make_pair(u, v));
    }

    if (time) {
        parlay::internal::timer t("Time");
        for (int i=0; i < 3; i++) {
        force_directed_drawing(E, n);
        t.next("drawing - " + std::string(argv[1]));
        }
    } else {
        auto positions = force_directed_drawing(E, n);
        std::cout << n << " " << m << std::endl;
        for (coord &c : positions)
        {
            std::cout << std::get<0>(c) << " " << std::get<1>(c) << std::endl;
        }
        for (edge &e : E)
        {
            std::cout << e.first << " " << e.second << std::endl;
        }
    }
    
    
}