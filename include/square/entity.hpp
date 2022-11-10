#ifndef SQUARE_ENTITY
#define SQUARE_ENTITY
#include "square/system.hpp"
#include <algorithm>
#include <memory>
#include <vector>

namespace square {
// Abstract base class for objects in a renderer.
//
// Objects can have any number of child objects and up to one parent object. Typically the renderer is the root
// object in an application.
//
// Existing objects can be attached or detached to another object and become a child of the object or generated and
// attached at once. All child objects run their update() and render() functions for each frame when the application is
// running. The methods are called in the order the objects were added to the parent and the  parent method will always
// be called first. The event callbacks (on_key, on_mouse_button, etc.) are called when events occur. The child objects
// are iterated in reverse order and the child methods are called before the parent method. If any child method returns
// true, the propogation up the tree stops and the event is considered handeled.
//
// When an object is loaded, all child objects are loaded. on_enter() and on_exit() are called for each child object in
// the object tree when the parent object is loaded/unloaded by an application.
//
// An object can be disabled. If the object is disabled, it will not be updated (no callbacks will be executed) until it
// is enabled again. the enter() and load() callbacks will be called regard of disabled state
class object {
  public:
    object();
    void on_load();
    void on_unload();
    // attaching an object will add the object to the list of child object and allow for it to be destroyed
    template <typename U, typename... Args> void attach_object(Args... args) {
        child_objects.push_back(std::make_unique<U>(args...));
    }
    // generating an object will attach and object and also return a pointer to it.
    // objects generated with this method cannot be destroyed until the parent is destroyed.
    template <typename U, typename... Args> U *gen_object(Args... args) {
        auto obj = std::make_unique<U>(args...);
        obj->destructible = false;
        auto obj_ptr = obj.get();
        child_objects.push_back(std::move(obj));
        return obj_ptr;
    }
    virtual void update(squint::quantities::time_f dt);
    virtual void render(squint::quantities::time_f dt);
    virtual bool on_key(const key_event &event);
    virtual bool on_mouse_button(const mouse_button_event &event);
    virtual bool on_mouse_move(const mouse_move_event &event);
    virtual bool on_mouse_wheel(const mouse_scroll_event &event);
    virtual bool on_resize(const window_resize_event &event);
    virtual ~object();
    bool disabled;
    void destroy();
    inline bool should_destroy() const { return destroy_flag; }
    // remove child objects marked for destruction recursively
    void prune();

  protected:
    virtual void on_enter() {}
    virtual void on_exit() {}

  private:
    std::vector<std::unique_ptr<object>> child_objects{};
    bool destroy_flag;
    bool destructible;
};
// Abstract base class of an entity in a renderer.
//
// An entity is an 'object' that contains systems. Each attached system runs their render, update, and event
// callbacks depending on their type when the object runs those callbacks.
//
// All concrete entities that inherit from this abstract base class must use the CRTP template pattern. Systems attached
// to the entity must be templated so as to be compatible with the entity typically by using concepts.
template <typename T> class entity : public object {
  public:
    virtual void update(squint::quantities::time_f dt) override final {
        if (!disabled) {
            std::for_each(physics_systems.begin(), physics_systems.end(),
                          [this, dt](auto &ps) { ps->update(dt, static_cast<T &>(*this)); });
            object::update(dt);
        }
    }
    virtual void render(squint::quantities::time_f dt) override final {
        if (!disabled) {
            std::for_each(render_systems.begin(), render_systems.end(),
                          [this, dt](auto &rs) { rs->render(dt, static_cast<T &>(*this)); });
            object::render(dt);
        }
    }
    virtual bool on_key(const key_event &event) override final {
        if (disabled) {
            return false;
        }
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(),
                                   [this, &event](auto &cs) { return cs->on_key(event, static_cast<T &>(*this)); });
        if (!handled) {
            return object::on_key(event);
        }
        return handled;
    }
    virtual bool on_mouse_button(const mouse_button_event &event) override final {
        if (disabled) {
            return false;
        }
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(), [this, &event](auto &cs) {
            return cs->on_mouse_button(event, static_cast<T &>(*this));
        });
        if (!handled) {
            return object::on_mouse_button(event);
        }
        return handled;
    }
    virtual bool on_mouse_move(const mouse_move_event &event) override final {
        if (disabled) {
            return false;
        }
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(), [this, &event](auto &cs) {
            return cs->on_mouse_move(event, static_cast<T &>(*this));
        });
        if (!handled) {
            return object::on_mouse_move(event);
        }
        return handled;
    }
    virtual bool on_mouse_wheel(const mouse_scroll_event &event) override final {
        if (disabled) {
            return false;
        }
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(), [this, &event](auto &cs) {
            return cs->on_mouse_wheel(event, static_cast<T &>(*this));
        });
        if (!handled) {
            return object::on_mouse_wheel(event);
        }
        return handled;
    }
    virtual bool on_resize(const window_resize_event &event) override final {
        if (disabled) {
            return false;
        }
        bool handled = std::any_of(controls_systems.rbegin(), controls_systems.rend(),
                                   [this, &event](auto &cs) { return cs->on_resize(event, static_cast<T &>(*this)); });
        if (!handled) {
            return object::on_resize(event);
        }
        return handled;
    }
    template <template <class V> class U, typename... Args> void attach_controls_system(Args... args) {
        controls_systems.push_back(std::make_unique<U<T>>(args...));
    }
    template <template <class V> class U, typename... Args> void attach_physics_system(Args... args) {
        physics_systems.push_back(std::make_unique<U<T>>(args...));
    }
    template <template <class V> class U, typename... Args> void attach_render_system(Args... args) {
        render_systems.push_back(std::make_unique<U<T>>(args...));
    }

  private:
    // destructor is private and T is a friend of entity<T>. This enforces use of CTRP for this class
    friend T;
    virtual ~entity() {}
    std::vector<std::unique_ptr<controls_system<T>>> controls_systems{};
    std::vector<std::unique_ptr<physics_system<T>>> physics_systems{};
    std::vector<std::unique_ptr<render_system<T>>> render_systems{};
};
} // namespace square
#endif