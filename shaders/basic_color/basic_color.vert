#version 450

in vec4 position;        // raw mesh model vertices
uniform mat4 projection; // camera projection
uniform mat4 view;       // inverse camera transform
uniform mat4 model;      // mesh transform

void main() { gl_Position = projection * view * model * position; }