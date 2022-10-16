#ifndef LIT_SPHERE_MESH
#define LIT_SPHERE_MESH
#include "lit/components/mesh.hpp"

namespace lit {
class sphere_mesh : public mesh {
    struct sphere_vertex {
        squint::fvec3 position;
        squint::fvec3 normal;
        squint::fvec2 tex_coords;
    };

  public:
    sphere_mesh(unsigned int recursionLevel, float radius) : mesh(draw_method::TRIANGLES, index_type::NONE) {
        auto the_renderer = app::instance().active_renderer();
        const float X = 0.525731112119133606f;
        const float Z = 0.850650808352039932f;
        const float N = 0.0f;

        float verts[36] = {-X, N,  Z, X, N,  Z,  -X, N, -Z, X,  N, -Z, N, Z,  X, N,  Z,  -X,
                           N,  -Z, X, N, -Z, -X, Z,  X, N,  -Z, X, N,  Z, -X, N, -Z, -X, N};

        unsigned int indes[60] = {0,  4, 1, 0, 9, 4, 9, 5,  4, 4, 5,  8,  4,  8, 1, 8,  10, 1,  8, 3,
                                  10, 5, 3, 8, 5, 2, 3, 2,  7, 3, 7,  10, 3,  7, 6, 10, 7,  11, 6, 11,
                                  0,  6, 0, 1, 6, 6, 1, 10, 9, 0, 11, 9,  11, 2, 9, 2,  5,  7,  2, 11};

        int count = 180 * (int)pow(4, recursionLevel);

        std::vector<squint::fvec3> vertices;
        vertices.reserve(count / 3);
        for (int i = 0; i < count; ++i) {
            vertices.push_back(squint::fvec3(0.0f));
        }

        unsigned int iter = 0;

        for (int i = 0; i < 20; i++) {
            squint::fvec3 v1 = {verts[3 * indes[3 * i]], verts[3 * indes[3 * i] + 1], verts[3 * indes[3 * i] + 2]};
            squint::fvec3 v2 = {verts[3 * indes[3 * i + 1]], verts[3 * indes[3 * i + 1] + 1],
                                verts[3 * indes[3 * i + 1] + 2]};
            squint::fvec3 v3 = {verts[3 * indes[3 * i + 2]], verts[3 * indes[3 * i + 2] + 1],
                                verts[3 * indes[3 * i + 2] + 2]};
            subdivide(v1, v2, v3, recursionLevel, vertices, iter);
        }

        std::vector<float> data;
        // data.reserve(count * 2);
        for (int i = 0; i < count / 9; i++) {
            squint::fvec3 v1 = radius * vertices[3 * i];
            squint::fvec3 v2 = radius * vertices[3 * i + 1];
            squint::fvec3 v3 = radius * vertices[3 * i + 2];
            // vertex
            data.push_back(v3[0]);
            data.push_back(v3[1]);
            data.push_back(v3[2]);
            // normal
            data.push_back(vertices[3 * i + 2][0]);
            data.push_back(vertices[3 * i + 2][1]);
            data.push_back(vertices[3 * i + 2][2]);
            // texture
            float lat = atan2(v3[2], sqrt(v3[0] * v3[0] + v3[1] * v3[1]));
            float lng = atan2(v3[1], v3[0]);
            data.push_back(0.5f + lng / (M_PI * 2.f));
            data.push_back(0.5f - lat / M_PI);
            // vertex
            data.push_back(v2[0]);
            data.push_back(v2[1]);
            data.push_back(v2[2]);
            // normal
            data.push_back(vertices[3 * i + 1][0]);
            data.push_back(vertices[3 * i + 1][1]);
            data.push_back(vertices[3 * i + 1][2]);
            // texture
            lat = atan2(v2[2], sqrt(v2[0] * v2[0] + v2[1] * v2[1]));
            lng = atan2(v2[1], v2[0]);
            data.push_back(0.5f + lng / (M_PI * 2.f));
            data.push_back(0.5f - lat / M_PI);
            // vertex
            data.push_back(v1[0]);
            data.push_back(v1[1]);
            data.push_back(v1[2]);
            // normal
            data.push_back(vertices[3 * i][0]);
            data.push_back(vertices[3 * i][1]);
            data.push_back(vertices[3 * i][2]);
            // texture
            lat = atan2(v1[2], sqrt(v1[0] * v1[0] + v1[1] * v1[1]));
            lng = atan2(v1[1], v1[0]);
            data.push_back(0.5f + lng / (M_PI * 2.f));
            data.push_back(0.5f - lat / M_PI);
        }
        add_vertex_buffer(the_renderer->gen_buffer<float>(data,
                                                          {
                                                              {buffer_attribute_type::POSITION_3D, "position"},
                                                              {buffer_attribute_type::NORMAL, "normal"},
                                                              {buffer_attribute_type::TEXTURE_MAP, "tex_coords"},
                                                          },
                                                          buffer_access_type::STATIC));
    }
    sphere_mesh(size_t n_lats, size_t n_lngs, float radius) : mesh(draw_method::TRIANGLES, index_type::UNSIGNED_INT) {
        auto the_renderer = app::instance().active_renderer();
        if (n_lngs < 3) {
            n_lngs = 3;
        }
        if (n_lats < 2) {
            n_lats = 2;
        }
        std::vector<sphere_vertex> data{};
        std::vector<unsigned int> indices;
        float delta_lat = M_PI / n_lats;
        float delta_lng = 2 * M_PI / n_lngs;
        float lat;
        float lng;
        // vertex data
        for (int i = 0; i <= n_lats; i++) {
            lat = M_PI / 2 - i * delta_lat; /* Starting -pi/2 to pi/2 */
            float xy = radius * cosf(lat);  /* r * cos(phi) */
            float z = radius * sinf(lat);   /* r * sin(phi )*/
            /*
             * We add (latitudes + 1) vertices per longitude because of equator,
             * the North pole and South pole are not counted here, as they overlap.
             * The first and last vertices have same position and normal, but
             * different tex coords.
             */
            for (int j = 0; j <= n_lngs; ++j) {
                lng = j * delta_lng;
                squint::fvec3 position;
                squint::fvec3 normal;
                squint::fvec2 tex_coord;
                position[0] = xy * cosf(lng);     /* x = r * cos(phi) * cos(theta)  */
                position[1] = xy * sinf(lng);     /* y = r * cos(phi) * sin(theta) */
                position[2] = z;                  /* z = r * sin(phi) */
                tex_coord[0] = (float)j / n_lngs; /* s */
                tex_coord[1] = (float)i / n_lats; /* t */
                // normalized vertex normal
                normal = position / radius;
                data.push_back({position, normal, tex_coord});
            }
        }
        /*
         *  Indices
         *  k1--k1+1
         *  |  / |
         *  | /  |
         *  k2--k2+1
         */
        unsigned int k1, k2;
        for (int i = 0; i < n_lats; ++i) {
            k1 = i * (n_lngs + 1);
            k2 = k1 + n_lngs + 1;
            // 2 Triangles per latitude block excluding the first and last longitudes blocks
            for (int j = 0; j < n_lngs; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (n_lats - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
        add_vertex_buffer(
            the_renderer->gen_buffer<sphere_vertex>(data,
                                                    {
                                                        {buffer_attribute_type::POSITION_3D, "position"},
                                                        {buffer_attribute_type::NORMAL, "normal"},
                                                        {buffer_attribute_type::TEXTURE_MAP, "tex_coords"},
                                                    },
                                                    buffer_access_type::STATIC));
        set_index_buffer(the_renderer->gen_buffer<unsigned int>(indices, {}, buffer_access_type::STATIC));
    }

  private:
    void subdivide(squint::fvec3 &v1, squint::fvec3 &v2, squint::fvec3 &v3, int depth,
                   std::vector<squint::fvec3> &vertices, unsigned int &iter) {
        squint::fvec3 v12, v23, v31;

        if (depth == 0) {
            vertices[iter] = v1;
            iter++;
            vertices[iter] = v2;
            iter++;
            vertices[iter] = v3;
            iter++;
            return;
        }

        // calculate midpoints of each side
        for (int i = 0; i < 3; i++) {
            v12[i] = (v1[i] + v2[i]) / 2.0f;
            v23[i] = (v2[i] + v3[i]) / 2.0f;
            v31[i] = (v3[i] + v1[i]) / 2.0f;
        }
        // extrude midpoints to lie on unit sphere
        v12 = v12 / squint::norm(v12);
        v23 = v23 / squint::norm(v23);
        v31 = v31 / squint::norm(v31);

        // recursively subdivide new triangles
        subdivide(v1, v12, v31, depth - 1, vertices, iter);
        subdivide(v2, v23, v12, depth - 1, vertices, iter);
        subdivide(v3, v31, v23, depth - 1, vertices, iter);
        subdivide(v12, v23, v31, depth - 1, vertices, iter);
    }
};
} // namespace lit
#endif