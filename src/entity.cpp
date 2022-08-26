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
void object::update(float dt) {
    std::for_each(child_objects.begin(), child_objects.end(), [dt](auto &obj) { obj->update(dt); });
}
void object::render(float dt) {
    std::for_each(child_objects.begin(), child_objects.end(), [dt](auto &obj) { obj->render(dt); });
}
bool object::on_key(const renderer_input &input) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&input](auto &obj) { return obj->on_key(input); });
}
bool object::on_mouse_button(const renderer_input &input) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&input](auto &obj) { return obj->on_mouse_button(input); });
}
bool object::on_mouse_move(const renderer_input &input) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&input](auto &obj) { return obj->on_mouse_move(input); });
}
bool object::on_mouse_wheel(const renderer_input &input) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&input](auto &obj) { return obj->on_mouse_wheel(input); });
}
bool object::on_resize(const renderer_input &input) {
    return std::any_of(child_objects.rbegin(), child_objects.rend(),
                       [&input](auto &obj) { return obj->on_resize(input); });
}
object::~object() {}
} // namespace lit