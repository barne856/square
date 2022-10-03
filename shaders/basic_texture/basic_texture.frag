#version 450

in vec2 vert_tex_coords;
out vec4 color;

uniform sampler2D tex;

void main() { color = texture(tex, vert_tex_coords); }