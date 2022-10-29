#ifndef SQUARE_TRANSFORM
#define SQUARE_TRANSFORM

#include "square/entity.hpp"
#include "squint/linalg.hpp"
#include "squint/quantity.hpp"
#include <concepts>

namespace square {
// This concept is used by templated systems (render, physics, and controls). All components should have a corresponding
// concept so that derived entities can be used with systems for that component.
template <typename T>
concept transformable = requires(T t) {
    { t.get_position() } -> std::same_as<squint::tensor<squint::quantities::length_f, 3>>;
    { t.get_scale() } -> std::same_as<squint::fvec3>;
    { t.get_translation_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_rotation_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_scale_matrix() } -> std::same_as<squint::fmat4>;
    { t.get_transformation_matrix() } -> std::same_as<const squint::fmat4 &>;
    { t.get_normal_matrix() } -> std::same_as<squint::fmat3>;
    { t.get_view_matrix() } -> std::same_as<squint::fmat4>;
    { t.face_towards(squint::tensor<squint::quantities::length_f, 3>(), squint::fvec3()) } -> std::same_as<void>;
    { t.translate(squint::tensor<squint::quantities::length_f, 3>()) } -> std::same_as<void>;
    { t.set_position(squint::tensor<squint::quantities::length_f, 3>()) } -> std::same_as<void>;
    { t.rotate(squint::fvec3(), 0.f) } -> std::same_as<void>;
    { t.set_rotation(squint::fvec3(), 0.f) } -> std::same_as<void>;
    { t.set_rotation_matrix(squint::fmat4()) } -> std::same_as<void>;
    { t.set_scale(squint::fvec3()) } -> std::same_as<void>;
    { t.get_forward_vector() } -> std::same_as<squint::fvec3>;
    { t.get_right_vector() } -> std::same_as<squint::fvec3>;
    { t.get_up_vector() } -> std::same_as<squint::fvec3>;
};

// A transform component provides the data and functions to transform an entity in space. The transform consists of a
// single fmat4 variable that is a TRS matrix or Translation * Rotation * Scale matrix. This means that the order of
// transformations applied is Scale then Rotate then Translate.
//
// Technically this matrix would have mixed dimension with the fourth column having dimension of length and the
// remaining columns being dimensionless, but internally it is stored as a dimensionless matrix.
class transform {
  public:
    transform();
    squint::tensor<squint::quantities::length_f, 3> get_position() const;
    squint::fvec3 get_scale() const;
    squint::fmat4 get_translation_matrix() const;
    squint::fmat4 get_rotation_matrix() const;
    squint::fmat4 get_scale_matrix() const;
    inline const squint::fmat4 &get_transformation_matrix() const { return transformation_matrix; };
    void set_transformation_matrix(const squint::fmat4 &transformation_matrix);
    squint::fmat3 get_normal_matrix() const;
    squint::fmat4 get_view_matrix() const;
    void face_towards(const squint::tensor<squint::quantities::length_f, 3> &point, const squint::fvec3 &up);
    void translate(const squint::tensor<squint::quantities::length_f, 3> &offset);
    void set_position(const squint::tensor<squint::quantities::length_f, 3> &position);
    void rotate(const squint::fvec3 &axis, float angle);
    void set_rotation(const squint::fvec3 &axis, float angle);
    void set_rotation_matrix(const squint::fmat4 &rotation_matrix);
    void set_scale(const squint::fvec3 &scale);
    squint::fvec3 get_forward_vector() const;
    squint::fvec3 get_right_vector() const;
    squint::fvec3 get_up_vector() const;

  private:
    squint::fmat4 transformation_matrix;
};

} // namespace square

#endif