#version 450

in vec4 position;        // raw mesh model vertices
in vec4 color;           // color of the vertex
uniform mat4 projection; // camera projection
uniform mat4 view;       // camera transform
uniform mat4 model;      // mesh transform

out vec4 vert_color; // output to the fragment shader

void main() {
  gl_Position = projection * view * model * position;
  vert_color = color;
}