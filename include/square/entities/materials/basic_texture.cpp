export module square:basic_texture;
import :material;

export namespace square {
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
        material_shader = std::move(app::renderer()->gen_shader("basic_color", {{shader_type::VERTEX_SHADER,
                                                                                 R"(
#version 450

in vec4 position;        // raw mesh model vertices
in vec2 tex_coords;      // texture coordinates
uniform mat4 projection; // camera projection
uniform mat4 view;       // inverse camera transform
uniform mat4 model;      // mesh transform

out vec2 vert_tex_coords; // output to the fragment shader

layout(std430, binding = 0) buffer model_instances { mat4 models[]; };

void main() {
  if (models.length() == 0) {
    gl_Position = projection * view * model * position;
  } else {
    gl_Position = projection * view * model * models[gl_InstanceID] * position;
  }
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
