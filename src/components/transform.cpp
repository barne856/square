#include "lit/components/transform.hpp"

namespace lit {
transform::transform() : transformation_matrix(squint::fmat4::I()) {}
transform::transform(squint::fmat4 transform_matrix) : transformation_matrix(transform_matrix) {}
squint::fvec3 transform::get_position() const {
    return {transformation_matrix[0][3], transformation_matrix[1][3], transformation_matrix[2][3]};
}
squint::fvec3 transform::get_scale() const {
    squint::fvec3 scale_vector{};
    scale_vector[0] = squint::norm(transformation_matrix.at<3>(0, 0));
    scale_vector[1] = squint::norm(transformation_matrix.at<3>(0, 1));
    scale_vector[2] = squint::norm(transformation_matrix.at<3>(0, 2));
    return scale_vector;
}
squint::fmat4 transform::get_translation_matrix() const {
    return squint::translate(squint::fmat4::I(), transformation_matrix.at<3>(0, 3));
}
squint::fmat4 transform::get_rotation_matrix() const {
    squint::fvec3 scale = get_scale();
    squint::fmat4 rotation = squint::fmat4::I();
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
squint::fmat4 transform::get_scale_matrix() const { return squint::scale(squint::fmat4::I(), get_scale()); }
squint::fmat4 transform::get_transformation_matrix() { return transformation_matrix; }
void transform::set_transformation_matrix(squint::fmat4 transformation_matrix) {
    transformation_matrix = transformation_matrix;
}
squint::fmat3 transform::get_normal_matrix() const {
    return squint::inv(transformation_matrix.at<3, 3>(0, 0)).transpose();
}
squint::fmat4 transform::get_view_matrix() const { return view_matrix; } // squint::inv(transformation_matrix); }
void transform::face_towards(squint::fvec3 point, squint::fvec3 up) {
    squint::fmat4 view = squint::look_at(get_position(), point, up);
    view_matrix = view;
    transformation_matrix = squint::inv(view) * get_scale_matrix();
}
void transform::translate(squint::fvec3 offset) { transformation_matrix.at<3>(0, 3) += offset; }
void transform::set_position(squint::fvec3 position) { transformation_matrix.at<3>(0, 3) = position; }
void transform::rotate(squint::fvec3 axis, float angle) {
    squint::fmat4 rotation = squint::rotate(squint::fmat4::I(), angle, axis) * get_rotation_matrix();
    transformation_matrix = get_translation_matrix() * rotation * get_scale_matrix();
}
void transform::set_rotation(squint::fvec3 axis, float angle) {
    squint::fmat4 rotation = squint::rotate(squint::fmat4::I(), angle, axis);
    transformation_matrix = get_translation_matrix() * rotation * get_scale_matrix();
}
void transform::set_rotation_matrix(squint::fmat4 rotation_matrix) {
    transformation_matrix = get_translation_matrix() * rotation_matrix * get_scale_matrix();
}
void transform::set_scale(squint::fvec3 scale) {
    squint::fmat4 scale_matrix = squint::scale(squint::fmat4::I(), scale);
    transformation_matrix = get_translation_matrix() * get_rotation_matrix() * scale_matrix;
}
squint::fvec3 transform::get_forward_vector() const {
    auto rotation_matrix = get_rotation_matrix();
    return -rotation_matrix.at<3>(0, 2);
}
squint::fvec3 transform::get_right_vector() const {
    auto rotation_matrix = get_rotation_matrix();
    return rotation_matrix.at<3>(0, 0);
}
squint::fvec3 transform::get_up_vector() const {
    auto rotation_matrix = get_rotation_matrix();
    return rotation_matrix.at<3>(0, 1);
}
} // namespace lit