#include "lit/renderer.hpp"
#include <chrono>

namespace lit {

void renderer::run_step() {
    if (active_object) {
        activate_context();
        poll_events();
        static auto t1 = std::chrono::high_resolution_clock::now();
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1);
        squint::quantities::time_f dt{time_span.count()};
        t1 = t2;
        update(properties.fixed_dt);
        render(dt);
        swap_buffers();
    }
}
void renderer::update(squint::quantities::time_f dt) { active_object->update(dt); }
void renderer::render(squint::quantities::time_f dt) { active_object->render(dt); }
bool renderer::on_key(const key_event &event) { return active_object->on_key(event); }
bool renderer::on_mouse_button(const mouse_button_event &event) { return active_object->on_mouse_button(event); }
bool renderer::on_mouse_move(const mouse_move_event &event) { return active_object->on_mouse_move(event); }
bool renderer::on_mouse_wheel(const mouse_scroll_event &event) { return active_object->on_mouse_wheel(event); }
bool renderer::on_resize(const window_resize_event &event) { return active_object->on_resize(event); }
void renderer::load_object(object *obj) {
    if (active_object) {
        active_object->on_unload();
    }
    active_object = obj;
    if (active_object) {
        active_object->on_load();
    }
}

} // namespace lit