#include "lit/entities/camera.hpp"
#include "lit/mesh.hpp"
#include "lit/sdl_gl.hpp"

using namespace lit;
using namespace squint;
using namespace squint::quantities;
// Quad Mesh -----------------------------------------------------------------------------------------------------------
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
// BASIC MATERIAL ------------------------------------------------------------------------------------------------------
template <typename T> class basic_material_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (entity.cam) {
            if (auto s = entity.basic_material_shader.get()) {
                s->activate();
                s->upload_mat4("projection", entity.cam->get_projection_matrix());
                s->upload_mat4("view", entity.cam->get_view_matrix());
            }
        }
    }
};
class basic_material : public entity<basic_material> {
  public:
    basic_material(camera *cam) : cam(cam) {
        // this render system will activate the material for use in renderable child objects
        gen_render_system<basic_material_render_system>();
    }
    void on_enter() override {
        basic_material_shader = std::move(app::instance().active_renderer()->gen_shader("../shaders/basic_texture"));
    }
    void on_exit() override {}
    camera *cam;
    std::unique_ptr<shader> basic_material_shader;
};

// SAMPLE OBJECT -------------------------------------------------------------------------------------------------------
class basic_material_properties {
  public:
    fvec4 color{};
};
template <typename T> class sample_obj_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (entity.mat && entity.mat->basic_material_shader.get()) {
            auto s = entity.mat->basic_material_shader.get();
            // entity.color = {0.3f, 0.3f, 0.8f, 1.0f};
            // s->upload_vec4("u_color", entity.color);
            s->upload_texture2D("tex", entity.tex.get());
            entity.mesh->face_towards({0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
            s->upload_mat4("model", entity.mesh->get_transformation_matrix());
            entity.mesh->draw();
        }
    }
};
class sample_obj : public entity<sample_obj>, public basic_material_properties {
  public:
    sample_obj(basic_material *mat) : mat(mat) { gen_render_system<sample_obj_render_system>(); }
    void on_enter() override {
        mesh = std::move(std::make_unique<quad_mesh>());
        tex = app::instance().active_renderer()->gen_texture("../textures/checkerboard.png");
        mesh->bind_shader(mat->basic_material_shader.get());
    }
    void on_exit() override { mesh = nullptr; }
    basic_material *mat;
    std::unique_ptr<quad_mesh> mesh;
    std::unique_ptr<texture2D> tex;
};

// LAYER ---------------------------------------------------------------------------------------------------------------
template <typename T> class sample_scene_layer_renderer : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        app::instance().active_renderer()->clear_color_buffer({0.1f, 0.1f, 0.1f, 1.0f});
    }
};
template <typename T> class sample_scene_physics : public physics_system<T> {
  public:
    void update(time_f dt, T &entity) const override {
        static time_f t = time_f::seconds(0);
        t += dt;
        entity.set_position({0.0f, -5.0f + 3.0f * std::sin(t.as_seconds()), std::sin(t.as_seconds())});
        entity.face_towards({0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});
    }
};
template <typename T> class sample_scene_controls : public controls_system<T> {
  public:
    virtual bool on_mouse_move(const mouse_move_event &event, T &entity) const override final {
        std::cout << event.x << ", " << event.y << std::endl;
        std::cout << event.xrel << ", " << event.yrel << std::endl;
        return true;
    }
};

class sample_scene_layer : public camera {
  public:
    sample_scene_layer(projection_type type, float aspect) : camera(type, aspect) {
        auto mat = std::make_unique<basic_material>(this);
        mat->gen_object<sample_obj>(mat.get());
        attach_object(std::move(mat));
        set_position({0.0f, -5.0f, 0.0f});
        // set up direction to z axis
        face_towards({0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});
        gen_render_system<sample_scene_layer_renderer>();
        gen_physics_system<sample_scene_physics>();
        gen_controls_system<sample_scene_controls>();
    }
    void on_enter() override {}
    void on_exit() override {}
};

// RENDERER ------------------------------------------------------------------------------------------------------------
class sample_scene_renderer : public sdl_gl_renderer {
  public:
    sample_scene_renderer() {
        properties.window_title = "untitled";
        properties.window_width = 1280;
        properties.window_height = 720;
        float init_aspect = float(properties.window_width) / float(properties.window_height);
        gen_object<sample_scene_layer>(projection_type::PERSPECTIVE, init_aspect);
    }
    void on_enter() override {
        // load the sample scene layer
        load_object(children()[0].get());
    }
    void on_exit() override {}
};

int main() {
    // create app
    app::instance().gen_renderer<sample_scene_renderer>();
    app::instance().run();
    return 0;
}