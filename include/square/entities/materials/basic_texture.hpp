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
    basic_texture(camera *cam) : material(cam) {}
    void set_texture(texture2D *tex) { get_shader()->upload_texture2D("tex", tex); }
    void on_enter() override {
        // we need to construct the shader here since we need the rendering API to be loaded first
        // material_shader = std::move(app::instance().active_renderer()->gen_shader("../shaders/basic_texture"));
        material_shader = std::move(app::instance().active_renderer()->gen_shader({{shader_type::VERTEX_SHADER,
                                                                                    R"(
#version 450

in vec4 position;        // raw mesh model vertices
in vec2 tex_coords;      // texture coordinates
uniform mat4 projection; // camera projection
uniform mat4 view;       // inverse camera transform
uniform mat4 model;      // mesh transform

out vec2 vert_tex_coords; // output to the fragment shader

void main() {
  gl_Position = projection * view * model * position;
  vert_tex_coords = tex_coords;
}
          )"},
                                                                                   {shader_type::FRAGMENT_SHADER,
                                                                                    R"(
#version 450

in vec2 vert_tex_coords;
uniform sampler2D tex;
out vec4 color;

void main() { color = texture(tex, vert_tex_coords); }
      )"}}));
    }
};
} // namespace square
#endif