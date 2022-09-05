#include "lit/renderer.hpp"
#include <chrono>

namespace lit {
void renderer::load_object(object *obj) {
    if (active_object) {
        active_object->on_unload();
    }
    active_object = obj;
    if (active_object) {
        active_object->on_load();
    }
}

void renderer::run_step() {
    if (active_object) {
        activate_context();
        poll_events();
        static auto t1 = std::chrono::high_resolution_clock::now();
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1);
        squint::quantities::time_f dt{time_span.count()};
        t1 = t2;
        update(fixed_dt);
        render(dt);
        swap_buffers();
    }
}
void app::run() {
    while (renderers.size()) {
        int i = 0;
        bool renderer_exit = false;
        for (auto &r : renderers) {
            if (r->running) {
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
void renderer::update(squint::quantities::time_f dt) { active_object->update(dt); }
void renderer::render(squint::quantities::time_f dt) { active_object->render(dt); }
bool renderer::on_key(const key_event &event) { return active_object->on_key(event); }
bool renderer::on_mouse_button(const mouse_button_event &event) { return active_object->on_mouse_button(event); }
bool renderer::on_mouse_move(const mouse_move_event &event) { return active_object->on_mouse_move(event); }
bool renderer::on_mouse_wheel(const mouse_scroll_event &event) { return active_object->on_mouse_wheel(event); }
bool renderer::on_resize(const window_resize_event &event) { return active_object->on_resize(event); }
std::unique_ptr<renderer> app::detach_renderer(size_t i) {
    auto r = std::move(renderers[i]);
    r->on_exit();
    r->destroy_context();
    renderers.erase(renderers.begin() + i);
    return r;
}
void app::attach_renderer(std::unique_ptr<renderer> r) {
    r->create_context();
    r->on_enter();
    renderers.push_back(std::move(r));
}
void sdl_gl::create_context() {
    auto window_flag = fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(window_title, 0, 0, window_width, window_height, SDL_WINDOW_OPENGL | window_flag);
    window_id = SDL_GetWindowID(window);
    glcontext = SDL_GL_CreateContext(window);
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
                    running = false;
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
                running = false;
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
} // namespace lit