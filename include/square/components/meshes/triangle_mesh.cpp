module;
#include <vector>
export module square:triangle_mesh;
import :mesh;
import :renderer;
import squint;

export namespace square {
// constructs a square mesh. Scale the mesh's transform to produce a general rectilinear quadtrangle
class triangle_mesh : public simple_mesh {
    struct quad_vertex {
        squint::fvec2 position;
        squint::fvec3 normal;
        squint::fvec2 tex_coords;
    };

  public:
    // This method will produce a triangle mesh from the three vertices provided
    // position, normal and texture coordinates are supported
    triangle_mesh(
        const squint::fvec2& v1,
        const squint::fvec2& v2,
        const squint::fvec2& v3
        ) : simple_mesh(draw_method::TRIANGLES, index_type::UNSIGNED_BYTE) {
        // geometry
        // compute the normal
        // create 3 vectors from the vertices
        const auto u1 = squint::fvec3{static_cast<float>(v1[0]), static_cast<float>(v1[1]), 0.f};
        const auto u2 = squint::fvec3{static_cast<float>(v2[0]), static_cast<float>(v2[1]), 0.f};
        const auto u3 = squint::fvec3{static_cast<float>(v3[0]), static_cast<float>(v3[1]), 0.f};
        auto normal = squint::cross(u2 - u1, u3 - u1);
        normal = normal / squint::norm(normal);
        std::vector<quad_vertex> geom;
        geom.reserve(3);
        geom.push_back({v1, normal, {0.f, 0.f}});
        geom.push_back({v2, normal, {1.f, 0.f}});
        geom.push_back({v3, normal, {1.f, 1.f}});
        add_vertex_buffer(
            app::renderer()->gen_buffer<quad_vertex>(geom,
                                                     {
                                                         {buffer_attribute_type::POSITION_2D, "position"},
                                                         {buffer_attribute_type::NORMAL, "normal"},
                                                         {buffer_attribute_type::TEXTURE_MAP, "tex_coords"},
                                                     },
                                                     buffer_access_type::STATIC));

        std::vector<uint8_t> indices;
        indices.reserve(3);
        // bottom right triangle
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        set_index_buffer(app::renderer()->gen_buffer<uint8_t>(indices, {{buffer_attribute_type::INDEX_BYTE, ""}},
                                                              buffer_access_type::STATIC));
    }
};
} // namespace square
