#ifndef SQUARE_LINE_MESH
#define SQUARE_LINE_MESH

#include "square/components/mesh.hpp"

namespace square {
// a line with only geometry, no normals or texture coordinates
class line_mesh : public simple_mesh {
  public:
    // The line will connect (-0.5,0.0) -- (0.5,0.0) in model space.
    // The line will take up 1 pixel in width. only vertices are provided
    line_mesh() : simple_mesh(draw_method::LINES, index_type::NONE) {
        std::vector<float> verts{-0.5f, 0.0f, 0.5f, 0.0f};
        add_vertex_buffer(app::renderer()->gen_buffer<float>(verts,
                                                             {
                                                                 {buffer_attribute_type::POSITION_2D, "position"},
                                                             },
                                                             buffer_access_type::STATIC));
    }
    // The line will connect (-0.5,0.0) -- (0.5,0.0) in model space.
    // will be centered on y axis with provided thickness
    line_mesh(float thickness) : simple_mesh(draw_method::TRIANGLES, index_type::NONE) {
        std::vector<float> verts{
            -0.5f, -thickness * 0.5f, 0.5f,  -thickness * 0.5f, 0.5f,  thickness * 0.5f,
            0.5f,  thickness * 0.5f,  -0.5f, thickness * 0.5f,  -0.5f, -thickness * 0.5f,
        };
        add_vertex_buffer(app::renderer()->gen_buffer<float>(verts,
                                                          {
                                                              {buffer_attribute_type::POSITION_2D, "position"},
                                                          },
                                                          buffer_access_type::STATIC));
    }
};
} // namespace square

#endif