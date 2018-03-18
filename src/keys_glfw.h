#ifndef KEYS_GLFW_H
#define KEYS_GLFW_H

void init_key_mappings()
{
    integer_Map_Init(&InputController.KeyMappings,hash_int_keys, GLFW_KEY_LAST);
    InputController.KeyMappings[GLFW_KEY_APOSTROPHE] = Key_Apostrophe;
    InputController.KeyMappings[GLFW_KEY_COMMA] = Key_Comma;
    InputController.KeyMappings[GLFW_KEY_MINUS] = Key_Minus;
    InputController.KeyMappings[GLFW_KEY_PERIOD] = Key_Period;
    InputController.KeyMappings[GLFW_KEY_SLASH] = Key_Slash;
    InputController.KeyMappings[GLFW_KEY_SEMICOLON] = Key_SemiColon;
    InputController.KeyMappings[GLFW_KEY_EQUAL] = Key_Equal;
    InputController.KeyMappings[GLFW_KEY_LEFT_BRACKET] = Key_LeftBracket;
    InputController.KeyMappings[GLFW_KEY_BACKSLASH] = Key_Backslash;
    InputController.KeyMappings[GLFW_KEY_RIGHT_BRACKET] = Key_RightBracket;
    InputController.KeyMappings[GLFW_KEY_GRAVE_ACCENT] = Key_GraveAccent;
    InputController.KeyMappings[GLFW_KEY_WORLD_1] = Key_World1;
    InputController.KeyMappings[GLFW_KEY_WORLD_2] = Key_World2;
    InputController.KeyMappings[GLFW_KEY_INSERT] = Key_Insert;
    InputController.KeyMappings[GLFW_KEY_PAGE_UP] = Key_PageUp;
    InputController.KeyMappings[GLFW_KEY_PAGE_DOWN] = Key_PageDown;
    InputController.KeyMappings[GLFW_KEY_HOME] = Key_Home;
    InputController.KeyMappings[GLFW_KEY_END] = Key_End;
    InputController.KeyMappings[GLFW_KEY_CAPS_LOCK] = Key_CapsLock;
    InputController.KeyMappings[GLFW_KEY_SCROLL_LOCK] = Key_ScrollLock;
    InputController.KeyMappings[GLFW_KEY_NUM_LOCK] = Key_NumLock;
    InputController.KeyMappings[GLFW_KEY_PRINT_SCREEN] = Key_PrintScreen;
    InputController.KeyMappings[GLFW_KEY_PAUSE] = Key_Pause;
    InputController.KeyMappings[GLFW_KEY_LEFT] = Key_Left;
    InputController.KeyMappings[GLFW_KEY_RIGHT] = Key_Right;
    InputController.KeyMappings[GLFW_KEY_UP] = Key_Up;
    InputController.KeyMappings[GLFW_KEY_DOWN] = Key_Down;
    InputController.KeyMappings[GLFW_KEY_ESCAPE] = Key_Escape;
    InputController.KeyMappings[GLFW_KEY_SPACE] = Key_Space;
    InputController.KeyMappings[GLFW_KEY_TAB] = Key_Tab;
    InputController.KeyMappings[GLFW_KEY_BACKSPACE] = Key_Backspace;
    InputController.KeyMappings[GLFW_KEY_LEFT_SHIFT] = Key_LeftShift;
    InputController.KeyMappings[GLFW_KEY_RIGHT_SHIFT] = Key_RightShift;
    InputController.KeyMappings[GLFW_KEY_ENTER] = Key_Enter;
    InputController.KeyMappings[GLFW_KEY_LEFT_CONTROL] = Key_LeftCtrl;
    InputController.KeyMappings[GLFW_KEY_RIGHT_CONTROL] = Key_RightCtrl;
    InputController.KeyMappings[GLFW_KEY_LEFT_ALT] = Key_LeftAlt;
    InputController.KeyMappings[GLFW_KEY_RIGHT_ALT] = Key_RightAlt;
    InputController.KeyMappings[GLFW_KEY_LEFT_SUPER] = Key_LeftSuper;
    InputController.KeyMappings[GLFW_KEY_RIGHT_SUPER] = Key_RightSuper;
    InputController.KeyMappings[GLFW_KEY_MENU] = Key_Menu;
    InputController.KeyMappings[GLFW_KEY_LAST] = Key_Last;
    InputController.KeyMappings[GLFW_KEY_A] = Key_A;
    InputController.KeyMappings[GLFW_KEY_B] = Key_B;
    InputController.KeyMappings[GLFW_KEY_C] = Key_C;
    InputController.KeyMappings[GLFW_KEY_D] = Key_D;
    InputController.KeyMappings[GLFW_KEY_E] = Key_E;
    InputController.KeyMappings[GLFW_KEY_F] = Key_F;
    InputController.KeyMappings[GLFW_KEY_G] = Key_G;
    InputController.KeyMappings[GLFW_KEY_H] = Key_H;
    InputController.KeyMappings[GLFW_KEY_I] = Key_I;
    InputController.KeyMappings[GLFW_KEY_J] = Key_J;
    InputController.KeyMappings[GLFW_KEY_K] = Key_K;
    InputController.KeyMappings[GLFW_KEY_L] = Key_L;
    InputController.KeyMappings[GLFW_KEY_M] = Key_M;
    InputController.KeyMappings[GLFW_KEY_N] = Key_N;
    InputController.KeyMappings[GLFW_KEY_O] = Key_O;
    InputController.KeyMappings[GLFW_KEY_P] = Key_P;
    InputController.KeyMappings[GLFW_KEY_Q] = Key_Q;
    InputController.KeyMappings[GLFW_KEY_R] = Key_R;
    InputController.KeyMappings[GLFW_KEY_S] = Key_S;
    InputController.KeyMappings[GLFW_KEY_T] = Key_T;
    InputController.KeyMappings[GLFW_KEY_U] = Key_U;
    InputController.KeyMappings[GLFW_KEY_V] = Key_V;
    InputController.KeyMappings[GLFW_KEY_W] = Key_W;
    InputController.KeyMappings[GLFW_KEY_X] = Key_X;
    InputController.KeyMappings[GLFW_KEY_Y] = Key_Y;
    InputController.KeyMappings[GLFW_KEY_Z] = Key_Z;
    InputController.KeyMappings[GLFW_KEY_0] = Key_0;
    InputController.KeyMappings[GLFW_KEY_1] = Key_1;
    InputController.KeyMappings[GLFW_KEY_2] = Key_2;
    InputController.KeyMappings[GLFW_KEY_3] = Key_3;
    InputController.KeyMappings[GLFW_KEY_4] = Key_4;
    InputController.KeyMappings[GLFW_KEY_5] = Key_5;
    InputController.KeyMappings[GLFW_KEY_6] = Key_6;
    InputController.KeyMappings[GLFW_KEY_7] = Key_7;
    InputController.KeyMappings[GLFW_KEY_8] = Key_8;
    InputController.KeyMappings[GLFW_KEY_9] = Key_9;
    InputController.KeyMappings[GLFW_KEY_F1] = Key_F1;
    InputController.KeyMappings[GLFW_KEY_F2] = Key_F2;
    InputController.KeyMappings[GLFW_KEY_F3] = Key_F3;
    InputController.KeyMappings[GLFW_KEY_F4] = Key_F4;
    InputController.KeyMappings[GLFW_KEY_F5] = Key_F5;
    InputController.KeyMappings[GLFW_KEY_F6] = Key_F6;
    InputController.KeyMappings[GLFW_KEY_F7] = Key_F7;
    InputController.KeyMappings[GLFW_KEY_F8] = Key_F8;
    InputController.KeyMappings[GLFW_KEY_F9] = Key_F9;
    InputController.KeyMappings[GLFW_KEY_F10] = Key_F10;
    InputController.KeyMappings[GLFW_KEY_F11] = Key_F11;
    InputController.KeyMappings[GLFW_KEY_F12] = Key_F12;
    InputController.KeyMappings[GLFW_KEY_F13] = Key_F13;
    InputController.KeyMappings[GLFW_KEY_F14] = Key_F14;
    InputController.KeyMappings[GLFW_KEY_F15] = Key_F15;
    InputController.KeyMappings[GLFW_KEY_F16] = Key_F16;
    InputController.KeyMappings[GLFW_KEY_F17] = Key_F17;
    InputController.KeyMappings[GLFW_KEY_F18] = Key_F18;
    InputController.KeyMappings[GLFW_KEY_F19] = Key_F19;
    InputController.KeyMappings[GLFW_KEY_F20] = Key_F20;
    InputController.KeyMappings[GLFW_KEY_F21] = Key_F21;
    InputController.KeyMappings[GLFW_KEY_F22] = Key_F22;
    InputController.KeyMappings[GLFW_KEY_F23] = Key_F23;
    InputController.KeyMappings[GLFW_KEY_F24] = Key_F24;
    InputController.KeyMappings[GLFW_KEY_F25] = Key_F25;
    InputController.KeyMappings[GLFW_KEY_KP_0] = Key_KP0;
    InputController.KeyMappings[GLFW_KEY_KP_1] = Key_KP1;
    InputController.KeyMappings[GLFW_KEY_KP_2] = Key_KP2;
    InputController.KeyMappings[GLFW_KEY_KP_3] = Key_KP3;
    InputController.KeyMappings[GLFW_KEY_KP_4] = Key_KP4;
    InputController.KeyMappings[GLFW_KEY_KP_5] = Key_KP5;
    InputController.KeyMappings[GLFW_KEY_KP_6] = Key_KP6;
    InputController.KeyMappings[GLFW_KEY_KP_7] = Key_KP7;
    InputController.KeyMappings[GLFW_KEY_KP_8] = Key_KP8;
    InputController.KeyMappings[GLFW_KEY_KP_9] = Key_KP9;
    InputController.KeyMappings[GLFW_KEY_KP_ADD] = Key_Add;
    InputController.KeyMappings[GLFW_KEY_KP_SUBTRACT] = Key_Subtract;
    InputController.KeyMappings[GLFW_KEY_KP_DECIMAL] = Key_KPDecimal;
    InputController.KeyMappings[GLFW_KEY_KP_DIVIDE] = Key_KPDivide;
    InputController.KeyMappings[GLFW_KEY_KP_MULTIPLY] = Key_KPMultiply;
    InputController.KeyMappings[GLFW_KEY_KP_ENTER] = Key_KPEnter;
    InputController.KeyMappings[GLFW_KEY_KP_EQUAL] = Key_KPEqual;
    InputController.KeyMappings[GLFW_KEY_DELETE] = Key_Delete;
}

