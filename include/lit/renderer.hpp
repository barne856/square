#ifndef LIT_RENDERER
#define LIT_RENDERER
#include "lit/entity.hpp"
#include <GL/gl.h>
#include <SDL.h>

namespace lit {

enum class cursor_type {
    DISABLED,
    ENABLED,
    ARROW,
    HZ_RESIZE,
    HAND,
    CROSSHAIRS,
};
enum class debug_mode {
    OFF,
    HIGH,
    MED,
    LOW,
    NOTIFICATION,
};
struct renderer_input {
    short mouse_scroll{}; /**< 0 == no scroll, 1 == scroll up, -1 = scroll down*/
    // glm::ivec2 mouse_pos{}; /**< window coordinates of mouse position (in pixels
    //                            from top left corner)*/
    // glm::ivec2 mouse_vel{}; /**< window coordinates of mouse velocity (in
    //                            pixels/frame from top left corner)*/
    // Mouse clicks
    bool LEFT_MOUSE_PRESSED{false};
    bool MIDDLE_MOUSE_PRESSED{false};
    bool RIGHT_MOUSE_PRESSED{false};
    bool LEFT_MOUSE_JUST_PRESSED{false};
    bool MIDDLE_MOUSE_JUST_PRESSED{false};
    bool RIGHT_MOUSE_JUST_PRESSED{false};
    bool LEFT_MOUSE_JUST_RELEASED{false};
    bool MIDDLE_MOUSE_JUST_RELEASED{false};
    bool RIGHT_MOUSE_JUST_RELEASED{false};
    // Key presses
    bool SPACE_PRESSED{false};
    bool SPACE_JUST_PRESSED{false};
    bool SPACE_JUST_RELEASED{false};
    bool APOSTROPHE_PRESSED{false};
    bool APOSTROPHE_JUST_PRESSED{false};
    bool APOSTROPHE_JUST_RELEASED{false};
    bool COMMA_PRESSED{false};
    bool COMMA_JUST_PRESSED{false};
    bool COMMA_JUST_RELEASED{false};
    bool MINUS_PRESSED{false};
    bool MINUS_JUST_PRESSED{false};
    bool MINUS_JUST_RELEASED{false};
    bool PERIOD_PRESSED{false};
    bool PERIOD_JUST_PRESSED{false};
    bool PERIOD_JUST_RELEASED{false};
    bool FORWARD_SLASH_PRESSED{false};
    bool FORWARD_SLASH_JUST_PRESSED{false};
    bool FORWARD_SLASH_JUST_RELEASED{false};
    bool ZERO_PRESSED{false};
    bool ZERO_JUST_PRESSED{false};
    bool ZERO_JUST_RELEASED{false};
    bool ONE_PRESSED{false};
    bool ONE_JUST_PRESSED{false};
    bool ONE_JUST_RELEASED{false};
    bool TWO_PRESSED{false};
    bool TWO_JUST_PRESSED{false};
    bool TWO_JUST_RELEASED{false};
    bool THREE_PRESSED{false};
    bool THREE_JUST_PRESSED{false};
    bool THREE_JUST_RELEASED{false};
    bool FOUR_PRESSED{false};
    bool FOUR_JUST_PRESSED{false};
    bool FOUR_JUST_RELEASED{false};
    bool FIVE_PRESSED{false};
    bool FIVE_JUST_PRESSED{false};
    bool FIVE_JUST_RELEASED{false};
    bool SIX_PRESSED{false};
    bool SIX_JUST_PRESSED{false};
    bool SIX_JUST_RELEASED{false};
    bool SEVEN_PRESSED{false};
    bool SEVEN_JUST_PRESSED{false};
    bool SEVEN_JUST_RELEASED{false};
    bool EIGHT_PRESSED{false};
    bool EIGHT_JUST_PRESSED{false};
    bool EIGHT_JUST_RELEASED{false};
    bool NINE_PRESSED{false};
    bool NINE_JUST_PRESSED{false};
    bool NINE_JUST_RELEASED{false};
    bool SEMICOLON_PRESSED{false};
    bool SEMICOLON_JUST_PRESSED{false};
    bool SEMICOLON_JUST_RELEASED{false};
    bool EQUAL_PRESSED{false};
    bool EQUAL_JUST_PRESSED{false};
    bool EQUAL_JUST_RELEASED{false};
    bool A_PRESSED{false};
    bool A_JUST_PRESSED{false};
    bool A_JUST_RELEASED{false};
    bool B_PRESSED{false};
    bool B_JUST_PRESSED{false};
    bool B_JUST_RELEASED{false};
    bool C_PRESSED{false};
    bool C_JUST_PRESSED{false};
    bool C_JUST_RELEASED{false};
    bool D_PRESSED{false};
    bool D_JUST_PRESSED{false};
    bool D_JUST_RELEASED{false};
    bool E_PRESSED{false};
    bool E_JUST_PRESSED{false};
    bool E_JUST_RELEASED{false};
    bool F_PRESSED{false};
    bool F_JUST_PRESSED{false};
    bool F_JUST_RELEASED{false};
    bool G_PRESSED{false};
    bool G_JUST_PRESSED{false};
    bool G_JUST_RELEASED{false};
    bool H_PRESSED{false};
    bool H_JUST_PRESSED{false};
    bool H_JUST_RELEASED{false};
    bool I_PRESSED{false};
    bool I_JUST_PRESSED{false};
    bool I_JUST_RELEASED{false};
    bool J_PRESSED{false};
    bool J_JUST_PRESSED{false};
    bool J_JUST_RELEASED{false};
    bool K_PRESSED{false};
    bool K_JUST_PRESSED{false};
    bool K_JUST_RELEASED{false};
    bool L_PRESSED{false};
    bool L_JUST_PRESSED{false};
    bool L_JUST_RELEASED{false};
    bool M_PRESSED{false};
    bool M_JUST_PRESSED{false};
    bool M_JUST_RELEASED{false};
    bool N_PRESSED{false};
    bool N_JUST_PRESSED{false};
    bool N_JUST_RELEASED{false};
    bool O_PRESSED{false};
    bool O_JUST_PRESSED{false};
    bool O_JUST_RELEASED{false};
    bool P_PRESSED{false};
    bool P_JUST_PRESSED{false};
    bool P_JUST_RELEASED{false};
    bool Q_PRESSED{false};
    bool Q_JUST_PRESSED{false};
    bool Q_JUST_RELEASED{false};
    bool R_PRESSED{false};
    bool R_JUST_PRESSED{false};
    bool R_JUST_RELEASED{false};
    bool S_PRESSED{false};
    bool S_JUST_PRESSED{false};
    bool S_JUST_RELEASED{false};
    bool T_PRESSED{false};
    bool T_JUST_PRESSED{false};
    bool T_JUST_RELEASED{false};
    bool U_PRESSED{false};
    bool U_JUST_PRESSED{false};
    bool U_JUST_RELEASED{false};
    bool V_PRESSED{false};
    bool V_JUST_PRESSED{false};
    bool V_JUST_RELEASED{false};
    bool W_PRESSED{false};
    bool W_JUST_PRESSED{false};
    bool W_JUST_RELEASED{false};
    bool X_PRESSED{false};
    bool X_JUST_PRESSED{false};
    bool X_JUST_RELEASED{false};
    bool Y_PRESSED{false};
    bool Y_JUST_PRESSED{false};
    bool Y_JUST_RELEASED{false};
    bool Z_PRESSED{false};
    bool Z_JUST_PRESSED{false};
    bool Z_JUST_RELEASED{false};
    bool LEFT_BRACKET_PRESSED{false};
    bool LEFT_BRACKET_JUST_PRESSED{false};
    bool LEFT_BRACKET_JUST_RELEASED{false};
    bool RIGHT_BRACKET_PRESSED{false};
    bool RIGHT_BRACKET_JUST_PRESSED{false};
    bool RIGHT_BRACKET_JUST_RELEASED{false};
    bool GRAVE_ACCENT_PRESSED{false};
    bool GRAVE_ACCENT_JUST_PRESSED{false};
    bool GRAVE_ACCENT_JUST_RELEASED{false};
    bool ESCAPE_PRESSED{false};
    bool ESCAPE_JUST_PRESSED{false};
    bool ESCAPE_JUST_RELEASED{false};
    bool ENTER_PRESSED{false};
    bool ENTER_JUST_PRESSED{false};
    bool ENTER_JUST_RELEASED{false};
    bool TAB_PRESSED{false};
    bool TAB_JUST_PRESSED{false};
    bool TAB_JUST_RELEASED{false};
    bool BACKSPACE_PRESSED{false};
    bool BACKSPACE_JUST_PRESSED{false};
    bool BACKSPACE_JUST_RELEASED{false};
    bool INSERT_PRESSED{false};
    bool INSERT_JUST_PRESSED{false};
    bool INSERT_JUST_RELEASED{false};
    bool DELETE_PRESSED{false};
    bool DELETE_JUST_PRESSED{false};
    bool DELETE_JUST_RELEASED{false};
    bool RIGHT_PRESSED{false};
    bool RIGHT_JUST_PRESSED{false};
    bool RIGHT_JUST_RELEASED{false};
    bool LEFT_PRESSED{false};
    bool LEFT_JUST_PRESSED{false};
    bool LEFT_JUST_RELEASED{false};
    bool UP_PRESSED{false};
    bool UP_JUST_PRESSED{false};
    bool UP_JUST_RELEASED{false};
    bool DOWN_PRESSED{false};
    bool DOWN_JUST_PRESSED{false};
    bool DOWN_JUST_RELEASED{false};
    bool PAGE_UP_PRESSED{false};
    bool PAGE_UP_JUST_PRESSED{false};
    bool PAGE_UP_JUST_RELEASED{false};
    bool PAGE_DOWN_PRESSED{false};
    bool PAGE_DOWN_JUST_PRESSED{false};
    bool PAGE_DOWN_JUST_RELEASED{false};
    bool HOME_PRESSED{false};
    bool HOME_JUST_PRESSED{false};
    bool HOME_JUST_RELEASED{false};
    bool END_PRESSED{false};
    bool END_JUST_PRESSED{false};
    bool END_JUST_RELEASED{false};
    bool CAPS_LOCK_PRESSED{false};
    bool CAPS_LOCK_JUST_PRESSED{false};
    bool CAPS_LOCK_JUST_RELEASED{false};
    bool SCROLL_LOCK_PRESSED{false};
    bool SCROLL_LOCK_JUST_PRESSED{false};
    bool SCROLL_LOCK_JUST_RELEASED{false};
    bool NUM_LOCK_PRESSED{false};
    bool NUM_LOCK_JUST_PRESSED{false};
    bool NUM_LOCK_JUST_RELEASED{false};
    bool PRINT_SCREEN_PRESSED{false};
    bool PRINT_SCREEN_JUST_PRESSED{false};
    bool PRINT_SCREEN_JUST_RELEASED{false};
    bool PAUSE_PRESSED{false};
    bool PAUSE_JUST_PRESSED{false};
    bool PAUSE_JUST_RELEASED{false};
    bool F1_PRESSED{false};
    bool F1_JUST_PRESSED{false};
    bool F1_JUST_RELEASED{false};
    bool F2_PRESSED{false};
    bool F2_JUST_PRESSED{false};
    bool F2_JUST_RELEASED{false};
    bool F3_PRESSED{false};
    bool F3_JUST_PRESSED{false};
    bool F3_JUST_RELEASED{false};
    bool F4_PRESSED{false};
    bool F4_JUST_PRESSED{false};
    bool F4_JUST_RELEASED{false};
    bool F5_PRESSED{false};
    bool F5_JUST_PRESSED{false};
    bool F5_JUST_RELEASED{false};
    bool F6_PRESSED{false};
    bool F6_JUST_PRESSED{false};
    bool F6_JUST_RELEASED{false};
    bool F7_PRESSED{false};
    bool F7_JUST_PRESSED{false};
    bool F7_JUST_RELEASED{false};
    bool F8_PRESSED{false};
    bool F8_JUST_PRESSED{false};
    bool F8_JUST_RELEASED{false};
    bool F9_PRESSED{false};
    bool F9_JUST_PRESSED{false};
    bool F9_JUST_RELEASED{false};
    bool F10_PRESSED{false};
    bool F10_JUST_PRESSED{false};
    bool F10_JUST_RELEASED{false};
    bool F11_PRESSED{false};
    bool F11_JUST_PRESSED{false};
    bool F11_JUST_RELEASED{false};
    bool F12_PRESSED{false};
    bool F12_JUST_PRESSED{false};
    bool F12_JUST_RELEASED{false};
    bool KEY_PAD_0_PRESSED{false};
    bool KEY_PAD_0_JUST_PRESSED{false};
    bool KEY_PAD_0_JUST_RELEASED{false};
    bool KEY_PAD_1_PRESSED{false};
    bool KEY_PAD_1_JUST_PRESSED{false};
    bool KEY_PAD_1_JUST_RELEASED{false};
    bool KEY_PAD_2_PRESSED{false};
    bool KEY_PAD_2_JUST_PRESSED{false};
    bool KEY_PAD_2_JUST_RELEASED{false};
    bool KEY_PAD_3_PRESSED{false};
    bool KEY_PAD_3_JUST_PRESSED{false};
    bool KEY_PAD_3_JUST_RELEASED{false};
    bool KEY_PAD_4_PRESSED{false};
    bool KEY_PAD_4_JUST_PRESSED{false};
    bool KEY_PAD_4_JUST_RELEASED{false};
    bool KEY_PAD_5_PRESSED{false};
    bool KEY_PAD_5_JUST_PRESSED{false};
    bool KEY_PAD_5_JUST_RELEASED{false};
    bool KEY_PAD_6_PRESSED{false};
    bool KEY_PAD_6_JUST_PRESSED{false};
    bool KEY_PAD_6_JUST_RELEASED{false};
    bool KEY_PAD_7_PRESSED{false};
    bool KEY_PAD_7_JUST_PRESSED{false};
    bool KEY_PAD_7_JUST_RELEASED{false};
    bool KEY_PAD_8_PRESSED{false};
    bool KEY_PAD_8_JUST_PRESSED{false};
    bool KEY_PAD_8_JUST_RELEASED{false};
    bool KEY_PAD_9_PRESSED{false};
    bool KEY_PAD_9_JUST_PRESSED{false};
    bool KEY_PAD_9_JUST_RELEASED{false};
    bool KEY_PAD_DECIMAL_PRESSED{false};
    bool KEY_PAD_DECIMAL_JUST_PRESSED{false};
    bool KEY_PAD_DECIMAL_JUST_RELEASED{false};
    bool KEY_PAD_DIVIDE_PRESSED{false};
    bool KEY_PAD_DIVIDE_JUST_PRESSED{false};
    bool KEY_PAD_DIVIDE_JUST_RELEASED{false};
    bool KEY_PAD_MULTIPLY_PRESSED{false};
    bool KEY_PAD_MULTIPLY_JUST_PRESSED{false};
    bool KEY_PAD_MULTIPLY_JUST_RELEASED{false};
    bool KEY_PAD_SUBTRACT_PRESSED{false};
    bool KEY_PAD_SUBTRACT_JUST_PRESSED{false};
    bool KEY_PAD_SUBTRACT_JUST_RELEASED{false};
    bool KEY_PAD_ADD_PRESSED{false};
    bool KEY_PAD_ADD_JUST_PRESSED{false};
    bool KEY_PAD_ADD_JUST_RELEASED{false};
    bool KEY_PAD_ENTER_PRESSED{false};
    bool KEY_PAD_ENTER_JUST_PRESSED{false};
    bool KEY_PAD_ENTER_JUST_RELEASED{false};
    bool KEY_PAD_EQUAL_PRESSED{false};
    bool KEY_PAD_EQUAL_JUST_PRESSED{false};
    bool KEY_PAD_EQUAL_JUST_RELEASED{false};
    bool LEFT_SHIFT_PRESSED{false};
    bool LEFT_SHIFT_JUST_PRESSED{false};
    bool LEFT_SHIFT_JUST_RELEASED{false};
    bool LEFT_CONTROL_PRESSED{false};
    bool LEFT_CONTROL_JUST_PRESSED{false};
    bool LEFT_CONTROL_JUST_RELEASED{false};
    bool LEFT_ALT_PRESSED{false};
    bool LEFT_ALT_JUST_PRESSED{false};
    bool LEFT_ALT_JUST_RELEASED{false};
    bool LEFT_SUPER_PRESSED{false};
    bool LEFT_SUPER_JUST_PRESSED{false};
    bool LEFT_SUPER_JUST_RELEASED{false};
    bool RIGHT_SHIFT_PRESSED{false};
    bool RIGHT_SHIFT_JUST_PRESSED{false};
    bool RIGHT_SHIFT_JUST_RELEASED{false};
    bool RIGHT_CONTROL_PRESSED{false};
    bool RIGHT_CONTROL_JUST_PRESSED{false};
    bool RIGHT_CONTROL_JUST_RELEASED{false};
    bool RIGHT_ALT_PRESSED{false};
    bool RIGHT_ALT_JUST_PRESSED{false};
    bool RIGHT_ALT_JUST_RELEASED{false};
    bool RIGHT_SUPER_PRESSED{false};
    bool RIGHT_SUPER_JUST_PRESSED{false};
    bool RIGHT_SUPER_JUST_RELEASED{false};
    bool MENU_PRESSED{false};
    bool MENU_JUST_PRESSED{false};
    bool MENU_JUST_RELEASED{false};
};
class renderer : public object {
  public:
    // create context and start the render loop
    virtual void run();

