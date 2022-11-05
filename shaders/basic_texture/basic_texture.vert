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