#ifndef LIT_BASIC_TEXTURE
#define LIT_BASIC_TEXTURE

#include "lit/entities/camera.hpp"
#include "lit/entity.hpp"
#include "lit/renderer.hpp"
#include "lit/system.hpp"
using namespace squint::quantities;

namespace lit {
template <typename T> class basic_texture_render_system : public render_system<T> {
  public:
    void render(time_f dt, T &entity) const override {
        if (entity.cam) {
            if (auto s = entity.basic_texture_shader.get()) {
                s->activate();
                s->upload_mat4("projection", entity.cam->get_projection_matrix());
                s->upload_mat4("view", entity.cam->get_view_matrix());
            }
        }
    }
};
class basic_texture : public entity<basic_texture> {
  public:
    basic_texture(camera *cam) : cam(cam) {
        // this render system will activate the material for use in renderable child objects
        gen_render_system<basic_texture_render_system>();
    }
    void on_enter() override {
        basic_texture_shader = std::move(app::instance().active_renderer()->gen_shader("../shaders/basic_texture"));
    }
    void on_exit() override {}
    camera *cam;
    std::unique_ptr<shader> basic_texture_shader;
};
} // namespace lit
#endif