module;
#include <concepts>
export module square:transform;
import :entity;
import squint;

using namespace squint::quantities;
using namespace squint;

export namespace square {
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
    transform(const squint::fmat4 &transform_matrix) : transformation_matrix(transform_matrix) {}
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

transform::transform() : transformation_matrix(fmat4::I()) {}
tensor<length_f, 3> transform::get_position() const { return transformation_matrix.at<3>(0, 3).copy_as<length_f>(); }
fvec3 transform::get_scale() const {
    fvec3 scale_vector{};
    scale_vector[0] = squint::norm(transformation_matrix.at<3>(0, 0));
    scale_vector[1] = squint::norm(transformation_matrix.at<3>(0, 1));
    scale_vector[2] = squint::norm(transformation_matrix.at<3>(0, 2));
    return scale_vector;
}
fmat4 transform::get_translation_matrix() const {
    return squint::translate(fmat4::I(), transformation_matrix.at<3>(0, 3));
}
fmat4 transform::get_rotation_matrix() const {
    fvec3 scale = get_scale();
    fmat4 rotation = fmat4::I();
    if (scale[0] != 0.0f) {
        rotation.at<3>(0, 0) = transformation_matrix.at<3>(0, 0) / scale[0];
    }
    if (scale[1] != 0.0f) {
        rotation.at<3>(0, 1) = transformation_matrix.at<3>(0, 1) / scale[1];
    }
    if (scale[2] != 0.0f) {
        rotation.at<3>(0, 2) = transformation_matrix.at<3>(0, 2) / scale[2];
    }
    return rotation;
}
fmat4 transform::get_scale_matrix() const { return scale(fmat4::I(), get_scale()); }
void transform::set_transformation_matrix(const fmat4 &transformation_matrix) {
    this->transformation_matrix = transformation_matrix;
}
fmat3 transform::get_normal_matrix() const { return inv(transformation_matrix.at<3, 3>(0, 0)).transpose(); }
fmat4 transform::get_view_matrix() const { return inv(transformation_matrix); }
void transform::face_towards(const tensor<length_f, 3> &point, const fvec3 &up) {
    auto view = squint::look_at(get_position().view_as<const float>(), point.view_as<const float>(), up.as_ref());
    transformation_matrix = inv(view) * get_scale_matrix();
}
void transform::translate(const tensor<length_f, 3> &offset) {
    transformation_matrix.at<3>(0, 3) += offset.view_as<const float>();
}
void transform::set_position(const tensor<length_f, 3> &position) {
    transformation_matrix.at<3>(0, 3) = position.view_as<const float>();
}
void transform::rotate(const fvec3 &axis, float angle) {
    fmat4 rotation = squint::rotate(fmat4::I(), angle, axis) * get_rotation_matrix();
    transformation_matrix = get_translation_matrix() * rotation * get_scale_matrix();
}
void transform::set_rotation(const fvec3 &axis, float angle) {
    fmat4 rotation = squint::rotate(fmat4::I(), angle, axis);
    transformation_matrix = get_translation_matrix() * rotation * get_scale_matrix();
}
void transform::set_rotation_matrix(const fmat4 &rotation_matrix) {
    transformation_matrix = get_translation_matrix() * rotation_matrix * get_scale_matrix();
}
void transform::set_scale(const fvec3 &scale) {
    fmat4 scale_matrix = squint::scale(fmat4::I(), scale);
    transformation_matrix = get_translation_matrix() * get_rotation_matrix() * scale_matrix;
}
fvec3 transform::get_forward_vector() const {
    auto rotation_matrix = get_rotation_matrix();
    return -rotation_matrix.at<3>(0, 2);
}
fvec3 transform::get_right_vector() const {
    auto rotation_matrix = get_rotation_matrix();
    return rotation_matrix.at<3>(0, 0);
}
fvec3 transform::get_up_vector() const {
    auto rotation_matrix = get_rotation_matrix();
    return rotation_matrix.at<3>(0, 1);
}

} // namespace square
