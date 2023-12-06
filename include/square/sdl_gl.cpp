module;
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstring>
#include <unordered_map>
#include <fstream>
export module square:sdl_gl;
import :renderer;

export namespace square {

class sdl_gl_renderer : public renderer {
    friend class app;

  public:
    virtual ~sdl_gl_renderer();
    static void init();
    static void quit();
    virtual void clear_color_buffer(squint::fvec4 color) override final;
    virtual void wireframe_mode(bool enable) override final;
    virtual void clear_depth_buffer() override final;
    virtual void enable_face_culling(bool enable) override final;
    virtual void enable_depth_testing(bool enable) override final;
    virtual void enable_blending(bool enable) override final;
    virtual std::unique_ptr<shader> gen_shader(const std::string &name,
                                               const std::filesystem::path &shader_src_directory) override final;
    virtual std::unique_ptr<shader> gen_shader(const std::string &name,
                                               const std::vector<shader_src> &shader_sources) override final;
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
    std::unordered_map<std::string, GLint> shader_name_binding_cache;
};
class sdl_gl_shader : public shader {

  public:
    virtual void activate() override final;
    sdl_gl_shader(const std::filesystem::path &shader_src_folder);
    sdl_gl_shader(const std::vector<shader_src> &shader_sources);
    sdl_gl_shader(GLint program) : program(program) {}
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

// Debug functions
std::string debug_source_string(GLenum source) {
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        return "WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        return "SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        return "THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
        return "APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
        return "OTHER";
    }
    return "INVALID_SOURCE";
}
std::string debug_type_string(GLenum type) {
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DEPRICATED BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEFINED BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
        return "PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
        return "PERFORMANCE";
    case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
    case GL_DEBUG_TYPE_MARKER:
        return "MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP:
        return "PUSH GROUP";
    case GL_DEBUG_TYPE_POP_GROUP:
        return "POP GROUP";
    }
    return "INVALID_TYPE";
}
std::string debug_severity_string(GLenum severity) {
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        return "***HIGH***";
    case GL_DEBUG_SEVERITY_MEDIUM:
        return "**MEDIUM**";
    case GL_DEBUG_SEVERITY_LOW:
        return "*LOW*";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        return "NOTIFICATION";
    }
    return "INVALID_SEVERITY";
}
GLenum gl_draw_method(draw_method method) {
    switch (method) {
    case draw_method::POINTS:
        return GL_POINTS;
    case draw_method::LINES:
        return GL_LINES;
    case draw_method::LINE_STRIP:
        return GL_LINE_STRIP;
    case draw_method::LINE_LOOP:
        return GL_LINE_LOOP;
    case draw_method::TRIANGLES:
        return GL_TRIANGLES;
    case draw_method::TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    case draw_method::TRIANGLE_FAN:
        return GL_TRIANGLE_FAN;
    default:
        return GL_NONE;
    }
}
GLenum gl_index_type(index_type type) {
    switch (type) {
    case index_type::UNSIGNED_BYTE:
        return GL_UNSIGNED_BYTE;
    case index_type::UNSIGNED_SHORT:
        return GL_UNSIGNED_SHORT;
    case index_type::UNSIGNED_INT:
        return GL_UNSIGNED_INT;
    default:
        return GL_NONE;
    }
}
GLenum gl_sized_tex_format(texture_type type) {
    switch (type) {
    case texture_type::R8:
        return GL_R8;
    case texture_type::RG8:
        return GL_RG8;
    case texture_type::RGB8:
        return GL_RGB8;
    case texture_type::RGBA8:
        return GL_RGBA8;
    case texture_type::R32F:
        return GL_R32F;
    case texture_type::RG32F:
        return GL_RG32F;
    case texture_type::RGB32F:
        return GL_RGB32F;
    case texture_type::RGBA32F:
        return GL_RGBA32F;
    case texture_type::DEPTH:
        return GL_DEPTH_COMPONENT32F;
    default:
        return GL_NONE;
    }
}

GLenum gl_tex_format(texture_type type) {
    switch (type) {
    case texture_type::R8:
        return GL_RED;
    case texture_type::RG8:
        return GL_RG;
    case texture_type::RGB8:
        return GL_RGB;
    case texture_type::RGBA8:
        return GL_RGBA;
    case texture_type::R32F:
        return GL_RED;
    case texture_type::RG32F:
        return GL_RG;
    case texture_type::RGB32F:
        return GL_RGB;
    case texture_type::RGBA32F:
        return GL_RGBA;
    case texture_type::DEPTH:
        return GL_DEPTH_COMPONENT;
    default:
        return GL_NONE;
    }
}

int gl_tex_bytes(texture_type type) {
    switch (type) {
    case texture_type::R8:
        return 1;
    case texture_type::RG8:
        return 1;
    case texture_type::RGB8:
        return 1;
    case texture_type::RGBA8:
        return 1;
    case texture_type::R32F:
        return 4;
    case texture_type::RG32F:
        return 4;
    case texture_type::RGB32F:
        return 4;
    case texture_type::RGBA32F:
        return 4;
    case texture_type::DEPTH:
        return 1;
    default:
        return 0;
    }
}

