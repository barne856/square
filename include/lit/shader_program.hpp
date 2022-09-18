#ifndef LIT_SHADER
#define LIT_SHADER

#include "squint/linalg.hpp"
#include <filesystem>
#include <fstream>
#include <unordered_map>

namespace lit {
enum class barrier_type {
    ATTRIBUTE,      /**< Specifies vertex data supplied as input attributes to
                       shaders.*/
    INDEX,          /**< Specifies index data from Index Buffers.*/
    UNIFORM,        /**< Specifies uniforms sourced from Buffers.*/
    TEXTURE_FETCH,  /**< Specifies textures sourced from Buffers.*/
    IMAGE,          /**< Specifies images sourced from Buffers.*/
    COMMAND,        /**< Specifies commands sourced from Buffers.*/
    BUFFER_UPDATE,  /**< Specifies reads or writes to any Buffer object.*/
    FRAMEBUFFER,    /**< Specifies framebuffers sourced from Buffers.*/
    ATOMIC_COUNTER, /**< Specifies accesses to atomic counters.*/
    STORAGE,        /**< Specifies Shader Storage Buffers objects.*/
    ALL             /**< Specifies any Buffer operation or modification.*/
};
enum class shader_type {
    VERTEX,
    TESSELATION_CONTROL,
    TESSELATION_EVALUATION,
    GEOMETRY,
    FRAGMENT,
    COMPUTE,
    INVALID_TYPE
};
const std::unordered_map<std::string, shader_type> shader_extension{{".vert", shader_type::VERTEX},
                                                                    {".tesc", shader_type::TESSELATION_CONTROL},
                                                                    {".tese", shader_type::TESSELATION_EVALUATION},
                                                                    {".geom", shader_type::GEOMETRY},
                                                                    {".frag", shader_type::FRAGMENT},
                                                                    {".comp", shader_type::COMPUTE}};
template <typename T>
concept buffer_like = requires(T t) {
    t.name();
};
class Texture2D;
class shader_program {
  public:
    shader_program(const std::filesystem::path &shader_directory) : program_ID{0} {
        for (const auto &file : std::filesystem::directory_iterator(shader_directory)) {
            std::string source{};
            read_shader(file, source);
            shaders.push_back(app::instance().active_renderer()->compile_shader(
                source, shader_extension.at(shader_directory.extension().string())));
        }
        program_ID = app::instance().active_renderer()->create_program(shaders);
    }
    ~shader_program() {}
    inline int ID() { return program_ID; }
    void activate() const { app::instance().active_renderer()->shader_program_activate(program_ID); }
    void upload_int(const char *name, int value) {
        app::instance().active_renderer()->shader_program_upload_int(name, value);
    }
    void upload_float(const char *name, float value) {
        app::instance().active_renderer()->shader_program_upload_float(name, value);
    }
    void upload_vec2(const char *name, squint::fvec2 value) {
        app::instance().active_renderer()->shader_program_upload_vec2(name, value);
    }
    void upload_vec3(const char *name, squint::fvec3 value) {
        app::instance().active_renderer()->shader_program_upload_vec3(name, value);
    }
    void upload_vec4(const char *name, squint::fvec4 value) {
        app::instance().active_renderer()->shader_program_upload_vec4(name, value);
    }
    void upload_mat3(const char *name, squint::fmat3 value) {
        app::instance().active_renderer()->shader_program_upload_mat3(name, value);
    }
    void upload_mat4(const char *name, squint::fmat4 value) {
        app::instance().active_renderer()->shader_program_upload_mat4(name, value);
    }
    template <buffer_like B> void upload_uniform(const char *name, B *uniform) {
        app::instance().active_renderer()->shader_program_upload_uniform(name, uniform);
    }
    template <buffer_like B> void upload_storage(const char *name, B *storage) {
        app::instance().active_renderer()->shader_program_upload_storage(name, storage);
    }
    void upload_texture2D(const char *name, Texture2D *texture2D) {
        app::instance().active_renderer()->shader_program_upload_storage(name, storage);
    }
    void upload_image2D(const char *name, Texture2D *texture2D) {
        app::instance().active_renderer()->shader_program_upload_storage(name, storage);
    }
    void barrier(barrier_type type);

  private:
    int program_ID;
    std::vector<uint32_t> shaders;
    void read_shader(const std::filesystem::path &shader_filepath, std::string &source) {
        std::ifstream s_file{shader_filepath};
        if (s_file.is_open()) {
            std::string line;
            while (std::getline(s_file, line)) {
                source += (line + '\n');
            }
            s_file.close();
        } else {
            std::cerr << "Unable to open shader source file: " << shader_filepath << std::endl;
        }
    }
};
} // namespace lit

#endif