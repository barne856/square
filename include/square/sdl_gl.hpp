#ifndef SDL_GL_RENDERER
#define SDL_GL_RENDERER
#include "square/renderer.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstring>
#include <unordered_map>

namespace square {

class sdl_gl_renderer : public renderer {

  public:
    static void init();
    static void quit();
    virtual void clear_color_buffer(squint::fvec4 color) override final;
    virtual void wireframe_mode(bool enable) override final;
    virtual void clear_depth_buffer() override final;
    virtual void enable_face_culling(bool enable) override final;
    virtual void enable_depth_testing(bool enable) override final;
    virtual void enable_blending(bool enable) override final;
    virtual std::unique_ptr<shader> gen_shader(const std::filesystem::path &shader_src_directory) override final;
    virtual std::unique_ptr<shader> gen_shader(const std::vector<shader_src> &shader_sources) override final;
    virtual std::unique_ptr<buffer> gen_buffer(const void *data, const size_t size_in_bytes,
                                               const buffer_format &format,
                                               const buffer_access_type type) override final;
    virtual std::unique_ptr<texture2D> gen_texture(const std::filesystem::path &image_filepath) override final;
    virtual std::unique_ptr<vertex_input_assembly> gen_vertex_input_assembly(index_type type) override final;
    virtual void draw_mesh(const simple_mesh *m, const transform *model, material *mat) override final;
    virtual void draw_mesh(const instanced_mesh *m, const transform *model, material *mat,
                           unsigned int instance_count) override final;
    virtual void set_viewport(size_t x, size_t y, size_t width, size_t height) override final;
    virtual void set_cursor(cursor_type type) override final;

  private:
    void create_context() override final;
    void poll_events() override final;
    void destroy_context() override final;
    void activate_context() override final;
    void swap_buffers() override final;
    using renderer::on_key;
    using renderer::on_mouse_button;
    using renderer::on_mouse_move;
    using renderer::on_mouse_wheel;
    using renderer::on_resize;
    using renderer::render;
    using renderer::update;
    SDL_GLContext glcontext = nullptr;
    SDL_Window *window = nullptr;
    unsigned int window_id = 0;
};
class sdl_gl_shader : public shader {

  public:
    virtual void activate() override final;
    sdl_gl_shader(const std::filesystem::path &shader_src_folder);
    sdl_gl_shader(const std::vector<shader_src> &shader_sources);
    virtual ~sdl_gl_shader();
    virtual void upload_mat4(const std::string &name, const squint::fmat4 &value,
                             bool suppress_warnings = false) override final;
    virtual void upload_vec4(const std::string &name, const squint::fvec4 &value,
                             bool suppress_warnings = false) override final;
    virtual void upload_texture2D(const std::string &name, const texture2D *texture,
                                  bool suppress_warnings = false) override final;
    virtual void upload_storage_buffer(const std::string &name, const buffer *ssbo,
                                       bool suppress_warnings = false) override final;
    virtual uint32_t get_id() override final;

