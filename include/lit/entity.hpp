#ifndef LIT_ENTITY
#define LIT_ENTITY
#include "lit/system.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace lit {
class object {
  public:
    object();
    void on_load();
    void on_unload();
    inline const std::vector<std::unique_ptr<object>> &children() { return child_objects; }
    inline object *parent() { return parent_object; }
    template <typename U, typename... Args> void gen_object(Args... args) {
        attach_object(std::make_unique<U>(args...));
    }
    std::unique_ptr<object> detach_object(size_t i);
    void attach_object(std::unique_ptr<object> obj);
    virtual void update(float dt);
    virtual void render(float dt);
    virtual bool on_key(const renderer_input &input);
    virtual bool on_mouse_button(const renderer_input &input);
    virtual bool on_mouse_move(const renderer_input &input);
    virtual bool on_mouse_wheel(const renderer_input &input);
    virtual bool on_resize(const renderer_input &input);
    virtual ~object();

  protected:
    virtual void on_enter() {}
    virtual void on_exit() {}

  private:
    object *parent_object;
    std::vector<std::unique_ptr<object>> child_objects{};
};
template <typename T>
// requires std::derived_from<T, entity<T>>
class entity : public object {
  public:
    virtual void update(float dt) override final {
        std::for_each(physics_systems.begin(), physics_systems.end(),
                      [this, dt](auto &ps) { ps->update(dt, static_cast<T &>(*this)); });
        object::update(dt);
    }
    virtual void render(float dt) override final {
        std::for_each(render_systems.begin(), render_systems.end(),
                      [this, dt](auto &rs) { rs->render(dt, static_cast<T &>(*this)); });
        object::render(dt);
    }
    virtual bool on_key(const renderer_input &input) override final {
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(),
                                   [this, &input](auto &cs) { return cs->on_key(input, static_cast<T &>(*this)); });
        if (!handled) {
            return object::on_key(input);
        }
        return handled;
    }
    virtual bool on_mouse_button(const renderer_input &input) override final {
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(), [this, &input](auto &cs) {
            return cs->on_mouse_button(input, static_cast<T &>(*this));
        });
        if (!handled) {
            return object::on_mouse_button(input);
        }
        return handled;
    }
    virtual bool on_mouse_move(const renderer_input &input) override final {
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(), [this, &input](auto &cs) {
            return cs->on_mouse_move(input, static_cast<T &>(*this));
        });
        if (!handled) {
            return object::on_mouse_move(input);
        }
        return handled;
    }
    virtual bool on_mouse_wheel(const renderer_input &input) override final {
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(), [this, &input](auto &cs) {
            return cs->on_mouse_wheel(input, static_cast<T &>(*this));
        });
        if (!handled) {
            return object::on_mouse_wheel(input);
        }
        return handled;
    }
    virtual bool on_resize(const renderer_input &input) override final {
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(),
                                   [this, &input](auto &cs) { return cs->on_resize(input, static_cast<T &>(*this)); });
        if (!handled) {
            return object::on_resize(input);
        }
        return handled;
    }
    template <typename U, typename... Args> void gen_controls_system(Args... args) {
        attach_controls_system(std::make_unique<U>(args...));
    }
    std::unique_ptr<object> detach_controls_system(size_t i) {
        auto cs = std::move(controls_systems[i]);
        controls_systems.erase(controls_systems.begin() + i);
        return cs;
    }
    void attach_controls_system(std::unique_ptr<controls_system<T>> cs) { controls_systems.push_back(std::move(cs)); }
    template <typename U, typename... Args> void gen_physics_system(Args... args) {
        attach_physics_system(std::make_unique<U>(args...));
    }
    std::unique_ptr<object> detach_physics_system(size_t i) {
        auto ps = std::move(physics_systems[i]);
        physics_systems.erase(physics_systems.begin() + i);
        return ps;
    }
    void attach_physics_system(std::unique_ptr<physics_system<T>> ps) { physics_systems.push_back(std::move(ps)); }
    template <typename U, typename... Args> void gen_render_system(Args... args) {
        attach_render_system(std::make_unique<U>(args...));
    }
    std::unique_ptr<object> detach_render_system(size_t i) {
        auto rs = std::move(render_systems[i]);
        render_systems.erase(render_systems.begin() + i);
        return rs;
    }
    void attach_render_system(std::unique_ptr<render_system<T>> rs) { render_systems.push_back(std::move(rs)); }

  private:
    // destructor is private and T is a friend of entity<T>. This enforces use of CTRP for this class
    friend T;
    virtual ~entity() {}
    std::vector<std::unique_ptr<controls_system<T>>> controls_systems{};
    std::vector<std::unique_ptr<physics_system<T>>> physics_systems{};
    std::vector<std::unique_ptr<render_system<T>>> render_systems{};
};
} // namespace lit
#endif