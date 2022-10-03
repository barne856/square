#ifndef SDL_GL_RENDERER
#define SDL_GL_RENDERER
#include "lit/renderer.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstring>
#include <unordered_map>

namespace lit {

class sdl_gl_renderer : public renderer {

  public:
    virtual void clear_color_buffer(squint::fvec4 color) override final;
    virtual std::unique_ptr<shader> gen_shader(const std::filesystem::path &shader_src_directory) override final;
    virtual std::unique_ptr<buffer> gen_buffer(const void *data, const size_t size_in_bytes, const size_t num_elements,
                                               const buffer_format &format,
                                               const buffer_access_type type) override final;
    virtual std::unique_ptr<texture2D> gen_texture(const std::filesystem::path &image_filepath) override final;
    virtual std::unique_ptr<vertex_input_assembly> gen_vertex_input_assembly(index_type type) override final;
    virtual void draw_mesh(mesh *m, draw_method method) override final;
    virtual void set_viewport(size_t x, size_t y, size_t width, size_t height) override final;
    virtual void set_cursor(cursor_type type) override final;

  private:
    void create_context();
    void poll_events();
    void destroy_context();
    void activate_context();
    void swap_buffers();
    using renderer::on_key;
    using renderer::on_mouse_button;
    using renderer::on_mouse_move;
    using renderer::on_mouse_wheel;
    using renderer::on_resize;
    using renderer::render;
    using renderer::update;
    SDL_GLContext glcontext;
    SDL_Window *window;
    unsigned int window_id;
};
class sdl_gl_shader : public shader {

  public:
    virtual void activate() override final;
    sdl_gl_shader(const std::filesystem::path &shader_src_folder);
    virtual ~sdl_gl_shader();
    virtual void upload_mat4(const std::string &name, squint::fmat4 value,
                             bool suppress_warnings = false) override final;
    virtual void upload_vec4(const std::string &name, squint::fvec4 value,
                             bool suppress_warnings = false) override final;
    virtual void upload_texture2D(const std::string &name, texture2D *texture,
                                  bool suppress_warnings = false) override final;
    virtual uint32_t get_id() override final;

  private:
    inline static const std::unordered_map<std::string, GLenum> shader_ext_type{
        {".vert", GL_VERTEX_SHADER},   {".tesc", GL_TESS_CONTROL_SHADER}, {".tese", GL_TESS_EVALUATION_SHADER},
        {".geom", GL_GEOMETRY_SHADER}, {".frag", GL_FRAGMENT_SHADER},     {".comp", GL_COMPUTE_SHADER}};
    struct shader_source {
        std::string source;
        GLenum shader_type;
    };
    static shader_source read_shader(const std::filesystem::path &shader_src_filepath);
    GLuint compile_shader(const std::filesystem::path &shader_src_filepath);
    GLuint create_program(const std::filesystem::path &shader_src_folder);
    GLuint program;
    std::unordered_map<std::string, GLint> resource_location_cache;
    std::unordered_map<std::string, GLint> texture_binding_cache;
};
class sdl_gl_buffer : public buffer {
  public:
    sdl_gl_buffer(const void *data, const size_t size_in_bytes, const size_t num_elements, const buffer_format &format,
                  const buffer_access_type type)
        : buffer(format, type, num_elements) {
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
    virtual const size_t size() const override final { return num_elements; }
    ~sdl_gl_buffer() { glDeleteBuffers(1, &buffer_id); }

  private:
    GLuint buffer_id;
};
class sdl_gl_texture2D : public texture2D {
  public:
    sdl_gl_texture2D(const std::filesystem::path &image_filepath);
    virtual const uint32_t get_id() const override final { return texture_id; }
    virtual const size_t size() const override final { return num_elements; }
    ~sdl_gl_texture2D() {
        glDeleteBuffers(1, &buffer_id);
        glDeleteTextures(1, &texture_id);
    }

  private:
    GLuint buffer_id;
    GLuint texture_id;
    texture_type type;
    int width;
    int height;
};
class sdl_gl_vertex_input_assembly : public vertex_input_assembly {
  public:
    sdl_gl_vertex_input_assembly(index_type type) : vertex_input_assembly(type) { glCreateVertexArrays(1, &vao); }
    virtual void bind_shader(shader *s) override final {
        int buffer_binding_index = 0;
        for (auto &buffer : vertex_buffers) {
            for (const auto &attrib : buffer->get_format().get_attributes()) {
                GLuint attrib_loc = glGetAttribLocation(static_cast<GLuint>(s->get_id()), attrib.name.c_str());
                // if material has this attribute, enable it
                if (attrib_loc != -1) {
                    glEnableVertexArrayAttrib(vao, attrib_loc);
                    glVertexArrayAttribFormat(vao, attrib_loc, static_cast<GLuint>(attrib.component_count()), GL_FLOAT,
                                              GL_FALSE, static_cast<GLint>(attrib.offset * 4));
                    glVertexArrayAttribBinding(vao, attrib_loc, static_cast<GLuint>(buffer_binding_index));
                    glVertexArrayVertexBuffer(vao, static_cast<GLuint>(buffer_binding_index),
                                              static_cast<GLuint>(buffer->get_id()), 0,
                                              static_cast<GLsizei>(buffer->get_format().get_stride() * 4));
                }
            }
            buffer_binding_index++;
        }
        if (index_buffer) {
            glVertexArrayElementBuffer(vao, static_cast<GLuint>(index_buffer->get_id()));
        }
    }
    virtual void activate() const override final { glBindVertexArray(vao); }
    ~sdl_gl_vertex_input_assembly() { glDeleteVertexArrays(1, &vao); }

  private:
    GLuint vao;
};
} // namespace lit

#endif
