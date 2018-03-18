#ifndef KEYS_GLFW_H
#define KEYS_GLFW_H

void init_key_mappings()
{
    integer_map_init(&input_controller.key_mappings,hash_int_keys, GLFW_KEY_LAST);
    input_controller.key_mappings[GLFW_KEY_APOSTROPHE] = Key_Apostrophe;
    input_controller.key_mappings[GLFW_KEY_COMMA] = Key_Comma;
    input_controller.key_mappings[GLFW_KEY_MINUS] = Key_Minus;
    input_controller.key_mappings[GLFW_KEY_PERIOD] = Key_Period;
    input_controller.key_mappings[GLFW_KEY_SLASH] = Key_Slash;
    input_controller.key_mappings[GLFW_KEY_SEMICOLON] = Key_SemiColon;
    input_controller.key_mappings[GLFW_KEY_EQUAL] = Key_Equal;
    input_controller.key_mappings[GLFW_KEY_LEFT_BRACKET] = Key_LeftBracket;
    input_controller.key_mappings[GLFW_KEY_BACKSLASH] = Key_Backslash;
    input_controller.key_mappings[GLFW_KEY_RIGHT_BRACKET] = Key_RightBracket;
    input_controller.key_mappings[GLFW_KEY_GRAVE_ACCENT] = Key_GraveAccent;
    input_controller.key_mappings[GLFW_KEY_WORLD_1] = Key_World1;
    input_controller.key_mappings[GLFW_KEY_WORLD_2] = Key_World2;
    input_controller.key_mappings[GLFW_KEY_INSERT] = Key_Insert;
    input_controller.key_mappings[GLFW_KEY_PAGE_UP] = Key_PageUp;
    input_controller.key_mappings[GLFW_KEY_PAGE_DOWN] = Key_PageDown;
    input_controller.key_mappings[GLFW_KEY_HOME] = Key_Home;
    input_controller.key_mappings[GLFW_KEY_END] = Key_End;
    input_controller.key_mappings[GLFW_KEY_CAPS_LOCK] = Key_CapsLock;
    input_controller.key_mappings[GLFW_KEY_SCROLL_LOCK] = Key_ScrollLock;
    input_controller.key_mappings[GLFW_KEY_NUM_LOCK] = Key_NumLock;
    input_controller.key_mappings[GLFW_KEY_PRINT_SCREEN] = Key_PrintScreen;
    input_controller.key_mappings[GLFW_KEY_PAUSE] = Key_Pause;
    input_controller.key_mappings[GLFW_KEY_LEFT] = Key_Left;
    input_controller.key_mappings[GLFW_KEY_RIGHT] = Key_Right;
    input_controller.key_mappings[GLFW_KEY_UP] = Key_Up;
    input_controller.key_mappings[GLFW_KEY_DOWN] = Key_Down;
    input_controller.key_mappings[GLFW_KEY_ESCAPE] = Key_Escape;
    input_controller.key_mappings[GLFW_KEY_SPACE] = Key_Space;
    input_controller.key_mappings[GLFW_KEY_TAB] = Key_Tab;
    input_controller.key_mappings[GLFW_KEY_BACKSPACE] = Key_Backspace;
    input_controller.key_mappings[GLFW_KEY_LEFT_SHIFT] = Key_LeftShift;
    input_controller.key_mappings[GLFW_KEY_RIGHT_SHIFT] = Key_RightShift;
    input_controller.key_mappings[GLFW_KEY_ENTER] = Key_Enter;
    input_controller.key_mappings[GLFW_KEY_LEFT_CONTROL] = Key_LeftCtrl;
    input_controller.key_mappings[GLFW_KEY_RIGHT_CONTROL] = Key_RightCtrl;
    input_controller.key_mappings[GLFW_KEY_LEFT_ALT] = Key_LeftAlt;
    input_controller.key_mappings[GLFW_KEY_RIGHT_ALT] = Key_RightAlt;
    input_controller.key_mappings[GLFW_KEY_LEFT_SUPER] = Key_LeftSuper;
    input_controller.key_mappings[GLFW_KEY_RIGHT_SUPER] = Key_RightSuper;
    input_controller.key_mappings[GLFW_KEY_MENU] = Key_Menu;
    input_controller.key_mappings[GLFW_KEY_LAST] = Key_Last;
    input_controller.key_mappings[GLFW_KEY_A] = Key_A;
    input_controller.key_mappings[GLFW_KEY_B] = Key_B;
    input_controller.key_mappings[GLFW_KEY_C] = Key_C;
    input_controller.key_mappings[GLFW_KEY_D] = Key_D;
    input_controller.key_mappings[GLFW_KEY_E] = Key_E;
    input_controller.key_mappings[GLFW_KEY_F] = Key_F;
    input_controller.key_mappings[GLFW_KEY_G] = Key_G;
    input_controller.key_mappings[GLFW_KEY_H] = Key_H;
    input_controller.key_mappings[GLFW_KEY_I] = Key_I;
    input_controller.key_mappings[GLFW_KEY_J] = Key_J;
    input_controller.key_mappings[GLFW_KEY_K] = Key_K;
    input_controller.key_mappings[GLFW_KEY_L] = Key_L;
    input_controller.key_mappings[GLFW_KEY_M] = Key_M;
    input_controller.key_mappings[GLFW_KEY_N] = Key_N;
    input_controller.key_mappings[GLFW_KEY_O] = Key_O;
    input_controller.key_mappings[GLFW_KEY_P] = Key_P;
    input_controller.key_mappings[GLFW_KEY_Q] = Key_Q;
    input_controller.key_mappings[GLFW_KEY_R] = Key_R;
    input_controller.key_mappings[GLFW_KEY_S] = Key_S;
    input_controller.key_mappings[GLFW_KEY_T] = Key_T;
    input_controller.key_mappings[GLFW_KEY_U] = Key_U;
    input_controller.key_mappings[GLFW_KEY_V] = Key_V;
    input_controller.key_mappings[GLFW_KEY_W] = Key_W;
    input_controller.key_mappings[GLFW_KEY_X] = Key_X;
    input_controller.key_mappings[GLFW_KEY_Y] = Key_Y;
    input_controller.key_mappings[GLFW_KEY_Z] = Key_Z;
    input_controller.key_mappings[GLFW_KEY_0] = Key_0;
    input_controller.key_mappings[GLFW_KEY_1] = Key_1;
    input_controller.key_mappings[GLFW_KEY_2] = Key_2;
    input_controller.key_mappings[GLFW_KEY_3] = Key_3;
    input_controller.key_mappings[GLFW_KEY_4] = Key_4;
    input_controller.key_mappings[GLFW_KEY_5] = Key_5;
    input_controller.key_mappings[GLFW_KEY_6] = Key_6;
    input_controller.key_mappings[GLFW_KEY_7] = Key_7;
    input_controller.key_mappings[GLFW_KEY_8] = Key_8;
    input_controller.key_mappings[GLFW_KEY_9] = Key_9;
    input_controller.key_mappings[GLFW_KEY_F1] = Key_F1;
    input_controller.key_mappings[GLFW_KEY_F2] = Key_F2;
    input_controller.key_mappings[GLFW_KEY_F3] = Key_F3;
    input_controller.key_mappings[GLFW_KEY_F4] = Key_F4;
    input_controller.key_mappings[GLFW_KEY_F5] = Key_F5;
    input_controller.key_mappings[GLFW_KEY_F6] = Key_F6;
    input_controller.key_mappings[GLFW_KEY_F7] = Key_F7;
    input_controller.key_mappings[GLFW_KEY_F8] = Key_F8;
    input_controller.key_mappings[GLFW_KEY_F9] = Key_F9;
    input_controller.key_mappings[GLFW_KEY_F10] = Key_F10;
    input_controller.key_mappings[GLFW_KEY_F11] = Key_F11;
    input_controller.key_mappings[GLFW_KEY_F12] = Key_F12;
    input_controller.key_mappings[GLFW_KEY_F13] = Key_F13;
    input_controller.key_mappings[GLFW_KEY_F14] = Key_F14;
    input_controller.key_mappings[GLFW_KEY_F15] = Key_F15;
    input_controller.key_mappings[GLFW_KEY_F16] = Key_F16;
    input_controller.key_mappings[GLFW_KEY_F17] = Key_F17;
    input_controller.key_mappings[GLFW_KEY_F18] = Key_F18;
    input_controller.key_mappings[GLFW_KEY_F19] = Key_F19;
    input_controller.key_mappings[GLFW_KEY_F20] = Key_F20;
    input_controller.key_mappings[GLFW_KEY_F21] = Key_F21;
    input_controller.key_mappings[GLFW_KEY_F22] = Key_F22;
    input_controller.key_mappings[GLFW_KEY_F23] = Key_F23;
    input_controller.key_mappings[GLFW_KEY_F24] = Key_F24;
    input_controller.key_mappings[GLFW_KEY_F25] = Key_F25;
    input_controller.key_mappings[GLFW_KEY_KP_0] = Key_KP0;
    input_controller.key_mappings[GLFW_KEY_KP_1] = Key_KP1;
    input_controller.key_mappings[GLFW_KEY_KP_2] = Key_KP2;
    input_controller.key_mappings[GLFW_KEY_KP_3] = Key_KP3;
    input_controller.key_mappings[GLFW_KEY_KP_4] = Key_KP4;
    input_controller.key_mappings[GLFW_KEY_KP_5] = Key_KP5;
    input_controller.key_mappings[GLFW_KEY_KP_6] = Key_KP6;
    input_controller.key_mappings[GLFW_KEY_KP_7] = Key_KP7;
    input_controller.key_mappings[GLFW_KEY_KP_8] = Key_KP8;
    input_controller.key_mappings[GLFW_KEY_KP_9] = Key_KP9;
    input_controller.key_mappings[GLFW_KEY_KP_ADD] = Key_Add;
    input_controller.key_mappings[GLFW_KEY_KP_SUBTRACT] = Key_Subtract;
    input_controller.key_mappings[GLFW_KEY_KP_DECIMAL] = Key_KPDecimal;
    input_controller.key_mappings[GLFW_KEY_KP_DIVIDE] = Key_KPDivide;
    input_controller.key_mappings[GLFW_KEY_KP_MULTIPLY] = Key_KPMultiply;
    input_controller.key_mappings[GLFW_KEY_KP_ENTER] = Key_KPEnter;
    input_controller.key_mappings[GLFW_KEY_KP_EQUAL] = Key_KPEqual;
    input_controller.key_mappings[GLFW_KEY_DELETE] = Key_Delete;
}

