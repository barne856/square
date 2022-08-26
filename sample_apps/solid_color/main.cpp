#include "lit/renderer.hpp"
#include <GL/gl.h>
#include <cmath>

// TODO: how to load scene from in a scene, how to access api methods of renderer

template <typename T> class solid_color_renderer : public lit::render_system<T> {
  public:
    void render(float dt, T &entity) const override {
        glClearColor(std::pow(std::sin(dt), 2), 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

class sandbox_layer : public lit::entity<sandbox_layer> {
  public:
    sandbox_layer() { gen_render_system<solid_color_renderer<sandbox_layer>>(); }
    void on_enter() override {}
    void on_exit() override {}
};

class sandbox : public lit::gl {
  public:
    sandbox() {
        window_title = "untitled";
        window_width = 1280;
        window_height = 720;
        gen_object<sandbox_layer>();
    }
    void startup() override { load_object(children()[0].get()); }
    void shutdown() override {}
};

int main() {
    auto app = sandbox();
    app.run();
    return 0;
}