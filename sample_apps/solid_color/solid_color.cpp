// A sample app displaying a solid color on one window to illustrate the .
import square;
import squint;

using namespace square;
using namespace squint;
using namespace squint::quantities;

// this is the main render loop for the scene
template <typename T> class render_main : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        app::renderer()->clear_color_buffer(entity.bg_color);
    }
};

// This is the main scene for the app
// It is a simple entity that has a background color
class main_scene : public entity<main_scene> {
  public:
    main_scene() {
        attach_render_system<render_main>();
    }
    fvec4 bg_color = color::parse_hexcode("E67825");
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
        scene = gen_object<main_scene>();
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