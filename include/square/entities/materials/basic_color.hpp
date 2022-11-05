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
    basic_color(camera *cam) : material(cam) {}
    void set_color(const squint::fvec4 &color) { get_shader()->upload_vec4("u_color", color); }
    void on_enter() override {
        // we need to construct the shader here since we need the rendering API to be loaded first
        // material_shader = std::move(app::instance().active_renderer()->gen_shader("../shaders/basic_color"));
        material_shader = std::move(app::instance().active_renderer()->gen_shader({{shader_type::VERTEX_SHADER,
                                                                                    R"(
#version 450

in vec4 position;        // raw mesh model vertices
uniform mat4 projection; // camera projection
uniform mat4 view;       // inverse camera transform
uniform mat4 model;      // mesh transform

layout(std430, binding = 0) buffer model_instances { mat4 models[]; };

void main() {
  if (models.length() == 0) {
    gl_Position = projection * view * model * position;
  } else {
    gl_Position = projection * view * model * models[gl_InstanceID] * position;
  }
}
          )"},
                                                                                   {shader_type::FRAGMENT_SHADER,
                                                                                    R"(
#version 450

out vec4 color;
uniform vec4 u_color;

void main() { color = u_color; }
      )"}}));
    }
};
} // namespace square
#endif