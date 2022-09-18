#ifndef SDL_GL_RENDERER
#define SDL_GL_RENDERER
#include "lit/renderer.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <unordered_map>

namespace lit {
class sdl_gl : public renderer {

  public:
    virtual void clear_color_buffer(squint::fvec4 color) override final;
    virtual std::unique_ptr<shader> gen_shader(const std::filesystem::path &shader_src_directory) override final;

  private:
    virtual void create_context() override final;
    virtual void poll_events() override final;
    virtual void destroy_context() override final;
    virtual void activate_context() override final;
    virtual void swap_buffers() override final;
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
    virtual void upload_mat4(const char *name, squint::fmat4 value, bool suppress_warnings = false) override final;
    virtual void upload_vec4(const char *name, squint::fvec4 value, bool suppress_warnings = false) override final;

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
};
} // namespace lit

#endif