  private:
    // create the context will be final in renderer impl
    virtual void create_context() = 0;
    virtual void poll_events() = 0;
    // cleanup the context
    virtual void destroy_context() = 0;
    virtual void update(float dt) override final;
    virtual void render(float dt) override final;
    virtual bool on_key(const renderer_input &input) override final;
    virtual bool on_mouse_button(const renderer_input &input) override final;
    virtual bool on_mouse_move(const renderer_input &input) override final;
    virtual bool on_mouse_wheel(const renderer_input &input) override final;
    virtual bool on_resize(const renderer_input &input) override final;
    object *root_object = nullptr;

  protected:
    // run any startup processes needed after context creation
    virtual void startup() {}
    // cleanup before context closes
    virtual void shutdown() {}
    // load the root object
    void load_object(object *obj);
    double current_time = 0.0f;
    const char *window_title = "untitled";
    int window_width = 1280;
    int window_height = 720;
    int samples = 0;
    bool wireframe = false;
    bool fullscreen = false;
    bool vsync = false;
    cursor_type cursor = cursor_type::ENABLED;
    debug_mode debug = debug_mode::NOTIFICATION;
    bool running = true;
    renderer_input input;
};
class gl : public renderer {
  private:
    virtual void create_context() override final;
    virtual void poll_events() override final;
    virtual void destroy_context() override final;
    SDL_GLContext glcontext;
    SDL_Window *window;
};
} // namespace lit
#endif