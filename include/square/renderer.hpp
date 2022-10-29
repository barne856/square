#ifndef SQUARE_RENDERER
#define SQUARE_RENDERER
#include "square/entity.hpp"
#include <cstring>
#include <filesystem>
#include <memory>

namespace square {
enum class cursor_type {
    DISABLED, // cursor will not be shown and only relative positions are returned from event callbacks
    ENABLED,  // cursor will be shown
};
enum class debug_mode {
    OFF,          // no debug messages will be returned
    HIGH,         // only high severity debug messages will be returned
    MED,          // only high or medium severity debug messages will be returned
    LOW,          // only high, medium, or low severity debug messages will be returned
    NOTIFICATION, // all debug messages will be returned
};
enum class buffer_access_type {
    STATIC = 0, /**< The buffer is written to once upon creation and the data
                   cannot be changed by any client-side (CPU) operation. Data must
                   be supplied at the time of creation. This is the default. */
    READ_ONLY,  /**< The buffer can be read from the client (CPU) but not written
                   to. Data must be supplied at the time of creation.*/
    WRITE_ONLY, /**< The buffer can be written to by the client (CPU) but not read
                   from.*/
    READ_WRITE, /**< The buffer can be read from and written to by the client
                   (CPU).*/
};
enum class buffer_attribute_type {
    POSITION_2D, /**< Represents 2D vertex data and is the equivalent of the glsl
                    input attribute `vec2`. Data in buffer will be interpreted as
                    two sequential 32-bit floating point numbers (x,y).*/
    POSITION_3D, /**< Represents 3D vertex data and is the equivalent of the glsl
                     input attribute `vec3`. Data in buffer will be interpreted as
                     three sequential 32-bit floating point numbers (x,y,z).*/
    NORMAL,      /**< Represents 3D normal data and is the equivalent of the glsl
                          input attribute `vec3`. Data in buffer will be interpreted as
                          three sequential 32-bit floating point numbers (x,y,z).*/
    COLOR,       /**< Represents RGBA color data and is the equivalent of the glsl input
                    attribute `vec4`. Data in the buffer will be interpreted as four
                    sequatial 32-bit floating point numbers (r,g,b,a).*/
    TEXTURE_MAP, /**< Represents 2D texture coordinate data and is the equivalent
                    of the glsl input attribute `vec2`. Data in buffer will be
                    interpreted as two sequential 32-bit floating point numbers
                    (u,v).*/
    UNIFORM,     /**< Represents arbitrary data and is the equivalent of the glsl
                    uniform block using the std140 layout. Data will be
                    interpreted as specified by the uniform block definition in the
                    shader.*/
    STORAGE      /**< Represetns arbitrary data and is the equivalent of the glsl
                    storage block (uniform buffer) using the std430 layout. Data will
                    be interpreted as specified by the storage block in the shader.*/
};
enum class draw_method {
    NONE,           /**< Signals no drawing will be done with this mesh.*/
    POINTS,         /**< The mesh's vertex buffers will be interpreted as points and each
                       vertex will be rendered individually.*/
    LINES,          /**< The mesh's vertex buffers will be interpreted as lines and each
                       pair of vertices will be rendered as a separate line.*/
    LINE_STRIP,     /**< The mesh's vertex bufferswill be interpreted as a strip of
                    lines where the first two vertices make a line and each vertex
                    following the first two extend the line.*/
    LINE_LOOP,      /**< Same as LINE_STRIP but the final vertex also
                       creates a line connecting to the first vertex in the vertex
                       buffer.*/
    TRIANGLES,      /**< Every three consecutive vertices in the mesh's vertex buffers
                       define a separate triangle to be rendered.*/
    TRIANGLE_STRIP, /**< The first three vertices in the mesh's vertex buffers
                       define a triangle and every additional vertex in the Vertex
                       Buffer defines another triangle consisting of that vertex
                       and the last two vertices in the vertex buffer. */
    TRIANGLE_FAN,   /**< The first three vertices in the mesh's vertex buffers
                      define a triangle and every additional vertex in the Vertex
                      Buffer defines another triangle consisting of that vertex
                      the previous vertex, and the first vertex in the vertex
                      buffer. */
};
enum class index_type {
    NONE,           // signals no index buffer exists on this mesh
    UNSIGNED_BYTE,  // 1 byte indices
    UNSIGNED_SHORT, // 2 byte indices
    UNSIGNED_INT,   // 4 byte indices
};
enum class texture_type {
    R8,      /**< The texture data is formated as single channel 8-bit bytes.*/
    RG8,     /**< The texture data is formated as two channel 8-bit bytes.*/
    RGB8,    /**< The texture data is formated as three channel 8-bit bytes.*/
    RGBA8,   /**< The texture data is formated as four channel 8-bit bytes.*/
    R32F,    /**< The texture data is formated as single channel 32-bit floating
                point numbers.*/
    RG32F,   /**< The texture data is formated as two channel 32-bit floating point
                numbers.*/
    RGB32F,  /**< The texture data is formated as three channel 32-bit floating
                point numbers.*/
    RGBA32F, /**< The texture data is formated as four channel 32-bit floating
                point numbers.*/
    DEPTH    /**< The texture data is formated as single channel 32-bit floating
                point numbers. And used to create textures of the depth values from a
                framebuffer.*/
};
// describes an attribute in a vertex buffer to corresponds to an input of a vertex shader
class buffer_attribute {
  public:
    // Construct a new buffer_attribute object.
    //
    // The buffer_attribute_type describes the data in the buffer. This must match the type of data in the shader
    // resource with the same name as attrib_name.
    //
    // the attrib_name is the name of the attribute used link the data in the buffer to a resource in the shader. This
    // must match the name of the resourse in the shader exactly.
    buffer_attribute(buffer_attribute_type attrib_type, const std::string& attrib_name)
        : type(attrib_type), name(attrib_name), offset(0), size(0) {}
    ~buffer_attribute(){};
    // Returns the number of components (or dimensions) in an attribute. The number of components is returned according
    // to the following table:
    //
    // AttributeType  | component_count()
    // -------------- | ---------------
    // POSITION_2D    | 2
    // POSITION_3D    | 3
    // NORMAL         | 3
    // COLOR          | 4
    // TEXTURE_MAP    | 2
    // UNIFORM        | 1
    // STORAGE        | 1
    const size_t component_count() const {
        switch (type) {
        case buffer_attribute_type::POSITION_2D:
            return 2;
            break;
        case buffer_attribute_type::POSITION_3D:
            return 3;
            break;
        case buffer_attribute_type::NORMAL:
            return 3;
            break;
        case buffer_attribute_type::COLOR:
            return 4;
            break;
        case buffer_attribute_type::TEXTURE_MAP:
            return 2;
            break;
        default:
            return 1;
            break;
        }
    }
    buffer_attribute_type type; /**< The type of the data in the buffer.*/
    std::string name;           /**< The name of the attribute used for uploading to a
                                   shader resource.*/
    size_t offset;              /**< The offset in bytes into the buffer where the first
                                   element of this data begins.*/
    size_t size;                /**< The size in bytes of a single element of this data.*/
};
// a collection of buffer attributes used to link a vertex buffer to the inputs of a vertex shader
class buffer_format {
  public:
    buffer_format(const std::initializer_list<buffer_attribute> &attribs) : stride(0), attributes(attribs) {
        size_t offset = 0;
        for (auto &attrib : attributes) {
            attrib.offset = offset;
            offset += attrib.component_count();
        }
        stride = offset;
    }
    inline const std::vector<buffer_attribute> &get_attributes() const { return attributes; }
    inline const size_t get_stride() const { return stride; }

