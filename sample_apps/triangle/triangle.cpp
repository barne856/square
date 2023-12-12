// A sample app displaying a solid color on one window to illustrate the
// use of the square module.
#include <memory>
import square;
import squint;

using namespace square;
using namespace squint;
using namespace squint::quantities;

template <typename T> class triangle_obj_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        auto mat = entity.mat;
        if (mat) {
            mat->set_color(entity.color);
            mat->set_model(entity.mesh.get());
            entity.mesh->draw(mat);
        }
    }
};

class triangle_obj : public entity<triangle_obj> {
  public:
    triangle_obj(basic_color *mat) : mat(mat) { attach_render_system<triangle_obj_render_system>(); }
    void on_enter() override {
        squint::fvec2 v1 = {-1.f, -1.f};
        squint::fvec2 v2 = {1.f, -1.f};
        squint::fvec2 v3 = {0.f, 1.f};
        // TODO make gen_component<>
        mesh = std::move(std::make_unique<triangle_mesh>(
            v1, v2, v3
        ));
        mesh->bind_material(mat);
    }
    basic_color *mat;
    std::unique_ptr<triangle_mesh> mesh;
    // color
    fvec4 color = color::parse_hexcode("E67825");
};

// this is the main render loop for the scene
template <typename T> class triangle_scene_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (auto renderer = app::renderer()) {
            // enable depth testing and face culling in this scene in case is was changed by another scene
            renderer->enable_depth_testing(true);
            renderer->enable_face_culling(true);
            // We need to clear the color and depth buffer so that the scene is properly rendered each frame
            renderer->clear_color_buffer(entity.bg_color);
            renderer->clear_depth_buffer();
        }
    }
};

// The scene that containes the object to be rendered
class main_scene : public entity<main_scene> {
  public:
    main_scene(projection_type type, float aspect){
        cam = gen_object<camera>(type, aspect);
        pos[2] = length_f::meters(3.0f);
        cam->set_position(pos);
        cam->face_towards(origin, up);
        // we create the material here and add all objects that will be rendered with that material
        auto mat = gen_object<basic_color>(cam);
        mat->attach_object<triangle_obj>(mat);
        // generate and attach the systems
        attach_render_system<triangle_scene_render_system>();
    }
    void on_enter() override {}
    void on_exit() override {}
    camera *cam;
    basic_texture *mat;
    fvec4 bg_color = color::parse_hexcode("000000"); // black
    tensor<length_f, 3> origin{};
    tensor<length_f, 3> pos{};
    fvec3 up{0.f, 1.f, 0.f};
};

// This is the main renderer for the app
// It sets up the window and rendering context and loads the main scene
class main_renderer : public sdl_gl_renderer {
  public:
    main_renderer() {
        // here we set the inital properties used the render the scene
        properties.window_title = "untitled";
        properties.window_width = 1280;
        properties.window_height = 720;
        float init_aspect = float(properties.window_width) / float(properties.window_height);
        scene = gen_object<main_scene>(projection_type::PERSPECTIVE, init_aspect);
    }
    void on_enter() override {
        // load the main scene, unloaded when the renderer is detached
        load_object(scene);
    }
    main_scene *scene;
};

int main() {
    // TODO, should init() and quit() be simplified? How to handle multiple windows?

    // initalize the renderer that will be used by the app.
    // this initalizes SDL2 
    sdl_gl_renderer::init();
    // add the renderer to the app
    // this will create the window and context, construct the renderer, and call on_enter
    app::attach_renderer<main_renderer>();
    // run the app
    // this will perform the main loop, updating the scenes via all attached systems
    // if the window is closed, the context will be destroyed, and on_exit will be called
    app::run();
    // once all renderers have been detached and the app run has ended, we clean up the renderer used by the app
    // This destroys SDL2
    sdl_gl_renderer::quit();
    return 0;
}