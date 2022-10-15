#include "lit/components/meshes/quad_mesh.hpp"
#include "lit/components/meshes/sphere_mesh.hpp"
#include "lit/components/meshes/torus_mesh.hpp"
#include "lit/entities/camera.hpp"
#include "lit/entities/materials/basic_texture.hpp"
#include "lit/sdl_gl.hpp"

using namespace lit;
using namespace squint;
using namespace squint::quantities;
// SAMPLE OBJECT -------------------------------------------------------------------------------------------------------
template <typename T> class sample_obj_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (entity.mat && entity.mat->basic_texture_shader.get()) {
            auto s = entity.mat->basic_texture_shader.get();
            s->upload_texture2D("tex", entity.tex.get());
            entity.mesh->face_towards({0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
            s->upload_mat4("model", entity.mesh->get_transformation_matrix());
            entity.mesh->draw();
        }
    }
};
class sample_obj : public entity<sample_obj> {
  public:
    sample_obj(basic_texture *mat) : mat(mat) { gen_render_system<sample_obj_render_system>(); }
    void on_enter() override {
        mesh = std::move(std::make_unique<sphere_mesh>(5, 1.0f));
        tex = app::instance().active_renderer()->gen_texture("../textures/sun.jpg");
        mesh->bind_shader(mat->basic_texture_shader.get());
    }
    void on_exit() override { mesh = nullptr; }
    basic_texture *mat;
    std::unique_ptr<sphere_mesh> mesh;
    std::unique_ptr<texture2D> tex;
};

// LAYER ---------------------------------------------------------------------------------------------------------------
template <typename T> class sample_scene_layer_renderer : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        app::instance().active_renderer()->enable_depth_testing(true);
        app::instance().active_renderer()->enable_face_culling(true);
        // Clear color and depth buffer
        app::instance().active_renderer()->clear_color_buffer({0.1f, 0.1f, 0.1f, 1.0f});
        app::instance().active_renderer()->clear_depth_buffer();
    }
};
template <typename T> class sample_scene_physics : public physics_system<T> {
  public:
    void update(time_f dt, T &entity) const override {
        static time_f t = time_f::seconds(0);
        t += dt / 5.f;
        entity.set_position(
            {3.0f * std::sin(t.as_seconds()), 3.0f * std::cos(t.as_seconds()), 2.0f * std::sin(t.as_seconds())});
        entity.face_towards({0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});
    }
};
template <typename T> class sample_scene_controls : public controls_system<T> {
  public:
    virtual bool on_key(const key_event &event, T &entity) const override final {
        static bool enabled = false;
        if (event == key_event::T_DOWN) {
            enabled = !enabled;
        }
        app::instance().active_renderer()->wireframe_mode(enabled);
        return true;
    }
    virtual bool on_mouse_move(const mouse_move_event &event, T &entity) const override final { return false; }
};

class sample_scene_layer : public camera {
  public:
    sample_scene_layer(projection_type type, float aspect) : camera(type, aspect) {
        auto mat = std::make_unique<basic_texture>(this);
        mat->gen_object<sample_obj>(mat.get());
        attach_object(std::move(mat));
        set_position({3.0f, 0.5f, 0.0f});
        // set up direction to z axis
        face_towards({0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});
        gen_render_system<sample_scene_layer_renderer>();
        gen_physics_system<sample_scene_physics>();
        gen_controls_system<sample_scene_controls>();
    }
    void on_enter() override {
        // set_position({3.0f, -3.0f, 0.0f});
        // face_towards({0.f, 0.f, 0.f}, {0.f, 0.f, 1.f});
        // app::instance().active_renderer()->wireframe_mode(true);
    }
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