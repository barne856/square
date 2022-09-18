#include "lit/components/camera.hpp"
#include "lit/sdl_gl_renderer.hpp"

using namespace lit;
using namespace squint;
using namespace squint::quantities;

// BASIC MATERIAL ------------------------------------------------------------------------------------------------------
class material {
  public:
    void set_shader(const std::filesystem::path &shader_src_dir) {
        shader_ptr = std::move(app::instance().active_renderer()->gen_shader(shader_src_dir));
    }
    shader *get_shader() { return shader_ptr.get(); }

  private:
    std::unique_ptr<shader> shader_ptr;
};
template <typename T> class basic_material_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (entity.cam) {
            if (auto s = entity.get_shader()) {
                s->activate();
                s->upload_mat4("projection", entity.cam->get_projection_matrix());
                s->upload_mat4("view", entity.cam->get_transformation_matrix());
            }
        }
    }
};
class basic_material : public entity<basic_material>, public material {
  public:
    basic_material(camera *cam) : cam(cam) {
        // this render system will activate the material for use in renderable child objects
        gen_render_system<basic_material_render_system>();
    }
    void on_enter() override { set_shader("../shaders/basic_material"); }
    void on_exit() override {}
    camera *cam;
};

// SAMPLE OBJECT -------------------------------------------------------------------------------------------------------
class basic_material_properties {
  public:
    fvec4 color{};
};
template <typename T> class sample_obj_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (entity.mat && entity.mat->get_shader()) {
            auto s = entity.mat->get_shader();
            entity.color = {0.5f, 0.1f, 0.6f, 0.8f};
            s->upload_vec4("u_color", entity.color);
        }
    }
};
class sample_obj : public entity<sample_obj>, public basic_material_properties {
  public:
    sample_obj(basic_material *mat) : mat(mat) { gen_render_system<sample_obj_render_system>(); }
    void on_enter() override {}
    void on_exit() override {}
    basic_material *mat;
};

// LAYER ---------------------------------------------------------------------------------------------------------------
class sample_scene_layer : public entity<sample_scene_layer>, public camera {
  public:
    sample_scene_layer(projection_type type, float aspect) : camera(type, aspect) {
        auto mat = std::make_unique<basic_material>(this);
        mat->gen_object<sample_obj>(mat.get());
        attach_object(std::move(mat));
    }
    void on_enter() override {}
    void on_exit() override {}
};

// RENDERER ------------------------------------------------------------------------------------------------------------
class sample_scene_renderer : public sdl_gl {
  public:
    sample_scene_renderer() {
        properties.window_title = "untitled";
        properties.window_width = 1280;
        properties.window_height = 720;
        float init_aspect = float(properties.window_width) / float(properties.window_height);
        gen_object<sample_scene_layer>(projection_type::ORTHOGRAPHIC, init_aspect);
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