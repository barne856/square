#ifndef LIT_TORUS_MESH
#define LIT_TORUS_MESH
#include "lit/components/mesh.hpp"

namespace lit {
class torus_mesh : public mesh {
    struct torus_vertex {
        squint::fvec3 position;
        squint::fvec3 normal;
        squint::fvec2 tex_coords;
    };

  public:
    torus_mesh(unsigned int n_segments, unsigned int n_rings, float inner_radius, float outer_radius)
        : mesh(draw_method::TRIANGLE_STRIP, index_type::UNSIGNED_INT), r(outer_radius), R(inner_radius), q(n_segments),
          p(n_rings) {
        auto the_renderer = app::instance().active_renderer();
        std::vector<torus_vertex> vertex_data;
        std::vector<unsigned int> indices;

        // Compute torus vertices and normals
        for (unsigned int i = 0; i <= p; ++i) {
            float u = 2.f * PI * float(i) / float(p);
            for (unsigned int j = 0; j <= q; ++j) {
                // Note that u is added to j. This creates a spiral pattern, which
                // allows us to draw the entire torus in one triangle strip without
                // using degenerate triangles.
                // (Yet we still need degenerate triangles to avoid texture
                // wrap-around)
                float v = 2.f * PI * (float(j) + u) / float(q);
                // Position
                float x = cos(u) * (r + cos(v) * R);
                float y = sin(u) * (r + cos(v) * R);
                float z = sin(v) * R;
                if (!std::isnormal(x)) {
                    std::cout << x << std::endl;
                }
                // Normal vector
                float nx = cos(u) * cos(v);
                float ny = sin(u) * cos(v);
                float nz = sin(v);
                // add vertex data to vector
                vertex_data.push_back({{x, y, z}, {nx, ny, nz}, {u / (2.f * PI), v / (2.f * PI)}});
            }
        }

        // Compute torus indices
        unsigned int n_vertices = static_cast<unsigned int>(vertex_data.size());
        for (unsigned int i = 0; i <= n_vertices; ++i) {
            indices.push_back(static_cast<unsigned int>(i % n_vertices));
            indices.push_back(static_cast<unsigned int>((i + q + 1) % n_vertices));
        }

        add_vertex_buffer(the_renderer->gen_buffer<torus_vertex>(vertex_data,
                                                                 {
                                                                     {buffer_attribute_type::POSITION_3D, "position"},
                                                                     {buffer_attribute_type::NORMAL, "normal"},
                                                                     {buffer_attribute_type::TEXTURE_MAP, "tex_coords"},
                                                                 },
                                                                 buffer_access_type::STATIC));

        set_index_buffer(the_renderer->gen_buffer<unsigned int>(indices, {}, buffer_access_type::STATIC));
    }

  private:
    float r;        // outer radius
    float R;        // inner radius
    unsigned int p; // number of mesh columns
    unsigned int q; // number of mesh row
    const float PI = 3.141592653f;

    // Fuctions to map the grid vertex (u_i,v_j) to the mesh vertex (f(u_i,v_j),
    // g(u_i,v_j), h(u_i,v_j)) on the torus.
    float f(int i, int j) { return ((R + r * cos((-1 + 2 * (float)j / q) * PI)) * cos((-1 + 2 * (float)i / p) * PI)); }

    float g(int i, int j) { return ((R + r * cos((-1 + 2 * (float)j / q) * PI)) * sin((-1 + 2 * (float)i / p) * PI)); }

    float h(int i, int j) { return (r * sin((-1 + 2 * (float)j / q) * PI)); }
};
} // namespace lit
#endif