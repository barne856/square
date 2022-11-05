#ifndef SQUARE_CUBE_MESH
#define SQUARE_CUBE_MESH

#include "square/components/mesh.hpp"

namespace square {
class cube_mesh : public simple_mesh {
  public:
    // Construct a mesh of a 3D cube. scale is the length of each face of the cube. Contains normals and vertices.
    cube_mesh(float scale) : simple_mesh(draw_method::TRIANGLES, index_type::UNSIGNED_BYTE) {
        auto the_renderer = app::instance().active_renderer();
        std::vector<squint::fvec3> data;
        std::vector<uint8_t> indices;

        // front face vertices and normals, counterclockwise around normal from
        // bottom left
        data.push_back({-0.5f * scale, -0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, 0.0f, 1.0f});
        data.push_back({0.5f * scale, -0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, 0.0f, 1.0f});
        data.push_back({0.5f * scale, 0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, 0.0f, 1.0f});
        data.push_back({-0.5f * scale, 0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, 0.0f, 1.0f});

        // right face vertices and normals, counterclockwise around normal from
        // bottom left
        data.push_back({0.5f * scale, -0.5f * scale, 0.5f * scale});
        data.push_back({1.0f, 0.0f, 0.0f});
        data.push_back({0.5f * scale, -0.5f * scale, -0.5f * scale});
        data.push_back({1.0f, 0.0f, 0.0f});
        data.push_back({0.5f * scale, 0.5f * scale, -0.5f * scale});
        data.push_back({1.0f, 0.0f, 0.0f});
        data.push_back({0.5f * scale, 0.5f * scale, 0.5f * scale});
        data.push_back({1.0f, 0.0f, 0.0f});

        // back face vertices and normals, counterclockwise around normal from
        // bottom left
        data.push_back({0.5f * scale, -0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, 0.0f, -1.0f});
        data.push_back({-0.5f * scale, -0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, 0.0f, -1.0f});
        data.push_back({-0.5f * scale, 0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, 0.0f, -1.0f});
        data.push_back({0.5f * scale, 0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, 0.0f, -1.0f});

        // left face vertices and normals, counterclockwise around normal from
        // bottom left
        data.push_back({-0.5f * scale, -0.5f * scale, -0.5f * scale});
        data.push_back({-1.0f, 0.0f, 0.0f});
        data.push_back({-0.5f * scale, -0.5f * scale, 0.5f * scale});
        data.push_back({-1.0f, 0.0f, 0.0f});
        data.push_back({-0.5f * scale, 0.5f * scale, 0.5f * scale});
        data.push_back({-1.0f, 0.0f, 0.0f});
        data.push_back({-0.5f * scale, 0.5f * scale, -0.5f * scale});
        data.push_back({-1.0f, 0.0f, 0.0f});

        // top face vertices and normals, counterclockwise around normal from bottom
        // left
        data.push_back({-0.5f * scale, 0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, 1.0f, 0.0f});
        data.push_back({0.5f * scale, 0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, 1.0f, 0.0f});
        data.push_back({0.5f * scale, 0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, 1.0f, 0.0f});
        data.push_back({-0.5f * scale, 0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, 1.0f, 0.0f});

        // bottom face vertices and normals, counterclockwise around normal from
        // bottom left
        data.push_back({-0.5f * scale, -0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, -1.0f, 0.0f});
        data.push_back({0.5f * scale, -0.5f * scale, -0.5f * scale});
        data.push_back({0.0f, -1.0f, 0.0f});
        data.push_back({0.5f * scale, -0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, -1.0f, 0.0f});
        data.push_back({-0.5f * scale, -0.5f * scale, 0.5f * scale});
        data.push_back({0.0f, -1.0f, 0.0f});

        // front face indices
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);

        // right face indices
        indices.push_back(4);
        indices.push_back(5);
        indices.push_back(6);
        indices.push_back(6);
        indices.push_back(7);
        indices.push_back(4);

        // back face indices
        indices.push_back(8);
        indices.push_back(9);
        indices.push_back(10);
        indices.push_back(10);
        indices.push_back(11);
        indices.push_back(8);

        // left face indices
        indices.push_back(12);
        indices.push_back(13);
        indices.push_back(14);
        indices.push_back(14);
        indices.push_back(15);
        indices.push_back(12);

        // top face indices
        indices.push_back(16);
        indices.push_back(17);
        indices.push_back(18);
        indices.push_back(18);
        indices.push_back(19);
        indices.push_back(16);

        // bottom face indices
        indices.push_back(20);
        indices.push_back(21);
        indices.push_back(22);
        indices.push_back(22);
        indices.push_back(23);
        indices.push_back(20);

        add_vertex_buffer(the_renderer->gen_buffer<squint::fvec3>(data,
                                                                  {
                                                                      {buffer_attribute_type::POSITION_3D, "position"},
                                                                      {buffer_attribute_type::NORMAL, "normal"},
                                                                  },
                                                                  buffer_access_type::STATIC));
        set_index_buffer(the_renderer->gen_buffer<uint8_t>(indices, {{buffer_attribute_type::INDEX_BYTE, ""}},
                                                           buffer_access_type::STATIC));
    }
};
} // namespace square

#endif