  private:
    std::vector<buffer_attribute> attributes;
    size_t stride; /**< The stride in elements between attributes of the same type in
                  the buffer. The total size of all the buffer attributes.*/
};

// These properties can be set when constructing a renderer
struct renderer_properties {
    const char *window_title = "untitled";
    uint64_t window_width = 1280;
    uint64_t window_height = 720;
    int samples = 1;
    bool wireframe = false;
    bool fullscreen = false;
    bool vsync = false;
    cursor_type cursor = cursor_type::ENABLED;
    debug_mode debug = debug_mode::NOTIFICATION;
    bool running = true;
    squint::quantities::time_f fixed_dt{1.f / 60.f};
};
// forward declaring these so we can work with them in the renderer and app classes
class app;
class shader;
class buffer;
class mesh;
class vertex_input_assembly;
class texture2D;
// This is an abstract base class for renderers that is implemented by rendering APIs.
//
// A renderer acts as the root object in an application. Loading an object in a renderer sets the active scene
// for the renderer. A renderer represents a renderable window or context to render into.
class renderer : public object {
    friend class app;

  public:
    inline void exit() { properties.running = false; }
    // api commands (mostly self explanitory see documentaiton for the rendering api for details on that api)
    virtual void clear_color_buffer(squint::fvec4 color) = 0;
    virtual void wireframe_mode(bool enable) = 0;
    virtual void clear_depth_buffer() = 0;
    virtual void enable_face_culling(bool enable) = 0;
    virtual void enable_depth_testing(bool enable) = 0;
    virtual void enable_blending(bool enable) = 0;
    virtual std::unique_ptr<shader> gen_shader(const std::filesystem::path &shader_src_directory) = 0;
    inline const renderer_properties &get_properties() const { return properties; }
    template <typename T>
    std::unique_ptr<buffer> gen_buffer(const std::vector<T> &data, const buffer_format &format,
                                       buffer_access_type type) {
        return gen_buffer(static_cast<const void *>(data.data()), data.size() * sizeof(T), data.size(), format, type);
    }
    virtual std::unique_ptr<buffer> gen_buffer(const void *data, const size_t size_in_bytes, const size_t num_elements,
                                               const buffer_format &format, const buffer_access_type type) = 0;
    virtual std::unique_ptr<texture2D> gen_texture(const std::filesystem::path &image_filepath) = 0;
    virtual std::unique_ptr<vertex_input_assembly> gen_vertex_input_assembly(index_type type) = 0;
    virtual void draw_mesh(mesh *m, draw_method method) = 0;
    virtual void set_viewport(size_t x, size_t y, size_t width, size_t height) = 0;
    virtual void set_cursor(cursor_type type) = 0;
    virtual ~renderer(){};

