#ifndef LIT_TRANSFORM
#define LIT_TRANSFORM

#include "squint/linalg.hpp"

namespace lit {
/**
 * @brief A transform component that provides the data and functions to
 * transform an entity in space.
 * @details The transform consists of a single fmat4 variable that is a TRS matrix or Translation * Rotation *
 * Scale matrix. This means that the order of transformations applied is Scale
 * then Rotate then Translate.
 */
class transform {
  public:
    transform() : transformation_matrix(squint::fmat4::I()) {}
    transform(squint::fmat4 transform_matrix) : transformation_matrix(transform_matrix) {}
    squint::fvec3 get_position() const {
        return {transformation_matrix[3][0], transformation_matrix[3][1], transformation_matrix[3][2]};
    }
    squint::fvec3 get_scale() const {
        squint::fvec3 scale_vector{};
        scale_vector[0] = squint::norm(transformation_matrix[0].transpose());
        scale_vector[1] = squint::norm(transformation_matrix[1].transpose());
        scale_vector[2] = squint::norm(transformation_matrix[2].transpose());
        return scale_vector;
    }
    squint::fmat4 get_translation_matrix() const {
        return squint::translate(squint::fmat4::I(), transformation_matrix.at<3>(0, 3));
    }
    squint::fmat4 get_rotation_matrix() const {
        squint::fvec3 scale = get_scale();
        squint::fmat4 rotation = squint::fmat4::I();
        if (scale[0] != 0.0f) {
            rotation[0][0] = transformation_matrix[0][0] / scale[0];
            rotation[0][1] = transformation_matrix[0][1] / scale[0];
            rotation[0][2] = transformation_matrix[0][2] / scale[0];
        }
        if (scale[1] != 0.0f) {
            rotation[1][0] = transformation_matrix[1][0] / scale[1];
            rotation[1][1] = transformation_matrix[1][1] / scale[1];
            rotation[1][2] = transformation_matrix[1][2] / scale[1];
        }
        if (scale[2] != 0.0f) {
            rotation[2][0] = transformation_matrix[2][0] / scale[2];
            rotation[2][1] = transformation_matrix[2][1] / scale[2];
            rotation[2][2] = transformation_matrix[2][2] / scale[2];
        }
        return rotation;
    }
    squint::fmat4 get_scale_matrix() const { return squint::scale(squint::fmat4::I(), get_scale()); }
    squint::fmat4 get_transformation_matrix() { return transformation_matrix; }
    void set_transformation_matrix(squint::fmat4 transformation_matrix) {
        transformation_matrix = transformation_matrix;
    }
    squint::fmat3 get_normal_matrix() const {
        squint::fmat3 A{};
        A[0][0] = transformation_matrix[0][0];
        A[1][0] = transformation_matrix[1][0];
        A[2][0] = transformation_matrix[2][0];
        A[0][1] = transformation_matrix[0][1];
        A[1][1] = transformation_matrix[1][1];
        A[2][1] = transformation_matrix[2][1];
        A[0][2] = transformation_matrix[0][2];
        A[1][2] = transformation_matrix[1][2];
        A[2][2] = transformation_matrix[2][2];
        return squint::inv(A).transpose();
    }
    squint::fmat4 get_view_matrix() const { return squint::inv(transformation_matrix); }
    void face_towards(squint::fvec3 point, squint::fvec3 up) {
        squint::fmat4 view = squint::look_at(get_position(), point, up);
        transformation_matrix = squint::inv(view) * get_scale_matrix();
    }
    void translate(squint::fvec3 offset) {
        transformation_matrix[3][0] += offset[0];
        transformation_matrix[3][1] += offset[1];
        transformation_matrix[3][2] += offset[2];
    }
    void set_position(squint::fvec3 position) {
        transformation_matrix[3][0] = position[0];
        transformation_matrix[3][1] = position[1];
        transformation_matrix[3][2] = position[2];
    }
    void rotate(squint::fvec3 axis, float angle) {
        squint::fmat4 rotation = squint::rotate(squint::fmat4::I(), angle, axis) * get_rotation_matrix();
        transformation_matrix = get_translation_matrix() * rotation * get_scale_matrix();
    }
    void set_rotation(squint::fvec3 axis, float angle) {
        squint::fmat4 rotation = squint::rotate(squint::fmat4::I(), angle, axis);
        transformation_matrix = get_translation_matrix() * rotation * get_scale_matrix();
    }
    void set_rotation_matrix(squint::fmat4 rotation_matrix) {
        transformation_matrix = get_translation_matrix() * rotation_matrix * get_scale_matrix();
    }
    void set_scale(squint::fvec3 scale) {
        squint::fmat4 scale_matrix = squint::scale(squint::fmat4::I(), scale);
        transformation_matrix = get_translation_matrix() * get_rotation_matrix() * scale_matrix;
    }
    squint::fvec3 get_forward_vector() const {
        auto rotation_matrix = get_rotation_matrix();
        return {-rotation_matrix[2][0], -rotation_matrix[2][1], -rotation_matrix[2][2]};
    }
    squint::fvec3 get_right_vector() const {
        auto rotation_matrix = get_rotation_matrix();
        return {rotation_matrix[0][0], rotation_matrix[0][1], rotation_matrix[0][2]};
    }
    squint::fvec3 get_up_vector() const {
        auto rotation_matrix = get_rotation_matrix();
        return {rotation_matrix[1][0], rotation_matrix[1][1], rotation_matrix[1][2]};
    }

  private:
    squint::fmat4 transformation_matrix;
};

} // namespace lit

#endif