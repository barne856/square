#ifndef LIT_RENDERER
#define LIT_RENDERER
#include "lit/entity.hpp"
#include <GL/gl.h>
#include <SDL.h>

namespace lit {
enum class cursor_type {
    DISABLED,
    ENABLED,
    ARROW,
    HZ_RESIZE,
    HAND,
    CROSSHAIRS,
};
enum class debug_mode {
    OFF,
    HIGH,
    MED,
    LOW,
    NOTIFICATION,
};
class app;
class sdl_gl;
class renderer : public object {
    friend class app;
    friend class sdl_gl;

  public:
    inline void exit() { running = false; }
    // api commands
    virtual void clear_color_buffer(squint::fvec4 color) = 0;

  private:
    void run_step();
    // create the context will be final in renderer impl
    virtual void create_context() = 0;
    virtual void poll_events() = 0;
    // cleanup the context
    virtual void destroy_context() = 0;
    virtual void activate_context() = 0;
    virtual void swap_buffers() = 0;
    virtual void update(squint::quantities::time_f dt) override final;
    virtual void render(squint::quantities::time_f dt) override final;
    virtual bool on_key(const key_event &event) override final;
    virtual bool on_mouse_button(const mouse_button_event &event) override final;
    virtual bool on_mouse_move(const mouse_move_event &event) override final;
    virtual bool on_mouse_wheel(const mouse_scroll_event &event) override final;
    virtual bool on_resize(const window_resize_event &event) override final;
    object *active_object = nullptr;

  protected:
    // load the root object
    void load_object(object *obj);
    const char *window_title = "untitled";
    int window_width = 1280;
    int window_height = 720;
    int samples = 0;
    bool wireframe = false;
    bool fullscreen = false;
    bool vsync = false;
    cursor_type cursor = cursor_type::ENABLED;
    debug_mode debug = debug_mode::NOTIFICATION;
    bool running = true;
    squint::quantities::time_f fixed_dt{1.f / 60.f};
};
class app {
  private:
    app() : active_renderer_ptr(nullptr) {}
    std::vector<std::unique_ptr<renderer>> renderers{};
    renderer *active_renderer_ptr;

  public:
    app(const app &) = delete;
    app(app &&) = delete;
    app &operator=(const app &) = delete;
    app &operator=(app &&) = delete;
    static app &instance() {
        static app INSTANCE;
        return INSTANCE;
    }
    inline renderer *active_renderer() { return active_renderer_ptr; }
    inline const std::vector<std::unique_ptr<renderer>> &get_renderers() { return renderers; }
    template <typename U, typename... Args> void gen_renderer(Args... args) {
        attach_renderer(std::make_unique<U>(args...));
    }
    std::unique_ptr<renderer> detach_renderer(size_t i);
    void attach_renderer(std::unique_ptr<renderer> obj);
    void run();
};
class sdl_gl : public renderer {
  public:
    virtual void clear_color_buffer(squint::fvec4 color);

  private:
    virtual void create_context() override final;
    virtual void poll_events() override final;
    virtual void destroy_context() override final;
    virtual void activate_context() override final;
    virtual void swap_buffers() override final;
    SDL_GLContext glcontext;
    SDL_Window *window;
    unsigned int window_id;
};
} // namespace lit
#endif