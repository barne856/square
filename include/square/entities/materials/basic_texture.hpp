#ifndef SQUARE_BASIC_TEXTURE
#define SQUARE_BASIC_TEXTURE

#include "square/entities/material.hpp"

namespace square {
// Vertex shader inputs:
// in vec4 position;        // raw mesh model vertices
// in vec2 tex_coords;      // texture coordinates
// uniform sampler2D tex;   // texture sampler2D
// uniform mat4 projection; // camera projection
// uniform mat4 view;       // inverse camera transform
// uniform mat4 model;      // mesh transform
class basic_texture : public material {
  public:
    basic_texture(camera *cam) : material(cam, "../shaders/basic_texture") {}
    void set_texture(texture2D *tex) { get_shader()->upload_texture2D("tex", tex); }
};
} // namespace square
#endif