void init_mouse_button_mappings()
{
    integer_map_init(&input_controller.mouse_button_mappings,hash_int_keys,7);
    input_controller.mouse_button_mappings[GLFW_MOUSE_BUTTON_LEFT] = Mouse_Left;
    input_controller.mouse_button_mappings[GLFW_MOUSE_BUTTON_RIGHT] = Mouse_Right;
    input_controller.mouse_button_mappings[GLFW_MOUSE_BUTTON_MIDDLE] = Mouse_Middle;
}


#define GLFW_JOYSTICK_17 16
#define GLFW_JOYSTICK_18 17

void init_controller_mappings()
{
    integer_map_init(&input_controller.controller_mappings, hash_int, 257);
    input_controller.controller_mappings[GLFW_JOYSTICK_1] = Joystick_1;
    input_controller.controller_mappings[GLFW_JOYSTICK_2] = Joystick_2;
    input_controller.controller_mappings[GLFW_JOYSTICK_3] = Joystick_3;
    input_controller.controller_mappings[GLFW_JOYSTICK_4] = Joystick_4;
    input_controller.controller_mappings[GLFW_JOYSTICK_5] = Joystick_5;
    input_controller.controller_mappings[GLFW_JOYSTICK_6] = Joystick_6;
    input_controller.controller_mappings[GLFW_JOYSTICK_7] = Joystick_7;
    input_controller.controller_mappings[GLFW_JOYSTICK_8] = Joystick_8;
    input_controller.controller_mappings[GLFW_JOYSTICK_9] = Joystick_9;
    input_controller.controller_mappings[GLFW_JOYSTICK_10] = Joystick_10;
    input_controller.controller_mappings[GLFW_JOYSTICK_11] = Joystick_11;
    input_controller.controller_mappings[GLFW_JOYSTICK_12] = Joystick_12;
    input_controller.controller_mappings[GLFW_JOYSTICK_13] = Joystick_13;
    input_controller.controller_mappings[GLFW_JOYSTICK_14] = Joystick_14;
    input_controller.controller_mappings[GLFW_JOYSTICK_15] = Joystick_15;
    input_controller.controller_mappings[GLFW_JOYSTICK_16] = Joystick_16;
    input_controller.controller_mappings[GLFW_JOYSTICK_17] = Joystick_LeftTrigger;
    input_controller.controller_mappings[GLFW_JOYSTICK_18] = Joystick_RightTrigger;
}