void init_mouse_button_mappings()
{
    integer_Map_Init(&InputController.MouseButtonMappings,hash_int_keys,7);
    InputController.MouseButtonMappings[GLFW_MOUSE_BUTTON_LEFT] = Mouse_Left;
    InputController.MouseButtonMappings[GLFW_MOUSE_BUTTON_RIGHT] = Mouse_Right;
    InputController.MouseButtonMappings[GLFW_MOUSE_BUTTON_MIDDLE] = Mouse_Middle;
}


#define GLFW_JOYSTICK_17 16
#define GLFW_JOYSTICK_18 17

void init_controller_mappings()
{
    integer_Map_Init(&InputController.ControllerMappings, hash_int, 257);
    InputController.ControllerMappings[GLFW_JOYSTICK_1] = Joystick_1;
    InputController.ControllerMappings[GLFW_JOYSTICK_2] = Joystick_2;
    InputController.ControllerMappings[GLFW_JOYSTICK_3] = Joystick_3;
    InputController.ControllerMappings[GLFW_JOYSTICK_4] = Joystick_4;
    InputController.ControllerMappings[GLFW_JOYSTICK_5] = Joystick_5;
    InputController.ControllerMappings[GLFW_JOYSTICK_6] = Joystick_6;
    InputController.ControllerMappings[GLFW_JOYSTICK_7] = Joystick_7;
    InputController.ControllerMappings[GLFW_JOYSTICK_8] = Joystick_8;
    InputController.ControllerMappings[GLFW_JOYSTICK_9] = Joystick_9;
    InputController.ControllerMappings[GLFW_JOYSTICK_10] = Joystick_10;
    InputController.ControllerMappings[GLFW_JOYSTICK_11] = Joystick_11;
    InputController.ControllerMappings[GLFW_JOYSTICK_12] = Joystick_12;
    InputController.ControllerMappings[GLFW_JOYSTICK_13] = Joystick_13;
    InputController.ControllerMappings[GLFW_JOYSTICK_14] = Joystick_14;
    InputController.ControllerMappings[GLFW_JOYSTICK_15] = Joystick_15;
    InputController.ControllerMappings[GLFW_JOYSTICK_16] = Joystick_16;
    InputController.ControllerMappings[GLFW_JOYSTICK_17] = Joystick_LeftTrigger;
    InputController.ControllerMappings[GLFW_JOYSTICK_18] = Joystick_RightTrigger;
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
        InputController.ControllerPresent = true;
        const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        
        if(strstr(name, "Xbox") != 0)
        {
            InputController.ControllerType = Controller_Xbox;
        }
        else if(strstr(name, "PS4") != 0 || strstr(name, "Wireless") != 0)
        {
            InputController.ControllerType = Controller_PS4;
        }
    }
    
    InputController.ControllerPresent = present;
    return present;
}

