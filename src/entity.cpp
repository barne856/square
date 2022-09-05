#include "lit/entity.hpp"

namespace lit {
object::object() : parent_object(nullptr) {}
void object::on_load() {
    on_enter();
    std::for_each(child_objects.begin(), child_objects.end(), [this](auto &obj) { obj->on_load(); });
}
void object::on_unload() {
    std::for_each(child_objects.begin(), child_objects.end(), [this](auto &obj) { obj->on_unload(); });
    on_exit();
};
std::unique_ptr<object> object::detach_object(size_t i) {
    auto obj = std::move(child_objects[i]);
    child_objects.erase(child_objects.begin() + i);
    obj->parent_object = nullptr;
    return obj;
}
void object::attach_object(std::unique_ptr<object> obj) {
    obj->parent_object = this;
    child_objects.push_back(std::move(obj));
}
void object::update(squint::quantities::time_f dt) {
    std::for_each(child_objects.begin(), child_objects.end(), [dt](auto &obj) { obj->update(dt); });
}
void object::render(squint::quantities::time_f dt) {
    std::for_each(child_objects.begin(), child_objects.end(), [dt](auto &obj) { obj->render(dt); });
}
bool object::on_key(const key_event &event) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_key(event); });
}
bool object::on_mouse_button(const mouse_button_event &event) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_mouse_button(event); });
}
bool object::on_mouse_move(const mouse_move_event &event) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_mouse_move(event); });
}
bool object::on_mouse_wheel(const mouse_scroll_event &event) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_mouse_wheel(event); });
}
bool object::on_resize(const window_resize_event &event) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&event](auto &obj) { return obj->on_resize(event); });
}
object::~object() {}
} // namespace lit