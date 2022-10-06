#ifndef LIT_QUAD_MESH
#define LIT_QUAD_MESH
#include "lit/components/mesh.hpp"

namespace lit {
class quad_mesh : public mesh {
    struct quad_vertex {
        squint::fvec2 position;
        squint::fvec4 color;
        squint::fvec2 tex_coords;
    };

  public:
    quad_mesh() : mesh(draw_method::TRIANGLES, index_type::UNSIGNED_BYTE) {
        auto the_renderer = app::instance().active_renderer();
        // geometry
        std::vector<quad_vertex> geom;
        geom.reserve(4);
        geom.push_back({{-0.5f, -0.5f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 0.f}});
        geom.push_back({{0.5f, -0.5f}, {0.f, 1.f, 0.f, 1.f}, {1.f, 0.f}});
        geom.push_back({{0.5f, 0.5f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 1.f}});
        geom.push_back({{-0.5f, 0.5f}, {1.f, 1.f, 1.f, 1.f}, {0.f, 1.f}});
        add_vertex_buffer(the_renderer->gen_buffer<quad_vertex>(geom,
                                                                {
                                                                    {buffer_attribute_type::POSITION_2D, "position"},
                                                                    {buffer_attribute_type::COLOR, "color"},
                                                                    {buffer_attribute_type::TEXTURE_MAP, "tex_coords"},
                                                                },
                                                                buffer_access_type::STATIC));
        // indices
        std::vector<uint8_t> indices;
        indices.reserve(6);
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);
        set_index_buffer(the_renderer->gen_buffer<uint8_t>(indices, {}, buffer_access_type::STATIC));
    }
};
} // namespace lit
#endif