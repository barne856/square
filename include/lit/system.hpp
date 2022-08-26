#ifndef LIT_SYSTEM
#define LIT_SYSTEM
namespace lit {

struct renderer_input;

template <typename T> class render_system {
  public:
    virtual void render(float dt, T &entity) const {}
    virtual ~render_system() {}
};
template <typename T> class physics_system {
  public:
    virtual void update(float dt, T &entity) const {}
    virtual ~physics_system() {}
};
template <typename T> class controls_system {
  public:
    virtual bool on_key(const renderer_input &input, T &entity) const { return false; }
    virtual bool on_mouse_button(const renderer_input &input, T &entity) const { return false; }
    virtual bool on_mouse_move(const renderer_input &input, T &entity) const { return false; }
    virtual bool on_mouse_wheel(const renderer_input &input, T &entity) const { return false; }
    virtual bool on_resize(const renderer_input &input, T &entity) const { return false; }
    virtual ~controls_system() {}
};

} // namespace lit
#endif