#include "lit/renderer.hpp"
#include <GL/gl.h>
#include <cmath>

template <typename T> class solid_color_renderer : public lit::render_system<T> {
  public:
    void render(squint::quantities::time_f dt, T &entity) const override {
        lit::app::instance().active_renderer()->clear_color_buffer({entity.r, 0.f, 0.f, 1.f});
    }
};

template <typename T> class switch_color_controls : public lit::controls_system<T> {
  public:
    bool on_key(const lit::key_event &event, T &entity) const override {
        if (event == lit::key_event::UP_DOWN) {
            entity.r = std::min(1.0f, entity.r + 0.1f);
            return true;
        } else if (event == lit::key_event::DOWN_DOWN) {
            entity.r = std::max(0.0f, entity.r - 0.1f);
            return true;
        } else if (event == lit::key_event::ESCAPE_DOWN) {
            lit::app::instance().active_renderer()->exit();
            return true;
        }
        return false;
    }
};

class sandbox_layer : public lit::entity<sandbox_layer> {
  public:
    sandbox_layer() {
        gen_render_system<solid_color_renderer>();
        gen_controls_system<switch_color_controls>();
    }
    void on_enter() override {}
    void on_exit() override {}
    float r = 1.0f;
};

class sandbox : public lit::sdl_gl {
  public:
    sandbox() {
        window_title = "untitled";
        window_width = 1280;
        window_height = 720;
        gen_object<sandbox_layer>();
    }
    void on_enter() override { load_object(children()[0].get()); }
    void on_exit() override {}
};

int main() {
    using namespace lit;
    app::instance().gen_renderer<sandbox>();
    app::instance().gen_renderer<sandbox>();
    app::instance().run();
    return 0;
}