module;
#include <vector>
export module square:square_mesh;
import :mesh;
import :renderer;
import squint;

export namespace square {
// constructs a square mesh. Scale the mesh's transform to produce a general rectilinear quadtrangle
class square_mesh : public simple_mesh {
    struct quad_vertex {
        squint::fvec2 position;
        squint::fvec3 normal;
        squint::fvec2 tex_coords;
    };

  public:
    // This method will produce a square mesh centered at the origin with side length 1 facing in the positive z
    // direction
    // position, normal and texture coordinates are supported
    square_mesh() : simple_mesh(draw_method::TRIANGLES, index_type::UNSIGNED_BYTE) {
        // geometry
        std::vector<quad_vertex> geom;
        geom.reserve(4);
        geom.push_back({{-0.5f, -0.5f}, {0.f, 0.f, 1.f}, {0.f, 0.f}});
        geom.push_back({{0.5f, -0.5f}, {0.f, 0.f, 1.f}, {1.f, 0.f}});
        geom.push_back({{0.5f, 0.5f}, {0.f, 0.f, 1.f}, {1.f, 1.f}});
        geom.push_back({{-0.5f, 0.5f}, {0.f, 0.f, 1.f}, {0.f, 1.f}});
        add_vertex_buffer(
            app::renderer()->gen_buffer<quad_vertex>(geom,
                                                     {
                                                         {buffer_attribute_type::POSITION_2D, "position"},
                                                         {buffer_attribute_type::NORMAL, "normal"},
                                                         {buffer_attribute_type::TEXTURE_MAP, "tex_coords"},
                                                     },
                                                     buffer_access_type::STATIC));
        // indices
        //  3----2
        //  |  / |
        //  | /  |
        //  0----1
        std::vector<uint8_t> indices;
        indices.reserve(6);
        // bottom right triangle
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        // top left triangle
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);
        set_index_buffer(app::renderer()->gen_buffer<uint8_t>(indices, {{buffer_attribute_type::INDEX_BYTE, ""}},
                                                              buffer_access_type::STATIC));
    }
};
} // namespace square
