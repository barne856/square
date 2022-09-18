#ifndef LIT_RENDERER
#define LIT_RENDERER
#include "lit/entity.hpp"
#include <filesystem>
#include <memory>

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

struct renderer_properties {
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
class app;
class shader;
class renderer : public object {
    friend class app;

  public:
    inline void exit() { properties.running = false; }
    // api commands
    virtual void clear_color_buffer(squint::fvec4 color) = 0;
    virtual std::unique_ptr<shader> gen_shader(const std::filesystem::path &shader_src_directory) = 0;
    inline const renderer_properties &get_properties() const { return properties; }

  private:
    void run_step();
    object *active_object = nullptr;

  protected:
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

    // load the root object
    void load_object(object *obj);
    renderer_properties properties{};
};
class app {
  private:
    app() : active_renderer_ptr(nullptr) {}
    std::vector<std::unique_ptr<renderer>> renderers{};
    renderer *active_renderer_ptr;

  public:
    // this is a singleton class so it should never be copied or moved
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
class shader {
  public:
    virtual void activate() = 0;
    virtual ~shader() {}
    virtual void upload_mat4(const char *name, squint::fmat4 value, bool suppress_warnings = false) = 0;
    virtual void upload_vec4(const char *name, squint::fvec4 value, bool suppress_warnings = false) = 0;
};
} // namespace lit
#endif