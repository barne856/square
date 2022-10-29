#version 450

in vec2 vert_tex_coords;
uniform sampler2D tex;
out vec4 color;

void main() { color = texture(tex, vert_tex_coords); }

// // Example of screen blending two textures
// uniform sampler2D earth_day;
// uniform sampler2D earth_clouds;
//
// void main() {
//   vec4 day_color = texture(earth_day, vert_tex_coords);
//   vec4 cloud_color = texture(earth_clouds, vert_tex_coords);
//   // 'screen' blend clouds onto earth
//   color = vec4(
//       vec3(1.0) - ((vec3(1.0) - day_color.rgb) * (vec3(1.0) -
//       cloud_color.rgb)), 1.0);
// }