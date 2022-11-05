#ifndef SQUARE_COLOR
#define SQUARE_COLOR

#include "squint/tensor.hpp"
#include <cstdio>
#include <string>

namespace square {
class color {
  public:
    static squint::fvec4 parse_hexcode(const std::string &code) {
        unsigned char r, g, b;
        std::sscanf(code.c_str(), "%02hhx%02hhx%02hhx", &r, &g, &b);
        return {(float)r / 255.f, (float)g / 255.f, (float)b / 255.f, 1.0f};
    }
};
} // namespace square

#endif