#include "square/entity.hpp"

namespace square {
object::object() : disabled(false), destroy_flag(false), destructible(true) {}
void object::destroy() {
    assert(destructible);
    destroy_flag = true;
}
void object::prune() {
    // first check if any children can be removed
    // if they can, call on_unload()
    for (auto &obj : child_objects) {
        if (obj->should_destroy()) {
            // recursively calls on_exit()
            obj->on_unload();
        }
    }
    std::erase_if(child_objects, [](std::unique_ptr<object> &obj) { return obj->should_destroy(); });
    // prune the remaining children
    for (auto &obj : child_objects) {
        obj->prune();
    }
}
void object::on_load() {
    on_enter();
    std::for_each(child_objects.begin(), child_objects.end(), [this](auto &obj) { obj->on_load(); });
}
void object::on_unload() {
    std::for_each(child_objects.begin(), child_objects.end(), [this](auto &obj) { obj->on_unload(); });
    on_exit();
};
void object::update(squint::quantities::time_f dt) {
    if (!disabled) {
        std::for_each(child_objects.begin(), child_objects.end(), [dt](auto &obj) { obj->update(dt); });
    }
}
void object::render(squint::quantities::time_f dt) {
    if (!disabled) {
        std::for_each(child_objects.begin(), child_objects.end(), [dt](auto &obj) { obj->render(dt); });
    }
}
bool object::on_key(const key_event &event) {
    if (disabled) {
        return false;
    }
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_key(event); });
}
bool object::on_mouse_button(const mouse_button_event &event) {
    if (disabled) {
        return false;
    }
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_mouse_button(event); });
}
bool object::on_mouse_move(const mouse_move_event &event) {
    if (disabled) {
        return false;
    }
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_mouse_move(event); });
}
bool object::on_mouse_wheel(const mouse_scroll_event &event) {
    if (disabled) {
        return false;
    }
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_mouse_wheel(event); });
}
bool object::on_resize(const window_resize_event &event) {
    if (disabled) {
        return false;
    }
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_resize(event); });
}
object::~object() {}
} // namespace square