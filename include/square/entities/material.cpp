module;
#include <concepts>
#include <vector>
export module square:material;
import :camera;
import :entity;
import :renderer;
import :system;
import :transform;
import squint;

export namespace square {
// TODO: this can't be in mesh.cpp. Where should it go? Combine mesh and material into one file?
// Abstract base class for all mesh types. All meshes are drawable and can be bound to a shader
class mesh : public transform {
  public:
    virtual void bind_material(material *mat) = 0;
    virtual void draw(material *mat) = 0;
    virtual void draw(material *mat, const transform *parent) = 0;
    virtual ~mesh() {}
};
// concept for templated systems
template <typename T>
concept material_like = requires(T t) {
    { t.get_shader() } -> std::same_as<shader *>;
    { t.get_camera() } -> std::same_as<const camera *>;
    { t.get_meshes() } -> std::same_as<std::vector<std::unique_ptr<mesh>> &>;
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
                for (const auto &mesh : mat.get_meshes()) {
                    mesh->draw(&mat);
                }
            }
        }
    }
};

// A material is an entitiy containing child entities that should be rendered with the same material.
class material : public entity<material> {
  public:
    material(const camera *cam) : cam(cam) { attach_render_system<material_render_system>(); }
    inline shader *get_shader() { return material_shader.get(); }
    inline const camera *get_camera() const { return cam; }
    void set_model(const transform *model) {
        if (material_shader) {
            material_shader->upload_mat4("model", model->get_transformation_matrix());
        }
    }
    inline std::vector<std::unique_ptr<mesh>> &get_meshes() { return meshes; }

  protected:
    const camera *cam;
    std::unique_ptr<shader> material_shader;
    std::vector<std::unique_ptr<mesh>> meshes;
};
} // namespace square
