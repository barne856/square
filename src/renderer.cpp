#include "lit/renderer.hpp"
#include <chrono>

namespace lit {
void renderer::load_object(object *obj) {
    if (root_object) {
        root_object->on_unload();
    }
    root_object = obj;
    if (root_object) {
        root_object->on_load();
    }
}
void renderer::run() {
    create_context();
    startup();
    // TODO change float dt to `time` dt
    auto t1 = std::chrono::high_resolution_clock::now();
    while (running) {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1);
        float dt = time_span.count();
        current_time += dt;
        if (root_object) {
            update(dt);
            render(dt);
            on_key(input);
            on_mouse_button(input);
            on_mouse_move(input);
            on_mouse_wheel(input);
            on_resize(input);
        }
        poll_events(); // poll events and check if window should close
    }
    shutdown();
    destroy_context();
}
void renderer::update(float dt) { root_object->update(dt); }
void renderer::render(float dt) { root_object->render(dt); }
bool renderer::on_key(const renderer_input &input) { return root_object->on_key(input); }
bool renderer::on_mouse_button(const renderer_input &input) { return root_object->on_mouse_button(input); }
bool renderer::on_mouse_move(const renderer_input &input) { return root_object->on_mouse_move(input); }
bool renderer::on_mouse_wheel(const renderer_input &input) { return root_object->on_mouse_wheel(input); }
bool renderer::on_resize(const renderer_input &input) { return root_object->on_resize(input); }
void gl::create_context() {
    auto window_flag = fullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow(window_title, 0, 0, window_width, window_height, SDL_WINDOW_OPENGL | window_flag);
    glcontext = SDL_GL_CreateContext(window);
}
void gl::poll_events() {
    SDL_GL_SwapWindow(window);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
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

        default:
            break;
        }
    }
}
void gl::destroy_context() { SDL_GL_DeleteContext(glcontext); }
} // namespace lit