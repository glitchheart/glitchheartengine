#ifndef KEYS_GLFW_H
#define KEYS_GLFW_H

void init_key_mappings()
{
    map_init(&input_controller.key_mappings, int_hash);
    map_put(&input_controller.key_mappings, GLFW_KEY_APOSTROPHE, Key_Apostrophe);
    map_put(&input_controller.key_mappings, GLFW_KEY_COMMA, Key_Comma);
    map_put(&input_controller.key_mappings, GLFW_KEY_MINUS, Key_Minus);
    map_put(&input_controller.key_mappings, GLFW_KEY_PERIOD, Key_Period);
    map_put(&input_controller.key_mappings, GLFW_KEY_SLASH, Key_Slash);
    map_put(&input_controller.key_mappings, GLFW_KEY_SEMICOLON, Key_SemiColon);
    map_put(&input_controller.key_mappings, GLFW_KEY_EQUAL, Key_Equal);
    map_put(&input_controller.key_mappings, GLFW_KEY_LEFT_BRACKET, Key_LeftBracket);
    map_put(&input_controller.key_mappings, GLFW_KEY_BACKSLASH, Key_Backslash);
    map_put(&input_controller.key_mappings, GLFW_KEY_RIGHT_BRACKET, Key_RightBracket);
    map_put(&input_controller.key_mappings, GLFW_KEY_GRAVE_ACCENT, Key_GraveAccent);
    map_put(&input_controller.key_mappings, GLFW_KEY_WORLD_1, Key_World1);
    map_put(&input_controller.key_mappings, GLFW_KEY_WORLD_2, Key_World2);
    map_put(&input_controller.key_mappings, GLFW_KEY_INSERT, Key_Insert);
    map_put(&input_controller.key_mappings, GLFW_KEY_PAGE_UP, Key_PageUp);
    map_put(&input_controller.key_mappings, GLFW_KEY_PAGE_DOWN, Key_PageDown);
    map_put(&input_controller.key_mappings, GLFW_KEY_HOME, Key_Home);
    map_put(&input_controller.key_mappings, GLFW_KEY_END, Key_End);
    map_put(&input_controller.key_mappings, GLFW_KEY_CAPS_LOCK, Key_CapsLock);
    map_put(&input_controller.key_mappings, GLFW_KEY_SCROLL_LOCK, Key_ScrollLock);
    map_put(&input_controller.key_mappings, GLFW_KEY_NUM_LOCK, Key_NumLock);
    map_put(&input_controller.key_mappings, GLFW_KEY_PRINT_SCREEN, Key_PrintScreen);
    map_put(&input_controller.key_mappings, GLFW_KEY_PAUSE, Key_Pause);
    map_put(&input_controller.key_mappings, GLFW_KEY_LEFT, Key_Left);
    map_put(&input_controller.key_mappings, GLFW_KEY_RIGHT, Key_Right);
    map_put(&input_controller.key_mappings, GLFW_KEY_UP, Key_Up);
    map_put(&input_controller.key_mappings, GLFW_KEY_DOWN, Key_Down);
    map_put(&input_controller.key_mappings, GLFW_KEY_ESCAPE, Key_Escape);
    map_put(&input_controller.key_mappings, GLFW_KEY_SPACE, Key_Space);
    map_put(&input_controller.key_mappings, GLFW_KEY_TAB, Key_Tab);
    map_put(&input_controller.key_mappings, GLFW_KEY_BACKSPACE, Key_Backspace);
    map_put(&input_controller.key_mappings, GLFW_KEY_LEFT_SHIFT, Key_LeftShift);
    map_put(&input_controller.key_mappings, GLFW_KEY_RIGHT_SHIFT, Key_RightShift);
    map_put(&input_controller.key_mappings, GLFW_KEY_ENTER, Key_Enter);
    map_put(&input_controller.key_mappings, GLFW_KEY_LEFT_CONTROL, Key_LeftCtrl);
    map_put(&input_controller.key_mappings, GLFW_KEY_RIGHT_CONTROL, Key_RightCtrl);
    map_put(&input_controller.key_mappings, GLFW_KEY_LEFT_ALT, Key_LeftAlt);
    map_put(&input_controller.key_mappings, GLFW_KEY_RIGHT_ALT, Key_RightAlt);
    map_put(&input_controller.key_mappings, GLFW_KEY_LEFT_SUPER, Key_LeftSuper);
    map_put(&input_controller.key_mappings, GLFW_KEY_RIGHT_SUPER, Key_RightSuper);
    map_put(&input_controller.key_mappings, GLFW_KEY_MENU, Key_Menu);
    map_put(&input_controller.key_mappings, GLFW_KEY_LAST, Key_Last);
    map_put(&input_controller.key_mappings, GLFW_KEY_A, Key_A);
    map_put(&input_controller.key_mappings, GLFW_KEY_B, Key_B);
    map_put(&input_controller.key_mappings, GLFW_KEY_C, Key_C);
    map_put(&input_controller.key_mappings, GLFW_KEY_D, Key_D);
    map_put(&input_controller.key_mappings, GLFW_KEY_E, Key_E);
    map_put(&input_controller.key_mappings, GLFW_KEY_F, Key_F);
    map_put(&input_controller.key_mappings, GLFW_KEY_G, Key_G);
    map_put(&input_controller.key_mappings, GLFW_KEY_H, Key_H);
    map_put(&input_controller.key_mappings, GLFW_KEY_I, Key_I);
    map_put(&input_controller.key_mappings, GLFW_KEY_J, Key_J);
    map_put(&input_controller.key_mappings, GLFW_KEY_K, Key_K);
    map_put(&input_controller.key_mappings, GLFW_KEY_L, Key_L);
    map_put(&input_controller.key_mappings, GLFW_KEY_M, Key_M);
    map_put(&input_controller.key_mappings, GLFW_KEY_N, Key_N);
    map_put(&input_controller.key_mappings, GLFW_KEY_O, Key_O);
    map_put(&input_controller.key_mappings, GLFW_KEY_P, Key_P);
    map_put(&input_controller.key_mappings, GLFW_KEY_Q, Key_Q);
    map_put(&input_controller.key_mappings, GLFW_KEY_R, Key_R);
    map_put(&input_controller.key_mappings, GLFW_KEY_S, Key_S);
    map_put(&input_controller.key_mappings, GLFW_KEY_T, Key_T);
    map_put(&input_controller.key_mappings, GLFW_KEY_U, Key_U);
    map_put(&input_controller.key_mappings, GLFW_KEY_V, Key_V);
    map_put(&input_controller.key_mappings, GLFW_KEY_W, Key_W);
    map_put(&input_controller.key_mappings, GLFW_KEY_X, Key_X);
    map_put(&input_controller.key_mappings, GLFW_KEY_Y, Key_Y);
    map_put(&input_controller.key_mappings, GLFW_KEY_Z, Key_Z);
    map_put(&input_controller.key_mappings, GLFW_KEY_0, Key_0);
    map_put(&input_controller.key_mappings, GLFW_KEY_1, Key_1);
    map_put(&input_controller.key_mappings, GLFW_KEY_2, Key_2);
    map_put(&input_controller.key_mappings, GLFW_KEY_3, Key_3);
    map_put(&input_controller.key_mappings, GLFW_KEY_4, Key_4);
    map_put(&input_controller.key_mappings, GLFW_KEY_5, Key_5);
    map_put(&input_controller.key_mappings, GLFW_KEY_6, Key_6);
    map_put(&input_controller.key_mappings, GLFW_KEY_7, Key_7);
    map_put(&input_controller.key_mappings, GLFW_KEY_8, Key_8);
    map_put(&input_controller.key_mappings, GLFW_KEY_9, Key_9);
    map_put(&input_controller.key_mappings, GLFW_KEY_F1, Key_F1);
    map_put(&input_controller.key_mappings, GLFW_KEY_F2, Key_F2);
    map_put(&input_controller.key_mappings, GLFW_KEY_F3, Key_F3);
    map_put(&input_controller.key_mappings, GLFW_KEY_F4, Key_F4);
    map_put(&input_controller.key_mappings, GLFW_KEY_F5, Key_F5);
    map_put(&input_controller.key_mappings, GLFW_KEY_F6, Key_F6);
    map_put(&input_controller.key_mappings, GLFW_KEY_F7, Key_F7);
    map_put(&input_controller.key_mappings, GLFW_KEY_F8, Key_F8);
    map_put(&input_controller.key_mappings, GLFW_KEY_F9, Key_F9);
    map_put(&input_controller.key_mappings, GLFW_KEY_F10, Key_F10);
    map_put(&input_controller.key_mappings, GLFW_KEY_F11, Key_F11);
    map_put(&input_controller.key_mappings, GLFW_KEY_F12, Key_F12);
    map_put(&input_controller.key_mappings, GLFW_KEY_F13, Key_F13);
    map_put(&input_controller.key_mappings, GLFW_KEY_F14, Key_F14);
    map_put(&input_controller.key_mappings, GLFW_KEY_F15, Key_F15);
    map_put(&input_controller.key_mappings, GLFW_KEY_F16, Key_F16);
    map_put(&input_controller.key_mappings, GLFW_KEY_F17, Key_F17);
    map_put(&input_controller.key_mappings, GLFW_KEY_F18, Key_F18);
    map_put(&input_controller.key_mappings, GLFW_KEY_F19, Key_F19);
    map_put(&input_controller.key_mappings, GLFW_KEY_F20, Key_F20);
    map_put(&input_controller.key_mappings, GLFW_KEY_F21, Key_F21);
    map_put(&input_controller.key_mappings, GLFW_KEY_F22, Key_F22);
    map_put(&input_controller.key_mappings, GLFW_KEY_F23, Key_F23);
    map_put(&input_controller.key_mappings, GLFW_KEY_F24, Key_F24);
    map_put(&input_controller.key_mappings, GLFW_KEY_F25, Key_F25);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_0, Key_KP0);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_1, Key_KP1);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_2, Key_KP2);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_3, Key_KP3);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_4, Key_KP4);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_5, Key_KP5);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_6, Key_KP6);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_7, Key_KP7);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_8, Key_KP8);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_9, Key_KP9);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_ADD, Key_Add);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_SUBTRACT, Key_Subtract);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_DECIMAL, Key_KPDecimal);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_DIVIDE, Key_KPDivide);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_MULTIPLY, Key_KPMultiply);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_ENTER, Key_KPEnter);
    map_put(&input_controller.key_mappings, GLFW_KEY_KP_EQUAL, Key_KPEqual);
    map_put(&input_controller.key_mappings, GLFW_KEY_DELETE, Key_Delete);
}

