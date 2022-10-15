#ifndef LIT_TRANSFORM
#define LIT_TRANSFORM

#include "lit/entity.hpp"
#include "squint/linalg.hpp"
#include <concepts>

namespace lit {
/**
 * @brief Concept for the transform component.
 *
 * @details Used as concept for render_systems, physics_systems, and controls_systems templates.
 */
template <typename T>
concept transformable = requires(T t) {
    { t.get_position() } -> std::same_as<squint::fvec3>;
    { t.get_scale() } -> std::same_as<squint::fvec3>;
    { t.get_translation_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_rotation_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_scale_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_transformation_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_normal_matrix() } -> std::same_as<squint::fmat3>;
    { t.get_view_matrix() } -> std::same_as<squint::fmat4>;
    { t.face_towards(squint::fvec3(0.f), squint::fvec3(0.f)) } -> std::same_as<void>;
    { t.translate(squint::fvec3(0.f)) } -> std::same_as<void>;
    { t.set_position(squint::fvec3(0.f)) } -> std::same_as<void>;
    { t.rotate(squint::fvec3(0.f), 0.f) } -> std::same_as<void>;
    { t.set_rotation(squint::fvec3(0.f), 0.f) } -> std::same_as<void>;
    { t.set_rotation_matrix(squint::fmat4(0.f)) } -> std::same_as<void>;
    { t.set_scale(squint::fvec3(0.f)) } -> std::same_as<void>;
    { t.get_forward_vector() } -> std::same_as<squint::fvec3>;
    { t.get_right_vector() } -> std::same_as<squint::fvec3>;
    { t.get_up_vector() } -> std::same_as<squint::fvec3>;
};
/**
 * @brief A transform component that provides the data and functions to
 * transform an entity in space.
 * @details The transform consists of a single fmat4 variable that is a TRS matrix or Translation * Rotation *
 * Scale matrix. This means that the order of transformations applied is Scale
 * then Rotate then Translate.
 *
 * This class provides some methods to modify and update a transform matrix.
 */
class transform {
  public:
    transform();
    transform(squint::fmat4 transform_matrix);
    squint::fvec3 get_position() const;
    squint::fvec3 get_scale() const;
    squint::fmat4 get_translation_matrix() const;
    squint::fmat4 get_rotation_matrix() const;
    squint::fmat4 get_scale_matrix() const;
    squint::fmat4 get_transformation_matrix();
    void set_transformation_matrix(squint::fmat4 transformation_matrix);
    squint::fmat3 get_normal_matrix() const;
    squint::fmat4 get_view_matrix() const;
    void face_towards(squint::fvec3 point, squint::fvec3 up);
    void translate(squint::fvec3 offset);
    void set_position(squint::fvec3 position);
    void rotate(squint::fvec3 axis, float angle);
    void set_rotation(squint::fvec3 axis, float angle);
    void set_rotation_matrix(squint::fmat4 rotation_matrix);
    void set_scale(squint::fvec3 scale);
    squint::fvec3 get_forward_vector() const;
    squint::fvec3 get_right_vector() const;
    squint::fvec3 get_up_vector() const;

  private:
    squint::fmat4 transformation_matrix;
    squint::fmat4 view_matrix;
};

} // namespace lit

#endif