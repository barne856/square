// A sample app displaying solid colors in two windows.
// This app will create two windows each displaing solid colors that
// are combinations of red and green. Each window can be controlled separately
// with the arrow keys.
import square;
import squint;
#include <cmath>
#include <concepts>
#include <algorithm>

using namespace square;
using namespace squint;
using namespace squint::quantities;

template <typename T> class render_main : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        // enable depth testing and face culling in this scene in case is was changed by another scene
        app::renderer()->enable_depth_testing(true);
        app::renderer()->enable_face_culling(true);
        // We need to clear the color and depth buffer so that the scene is properly rendered each frame
        app::renderer()->clear_color_buffer(entity.bg_color);
        app::renderer()->clear_depth_buffer();
    }
};

class main_scene : public entity<main_scene> {
  public:
    main_scene(projection_type type, float aspect) {
        // the scene's main camera is created
        cam = gen_object<camera>(type, aspect);
        // we create the material here and attach all objects that will be rendered with that material
        mat = gen_object<basic_color>(cam);
        // generate and attach the systems
        attach_render_system<render_main>();
    }
    void on_enter() override {
        position[2] = length_f::meters(5.0f);
        cam->set_position(position);
        cam->face_towards(origin, {0.0f, 1.0f, 0.0f});
    }
    tensor<length_f, 3> origin{};
    tensor<length_f, 3> position{};
    camera *cam;
    basic_color *mat;
    fvec4 bg_color = color::parse_hexcode("FEFBF3");
};

class main_renderer : public sdl_gl_renderer {
  public:
    main_renderer() {
        // here we set the inital properties used the render the scene
        properties.window_title = "untitled";
        properties.window_width = 1280;
        properties.window_height = 720;
        properties.samples = 4;
        float init_aspect = float(properties.window_width) / float(properties.window_height);
        scene = gen_object<main_scene>(projection_type::PERSPECTIVE, init_aspect);
        // std::srand(std::time(nullptr));
        std::srand(0); // make random numbers reproducable
    }
    void on_enter() override {
        // load the sample scene layer
        load_object(scene);
    }
    void on_exit() override { load_object(nullptr); }
    main_scene *scene;
};

int main() {
    // first we need to initalize the renderer that will be used by the app
    sdl_gl_renderer::init();
    // next, we add the renderer to the app
    app::attach_renderer<main_renderer>();
    // and we run the app
    app::run();
    // once all renderers have been detached and the app run has ended, we clean up the renderer used by the app
    sdl_gl_renderer::quit();
    return 0;
}