void init_mouse_button_mappings()
{
    map_init(&input_controller.mouse_button_mappings, int_hash);
    map_put(&input_controller.mouse_button_mappings, GLFW_MOUSE_BUTTON_LEFT, (i32)Mouse_Left);
    map_put(&input_controller.mouse_button_mappings, GLFW_MOUSE_BUTTON_RIGHT, Mouse_Right);
    map_put(&input_controller.mouse_button_mappings, GLFW_MOUSE_BUTTON_MIDDLE, Mouse_Middle);
}


#define GLFW_JOYSTICK_17 16
#define GLFW_JOYSTICK_18 17

void init_controller_mappings()
{
    map_init(&input_controller.controller_mappings, int_hash);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_1, (i32)Joystick_1);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_2, Joystick_2);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_3, Joystick_3);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_4, Joystick_4);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_5, Joystick_5);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_6, Joystick_6);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_7, Joystick_7);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_8, Joystick_8);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_9, Joystick_9);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_10, Joystick_10);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_11, Joystick_11);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_12, Joystick_12);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_13, Joystick_13);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_14, Joystick_14);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_15, Joystick_15);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_16, Joystick_16);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_17, Joystick_LeftTrigger);
    map_put(&input_controller.controller_mappings, GLFW_JOYSTICK_18, Joystick_RightTrigger);
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
        
        if(strstr(name, "Xbox") != nullptr)
        {
            input_controller.controller_type = CONTROLLER_XBOX;
        }
        else if(strstr(name, "PS4") != nullptr || strstr(name, "Wireless") != nullptr)
        {
            input_controller.controller_type = CONTROLLER_PS4;
        }
    }
    
    input_controller.controller_present = present;
    return present;
}

