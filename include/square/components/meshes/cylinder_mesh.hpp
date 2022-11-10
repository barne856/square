#ifndef SQUARE_CYLINDER_MESH
#define SQUARE_CYLINDER_MESH

#include "square/components/mesh.hpp"

namespace square {
class cylinder_mesh : public simple_mesh {
  public:
    // Construct a new cylinder mesh with vertices and normals.
    //
    // start_angle The starting angle of the cylinder in radians.
    // end_angle The ending angle of the cylinder in radians.
    // sides The sides used to approximate the cylinder.
    cylinder_mesh(float start_angle, float end_angle, int sides)
        : simple_mesh(draw_method::TRIANGLES, index_type::UNSIGNED_INT) {
        std::vector<float> data;
        std::vector<unsigned int> indices;

        float theta = end_angle - start_angle;
        float dtheta = theta / sides;

        // outside edge
        float angle = start_angle;
        for (int i = 0; i < sides + 1; i++) {
            data.push_back((0.5f) * cos(angle));
            data.push_back((0.5f) * sin(angle));
            data.push_back(0.0f);
            data.push_back(cos(angle));
            data.push_back(sin(angle));
            data.push_back(0.0f);
            angle += dtheta;
        }

        angle = start_angle;
        for (int i = 0; i < sides + 1; i++) {
            data.push_back((0.5f) * cos(angle));
            data.push_back((0.5f) * sin(angle));
            data.push_back(1.0f);
            data.push_back(cos(angle));
            data.push_back(sin(angle));
            data.push_back(0.0f);
            angle += dtheta;
        }

        for (int i = 0; i < sides; i++) {
            indices.push_back(i + 1);
            indices.push_back(sides + i + 1);
            indices.push_back(i);
            indices.push_back(sides + i + 2);
            indices.push_back(sides + i + 1);
            indices.push_back(i + 1);
        }

        // bottom edge
        angle = start_angle;
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(-1.0f);
        for (int i = 0; i < sides + 1; i++) {
            data.push_back(0.5f * cos(-angle));
            data.push_back(0.5f * sin(-angle));
            data.push_back(0.0f);
            data.push_back(0.0f);
            data.push_back(0.0f);
            data.push_back(-1.0f);
            angle += dtheta;
        }

        for (int i = 0; i < sides + 1; i++) {
            indices.push_back(2 * (sides + 1));
            indices.push_back(2 * (sides + 1) + i + 1);
            indices.push_back(2 * (sides + 1) + i + 2);
        }

        // top edge
        angle = start_angle;
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(1.0f);
        data.push_back(0.0f);
        data.push_back(0.0f);
        data.push_back(1.0f);
        for (int i = 0; i < sides + 1; i++) {
            data.push_back(0.5f * cos(angle));
            data.push_back(0.5f * sin(angle));
            data.push_back(1.0f);
            data.push_back(0.0f);
            data.push_back(0.0f);
            data.push_back(1.0f);
            angle += dtheta;
        }

        for (int i = 0; i < sides + 1; i++) {
            indices.push_back(3 * (sides + 1) + 1);
            indices.push_back(3 * (sides + 1) + i + 2);
            indices.push_back(3 * (sides + 1) + i + 3);
        }

        add_vertex_buffer(app::renderer()->gen_buffer<float>(data,
                                                             {
                                                                 {buffer_attribute_type::POSITION_3D, "position"},
                                                                 {buffer_attribute_type::NORMAL, "normal"},
                                                             },
                                                             buffer_access_type::STATIC));
        set_index_buffer(app::renderer()->gen_buffer<unsigned int>(indices, {{buffer_attribute_type::INDEX_INT, ""}},
                                                                   buffer_access_type::STATIC));
    }
};
} // namespace square

#endif