  private:
    void run_step();
    object *active_object = nullptr;

  protected:
    // create the context. This will be final in renderer impl
    virtual void create_context() = 0;
    // see what input events have happened
    virtual void poll_events() = 0;
    // cleanup the context
    virtual void destroy_context() = 0;
    // set this as the current context
    virtual void activate_context() = 0;
    // swap front and back buffers for use in double buffer rendering
    virtual void swap_buffers() = 0;

    // event, render, and physics callbacks
    virtual void update(squint::quantities::time_f dt) override final;
    virtual void render(squint::quantities::time_f dt) override final;
    virtual bool on_key(const key_event &event) override final;
    virtual bool on_mouse_button(const mouse_button_event &event) override final;
    virtual bool on_mouse_move(const mouse_move_event &event) override final;
    virtual bool on_mouse_wheel(const mouse_scroll_event &event) override final;
    virtual bool on_resize(const window_resize_event &event) override final;

    // load the root object (most likely a scene)
    void load_object(object *obj);
    renderer_properties properties{};
};
// An application containing one or more renderers.
//
// This is a singleton class that is used to run the program. Rendering is orchestrated by the app and multiple
// renderers can be attached at a time and can be of different rendering APIs.
class app {
  private:
    app() : active_renderer_ptr(nullptr) {}
    std::vector<std::unique_ptr<renderer>> renderers{};
    renderer *active_renderer_ptr;

