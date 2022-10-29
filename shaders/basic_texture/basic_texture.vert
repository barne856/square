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