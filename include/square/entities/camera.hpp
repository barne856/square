#ifndef SQUARE_CAMERA
#define SQUARE_CAMERA

#include "square/components/transform.hpp"
#include "square/entity.hpp"
#include "square/renderer.hpp"
#include <concepts>

namespace square {
enum class projection_type { PERSPECTIVE, ORTHOGRAPHIC };
// concept for templated systems
template <typename T>
concept projectable = requires(T t) {
    requires transformable<T>;
    { t.recalculate_projection() } -> std::same_as<void>;
    { t.set_type(projection_type::ORTHOGRAPHIC) } -> std::same_as<void>;
    { t.set_fovy(0.f) } -> std::same_as<void>;
    { t.set_ortho_scale(0.f) } -> std::same_as<void>;
    { t.set_aspect(1.0f) } -> std::same_as<void>;
    { t.set_near_clip_plane_ortho(1.0f) } -> std::same_as<void>;
    { t.set_far_clip_plane_ortho(1.0f) } -> std::same_as<void>;
    { t.set_near_clip_plane_persp(1.0f) } -> std::same_as<void>;
    { t.set_far_clip_plane_persp(1.0f) } -> std::same_as<void>;
    { t.get_type() } -> std::same_as<projection_type>;
    { t.get_projection_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_fovy() } -> std::same_as<float>;
    { t.get_ortho_scale() } -> std::same_as<float>;
    { t.get_aspect() } -> std::same_as<float>;
    { t.get_near_clip_plane_ortho() } -> std::same_as<float>;
    { t.get_far_clip_plane_ortho() } -> std::same_as<float>;
    { t.get_near_clip_plane_persp() } -> std::same_as<float>;
    { t.get_far_clip_plane_persp() } -> std::same_as<float>;
};
// Controller that handles updating the camera when the window resizes.
template <projectable T> class camera_resize_controller : public controls_system<T> {
  public:
    virtual bool on_resize(const window_resize_event &event, T &cam) const override final {
        app::instance().active_renderer()->set_viewport(0, 0, event.width, event.height);
        float aspect = float(event.width) / float(event.height);
        cam.set_aspect(aspect);
        cam.recalculate_projection();
        return true;
    }
};
// This is the camera entity used for rendering scenes. All scenes that render objects will contain at least one camera
// entity. Multiple cameras can be used on a single scene. For example, a single scene may render 3D world objects using
// a perspective projection and overlay UI elements rendered with another camera using an orthographic projection.
//
// This class provides some methods to modify and update the camera. You must call 'recalculate_projection()' after you
// update the camera for the changes to take effect.
//
// The clip planes are to be negative if they are behind the viewer and positive if they are in front of the viewer.
// Perspective projection clip planes must be positive. fovy is the vertical field of view if type is PERSPECTIVE and is
// in radians. ortho_scale is the scale of the camera view if type is ORTHOGRAPHIC and is measured vertically from top
// to center (full vertical height is 2*ortho_scale and full horizontal width is 2*ortho_scale*aspect).
class camera : public entity<camera>, public transform {
  public:
    camera(projection_type type, float aspect) : type(type), aspect(aspect) {
        gen_controls_system<camera_resize_controller>();
        recalculate_projection();
    }
    void recalculate_projection() {
        switch (type) {
        case projection_type::PERSPECTIVE:
            projection = squint::perspective(fovy, aspect, persp_near, persp_far);
            break;
        case projection_type::ORTHOGRAPHIC:
            projection = squint::ortho(-ortho_scale * aspect, ortho_scale * aspect, -ortho_scale, ortho_scale,
                                       ortho_near, ortho_far);
            break;
        }
    }
    inline void set_type(projection_type type) { this->type = type; }
    inline void set_fovy(float fovy) { this->fovy = fovy; }
    inline void set_ortho_scale(float ortho_scale) { this->ortho_scale = ortho_scale; }
    inline void set_aspect(float aspect) { this->aspect = aspect; }
    inline void set_near_clip_plane_ortho(float near) { this->ortho_near = near; }
    inline void set_far_clip_plane_ortho(float far) { this->ortho_far = far; }
    inline void set_near_clip_plane_persp(float near) { this->persp_near = near; }
    inline void set_far_clip_plane_persp(float far) { this->persp_far = far; }
    inline projection_type get_type() const { return type; }
    inline squint::fmat4 get_projection_matrix() const { return projection; }
    inline float get_fovy() const { return fovy; }
    inline float get_ortho_scale() const { return ortho_scale; }
    inline float get_aspect() const { return aspect; }
    inline float get_near_clip_plane_ortho() const { return ortho_near; }
    inline float get_far_clip_plane_ortho() const { return ortho_far; }
    inline float get_near_clip_plane_persp() const { return persp_near; }
    inline float get_far_clip_plane_persp() const { return persp_far; }

  private:
    squint::fmat4 projection;
    projection_type type;
    // default values for the projections
    float fovy = M_PI_4f;
    float ortho_scale = 1.0f;
    float aspect = 1.0f;
    float ortho_near = -1.0f;
    float ortho_far = 1.0f;
    float persp_near = 0.1f;
    float persp_far = 10.0f;
};
} // namespace square
#endif