GLenum gl_tex_type(texture_type type) {
    switch (type) {
    case texture_type::R8:
        return GL_UNSIGNED_BYTE;
    case texture_type::RG8:
        return GL_UNSIGNED_BYTE;
    case texture_type::RGB8:
        return GL_UNSIGNED_BYTE;
    case texture_type::RGBA8:
        return GL_UNSIGNED_BYTE;
    case texture_type::R32F:
        return GL_FLOAT;
    case texture_type::RG32F:
        return GL_FLOAT;
    case texture_type::RGB32F:
        return GL_FLOAT;
    case texture_type::RGBA32F:
        return GL_FLOAT;
    case texture_type::DEPTH:
        return GL_FLOAT;
    default:
        return GL_NONE;
    }
}
void GLAPIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                       const GLchar *message, const void *userParam) {
    auto debug_state = static_cast<const sdl_gl_renderer *>(userParam)->get_properties().debug;
    if (severity == GL_DEBUG_SEVERITY_HIGH &&
        (debug_state == debug_mode::HIGH || debug_state == debug_mode::MED || debug_state == debug_mode::LOW ||
         debug_state == debug_mode::NOTIFICATION)) {
        std::cerr << "GL_" << debug_source_string(source) << "_" << debug_type_string(type) << ": "
                  << debug_severity_string(severity) << " - " << message << std::endl;
    }
    if (severity == GL_DEBUG_SEVERITY_MEDIUM &&
        (debug_state == debug_mode::MED || debug_state == debug_mode::LOW || debug_state == debug_mode::NOTIFICATION)) {
        std::cerr << "GL_" << debug_source_string(source) << "_" << debug_type_string(type) << ": "
                  << debug_severity_string(severity) << " - " << message << std::endl;
    }
    if (severity == GL_DEBUG_SEVERITY_LOW &&
        (debug_state == debug_mode::LOW || debug_state == debug_mode::NOTIFICATION)) {
        std::cerr << "GL_" << debug_source_string(source) << "_" << debug_type_string(type) << ": "
                  << debug_severity_string(severity) << " - " << message << std::endl;
    }
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION && debug_state == debug_mode::NOTIFICATION) {
        std::cout << "GL_" << debug_source_string(source) << "_" << debug_type_string(type) << ": "
                  << debug_severity_string(severity) << " - " << message << std::endl;
    }
}
sdl_gl_renderer::~sdl_gl_renderer() {
    for (const auto &[name, program] : shader_name_binding_cache) {
        glDeleteProgram(program);
    }
}
void sdl_gl_renderer::init() {
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland,x11");
    SDL_Init(SDL_INIT_VIDEO); // Init SDL2, VIDEO also inits EVENTS
    // Initialize PNG loading
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        throw std::runtime_error("SDL_image could not initialize! SDL_image Error: " + std::string(IMG_GetError()));
    }
}
void sdl_gl_renderer::quit() {
    IMG_Quit();
    SDL_Quit();
}
void sdl_gl_renderer::create_context() {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, properties.samples);
    auto window_flag = properties.fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(properties.window_title, 0, 0, properties.window_width, properties.window_height,
                              SDL_WINDOW_OPENGL | window_flag | SDL_WINDOW_ALLOW_HIGHDPI);

    window_id = SDL_GetWindowID(window);
    glcontext = SDL_GL_CreateContext(window);
    if (properties.vsync) {
        SDL_GL_SetSwapInterval(1);
    }
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Error GLEW failed to initalize:\n" +
                                 std::string(reinterpret_cast<const char *>(glewGetErrorString(err))) + "\n");
    }
    if (properties.samples > 1) {
        glEnable(GL_MULTISAMPLE);
    }
    if (properties.debug != debug_mode::OFF) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_message_callback, this);
        if (properties.debug == debug_mode::NOTIFICATION) {
            std::cout << "GL VENDOR: " << glGetString(GL_VENDOR) << "\n";
            std::cout << "GL VERSION: " << glGetString(GL_VERSION) << "\n";
            std::cout << "GL RENDERER: " << glGetString(GL_RENDERER) << std::endl;
            std::cout << "SDL VIDEO DRIVER: " << SDL_GetCurrentVideoDriver() << std::endl;
        }
    }
    set_cursor(properties.cursor);
    // set the window width and height and gl viewport to actual pixels since high DPI reports a scaled coordinate
    int w, h;
    SDL_GL_GetDrawableSize(window, &w, &h);
    properties.window_width = w;
    properties.window_height = h;
    glViewport(0, 0, w, h);
}
void sdl_gl_renderer::poll_events() {
    std::vector<SDL_Event> unhandled_events{};
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.window.windowID == window_id) {
            switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    destroy();
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    // properties.window_width = static_cast<uint64_t>(event.window.data1);
                    // properties.window_height = static_cast<uint64_t>(event.window.data2);
                    // high DPI monitors will not report actual pixels, so we get them this way
                    int w, h;
                    SDL_GL_GetDrawableSize(window, &w, &h);
                    properties.window_width = w;
                    properties.window_height = h;
                    on_resize({properties.window_width, properties.window_height});
                    break;
                default:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    on_mouse_button(mouse_button_event::LEFT_MOUSE_UP);
                    break;
                case SDL_BUTTON_RIGHT:
                    on_mouse_button(mouse_button_event::RIGHT_MOUSE_UP);
                    break;
                case SDL_BUTTON_MIDDLE:
                    on_mouse_button(mouse_button_event::MIDDLE_MOUSE_UP);
                    break;
                default:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (event.button.button) {
                case SDL_BUTTON_LEFT:
                    on_mouse_button(mouse_button_event::LEFT_MOUSE_DOWN);
                    break;
                case SDL_BUTTON_RIGHT:
                    on_mouse_button(mouse_button_event::RIGHT_MOUSE_DOWN);
                    break;
                case SDL_BUTTON_MIDDLE:
                    on_mouse_button(mouse_button_event::MIDDLE_MOUSE_DOWN);
                    break;
                default:
                    break;
                }
                break;
            case SDL_MOUSEWHEEL:
                on_mouse_wheel({event.wheel.preciseX, event.wheel.preciseY});
                break;
            case SDL_MOUSEMOTION:
                on_mouse_move({event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel});
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    on_key(key_event::SPACE_UP);
                    break;
                case SDLK_QUOTE:
                    on_key(key_event::APOSTROPHE_UP);
                    break;
                case SDLK_COMMA:
                    on_key(key_event::COMMA_UP);
                    break;
                case SDLK_MINUS:
                    on_key(key_event::MINUS_UP);
                    break;
                case SDLK_PERIOD:
                    on_key(key_event::PERIOD_UP);
                    break;
                case SDLK_SLASH:
                    on_key(key_event::FORWARD_SLASH_UP);
                    break;
                case SDLK_0:
                    on_key(key_event::ZERO_UP);
                    break;
                case SDLK_1:
                    on_key(key_event::ONE_UP);
                    break;
                case SDLK_2:
                    on_key(key_event::TWO_UP);
                    break;
                case SDLK_3:
                    on_key(key_event::THREE_UP);
                    break;
                case SDLK_4:
                    on_key(key_event::FOUR_UP);
                    break;
                case SDLK_5:
                    on_key(key_event::FIVE_UP);
                    break;
                case SDLK_6:
                    on_key(key_event::SIX_UP);
                    break;
                case SDLK_7:
                    on_key(key_event::SEVEN_UP);
                    break;
                case SDLK_8:
                    on_key(key_event::EIGHT_UP);
                    break;
                case SDLK_9:
                    on_key(key_event::NINE_UP);
                    break;
                case SDLK_SEMICOLON:
                    on_key(key_event::SEMICOLON_UP);
                    break;
                case SDLK_EQUALS:
                    on_key(key_event::EQUAL_UP);
                    break;
                case SDLK_a:
                    on_key(key_event::A_UP);
                    break;
                case SDLK_b:
                    on_key(key_event::B_UP);
                    break;
                case SDLK_c:
                    on_key(key_event::C_UP);
                    break;
                case SDLK_d:
                    on_key(key_event::D_UP);
                    break;
                case SDLK_e:
                    on_key(key_event::E_UP);
                    break;
                case SDLK_f:
                    on_key(key_event::F_UP);
                    break;
                case SDLK_g:
                    on_key(key_event::G_UP);
                    break;
                case SDLK_h:
                    on_key(key_event::H_UP);
                    break;
                case SDLK_i:
                    on_key(key_event::I_UP);
                    break;
                case SDLK_j:
                    on_key(key_event::J_UP);
                    break;
                case SDLK_k:
                    on_key(key_event::K_UP);
                    break;
                case SDLK_l:
                    on_key(key_event::L_UP);
                    break;
                case SDLK_m:
                    on_key(key_event::M_UP);
                    break;
                case SDLK_n:
                    on_key(key_event::N_UP);
                    break;
                case SDLK_o:
                    on_key(key_event::O_UP);
                    break;
                case SDLK_p:
                    on_key(key_event::P_UP);
                    break;
                case SDLK_q:
                    on_key(key_event::Q_UP);
                    break;
                case SDLK_r:
                    on_key(key_event::R_UP);
                    break;
                case SDLK_s:
                    on_key(key_event::S_UP);
                    break;
                case SDLK_t:
                    on_key(key_event::T_UP);
                    break;
                case SDLK_u:
                    on_key(key_event::U_UP);
                    break;
                case SDLK_v:
                    on_key(key_event::V_UP);
                    break;
                case SDLK_w:
                    on_key(key_event::W_UP);
                    break;
                case SDLK_x:
                    on_key(key_event::X_UP);
                    break;
                case SDLK_y:
                    on_key(key_event::Y_UP);
                    break;
                case SDLK_z:
                    on_key(key_event::Z_UP);
                    break;
                case SDLK_LEFTBRACKET:
                    on_key(key_event::LEFT_BRACKET_UP);
                    break;
                case SDLK_RIGHTBRACKET:
                    on_key(key_event::RIGHT_BRACKET_UP);
                    break;
                case SDLK_BACKQUOTE:
                    on_key(key_event::GRAVE_ACCENT_UP);
                    break;
                case SDLK_ESCAPE:
                    on_key(key_event::ESCAPE_UP);
                    break;
                case SDLK_RETURN:
                    on_key(key_event::ENTER_UP);
                    break;
                case SDLK_TAB:
                    on_key(key_event::TAB_UP);
                    break;
                case SDLK_BACKSPACE:
                    on_key(key_event::BACKSPACE_UP);
                    break;
                case SDLK_INSERT:
                    on_key(key_event::INSERT_UP);
                    break;
                case SDLK_DELETE:
                    on_key(key_event::DELETE_UP);
                    break;
                case SDLK_LEFT:
                    on_key(key_event::LEFT_UP);
                    break;
                case SDLK_RIGHT:
                    on_key(key_event::RIGHT_UP);
                    break;
                case SDLK_UP:
                    on_key(key_event::UP_UP);
                    break;
                case SDLK_DOWN:
                    on_key(key_event::DOWN_UP);
                    break;
                case SDLK_PAGEUP:
                    on_key(key_event::PAGE_UP_UP);
                    break;
                case SDLK_PAGEDOWN:
                    on_key(key_event::PAGE_DOWN_UP);
                    break;
                case SDLK_HOME:
                    on_key(key_event::HOME_UP);
                    break;
                case SDLK_END:
                    on_key(key_event::END_UP);
                    break;
                case SDLK_CAPSLOCK:
                    on_key(key_event::CAPS_LOCK_UP);
                    break;
                case SDLK_SCROLLLOCK:
                    on_key(key_event::SCROLL_LOCK_UP);
                    break;
                case SDLK_NUMLOCKCLEAR:
                    on_key(key_event::NUM_LOCK_UP);
                    break;
                case SDLK_PRINTSCREEN:
                    on_key(key_event::PRINT_SCREEN_UP);
                    break;
                case SDLK_PAUSE:
                    on_key(key_event::PAUSE_UP);
                    break;
                case SDLK_F1:
                    on_key(key_event::F1_UP);
                    break;
                case SDLK_F2:
                    on_key(key_event::F2_UP);
                    break;
                case SDLK_F3:
                    on_key(key_event::F3_UP);
                    break;
                case SDLK_F4:
                    on_key(key_event::F4_UP);
                    break;
                case SDLK_F5:
                    on_key(key_event::F5_UP);
                    break;
                case SDLK_F6:
                    on_key(key_event::F6_UP);
                    break;
                case SDLK_F7:
                    on_key(key_event::F7_UP);
                    break;
                case SDLK_F8:
                    on_key(key_event::F8_UP);
                    break;
                case SDLK_F9:
                    on_key(key_event::F9_UP);
                    break;
                case SDLK_F10:
                    on_key(key_event::F10_UP);
                    break;
                case SDLK_F11:
                    on_key(key_event::F11_UP);
                    break;
                case SDLK_F12:
                    on_key(key_event::F12_UP);
                    break;
                case SDLK_KP_0:
                    on_key(key_event::KEY_PAD_0_UP);
                    break;
                case SDLK_KP_1:
                    on_key(key_event::KEY_PAD_1_UP);
                    break;
                case SDLK_KP_2:
                    on_key(key_event::KEY_PAD_2_UP);
                    break;
                case SDLK_KP_3:
                    on_key(key_event::KEY_PAD_3_UP);
                    break;
                case SDLK_KP_4:
                    on_key(key_event::KEY_PAD_4_UP);
                    break;
                case SDLK_KP_5:
                    on_key(key_event::KEY_PAD_5_UP);
                    break;
                case SDLK_KP_6:
                    on_key(key_event::KEY_PAD_6_UP);
                    break;
                case SDLK_KP_7:
                    on_key(key_event::KEY_PAD_7_UP);
                    break;
                case SDLK_KP_8:
                    on_key(key_event::KEY_PAD_8_UP);
                    break;
                case SDLK_KP_9:
                    on_key(key_event::KEY_PAD_9_UP);
                    break;
                case SDLK_KP_DECIMAL:
                    on_key(key_event::KEY_PAD_DECIMAL_UP);
                    break;
                case SDLK_KP_DIVIDE:
                    on_key(key_event::KEY_PAD_DIVIDE_UP);
                    break;
                case SDLK_KP_MULTIPLY:
                    on_key(key_event::KEY_PAD_MULTIPLY_UP);
                    break;
                case SDLK_KP_MINUS:
                    on_key(key_event::KEY_PAD_SUBTRACT_UP);
                    break;
                case SDLK_KP_PLUS:
                    on_key(key_event::KEY_PAD_ADD_UP);
                    break;
                case SDLK_KP_ENTER:
                    on_key(key_event::KEY_PAD_ENTER_UP);
                    break;
                case SDLK_KP_EQUALS:
                    on_key(key_event::KEY_PAD_EQUAL_UP);
                    break;
                case SDLK_LSHIFT:
                    on_key(key_event::LEFT_SHIFT_UP);
                    break;
                case SDLK_LCTRL:
                    on_key(key_event::LEFT_CONTROL_UP);
                    break;
                case SDLK_LALT:
                    on_key(key_event::LEFT_ALT_UP);
                    break;
                case SDLK_LGUI:
                    on_key(key_event::LEFT_SUPER_UP);
                    break;
                case SDLK_RSHIFT:
                    on_key(key_event::RIGHT_SHIFT_UP);
                    break;
                case SDLK_RCTRL:
                    on_key(key_event::RIGHT_CONTROL_UP);
                    break;
                case SDLK_RALT:
                    on_key(key_event::RIGHT_ALT_UP);
                    break;
                case SDLK_RGUI:
                    on_key(key_event::RIGHT_SUPER_UP);
                    break;
                case SDLK_MENU:
                    on_key(key_event::MENU_UP);
                    break;
                default:
                    break;
                }
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_SPACE:
                    on_key(key_event::SPACE_DOWN);
                    break;
                case SDLK_QUOTE:
                    on_key(key_event::APOSTROPHE_DOWN);
                    break;
                case SDLK_COMMA:
                    on_key(key_event::COMMA_DOWN);
                    break;
                case SDLK_MINUS:
                    on_key(key_event::MINUS_DOWN);
                    break;
                case SDLK_PERIOD:
                    on_key(key_event::PERIOD_DOWN);
                    break;
                case SDLK_SLASH:
                    on_key(key_event::FORWARD_SLASH_DOWN);
                    break;
                case SDLK_0:
                    on_key(key_event::ZERO_DOWN);
                    break;
                case SDLK_1:
                    on_key(key_event::ONE_DOWN);
                    break;
                case SDLK_2:
                    on_key(key_event::TWO_DOWN);
                    break;
                case SDLK_3:
                    on_key(key_event::THREE_DOWN);
                    break;
                case SDLK_4:
                    on_key(key_event::FOUR_DOWN);
                    break;
                case SDLK_5:
                    on_key(key_event::FIVE_DOWN);
                    break;
                case SDLK_6:
                    on_key(key_event::SIX_DOWN);
                    break;
                case SDLK_7:
                    on_key(key_event::SEVEN_DOWN);
                    break;
                case SDLK_8:
                    on_key(key_event::EIGHT_DOWN);
                    break;
                case SDLK_9:
                    on_key(key_event::NINE_DOWN);
                    break;
                case SDLK_SEMICOLON:
                    on_key(key_event::SEMICOLON_DOWN);
                    break;
                case SDLK_EQUALS:
                    on_key(key_event::EQUAL_DOWN);
                    break;
                case SDLK_a:
                    on_key(key_event::A_DOWN);
                    break;
                case SDLK_b:
                    on_key(key_event::B_DOWN);
                    break;
                case SDLK_c:
                    on_key(key_event::C_DOWN);
                    break;
                case SDLK_d:
                    on_key(key_event::D_DOWN);
                    break;
                case SDLK_e:
                    on_key(key_event::E_DOWN);
                    break;
                case SDLK_f:
                    on_key(key_event::F_DOWN);
                    break;
                case SDLK_g:
                    on_key(key_event::G_DOWN);
                    break;
                case SDLK_h:
                    on_key(key_event::H_DOWN);
                    break;
                case SDLK_i:
                    on_key(key_event::I_DOWN);
                    break;
                case SDLK_j:
                    on_key(key_event::J_DOWN);
                    break;
                case SDLK_k:
                    on_key(key_event::K_DOWN);
                    break;
                case SDLK_l:
                    on_key(key_event::L_DOWN);
                    break;
                case SDLK_m:
                    on_key(key_event::M_DOWN);
                    break;
                case SDLK_n:
                    on_key(key_event::N_DOWN);
                    break;
                case SDLK_o:
                    on_key(key_event::O_DOWN);
                    break;
                case SDLK_p:
                    on_key(key_event::P_DOWN);
                    break;
                case SDLK_q:
                    on_key(key_event::Q_DOWN);
                    break;
                case SDLK_r:
                    on_key(key_event::R_DOWN);
                    break;
                case SDLK_s:
                    on_key(key_event::S_DOWN);
                    break;
                case SDLK_t:
                    on_key(key_event::T_DOWN);
                    break;
                case SDLK_u:
                    on_key(key_event::U_DOWN);
                    break;
                case SDLK_v:
                    on_key(key_event::V_DOWN);
                    break;
                case SDLK_w:
                    on_key(key_event::W_DOWN);
                    break;
                case SDLK_x:
                    on_key(key_event::X_DOWN);
                    break;
                case SDLK_y:
                    on_key(key_event::Y_DOWN);
                    break;
                case SDLK_z:
                    on_key(key_event::Z_DOWN);
                    break;
                case SDLK_LEFTBRACKET:
                    on_key(key_event::LEFT_BRACKET_DOWN);
                    break;
                case SDLK_RIGHTBRACKET:
                    on_key(key_event::RIGHT_BRACKET_DOWN);
                    break;
                case SDLK_BACKQUOTE:
                    on_key(key_event::GRAVE_ACCENT_DOWN);
                    break;
                case SDLK_ESCAPE:
                    on_key(key_event::ESCAPE_DOWN);
                    break;
                case SDLK_RETURN:
                    on_key(key_event::ENTER_DOWN);
                    break;
                case SDLK_TAB:
                    on_key(key_event::TAB_DOWN);
                    break;
                case SDLK_BACKSPACE:
                    on_key(key_event::BACKSPACE_DOWN);
                    break;
                case SDLK_INSERT:
                    on_key(key_event::INSERT_DOWN);
                    break;
                case SDLK_DELETE:
                    on_key(key_event::DELETE_DOWN);
                    break;
                case SDLK_LEFT:
                    on_key(key_event::LEFT_DOWN);
                    break;
                case SDLK_RIGHT:
                    on_key(key_event::RIGHT_DOWN);
                    break;
                case SDLK_UP:
                    on_key(key_event::UP_DOWN);
                    break;
                case SDLK_DOWN:
                    on_key(key_event::DOWN_DOWN);
                    break;
                case SDLK_PAGEUP:
                    on_key(key_event::PAGE_UP_DOWN);
                    break;
                case SDLK_PAGEDOWN:
                    on_key(key_event::PAGE_DOWN_DOWN);
                    break;
                case SDLK_HOME:
                    on_key(key_event::HOME_DOWN);
                    break;
                case SDLK_END:
                    on_key(key_event::END_DOWN);
                    break;
                case SDLK_CAPSLOCK:
                    on_key(key_event::CAPS_LOCK_DOWN);
                    break;
                case SDLK_SCROLLLOCK:
                    on_key(key_event::SCROLL_LOCK_DOWN);
                    break;
                case SDLK_NUMLOCKCLEAR:
                    on_key(key_event::NUM_LOCK_DOWN);
                    break;
                case SDLK_PRINTSCREEN:
                    on_key(key_event::PRINT_SCREEN_DOWN);
                    break;
                case SDLK_PAUSE:
                    on_key(key_event::PAUSE_DOWN);
                    break;
                case SDLK_F1:
                    on_key(key_event::F1_DOWN);
                    break;
                case SDLK_F2:
                    on_key(key_event::F2_DOWN);
                    break;
                case SDLK_F3:
                    on_key(key_event::F3_DOWN);
                    break;
                case SDLK_F4:
                    on_key(key_event::F4_DOWN);
                    break;
                case SDLK_F5:
                    on_key(key_event::F5_DOWN);
                    break;
                case SDLK_F6:
                    on_key(key_event::F6_DOWN);
                    break;
                case SDLK_F7:
                    on_key(key_event::F7_DOWN);
                    break;
                case SDLK_F8:
                    on_key(key_event::F8_DOWN);
                    break;
                case SDLK_F9:
                    on_key(key_event::F9_DOWN);
                    break;
                case SDLK_F10:
                    on_key(key_event::F10_DOWN);
                    break;
                case SDLK_F11:
                    on_key(key_event::F11_DOWN);
                    break;
                case SDLK_F12:
                    on_key(key_event::F12_DOWN);
                    break;
                case SDLK_KP_0:
                    on_key(key_event::KEY_PAD_0_DOWN);
                    break;
                case SDLK_KP_1:
                    on_key(key_event::KEY_PAD_1_DOWN);
                    break;
                case SDLK_KP_2:
                    on_key(key_event::KEY_PAD_2_DOWN);
                    break;
                case SDLK_KP_3:
                    on_key(key_event::KEY_PAD_3_DOWN);
                    break;
                case SDLK_KP_4:
                    on_key(key_event::KEY_PAD_4_DOWN);
                    break;
                case SDLK_KP_5:
                    on_key(key_event::KEY_PAD_5_DOWN);
                    break;
                case SDLK_KP_6:
                    on_key(key_event::KEY_PAD_6_DOWN);
                    break;
                case SDLK_KP_7:
                    on_key(key_event::KEY_PAD_7_DOWN);
                    break;
                case SDLK_KP_8:
                    on_key(key_event::KEY_PAD_8_DOWN);
                    break;
                case SDLK_KP_9:
                    on_key(key_event::KEY_PAD_9_DOWN);
                    break;
                case SDLK_KP_DECIMAL:
                    on_key(key_event::KEY_PAD_DECIMAL_DOWN);
                    break;
                case SDLK_KP_DIVIDE:
                    on_key(key_event::KEY_PAD_DIVIDE_DOWN);
                    break;
                case SDLK_KP_MULTIPLY:
                    on_key(key_event::KEY_PAD_MULTIPLY_DOWN);
                    break;
                case SDLK_KP_MINUS:
                    on_key(key_event::KEY_PAD_SUBTRACT_DOWN);
                    break;
                case SDLK_KP_PLUS:
                    on_key(key_event::KEY_PAD_ADD_DOWN);
                    break;
                case SDLK_KP_ENTER:
                    on_key(key_event::KEY_PAD_ENTER_DOWN);
                    break;
                case SDLK_KP_EQUALS:
                    on_key(key_event::KEY_PAD_EQUAL_DOWN);
                    break;
                case SDLK_LSHIFT:
                    on_key(key_event::LEFT_SHIFT_DOWN);
                    break;
                case SDLK_LCTRL:
                    on_key(key_event::LEFT_CONTROL_DOWN);
                    break;
                case SDLK_LALT:
                    on_key(key_event::LEFT_ALT_DOWN);
                    break;
                case SDLK_LGUI:
                    on_key(key_event::LEFT_SUPER_DOWN);
                    break;
                case SDLK_RSHIFT:
                    on_key(key_event::RIGHT_SHIFT_DOWN);
                    break;
                case SDLK_RCTRL:
                    on_key(key_event::RIGHT_CONTROL_DOWN);
                    break;
                case SDLK_RALT:
                    on_key(key_event::RIGHT_ALT_DOWN);
                    break;
                case SDLK_RGUI:
                    on_key(key_event::RIGHT_SUPER_DOWN);
                    break;
                case SDLK_MENU:
                    on_key(key_event::MENU_DOWN);
                    break;
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                destroy();
            default:
                break;
            }
        } else {
            unhandled_events.push_back(event);
        }
    }
    for (auto &e : unhandled_events) {
        SDL_PushEvent(&e);
    }
}
void sdl_gl_renderer::destroy_context() {
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
}
void sdl_gl_renderer::activate_context() { SDL_GL_MakeCurrent(window, glcontext); }
void sdl_gl_renderer::swap_buffers() { SDL_GL_SwapWindow(window); }