  private:
    inline static const std::unordered_map<std::string, shader_type> shader_ext_type{
        {".vert", shader_type::VERTEX_SHADER},          {".tesc", shader_type::TESS_CONTROL_SHADER},
        {".tese", shader_type::TESS_EVALUATION_SHADER}, {".geom", shader_type::GEOMETRY_SHADER},
        {".frag", shader_type::FRAGMENT_SHADER},        {".comp", shader_type::COMPUTE_SHADER}};
    inline static const std::unordered_map<shader_type, GLenum> shader_gl_type{
        {shader_type::VERTEX_SHADER, GL_VERTEX_SHADER},
        {shader_type::TESS_CONTROL_SHADER, GL_TESS_CONTROL_SHADER},
        {shader_type::TESS_EVALUATION_SHADER, GL_TESS_EVALUATION_SHADER},
        {shader_type::GEOMETRY_SHADER, GL_GEOMETRY_SHADER},
        {shader_type::FRAGMENT_SHADER, GL_FRAGMENT_SHADER},
        {shader_type::COMPUTE_SHADER, GL_COMPUTE_SHADER}};
    static shader_src read_shader(const std::filesystem::path &shader_src_filepath);
    GLuint compile_shader(const shader_src &source);
    GLuint create_program(const std::vector<shader_src> &sources);
    GLuint program;
    std::unordered_map<std::string, GLint> resource_location_cache;
    std::unordered_map<std::string, GLint> texture_binding_cache;
    std::unordered_map<std::string, GLint> storage_binding_cache;
};
class sdl_gl_buffer : public buffer {
  public:
    sdl_gl_buffer(const void *data, const size_t size_in_bytes, const buffer_format &format,
                  const buffer_access_type type)
        : buffer(format, type, size_in_bytes) {
        // Create the buffer
        glCreateBuffers(1, &buffer_id);
        GLbitfield flags = 0;
        switch (type) {
        case buffer_access_type::STATIC:
            break;
        case buffer_access_type::READ_ONLY:
            flags = GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            break;
        case buffer_access_type::WRITE_ONLY:
            flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            break;
        case buffer_access_type::READ_WRITE:
            flags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
            break;
        default:
            break;
        }
        glNamedBufferStorage(buffer_id, size_in_bytes, data, flags);
        if (type != buffer_access_type::STATIC) {
            buffer_ptr = glMapNamedBufferRange(buffer_id, 0, size_in_bytes, flags);
        }
    }
    virtual const uint32_t get_id() const override final { return buffer_id; }
    ~sdl_gl_buffer() { glDeleteBuffers(1, &buffer_id); }

  private:
    GLuint buffer_id;
};
class sdl_gl_texture2D : public texture2D {
  public:
    sdl_gl_texture2D(const std::filesystem::path &image_filepath);
    virtual const uint32_t get_id() const override final { return texture_id; }
    ~sdl_gl_texture2D() {
        glDeleteBuffers(1, &buffer_id);
        glDeleteTextures(1, &texture_id);
    }

  private:
    GLuint buffer_id;
    GLuint texture_id;
    texture_type tex_type;
    int width;
    int height;
};
class sdl_gl_vertex_input_assembly : public vertex_input_assembly {
  public:
    sdl_gl_vertex_input_assembly(index_type type) : vertex_input_assembly(type) { glCreateVertexArrays(1, &vao); }
    virtual void bind_shader(shader *s) override final {
        if (s) {
            int buffer_binding_index = 0;
            for (auto &buffer : vertex_buffers) {
                for (const auto &attrib : buffer->get_format().get_attributes()) {
                    GLuint attrib_loc = glGetAttribLocation(static_cast<GLuint>(s->get_id()), attrib.name.c_str());
                    // if material has this attribute, enable it
                    if (attrib_loc != -1) {
                        glEnableVertexArrayAttrib(vao, attrib_loc);
                        glVertexArrayAttribFormat(vao, attrib_loc, static_cast<GLuint>(attrib.component_count()),
                                                  GL_FLOAT, GL_FALSE, static_cast<GLint>(attrib.offset));
                        glVertexArrayAttribBinding(vao, attrib_loc, static_cast<GLuint>(buffer_binding_index));
                        glVertexArrayVertexBuffer(vao, static_cast<GLuint>(buffer_binding_index),
                                                  static_cast<GLuint>(buffer->get_id()), 0,
                                                  static_cast<GLsizei>(buffer->get_format().get_stride()));
                    }
                }
                buffer_binding_index++;
            }
            if (index_buffer) {
                glVertexArrayElementBuffer(vao, static_cast<GLuint>(index_buffer->get_id()));
            }
        }
    }
    virtual void activate() const override final { glBindVertexArray(vao); }
    ~sdl_gl_vertex_input_assembly() { glDeleteVertexArrays(1, &vao); }

  private:
    GLuint vao;
};
} // namespace square

#endif
