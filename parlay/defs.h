#ifndef DEFS_H_
#define DEFS_H_

#include <iostream>
#include <cmath>

using real = float;
using edge = std::pair<int, int>;
using coord = std::pair<real, real>;

auto norm(real x, real y) {
    return std::sqrt(x * x + y * y);
}

#endif