  public:
    // this is a singleton class so it should never be copied or moved
    app(const app &) = delete;
    app(app &&) = delete;
    app &operator=(const app &) = delete;
    app &operator=(app &&) = delete;
    static app &instance() {
        static app INSTANCE;
        return INSTANCE;
    }
    // get the active renderer. This is used by entities inside a renderer so that they can run commands from the
    // renderer they are being rendered with
    inline renderer *active_renderer() { return active_renderer_ptr; }
    inline const std::vector<std::unique_ptr<renderer>> &get_renderers() { return renderers; }
    template <typename U, typename... Args> void gen_renderer(Args... args) {
        attach_renderer(std::make_unique<U>(args...));
    }
    std::unique_ptr<renderer> detach_renderer(size_t i) {
        auto r = std::move(renderers[i]);
        active_renderer_ptr = r.get();
        r->on_exit();
        r->destroy_context();
        renderers.erase(renderers.begin() + i);
        active_renderer_ptr = nullptr;
        return r;
    }
    // all attached renderers will be rendered
    void attach_renderer(std::unique_ptr<renderer> r) {
        active_renderer_ptr = r.get();
        r->create_context();
        r->on_enter();
        renderers.push_back(std::move(r));
        active_renderer_ptr = nullptr;
    }
    // main program loop. This will loop as long as there are renderers attached.
    // renderers are automatically detached if an exit signal is sent from a renderer
    void run() {
        while (renderers.size()) {
            int i = 0;
            bool renderer_exit = false;
            for (auto &r : renderers) {
                if (r->properties.running) {
                    active_renderer_ptr = r.get();
                    r->run_step();
                    active_renderer_ptr = nullptr;
                } else {
                    renderer_exit = true;
                    break;
                }
                i++;
            }
            if (renderer_exit) {
                detach_renderer(i);
            }
        }
    }
};
// An abstract base class for shaders
class shader {
  public:
    virtual void activate() = 0;
    virtual ~shader() {}
    virtual void upload_mat4(const std::string &name, const squint::fmat4 &value, bool suppress_warnings = false) = 0;
    virtual void upload_vec4(const std::string &name, const squint::fvec4 &value, bool suppress_warnings = false) = 0;
    virtual void upload_texture2D(const std::string &name, texture2D *tex, bool supress_warnings = false) = 0;
    virtual uint32_t get_id() = 0;
};
// An abstract base class for buffers
class buffer {
  public:
    buffer(const buffer_format &format, buffer_access_type type, size_t num_elements)
        : format(format), type(type), num_elements(num_elements), buffer_ptr(nullptr) {}
    template <typename T> std::vector<T> get_elements() {
        std::vector<T> result{};
        if (buffer_ptr) {
            result.resize(num_elements);
            std::memcpy(static_cast<void *>(result.data()), buffer_ptr, sizeof(T) * num_elements);
        }
        return result;
    }
    virtual ~buffer(){};
    inline const buffer_format &get_format() const { return format; }
    virtual const uint32_t get_id() const = 0;
    virtual const size_t size() const = 0;

  protected:
    buffer_format format;
    buffer_access_type type;
    void *buffer_ptr;
    size_t num_elements;
};
// An abstract base class for 2D textures. These are used in shaders with the sampler2D uniform
class texture2D : public buffer {
  public:
    texture2D() : buffer({}, buffer_access_type::STATIC, 0) {}
    virtual ~texture2D(){};
};
// An abstract base class for vertex input assembly
//
// This organizes a set of vertex buffers and possibly an index buffer such that they can be rendered together. Also
// binds the buffer attributes to buffer binding points.
// Note: multiple vertex buffers can be attached, but only one index buffer can be attached at a time.
class vertex_input_assembly {
  public:
    vertex_input_assembly(index_type type) : type(type) {}
    // links the buffer attributes to the vertex shader inputs
    virtual void bind_shader(shader *s) = 0;
    virtual void activate() const = 0;
    void add_vertex_buffer(std::unique_ptr<buffer> vertex_buffer) {
        vertex_buffers.push_back(std::move(vertex_buffer));
    }
    void set_index_buffer(std::unique_ptr<buffer> index_buffer) { this->index_buffer = std::move(index_buffer); }
    inline const std::vector<std::unique_ptr<buffer>> &get_vertex_buffers() const { return vertex_buffers; }
    inline const buffer *get_index_buffer() const { return index_buffer.get(); }
    inline index_type get_index_type() const { return type; }
    virtual ~vertex_input_assembly(){};

  protected:
    std::vector<std::unique_ptr<buffer>> vertex_buffers;
    std::unique_ptr<buffer> index_buffer;
    index_type type;
};
} // namespace square
#endif