void init_keys()
{
    init_key_mappings();
    init_mouse_button_mappings();
    init_controller_mappings();
}

static b32 controller_present()
{
    int present = glfwJoystickPresent(GLFW_JOYSTICK_1); 
    
    if(present)
    {
        input_controller.controller_present = true;
        const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        
        if(strstr(name, "Xbox") != 0)
        {
            input_controller.controller_type = CONTROLLER_XBOX;
        }
        else if(strstr(name, "PS4") != 0 || strstr(name, "Wireless") != 0)
        {
            input_controller.controller_type = CONTROLLER_PS4;
        }
    }
    
    input_controller.controller_present = present;
    return present;
}

static void controller_key_callback(int key, int action)
{
    if (action == GLFW_PRESS)
    {
        input_controller.any_key_pressed = true;
        if (input_controller.joystick_keys_just_pressed[input_controller.controller_mappings[key]] == KEY_NOT_PRESSED)
        {
            input_controller.joystick_keys_just_pressed[input_controller.controller_mappings[key]] = KEY_JUST_PRESSED;
        }
        else if(input_controller.joystick_keys_just_pressed[input_controller.controller_mappings[key]] == KEY_JUST_PRESSED)
        {
            // NOTE(niels): Do we ever even get in here???
            input_controller.joystick_keys_just_pressed[input_controller.controller_mappings[key]] = KEY_INVALID;
            
        }
        input_controller.joystick_keys_down[input_controller.controller_mappings[key]] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input_controller.joystick_keys_just_pressed[input_controller.controller_mappings[key]] = KEY_NOT_PRESSED;
        input_controller.joystick_keys_down[input_controller.controller_mappings[key]] = false;
    }
}

