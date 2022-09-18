#include "lit/sdl_gl_renderer.hpp"
#include <fstream>
namespace lit {
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
void GLAPIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                       const GLchar *message, const void *userParam) {
    auto debug_state = static_cast<const sdl_gl *>(userParam)->get_properties().debug;
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
void sdl_gl::create_context() {
    auto window_flag = properties.fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(properties.window_title, 0, 0, properties.window_width, properties.window_height,
                              SDL_WINDOW_OPENGL | window_flag);
    window_id = SDL_GetWindowID(window);
    glcontext = SDL_GL_CreateContext(window);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Error GLEW failed to initalize:\n" +
                                 std::string(reinterpret_cast<const char *>(glewGetErrorString(err))) + "\n");
    }
    if (properties.debug != debug_mode::OFF) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debug_message_callback, this);
        if (properties.debug == debug_mode::NOTIFICATION) {
            std::cout << "GL VENDOR: " << glGetString(GL_VENDOR) << "\n";
            std::cout << "GL VERSION: " << glGetString(GL_VERSION) << "\n";
            std::cout << "GL RENDERER: " << glGetString(GL_RENDERER) << std::endl;
        }
    }
}
void sdl_gl::poll_events() {
    std::vector<SDL_Event> unhandled_events{};
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.window.windowID == window_id) {
            switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                    properties.running = false;
                    break;

                default:
                    break;
                }
                break;
            case SDL_KEYUP:
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
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
                case SDLK_ESCAPE:
                    on_key(key_event::ESCAPE_DOWN);
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                properties.running = false;
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
void sdl_gl::destroy_context() {
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
}
void sdl_gl::activate_context() { SDL_GL_MakeCurrent(window, glcontext); }
void sdl_gl::swap_buffers() { SDL_GL_SwapWindow(window); }

void sdl_gl::clear_color_buffer(squint::fvec4 color) {
    glClearColor(color[0], color[1], color[2], color[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}
std::unique_ptr<shader> sdl_gl::gen_shader(const std::filesystem::path &shader_src_directory) {
    return std::make_unique<sdl_gl_shader>(shader_src_directory);
}
sdl_gl_shader::sdl_gl_shader(const std::filesystem::path &shader_src_folder) {
    program = create_program(shader_src_folder);
}
sdl_gl_shader::~sdl_gl_shader() {
    glDeleteProgram(program); // Silently ignored if program is 0
}
void sdl_gl_shader::activate() { glUseProgram(program); }
sdl_gl_shader::shader_source sdl_gl_shader::read_shader(const std::filesystem::path &shader_src_filepath) {
    shader_source src{};
    src.shader_type = shader_ext_type.at(shader_src_filepath.extension().string());
    std::ifstream s_file{shader_src_filepath};
    if (s_file.is_open()) {
        std::string line;
        while (std::getline(s_file, line)) {
            src.source += (line + '\n');
        }
        s_file.close();
    } else {
        throw std::runtime_error("Unable to open shader source file: " + shader_src_filepath.string());
    }
    return src;
}
GLuint sdl_gl_shader::compile_shader(const std::filesystem::path &shader_src_filepath) {
    const auto shader_src = read_shader(shader_src_filepath);
    const GLchar *src{shader_src.source.c_str()};
    GLuint shader = glCreateShader(shader_src.shader_type);
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
        ss << shader_src_filepath << " SHADER FAILED TO COMPILE:\n";
        for (auto &c : error_log) {
            ss << c;
        }

        glDeleteShader(shader);
        throw std::runtime_error(ss.str());
    }
    return shader;
}
GLuint sdl_gl_shader::create_program(const std::filesystem::path &shader_src_folder) {
    std::vector<GLuint> shaders;
    for (const auto &src_file : std::filesystem::directory_iterator(shader_src_folder)) {
        shaders.push_back(compile_shader(src_file));
    }

    GLuint program = glCreateProgram();
    for (auto s : shaders) {
        glAttachShader(program, s);
    }
    glLinkProgram(program);

    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
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
        glDetachShader(program, s);
        glDeleteShader(s);
    }
    shaders.clear();
    return program;
}
void sdl_gl_shader::upload_mat4(const char *name, squint::fmat4 value, bool suppress_warnings) {
    if (!resource_location_cache.count(name)) {
        // cache the location
        resource_location_cache.insert_or_assign(name, glGetProgramResourceLocation(program, GL_UNIFORM, name));
    }
    if (resource_location_cache[name] == -1 && suppress_warnings == false) {
        std::cerr << "SHADER WARNING: No uniform '" << name << "' exists in the shader" << std::endl;
    } else {
        glUniformMatrix4fv(resource_location_cache[name], 1, GL_FALSE, value.data());
    }
}
void sdl_gl_shader::upload_vec4(const char *name, squint::fvec4 value, bool suppress_warnings) {
    if (!resource_location_cache.count(name)) {
        // cache the location
        resource_location_cache.insert_or_assign(name, glGetProgramResourceLocation(program, GL_UNIFORM, name));
    }
    if (resource_location_cache[name] == -1 && suppress_warnings == false) {
        std::cerr << "SHADER WARNING: No uniform '" << name << "' exists in the shader" << std::endl;
    } else {
        glUniform4f(resource_location_cache[name], value[0], value[1], value[2], value[3]);
    }
}
} // namespace lit