static void controller_key_callback(int key, int action)
{
    if (action == GLFW_PRESS)
    {
        InputController.AnyKeyPressed = true;
        if (InputController.JoystickKeysJustPressed[InputController.ControllerMappings[key]] == Key_NotPressed)
        {
            InputController.JoystickKeysJustPressed[InputController.ControllerMappings[key]] = Key_JustPressed;
        }
        else if(InputController.JoystickKeysJustPressed[InputController.ControllerMappings[key]] == Key_JustPressed)
        {
            // NOTE(niels): Do we ever even get in here???
            InputController.JoystickKeysJustPressed[InputController.ControllerMappings[key]] = Key_Invalid;
            
        }
        InputController.JoystickKeysDown[InputController.ControllerMappings[key]] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        InputController.JoystickKeysJustPressed[InputController.ControllerMappings[key]] = Key_NotPressed;
        InputController.JoystickKeysDown[InputController.ControllerMappings[key]] = false;
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
        InputController.Axes[Index] = Axes[Index];
        
        if(Index == 5 && InputController.ControllerType == Controller_Xbox)
        {
            r32 lt_axis = axes[4];
            r32 rt_axis = axes[5];
            
            InputController.JoystickKeysDown[Joystick_LeftTrigger] = lt_axis > 0;
            
            if(lt_axis > 0  && InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] != Key_Invalid)
            {
                InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] = Key_JustPressed;
            }
            else if(lt_axis <= 0)
            {
                InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] = Key_NotPressed;
            }
            
            InputController.JoystickKeysDown[Joystick_RightTrigger] = rt_axis > 0;
            
            if(rt_axis > 0 && InputController.JoystickKeysJustPressed[Joystick_RightTrigger] != Key_Invalid)
            {
                InputController.JoystickKeysJustPressed[Joystick_RightTrigger] = Key_JustPressed;
            }
            else if(rt_axis <= 0)
            {
                InputController.JoystickKeysJustPressed[Joystick_RightTrigger] = Key_NotPressed;
            }
        }
        
        if(Abs(Axes[Index]) > InputController.AxesUsedZone)
        {
            if(InputController.AxesJustPressed[Index] == Key_JustPressed)
            {
                InputController.AxesJustPressed[Index] = Key_Invalid;
            }
            else if(InputController.AxesJustPressed[Index] != Key_Invalid)
            {
                InputController.AxesJustPressed[Index] = Key_JustPressed;
            }
        }
        else if(Abs(Axes[Index]) <= InputController.ControllerDeadzone)
        {
            InputController.AxesJustPressed[Index] = Key_NotPressed;
        }
    }
}

