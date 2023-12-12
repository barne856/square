// A sample app showing a torus with a checkerboard texture where a perspective projection camera orbits the mesh.
// Pressing the 'T' key will toggle wireframe mode
#include <memory>
import square;
import squint;

using namespace square;
using namespace squint;
using namespace squint::quantities;
// SAMPLE OBJECT -------------------------------------------------------------------------------------------------------
// This render system just sets the model matrix of the torus and it's texture in the material
template <typename T> class sample_obj_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        auto mat = entity.mat;
        if (mat) {
            mat->set_texture(entity.checkerboard_tex.get());
            mat->set_model(entity.mesh.get());
            entity.mesh->draw(mat);
        }
    }
};
// This entity contains the torus mesh, checkerboard texture, and material used to render the object
class sample_obj : public entity<sample_obj> {
  public:
    sample_obj(basic_texture *mat) : mat(mat) { attach_render_system<sample_obj_render_system>(); }
    void on_enter() override {
        mesh = std::move(std::make_unique<torus_mesh>(100, 200, 0.5f, 1.0f));
        checkerboard_tex = app::renderer()->gen_texture("textures/checkerboard.png");
        mesh->bind_material(mat);
    }
    basic_texture *mat;
    std::unique_ptr<torus_mesh> mesh;
    std::unique_ptr<texture2D> checkerboard_tex;
};

// LAYER ---------------------------------------------------------------------------------------------------------------
template <typename T> class sample_scene_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (auto renderer = app::renderer()) {
            // enable depth testing and face culling in this scene in case is was changed by another scene
            renderer->enable_depth_testing(true);
            renderer->enable_face_culling(true);
            // We need to clear the color and depth buffer so that the scene is properly rendered each frame
            renderer->clear_color_buffer({0.1f, 0.1f, 0.1f, 1.0f});
            renderer->clear_depth_buffer();
        }
    }
};
template <typename T> class sample_scene_physics_system : public physics_system<T> {
  public:
    void update(time_f dt, T &entity) const override {
        // this makes the camera orbit the origin
        static time_f t = time_f::seconds(0);
        t += dt / 5.f;
        static tensor<length_f, 3> pos{};
        pos[0] = length_f::meters(3.0f) * std::sin(t.as_seconds());
        pos[1] = length_f::meters(3.0f) * std::sin(t.as_seconds());
        pos[2] = length_f::meters(3.0f) * std::cos(t.as_seconds());
        entity.cam->set_position(pos);
        entity.cam->face_towards(origin, {0.f, 1.f, 0.f});
    }
    tensor<length_f, 3> origin{};
};
template <typename T> class sample_scene_controls_system : public controls_system<T> {
  public:
    virtual bool on_key(const key_event &event, T &entity) const override final {
        // When we press the 'T' key, toggle wireframe mode
        static bool enabled = false;
        if (event == key_event::T_DOWN) {
            enabled = !enabled;
        }
        app::renderer()->wireframe_mode(enabled);
        return true;
    }
};
// The scene that containes the object to be rendered
class sample_scene : public entity<sample_scene> {
  public:
    sample_scene(projection_type type, float aspect){
        cam = gen_object<camera>(type, aspect);
        // we create the material here and add all objects that will be rendered with that material
        auto mat = gen_object<basic_texture>(cam);
        mat->attach_object<sample_obj>(mat);
        // generate and attach the systems
        attach_render_system<sample_scene_render_system>();
        attach_physics_system<sample_scene_physics_system>();
        attach_controls_system<sample_scene_controls_system>();
    }
    void on_enter() override {}
    void on_exit() override {}
    camera *cam;
    basic_texture *mat;
};

// RENDERER ------------------------------------------------------------------------------------------------------------
class sample_scene_renderer : public sdl_gl_renderer {
  public:
    sample_scene_renderer() {
        // here we set the inital properties used the render the scene
        properties.window_title = "untitled";
        properties.window_width = 1280;
        properties.window_height = 720;
        properties.samples = 4;
        float init_aspect = float(properties.window_width) / float(properties.window_height);
        scene = gen_object<sample_scene>(projection_type::PERSPECTIVE, init_aspect);
    }
    void on_enter() override {
        // load the sample scene layer
        load_object(scene);
    }
    void on_exit() override {}
    // scene
    sample_scene* scene;
};

int main() {
    // first we need to initalize the renderer that will be used by the app
    sdl_gl_renderer::init();
    // next, we add the renderer to the app
    app::attach_renderer<sample_scene_renderer>();
    // and we run the app
    app::run();
    // once all renderers have been detached and the app run has ended, we clean up the renderer used by the app
    sdl_gl_renderer::quit();
    return 0;
}