static void controller_key_callback(int key, int action)
{
    i32 key_mapped = (i32)(imm)map_get(&input_controller.controller_mappings, key);
    if (action == GLFW_PRESS)
    {
	input_controller.current_input_type = InputType::CONTROLLER;
        input_controller.any_key_pressed = true;
	
        if (input_controller.joystick_keys_just_pressed[key_mapped] == KEY_NOT_PRESSED)
        {
            input_controller.joystick_keys_just_pressed[key_mapped] = KEY_JUST_PRESSED;
        }
        else if(input_controller.joystick_keys_just_pressed[key_mapped] == KEY_JUST_PRESSED)
        {
            // NOTE(niels): Do we ever even get in here???
            input_controller.joystick_keys_just_pressed[key_mapped] = KEY_INVALID;
            
        }
        input_controller.joystick_keys_down[key_mapped] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input_controller.joystick_keys_just_pressed[key_mapped] = KEY_NOT_PRESSED;
        input_controller.joystick_keys_down[key_mapped] = false;
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
		input_controller.current_input_type = InputType::CONTROLLER;
                input_controller.joystick_keys_just_pressed[Joystick_LeftTrigger] = KEY_JUST_PRESSED;
            }
            else if(lt_axis <= 0)
            {
                input_controller.joystick_keys_just_pressed[Joystick_LeftTrigger] = KEY_NOT_PRESSED;
            }
            
            input_controller.joystick_keys_down[Joystick_RightTrigger] = rt_axis > 0;
            
            if(rt_axis > 0 && input_controller.joystick_keys_just_pressed[Joystick_RightTrigger] != KEY_INVALID)
            {
		input_controller.current_input_type = InputType::CONTROLLER;
                input_controller.joystick_keys_just_pressed[Joystick_RightTrigger] = KEY_JUST_PRESSED;
            }
            else if(rt_axis <= 0)
            {
                input_controller.joystick_keys_just_pressed[Joystick_RightTrigger] = KEY_NOT_PRESSED;
            }
        }
        
        if(ABS(axes[index]) > input_controller.axes_used_zone)
        {
            if(input_controller.axes_just_pressed[index] == KEY_JUST_PRESSED)
            {
                input_controller.axes_just_pressed[index] = KEY_INVALID;
            }
            else if(input_controller.axes_just_pressed[index] != KEY_INVALID)
            {
		input_controller.current_input_type = InputType::CONTROLLER;
                input_controller.axes_just_pressed[index] = KEY_JUST_PRESSED;
            }
        }
        else if(ABS(axes[index]) <= input_controller.controller_deadzone)
        {
            input_controller.axes_just_pressed[index] = KEY_NOT_PRESSED;
        }
    }
}

