#ifndef SQUARE_MATERIAL
#define SQUARE_MATERIAL

#include "square/entities/camera.hpp"
#include "square/entity.hpp"
#include "square/renderer.hpp"
#include "square/system.hpp"

namespace square {
// concept for templated systems
template <typename T>
concept material_like = requires(T t) {
    { t.get_shader() } -> std::same_as<shader *>;
    { t.get_camera() } -> std::same_as<const camera *>;
};
// this render system will activate the shader and upload the view and projection matricies for use in renderable child
// objects
template <material_like T> class material_render_system : public render_system<T> {
  public:
    void render(squint::quantities::time_f dt, T &mat) const override {
        if (mat.get_camera()) {
            if (auto s = mat.get_shader()) {
                s->activate();
                s->upload_mat4("projection", mat.get_camera()->get_projection_matrix());
                s->upload_mat4("view", mat.get_camera()->get_view_matrix());
            }
        }
    }
};
// A material is an entitiy containing child entities that should be rendered with the same material.
class material : public entity<material> {
  public:
    material(const camera *cam) : cam(cam) { gen_render_system<material_render_system>(); }
    inline shader *get_shader() { return material_shader.get(); }
    inline const camera *get_camera() const { return cam; }
    void set_model(const transform *model) {
        if (material_shader) {
            material_shader->upload_mat4("model", model->get_transformation_matrix());
        }
    }

  protected:
    const camera *cam;
    std::unique_ptr<shader> material_shader;
};
} // namespace square
#endif