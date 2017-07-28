#ifndef KEYS_GLFW_H
#define KEYS_GLFW_H

void InitKeyMappings()
{
    integer_Map_Init(&InputController.KeyMappings,HashIntKeys, 348);
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
    InputController.KeyMappings[GLFW_KEY_KP_ADD] = Key_Add;
    InputController.KeyMappings[GLFW_KEY_KP_SUBTRACT] = Key_Subtract;
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
    InputController.KeyMappings[GLFW_KEY_DELETE] = Key_Delete;
}

void InitMouseButtonMappings()
{
    integer_Map_Init(&InputController.MouseButtonMappings,HashIntKeys,7);
    InputController.MouseButtonMappings[GLFW_MOUSE_BUTTON_LEFT] = Mouse_Left;
    InputController.MouseButtonMappings[GLFW_MOUSE_BUTTON_RIGHT] = Mouse_Right;
    InputController.MouseButtonMappings[GLFW_MOUSE_BUTTON_MIDDLE] = Mouse_Middle;
}


#define GLFW_JOYSTICK_17 16
#define GLFW_JOYSTICK_18 17

void InitControllerMappings()
{
    integer_Map_Init(&InputController.ControllerMappings, HashInt, 257);
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

void InitKeys()
{
    InitKeyMappings();
    InitMouseButtonMappings();
    InitControllerMappings();
}

static b32 ControllerPresent()
{
    int Present = glfwJoystickPresent(GLFW_JOYSTICK_1); 
    
    if(Present)
    {
        InputController.ControllerPresent = true;
        const char* Name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        
        if(strstr(Name, "Xbox") != 0)
        {
            InputController.ControllerType = Controller_Xbox;
        }
        else if(strstr(Name, "PS4") != 0 || strstr(Name, "Wireless") != 0)
        {
            InputController.ControllerType = Controller_PS4;
        }
    }
    
    InputController.ControllerPresent = Present;
    return Present;
}

static void ControllerKeyCallback(int Key, int Action)
{
    if (Action == GLFW_PRESS)
    {
        InputController.AnyKeyPressed = true;
        if (InputController.JoystickKeysJustPressed[InputController.ControllerMappings[Key]] == Key_NotPressed)
        {
            InputController.JoystickKeysJustPressed[InputController.ControllerMappings[Key]] = Key_JustPressed;
        }
        else if(InputController.JoystickKeysJustPressed[InputController.ControllerMappings[Key]] == Key_JustPressed)
        {
            // NOTE(niels): Do we ever even get in here???
            InputController.JoystickKeysJustPressed[InputController.ControllerMappings[Key]] = Key_Invalid;
            
        }
        InputController.JoystickKeysDown[InputController.ControllerMappings[Key]] = true;
    }
    else if (Action == GLFW_RELEASE)
    {
        InputController.JoystickKeysJustPressed[InputController.ControllerMappings[Key]] = Key_NotPressed;
        InputController.JoystickKeysDown[InputController.ControllerMappings[Key]] = false;
    }
}

static void ControllerKeys(u32 Joystick)
{
    i32 Count;
    const unsigned char* ButtonState = glfwGetJoystickButtons(Joystick,&Count);
    
    for(i32 i = 0; i < Count; i++)
    {
        ControllerKeyCallback(i,ButtonState[i]);
    }
    
    const float* Axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &Count);
    
    for(int Index = 0; Index < NUM_AXES; Index++)
    {
        InputController.Axes[Index] = Axes[Index];
        
        if(Index == 5 && InputController.ControllerType == Controller_Xbox)
        {
            r32 LTAxis = Axes[4];
            r32 RTAxis = Axes[5];
            
            InputController.JoystickKeysDown[Joystick_LeftTrigger] = LTAxis > 0;
            
            if(LTAxis > 0  && InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] != Key_Invalid)
            {
                InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] = Key_JustPressed;
            }
            else if(LTAxis <= 0)
            {
                InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] = Key_NotPressed;
            }
            
            InputController.JoystickKeysDown[Joystick_RightTrigger] = RTAxis > 0;
            
            if(RTAxis > 0 && InputController.JoystickKeysJustPressed[Joystick_RightTrigger] != Key_Invalid)
            {
                InputController.JoystickKeysJustPressed[Joystick_RightTrigger] = Key_JustPressed;
            }
            else if(RTAxis <= 0)
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

static void CursorPositionCallback(GLFWwindow *Window, double XPos, double YPos)
{
    InputController.MouseX = XPos;
    InputController.MouseY = YPos;
}

void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    InputController.ScrollX = XOffset;
    InputController.ScrollY = YOffset;
}

static void KeyCallback(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods)
{
    if(Key != GLFW_KEY_COMMA && Key != GLFW_KEY_PERIOD && (Key < 320 || Key > 329))
    {
        if (Action == GLFW_PRESS)
        {
            InputController.AnyKeyPressed = true;
            if (InputController.KeysJustPressed[InputController.KeyMappings[Key]] == Key_NotPressed)
            {
                InputController.KeysJustPressed[InputController.KeyMappings[Key]] = Key_JustPressed;
            }
            
            InputController.KeysDown[InputController.KeyMappings[Key]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            InputController.KeysUp[InputController.KeyMappings[Key]] = true;
            InputController.KeysJustPressed[InputController.KeyMappings[Key]] = Key_NotPressed;
            InputController.KeysDown[InputController.KeyMappings[Key]] = false;
        }
    }
}

static void MouseButtonCallback(GLFWwindow *Window, int Button, int Action, int Mods)
{
    if (Action == GLFW_PRESS)
    {
        InputController.AnyKeyPressed = true;
        if (InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[Button]] == Key_NotPressed)
        {
            InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[Button]] = Key_JustPressed;
        }
        else if (InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[Button]] == Key_JustPressed)
        {
            // NOTE(niels): Do we ever even get in here???
            InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[Button]] = Key_Invalid;
        }
        
        InputController.MouseButtonDown[InputController.MouseButtonMappings[Button]] = true;
    }
    else if (Action == GLFW_RELEASE)
    {
        InputController.MouseButtonJustPressed[InputController.MouseButtonMappings[Button]] = Key_NotPressed;
        InputController.MouseButtonDown[InputController.MouseButtonMappings[Button]] = false;
    }
}

void CharacterCallback(GLFWwindow *Window, unsigned int Codepoint)
{
    InputController.CurrentCharacter = (char)Codepoint;
}

#endif