static void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    
    glfwGetCursorPos(window, &x_pos, &y_pos);
        
    input_controller.mouse_x_delta = x_pos - input_controller.mouse_x;
    input_controller.mouse_y_delta = y_pos - input_controller.mouse_y;
    
    if(input_controller.mouse_x_delta > 100)
        input_controller.mouse_x_delta = 0.0;
    if(input_controller.mouse_y_delta > 100)
        input_controller.mouse_y_delta = 0.0;
    
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
    i32 key_mapped = (i32)(umm)map_get(&input_controller.key_mappings, key);
    if (action == GLFW_PRESS && key >= 0)
    {
	input_controller.current_input_type = InputType::KEYBOARD_MOUSE;
	
        input_controller.any_key_pressed = true;
        if (input_controller.keys_just_pressed[key_mapped] == KEY_NOT_PRESSED)
        {
            input_controller.keys_just_pressed[key_mapped] = KEY_JUST_PRESSED;
        }
        
        input_controller.keys_down[key_mapped] = true;
    }
    else if (action == GLFW_RELEASE && key >= 0)
    {
        input_controller.keys_up[key_mapped] = true;
        input_controller.keys_just_pressed[key_mapped] = KEY_NOT_PRESSED;
        
        input_controller.keys_down[key_mapped] = false;
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    i32 button_mapped = (i32)(imm)map_get(&input_controller.mouse_button_mappings, button);
    if (action == GLFW_PRESS)
    {
	input_controller.current_input_type = InputType::KEYBOARD_MOUSE;
        input_controller.any_key_pressed = true;
        if (input_controller.mouse_button_just_pressed[button_mapped] == KEY_NOT_PRESSED)
        {
            input_controller.mouse_button_just_pressed[button_mapped] = KEY_JUST_PRESSED;
        }
        else if (input_controller.mouse_button_just_pressed[button_mapped] == KEY_JUST_PRESSED)
        {
            // NOTE(niels): Do we ever even get in here???
            input_controller.mouse_button_just_pressed[button_mapped] = KEY_INVALID;
        }
        
        input_controller.mouse_button_down[button_mapped] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input_controller.mouse_buttons_up[button_mapped] = true;
        input_controller.mouse_button_just_pressed[button_mapped] = KEY_NOT_PRESSED;
        input_controller.mouse_button_down[button_mapped] = false;
    }
}

void character_callback(GLFWwindow *window, unsigned int codepoint)
{
    input_controller.current_character = (char)codepoint;
}

static void set_invalid_keys()
{
    input_controller.any_key_pressed = false;
    for(u32 key_code = 1; key_code < NUM_KEYS + 1; key_code++)
    {
        if(input_controller.keys_just_pressed[key_code] == KEY_JUST_PRESSED)
        {
            input_controller.keys_just_pressed[key_code] = KEY_INVALID;
        }
        input_controller.keys_up[key_code] = false;
    } 
    input_controller.current_character = 0x0;
}

static void set_controller_invalid_keys()
{
    input_controller.any_key_pressed = false;
    for(u32 key_code = 0; key_code < NUM_JOYSTICK_KEYS + 1; key_code++)
    {
        if(input_controller.joystick_keys_just_pressed[key_code] == KEY_JUST_PRESSED)
        {
            input_controller.joystick_keys_just_pressed[key_code] = KEY_INVALID;
        }
    }
}

static void set_mouse_invalid_keys()
{
    input_controller.any_key_pressed = false;
    for(u32 key_code = 0; key_code < NUM_MOUSE_BUTTONS + 1; key_code++)
    {
        if(input_controller.mouse_button_just_pressed[key_code] == KEY_JUST_PRESSED)
        {
            input_controller.mouse_button_just_pressed[key_code] = KEY_INVALID;
        }
        input_controller.mouse_buttons_up[key_code] = false;
    }
    input_controller.scroll_x = 0;
    input_controller.scroll_y = 0;
    input_controller.mouse_x_delta = 0.0;
    input_controller.mouse_y_delta = 0.0;
}


#endif
