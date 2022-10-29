#ifndef SQUARE_BASIC_TEXTURE
#define SQUARE_BASIC_TEXTURE

#include "square/entities/material.hpp"

namespace square {
// Vertex shader inputs:
// in vec4 position;        // raw mesh model vertices
// uniform mat4 projection; // camera projection
// uniform mat4 view;       // inverse camera transform
// uniform mat4 model;      // mesh transform
// uniform vec4 u_color;
class basic_color : public material {
  public:
    basic_color(camera *cam) : material(cam, "../shaders/basic_color") {}
    void set_color(const squint::fvec4 &color) { get_shader()->upload_vec4("u_color", color); }
};
} // namespace square
#endif