static void controller_keys(i32 joystick)
{
    i32 count;
    const unsigned char* button_state = glfwGetJoystickButtons(joystick, &count);
    
    for(i32 i = 0; i < count; i++)
    {
        controller_key_callback(i, button_state[i]);
    }
    
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
    
    for(int index = 0; index < NUM_AXES; index++)
    {
        input_controller.axes[index] = axes[index];
        
        if(index == 5 && input_controller.controller_type == CONTROLLER_XBOX)
        {
            r32 lt_axis = axes[4];
            r32 rt_axis = axes[5];
            
            input_controller.joystick_keys_down[Joystick_LeftTrigger] = lt_axis > 0;
            
            if(lt_axis > 0  && input_controller.joystick_keys_just_pressed[Joystick_LeftTrigger] != KEY_INVALID)
            {
                input_controller.joystick_keys_just_pressed[Joystick_LeftTrigger] = KEY_JUST_PRESSED;
            }
            else if(lt_axis <= 0)
            {
                input_controller.joystick_keys_just_pressed[Joystick_LeftTrigger] = KEY_NOT_PRESSED;
            }
            
            input_controller.joystick_keys_down[Joystick_RightTrigger] = rt_axis > 0;
            
            if(rt_axis > 0 && input_controller.joystick_keys_just_pressed[Joystick_RightTrigger] != KEY_INVALID)
            {
                input_controller.joystick_keys_just_pressed[Joystick_RightTrigger] = KEY_JUST_PRESSED;
            }
            else if(rt_axis <= 0)
            {
                input_controller.joystick_keys_just_pressed[Joystick_RightTrigger] = KEY_NOT_PRESSED;
            }
        }
        
        if(Abs(axes[index]) > input_controller.axes_used_zone)
        {
            if(input_controller.axes_just_pressed[index] == KEY_JUST_PRESSED)
            {
                input_controller.axes_just_pressed[index] = KEY_INVALID;
            }
            else if(input_controller.axes_just_pressed[index] != KEY_INVALID)
            {
                input_controller.axes_just_pressed[index] = KEY_JUST_PRESSED;
            }
        }
        else if(Abs(axes[index]) <= input_controller.controller_deadzone)
        {
            input_controller.axes_just_pressed[index] = KEY_NOT_PRESSED;
        }
    }
}

static void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    input_controller.mouse_x = x_pos;
    input_controller.mouse_y = y_pos;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    input_controller.scroll_x = x_offset;
    input_controller.scroll_y = y_offset;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key >= 0)
    {
        input_controller.any_key_pressed = true;
        if (input_controller.keys_just_pressed[input_controller.key_mappings[key]] == KEY_NOT_PRESSED)
        {
            input_controller.keys_just_pressed[input_controller.key_mappings[key]] = KEY_JUST_PRESSED;
        }
        
        input_controller.keys_down[input_controller.key_mappings[key]] = true;
    }
    else if (action == GLFW_RELEASE && key >= 0)
    {
        input_controller.keys_up[input_controller.key_mappings[key]] = true;
        input_controller.keys_just_pressed[input_controller.key_mappings[key]] = KEY_NOT_PRESSED;
        
        input_controller.keys_down[input_controller.key_mappings[key]] = false;
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        input_controller.any_key_pressed = true;
        if (input_controller.mouse_button_just_pressed[input_controller.mouse_button_mappings[button]] == KEY_NOT_PRESSED)
        {
            input_controller.mouse_button_just_pressed[input_controller.mouse_button_mappings[button]] = KEY_JUST_PRESSED;
        }
        else if (input_controller.mouse_button_just_pressed[input_controller.mouse_button_mappings[button]] == KEY_JUST_PRESSED)
        {
            // NOTE(niels): Do we ever even get in here???
            input_controller.mouse_button_just_pressed[input_controller.mouse_button_mappings[button]] = KEY_INVALID;
        }
        
        input_controller.mouse_button_down[input_controller.mouse_button_mappings[button]] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input_controller.mouse_buttons_up[input_controller.mouse_button_mappings[button]] = true;
        input_controller.mouse_button_just_pressed[input_controller.mouse_button_mappings[button]] = KEY_NOT_PRESSED;
        input_controller.mouse_button_down[input_controller.mouse_button_mappings[button]] = false;
    }
}

void character_callback(GLFWwindow *window, unsigned int codepoint)
{
    input_controller.current_character = (char)codepoint;
}

#endif