void sdl_gl_renderer::clear_color_buffer(squint::fvec4 color) {
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}
void sdl_gl_renderer::wireframe_mode(bool enable) {
    if (enable) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
void sdl_gl_renderer::clear_depth_buffer() { glClear(GL_DEPTH_BUFFER_BIT); }
void sdl_gl_renderer::enable_face_culling(bool enable) {
    if (enable) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }
}
void sdl_gl_renderer::enable_depth_testing(bool enable) {
    if (enable) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
    } else {
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    }
}
void sdl_gl_renderer::enable_blending(bool enable) {
    if (enable) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glDisable(GL_BLEND);
    }
}
std::unique_ptr<shader> sdl_gl_renderer::gen_shader(const std::string &name,
                                                    const std::filesystem::path &shader_src_directory) {
    // There is no need to recompile the shader if it has already been compiled, so we just construct it from the
    // existing program id
    if (shader_name_binding_cache.find(name) == shader_name_binding_cache.end()) {
        auto new_shader = std::make_unique<sdl_gl_shader>(shader_src_directory);
        shader_name_binding_cache[name] = new_shader->get_id();
        return new_shader;
    } else {
        return std::make_unique<sdl_gl_shader>(shader_name_binding_cache[name]);
    }
}
std::unique_ptr<shader> sdl_gl_renderer::gen_shader(const std::string &name,
                                                    const std::vector<shader_src> &shader_sources) {
    // There is no need to recompile the shader if it has already been compiled, so we just construct it from the
    // existing program id
    if (shader_name_binding_cache.find(name) == shader_name_binding_cache.end()) {
        auto new_shader = std::make_unique<sdl_gl_shader>(shader_sources);
        shader_name_binding_cache[name] = new_shader->get_id();
        return new_shader;
    } else {
        return std::make_unique<sdl_gl_shader>(shader_name_binding_cache[name]);
    }
}
std::unique_ptr<buffer> sdl_gl_renderer::gen_buffer(const void *data, const size_t size_in_bytes,
                                                    const buffer_format &format, const buffer_access_type type) {
    return std::make_unique<sdl_gl_buffer>(data, size_in_bytes, format, type);
}
std::unique_ptr<texture2D> sdl_gl_renderer::gen_texture(const std::filesystem::path &image_filepath) {
    return std::make_unique<sdl_gl_texture2D>(image_filepath);
}
std::unique_ptr<vertex_input_assembly> sdl_gl_renderer::gen_vertex_input_assembly(index_type type) {
    return std::make_unique<sdl_gl_vertex_input_assembly>(type);
}
void sdl_gl_renderer::draw_mesh(const simple_mesh *m, const transform *model, material *mat) {
    const vertex_input_assembly *input_assembly = m->get_input_assembly();
    if (input_assembly) {
        input_assembly->activate();
        mat->set_model(model);
        if (input_assembly->get_index_buffer()) {
            glDrawElements(gl_draw_method(m->get_draw_method()),
                           static_cast<GLsizei>(input_assembly->get_index_buffer()->count()),
                           gl_index_type(input_assembly->get_index_type()), nullptr);
        } else {
            glDrawArrays(gl_draw_method(m->get_draw_method()), 0,
                         static_cast<GLsizei>(input_assembly->get_vertex_buffers()[0]->count()));
        }
    }
}
void sdl_gl_renderer::draw_mesh(const instanced_mesh *m, const transform *model, material *mat,
                                unsigned int instance_count) {
    const vertex_input_assembly *input_assembly = m->get_input_assembly();
    if (input_assembly) {
        input_assembly->activate();
        mat->set_model(model);
        mat->get_shader()->upload_storage_buffer("model_instances", m->get_transform_buffer());
        if (input_assembly->get_index_buffer()) {
            glDrawElementsInstancedBaseVertex(
                gl_draw_method(m->get_draw_method()), static_cast<GLsizei>(input_assembly->get_index_buffer()->count()),
                gl_index_type(input_assembly->get_index_type()), nullptr, m->get_instance_count(), 0);
        } else {
            glDrawArraysInstanced(gl_draw_method(m->get_draw_method()), 0,
                                  static_cast<GLsizei>(input_assembly->get_vertex_buffers()[0]->count()),
                                  m->get_instance_count());
        }
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
}
void sdl_gl_renderer::set_viewport(size_t x, size_t y, size_t width, size_t height) { glViewport(x, y, width, height); }
void sdl_gl_renderer::set_cursor(cursor_type type) {
    properties.cursor = type;
    SDL_SetRelativeMouseMode(SDL_FALSE);
    SDL_ShowCursor(SDL_ENABLE);
    switch (properties.cursor) {
    case cursor_type::DISABLED:
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_ShowCursor(SDL_DISABLE);
        break;
    default:
        break;
    }
}
sdl_gl_shader::sdl_gl_shader(const std::filesystem::path &shader_src_folder) {
    std::vector<shader_src> sources{};
    for (const auto &src_file : std::filesystem::directory_iterator(shader_src_folder)) {
        sources.push_back(read_shader(src_file));
    }
    program = create_program(sources);
}
sdl_gl_shader::sdl_gl_shader(const std::vector<shader_src> &shader_sources) {
    program = create_program(shader_sources);
}
sdl_gl_shader::~sdl_gl_shader() {
    // program is deleted in the renderer destructor
    // glDeleteProgram(program); // Silently ignored if program is 0
}
void sdl_gl_shader::activate() { glUseProgram(program); }
shader_src sdl_gl_shader::read_shader(const std::filesystem::path &shader_src_filepath) {
    shader_src src{};
    src.type = shader_ext_type.at(shader_src_filepath.extension().string());
    std::ifstream s_file{shader_src_filepath};
    if (s_file.is_open()) {
        std::string line;
        while (std::getline(s_file, line)) {
            src.src += (line + '\n');
        }
        s_file.close();
    } else {
        throw std::runtime_error("Unable to open shader source file: " + shader_src_filepath.string());
    }
    return src;
}
GLuint sdl_gl_shader::compile_shader(const shader_src &source) {
    const GLchar *src{source.src.c_str()};
    GLuint shader = glCreateShader(shader_gl_type.at(source.type));
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> error_log(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &error_log[0]);

        std::stringstream ss{};
        ss << "SHADER FAILED TO COMPILE:\n";
        for (auto &c : error_log) {
            ss << c;
        }

        glDeleteShader(shader);
        throw std::runtime_error(ss.str());
    }
    return shader;
}
GLuint sdl_gl_shader::create_program(const std::vector<shader_src> &sources) {
    std::vector<GLuint> shaders;
    for (const auto &src : sources) {
        shaders.push_back(compile_shader(src));
    }

    GLuint new_program = glCreateProgram();
    for (auto s : shaders) {
        glAttachShader(new_program, s);
    }
    glLinkProgram(new_program);

    GLint isLinked = 0;
    glGetProgramiv(new_program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(new_program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(new_program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(new_program);
        for (auto s : shaders) {
            glDeleteShader(s);
        }
        shaders.clear();

        std::stringstream ss{};
        ss << "SHADER PROGRAM FAILED TO LINK:\n";
        for (auto &c : infoLog) {
            ss << c;
        }

        throw std::runtime_error(ss.str());
    }

    // Always detach shaders after a successful link.
    for (auto s : shaders) {
        glDetachShader(new_program, s);
        glDeleteShader(s);
    }
    shaders.clear();
    return new_program;
}
void sdl_gl_shader::upload_mat4(const std::string &name, const squint::fmat4 &value, bool suppress_warnings) {
    if (!resource_location_cache.count(name)) {
        // cache the location
        resource_location_cache.insert_or_assign(name, glGetProgramResourceLocation(program, GL_UNIFORM, name.c_str()));
    }
    if (resource_location_cache[name] == -1 && suppress_warnings == false) {
        std::cerr << "SHADER WARNING: No uniform '" << name << "' exists in the shader" << std::endl;
    } else {
        glUniformMatrix4fv(resource_location_cache[name], 1, GL_FALSE, value.data());
    }
}
void sdl_gl_shader::upload_vec4(const std::string &name, const squint::fvec4 &value, bool suppress_warnings) {
    if (!resource_location_cache.count(name)) {
        // cache the location
        resource_location_cache.insert_or_assign(name, glGetProgramResourceLocation(program, GL_UNIFORM, name.c_str()));
    }
    if (resource_location_cache[name] == -1 && suppress_warnings == false) {
        std::cerr << "SHADER WARNING: No uniform '" << name << "' exists in the shader" << std::endl;
    } else {
        glUniform4f(resource_location_cache[name], value[0], value[1], value[2], value[3]);
    }
}
void sdl_gl_shader::upload_texture2D(const std::string &name, const texture2D *texture, bool suppress_warnings) {
    if (!texture) {
        // no data to upload
        return;
    }
    if (!resource_location_cache.count(name)) {
        // cache the location
        resource_location_cache.insert_or_assign(name, glGetProgramResourceLocation(program, GL_UNIFORM, name.c_str()));
        texture_binding_cache.insert_or_assign(name, static_cast<GLuint>(texture_binding_cache.size()));
        glUniform1i(resource_location_cache[name], texture_binding_cache[name]);
    }
    if (resource_location_cache[name] == -1 && suppress_warnings == false) {
        std::cerr << "SHADER WARNING: No uniform sampler2D '" << name << "' exists in the shader" << std::endl;
    } else {
        glBindTextureUnit(texture_binding_cache[name], texture->get_id());
    }
}
void sdl_gl_shader::upload_storage_buffer(const std::string &name, const buffer *ssbo, bool suppress_warnings) {
    if (!ssbo) {
        // no data to upload
        return;
    }
    if (!resource_location_cache.count(name)) {
        // cache the location
        resource_location_cache.insert_or_assign(
            name, glGetProgramResourceIndex(program, GL_SHADER_STORAGE_BLOCK, name.c_str()));
        storage_binding_cache.insert_or_assign(name, static_cast<GLuint>(storage_binding_cache.size()));
        glShaderStorageBlockBinding(program, resource_location_cache[name], storage_binding_cache[name]);
    }
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, storage_binding_cache[name], ssbo->get_id());
    if (resource_location_cache[name] == GL_INVALID_INDEX && suppress_warnings == false) {
        std::cerr << "SHADER WARNING: No storage buffer block '" << name << "' exists in the shader" << std::endl;
    }
}
uint32_t sdl_gl_shader::get_id() { return program; }
sdl_gl_texture2D::sdl_gl_texture2D(const std::filesystem::path &image_filepath) {
    SDL_Surface *surface = IMG_Load(image_filepath.string().c_str());
    if (surface == nullptr) {
        throw std::runtime_error("Could not read image " + image_filepath.string());
    }
    width = surface->w;
    height = surface->h;
    uint8_t channels = surface->format->BytesPerPixel;
    glCreateBuffers(1, &buffer_id);
    glNamedBufferStorage(buffer_id, width * height * channels, surface->pixels, 0);
    SDL_FreeSurface(surface);

    glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
    switch (channels) {
    case 1:
        tex_type = texture_type::R8;
        break;
    case 2:
        tex_type = texture_type::RG8;
        break;
    case 3:
        tex_type = texture_type::RGB8;
        break;
    case 4:
        tex_type = texture_type::RGBA8;
        break;
    }
    glTextureStorage2D(texture_id, 1, gl_sized_tex_format(tex_type), width, height);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_id);
    glTextureSubImage2D(texture_id, 0, 0, 0, width, height, gl_tex_format(tex_type), gl_tex_type(tex_type), nullptr);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
} // namespace square
