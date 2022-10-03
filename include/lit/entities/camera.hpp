#ifndef LIT_CAMERA
#define LIT_CAMERA

#include "lit/components/transform.hpp"
#include "lit/entity.hpp"
#include "lit/renderer.hpp"
#include <concepts>

namespace lit {
enum class projection_type { PERSPECTIVE, ORTHOGRAPHIC };
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
    void set_type(projection_type type) { this->type = type; }
    void set_fovy(float fovy) { this->fovy = fovy; }
    void set_ortho_scale(float ortho_scale) { this->ortho_scale = ortho_scale; }
    void set_aspect(float aspect) { this->aspect = aspect; }
    void set_near_clip_plane_ortho(float near) { this->ortho_near = near; }
    void set_far_clip_plane_ortho(float far) { this->ortho_far = far; }
    void set_near_clip_plane_persp(float near) { this->persp_near = near; }
    void set_far_clip_plane_persp(float far) { this->persp_far = far; }
    projection_type get_type() const { return type; }
    squint::fmat4 get_projection_matrix() const { return projection; }
    float get_fovy() const { return fovy; }
    float get_ortho_scale() const { return ortho_scale; }
    float get_aspect() const { return aspect; }
    float get_near_clip_plane_ortho() const { return ortho_near; }
    float get_far_clip_plane_ortho() const { return ortho_far; }
    float get_near_clip_plane_persp() const { return persp_near; }
    float get_far_clip_plane_persp() const { return persp_far; }

  private:
    squint::fmat4 projection; /**< The projection matrix of the camera.*/
    float fovy = M_PI_4f;     /**< The vertical field of view of the Camera with type
                                projection_type::PERSPECTIVE.*/
    float ortho_scale = 1.0f; /**< The scale of the Camera's view with type
                                  projection_type::ORTHOGRAPHIC.*/
    float aspect = 1.0f;      /**< The aspect of the Camera's view.*/
    float ortho_near =
        -1.0f;               /**< The distance to the near clip plane of the
                                 Camera using projection_type::ORTHOGRAPHIC. To be negative if plane is behind the viewer*/
    float ortho_far = 1.0f;  /**< The distance to the far clip plane of the Camera
                    using projection_type::ORTHOGRAPHIC.*/
    float persp_near = 0.1f; /**< The distance to the near clip plane of the
                   Camera using projection_type::PERSPECTIVE.*/
    float persp_far = 10.0f; /**< The distance to the far clip plane of the
                   Camera using projection_type::PERSPECTIVE.*/

    projection_type type; /**< The projection_type of the Camera.*/
};
} // namespace lit
#endif