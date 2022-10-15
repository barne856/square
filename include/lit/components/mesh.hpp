#ifndef LIT_MESH
#define LIT_MESH
#include "lit/components/transform.hpp"
#include "lit/entity.hpp"
#include "lit/renderer.hpp"

namespace lit {
/**
 * @brief A drawable mesh.
 *
 * @details A mesh contains a transform (or model matrix) along with a vertex buffer and possibly an index buffer. The
 * vertex buffer must be compatible with any shader bound to the mesh in order to be rendered successfully. This means
 * the buffer attributes must match the inputs to the vertex shader. If index_type is not NONE, an index buffer must be
 * provided to specify the order to render the vertices in. The draw methods are identical to OpenGL draw methods.
 *
 */
class mesh : public transform {
  public:
    mesh(draw_method method, index_type type = index_type::NONE) : method(method) {
        input_assembly = app::instance().active_renderer()->gen_vertex_input_assembly(type);
    }
    void bind_shader(shader *s) { input_assembly->bind_shader(s); }
    void set_index_buffer(std::unique_ptr<buffer> index_buffer) {
        this->input_assembly->set_index_buffer(std::move(index_buffer));
    }
    void add_vertex_buffer(std::unique_ptr<buffer> vertex_buffer) {
        this->input_assembly->add_vertex_buffer(std::move(vertex_buffer));
    }
    void draw() { app::instance().active_renderer()->draw_mesh(this, method); }
    inline const vertex_input_assembly *get_input_assembly() const { return input_assembly.get(); }

  private:
    std::unique_ptr<vertex_input_assembly> input_assembly;
    draw_method method;
};

} // namespace lit
#endif