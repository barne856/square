#ifndef LIT_CAMERA
#define LIT_CAMERA

#include "lit/components/transform.hpp"

namespace lit {
enum class projection_type { PERSPECTIVE, ORTHOGRAPHIC };

class camera : public transform {
  public:
    camera(projection_type type, float aspect) : type(type), aspect(aspect) { recalculate_projection(); }
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
    void set_type(projection_type type) {
        type = type;
        recalculate_projection();
    }
    void set_fovy(float fovy) {
        fovy = fovy;
        recalculate_projection();
    }
    void set_ortho_scale(float ortho_scale) {
        ortho_scale = ortho_scale;
        recalculate_projection();
    }
    void set_aspect(float aspect) {
        aspect = aspect;
        recalculate_projection();
    }
    void set_near_clip_plane_ortho(float near) {
        ortho_near = near;
        recalculate_projection();
    }
    void set_far_clip_plane_ortho(float far) {
        ortho_far = far;
        recalculate_projection();
    }
    void set_near_clip_plane_persp(float near) {
        persp_near = near;
        recalculate_projection();
    }
    void set_far_clip_plane_persp(float far) {
        persp_far = far;
        recalculate_projection();
    }
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
    float ortho_near = -1.0f; /**< The distance to the near clip plane of the
                                  Camera using projection_type::ORTHOGRAPHIC.*/
    float ortho_far = 1.0f;   /**< The distance to the far clip plane of the Camera
                     using projection_type::ORTHOGRAPHIC.*/
    float persp_near = 0.1f;  /**< The distance to the near clip plane of the
                    Camera using projection_type::PERSPECTIVE.*/
    float persp_far = 10.0f;  /**< The distance to the far clip plane of the
                    Camera using projection_type::PERSPECTIVE.*/

    projection_type type; /**< The projection_type of the Camera.*/
};
} // namespace lit
#endif