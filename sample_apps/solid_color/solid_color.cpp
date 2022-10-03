/**
 * @file solid_color.cpp
 * @brief Sample app displaying solid colors in two windows
 * @details This app will create two windows each displaing solid colors that
 * are combinations of red and green. Each window can be controlled separately
 * with the arrow keys.
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "lit/sdl_gl.hpp"
#include <cmath>

using namespace lit;
using namespace squint;
using namespace squint::quantities;

// create concept for the entity that the systems can operate on.
// The systems require that the entity contain two colors (red and green).
template <typename T>
concept solid_color_like = requires(T t) {
    { t.r } -> std::same_as<float &>;
    { t.g } -> std::same_as<float &>;
};

// create the render system to update the color each frame
template <typename T>
requires solid_color_like<T>
class solid_color_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        app::instance().active_renderer()->clear_color_buffer({entity.r, entity.g, 0.f, 1.f});
    }
};

// create the controls system to update the color when arrow keys are pressed
// and close the window when the escape key is pressed
template <typename T>
requires solid_color_like<T>
class switch_color_controls_system : public controls_system<T> {
  public:
    bool on_key(const key_event &event, T &entity) const override {
        if (event == key_event::UP_DOWN) {
            entity.r = std::min(1.0f, entity.r + 0.1f);
            return true;
        } else if (event == key_event::DOWN_DOWN) {
            entity.r = std::max(0.0f, entity.r - 0.1f);
            return true;
        } else if (event == key_event::RIGHT_DOWN) {
            entity.g = std::min(1.0f, entity.g + 0.1f);
            return true;
        } else if (event == key_event::LEFT_DOWN) {
            entity.g = std::max(0.0f, entity.g - 0.1f);
            return true;
        } else if (event == key_event::ESCAPE_DOWN) {
            app::instance().active_renderer()->exit();
            return true;
        }
        return false;
    }
};

// create the layer that contains the colors and attach to it the systems
// defined above
class solid_color_layer : public entity<solid_color_layer> {
  public:
    solid_color_layer() {
        gen_render_system<solid_color_render_system>();
        gen_controls_system<switch_color_controls_system>();
    }
    void on_enter() override {}
    void on_exit() override {}
    // variables can be defined here in the class or inherited from components
    float r = 1.0f;
    float g = 1.0f;
};

// create the renderer and set the window properties used to render the layer.
// This attaches a single layer and activates it when the renderer is loaded
// (attached to the app)
class solid_color_renderer : public sdl_gl_renderer {
  public:
    solid_color_renderer() {
        properties.window_title = "untitled";
        properties.window_width = 1280;
        properties.window_height = 720;
        gen_object<solid_color_layer>();
    }
    void on_enter() override {
        // load the solid color layer
        load_object(children()[0].get());
    }
    void on_exit() override {}
};

int main() {
    // attach two renderers to the app. This will create two windows each with
    // their own controls and rendering systems.
    app::instance().gen_renderer<solid_color_renderer>();
    app::instance().gen_renderer<solid_color_renderer>();
    // run the app
    app::instance().run();
    return 0;
}