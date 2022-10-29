#ifndef SQUARE_CIRCLE_MESH
#define SQUARE_CIRCLE_MESH
#include "square/components/mesh.hpp"
#include "squint/quantity.hpp"

namespace square {
// constructs a circle mesh.
class circle_mesh : public mesh {
    struct circle_vertex {
        squint::fvec2 position;
        squint::fvec3 normal;
    };

  public:
    // This method will produce a circle mesh centered at the origin with radius r facing in the positive z
    // direction
    // position and normals are supported
    circle_mesh(int sides, float radius) : mesh(draw_method::TRIANGLE_FAN, index_type::NONE) {
        auto the_renderer = app::instance().active_renderer();
        // geometry
        std::vector<circle_vertex> geom;
        geom.reserve(sides + 2);
        geom.push_back({{0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});
        for (int i = 0; i < sides; i++) {
            float a = 2.0f * 3.141592653f * (float(i) / float(sides));
            geom.push_back({{radius * cos(a), radius * sin(a)}, {0.0f, 0.0f, 1.0f}});
        }
        geom.push_back({{radius, 0.0f}, {0.0f, 0.0f, 1.0f}});
        add_vertex_buffer(the_renderer->gen_buffer<circle_vertex>(geom,
                                                                  {
                                                                      {buffer_attribute_type::POSITION_2D, "position"},
                                                                      {buffer_attribute_type::NORMAL, "normal"},
                                                                  },
                                                                  buffer_access_type::STATIC));
    }
};
} // namespace square
#endif