static void cursor_position_callback(GLFWwindow *window, double x_pos, double y_pos)
{
    InputController.MouseX = x_pos;
    InputController.MouseY = y_pos;
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    InputController.ScrollX = x_offset;
    InputController.ScrollY = y_offset;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS && key >= 0)
    {
        InputController.AnyKeyPressed = true;
        if (InputController.KeysJustPressed[InputController.KeyMappings[key]] == Key_NotPressed)
        {
            InputController.KeysJustPressed[InputController.KeyMappings[key]] = Key_JustPressed;
        }
        
        InputController.KeysDown[InputController.KeyMappings[key]] = true;
    }
    else if (action == GLFW_RELEASE && key >= 0)
    {
        InputController.KeysUp[InputController.KeyMappings[key]] = true;
        InputController.KeysJustPressed[InputController.KeyMappings[key]] = Key_NotPressed;
        
        InputController.KeysDown[InputController.KeyMappings[key]] = false;
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        InputController.AnyKeyPressed = true;
        if (InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[button]] == Key_NotPressed)
        {
            InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[button]] = Key_JustPressed;
        }
        else if (InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[button]] == Key_JustPressed)
        {
            // NOTE(niels): Do we ever even get in here???
            InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[button]] = Key_Invalid;
        }
        
        InputController.MouseButtonDown[InputController.MouseButtonMappings[button]] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        InputController.MouseButtonsUp[InputController.MouseButtonMappings[button]] = true;
        InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[button]] = Key_NotPressed;
        InputController.MouseButtonDown[InputController.MouseButtonMappings[button]] = false;
    }
}

void character_callback(GLFWwindow *window, unsigned int codepoint)
{
    InputController.CurrentCharacter = (char)codepoint;
}

#endif
