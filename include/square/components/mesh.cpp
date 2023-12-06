export module square:mesh;
import :transform;
import :material;
import :entity;
import :renderer;

export namespace square {
// Abstract base class for all mesh types. All meshes are drawable and can be bound to a shader
class mesh : public transform {
  public:
    virtual void bind_material(material *mat) = 0;
    virtual void draw(material *mat) = 0;
    virtual void draw(material *mat, const transform *parent) = 0;
    virtual ~mesh() {}
};
// A simple mesh contains a transform (or model matrix) along with a vertex buffer and possibly an index buffer. The
// vertex buffer must be compatible with any shader bound to the mesh in order to be rendered successfully. This means
// that any inputs to the vertex shader should be available as buffer attributes in the vertex buffer.
// If index_type is not NONE, an index buffer must be provided to specify the order to render the vertices in. The draw
// methods are identical to OpenGL draw methods.
class simple_mesh : public mesh {
  public:
    // construct a mesh of a certain draw_method and index_type. A vertex_input_assembly is used to manage the state of
    // the vertex inputs to a bound shader.
    simple_mesh(draw_method method, index_type type = index_type::NONE)
        : method(method), input_assembly(app::renderer()->gen_vertex_input_assembly(type)) {}
    // once a shader is bound, the input assembly is updated to bind the vertex attribs to the shader inputs.
    // The bound shader must be activated before draw() is called.
    virtual void bind_material(material *mat) override final { input_assembly->bind_shader(mat->get_shader()); }
    void set_index_buffer(std::unique_ptr<buffer> index_buffer) {
        this->input_assembly->set_index_buffer(std::move(index_buffer));
    }
    void add_vertex_buffer(std::unique_ptr<buffer> vertex_buffer) {
        this->input_assembly->add_vertex_buffer(std::move(vertex_buffer));
    }
    // Draws the mesh using the bound shader. The shader must be active and must be the shader that was bound.
    virtual void draw(material *mat) override final { app::renderer()->draw_mesh(this, this, mat); }
    // Draws the mesh using the bound shader and a parent transform. The shader must be active.
    virtual void draw(material *mat, const transform *parent) override final {
        transform model;
        model.set_transformation_matrix(parent->get_transformation_matrix() * this->get_transformation_matrix());
        app::renderer()->draw_mesh(this, &model, mat);
    }
    inline const vertex_input_assembly *get_input_assembly() const { return input_assembly.get(); }
    inline vertex_input_assembly *get_input_assembly() { return input_assembly.get(); }
    inline draw_method get_draw_method() const { return method; }

  private:
    std::unique_ptr<vertex_input_assembly> input_assembly;
    draw_method method;
};
// construct a mesh that is to be an instanced rendering of a simple mesh
class instanced_mesh : public mesh {
  public:
    // construct an instanced mesh with a maximum number of instances of a base mesh
    // instance count starts at zero and you must push and pop instances to change the instance count
    instanced_mesh(std::unique_ptr<simple_mesh> base_mesh, unsigned int max_instance_count)
        : base_mesh(std::move(base_mesh)), max_instance_count(max_instance_count), instance_count(0) {
        transforms = std::move(app::renderer()->gen_buffer(nullptr, sizeof(transform) * max_instance_count,
                                                           {{buffer_attribute_type::STORAGE, "models"}},
                                                           buffer_access_type::READ_WRITE));
    }
    // once a shader is bound, the input assembly is updated to bind the vertex attribs to the shader inputs.
    // The bound shader must be activated before draw() is called.
    virtual void bind_material(material *mat) override final {
        if (base_mesh) {
            base_mesh->get_input_assembly()->bind_shader(mat->get_shader());
        }
    }
    // Draws the mesh using the bound shader. The shader must be active and must be the shader that was bound.
    virtual void draw(material *mat) override final {
        transform model;
        model.set_transformation_matrix(this->get_transformation_matrix() * base_mesh->get_transformation_matrix());
        app::renderer()->draw_mesh(this, &model, mat, instance_count);
    }
    // Draws the mesh using the bound shader and a parent transform. The shader must be active.
    virtual void draw(material *mat, const transform *parent) override final {
        transform model;
        model.set_transformation_matrix(parent->get_transformation_matrix() * this->get_transformation_matrix() *
                                        base_mesh->get_transformation_matrix());
        app::renderer()->draw_mesh(this, &model, mat, instance_count);
    }
    inline const vertex_input_assembly *get_input_assembly() const {
        if (base_mesh) {
            return base_mesh->get_input_assembly();
        }
        return nullptr;
    }
    inline vertex_input_assembly *get_input_assembly() {
        if (base_mesh) {
            return base_mesh->get_input_assembly();
        }
        return nullptr;
    }
    inline draw_method get_draw_method() const {
        if (base_mesh) {
            return base_mesh->get_draw_method();
        }
        return draw_method::NONE;
    }
    void push_instance(const transform &model) {
        transforms->get<transform>(instance_count) = model;
        instance_count++;
    }
    void pop_instance() {
        if (instance_count > 0) {
            instance_count--;
        }
    }
    void clear_instances() { instance_count = 0; }
    inline const buffer *get_transform_buffer() const { return transforms.get(); }
    inline unsigned int get_instance_count() const { return instance_count; }
    inline transform &get_transform(size_t i) { return transforms->get<transform>(i); }
    inline const transform &get_transform(size_t i) const { return transforms->get<transform>(i); }

  private:
    std::unique_ptr<simple_mesh> base_mesh;
    unsigned int instance_count;
    unsigned int max_instance_count;
    std::unique_ptr<buffer> transforms;
};
// A composite mesh is a collection of abstract meshes that share a parent transform and material
class composite_mesh : public mesh {
  public:
    composite_mesh() {}
    inline std::vector<std::unique_ptr<mesh>> &get_meshes() { return meshes; }
    virtual void bind_material(material *mat) override final {
        for (auto &mesh : meshes) {
            mesh->bind_material(mat);
        }
    }
    virtual void draw(material *mat) override final {
        for (auto &mesh : meshes) {
            mesh->draw(mat, this);
        }
    }
    virtual void draw(material *mat, const transform *parent) override final {
        transform model;
        model.set_transformation_matrix(parent->get_transformation_matrix() * this->get_transformation_matrix());
        for (auto &mesh : meshes) {
            mesh->draw(mat, &model);
        }
    }

  private:
    std::vector<std::unique_ptr<mesh>> meshes;
};

} // namespace square
