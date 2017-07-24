#ifndef KEYS_GLFW_H
#define KEYS_GLFW_H

void InitKeyMappings(game_state* GameState)
{
    integer_Map_Init(&GameState->KeyMappings,HashIntKeys, 348);
    GameState->KeyMappings[GLFW_KEY_LEFT] = Key_Left;
    GameState->KeyMappings[GLFW_KEY_RIGHT] = Key_Right;
    GameState->KeyMappings[GLFW_KEY_UP] = Key_Up;
    GameState->KeyMappings[GLFW_KEY_DOWN] = Key_Down;
    GameState->KeyMappings[GLFW_KEY_ESCAPE] = Key_Escape;
    GameState->KeyMappings[GLFW_KEY_SPACE] = Key_Space;
    GameState->KeyMappings[GLFW_KEY_TAB] = Key_Tab;
    GameState->KeyMappings[GLFW_KEY_BACKSPACE] = Key_Backspace;
    GameState->KeyMappings[GLFW_KEY_LEFT_SHIFT] = Key_LeftShift;
    GameState->KeyMappings[GLFW_KEY_RIGHT_SHIFT] = Key_RightShift;
    GameState->KeyMappings[GLFW_KEY_ENTER] = Key_Enter;
    GameState->KeyMappings[GLFW_KEY_LEFT_CONTROL] = Key_LeftCtrl;
    GameState->KeyMappings[GLFW_KEY_RIGHT_CONTROL] = Key_RightCtrl;
    GameState->KeyMappings[GLFW_KEY_A] = Key_A;
    GameState->KeyMappings[GLFW_KEY_B] = Key_B;
    GameState->KeyMappings[GLFW_KEY_C] = Key_C;
    GameState->KeyMappings[GLFW_KEY_D] = Key_D;
    GameState->KeyMappings[GLFW_KEY_E] = Key_E;
    GameState->KeyMappings[GLFW_KEY_F] = Key_F;
    GameState->KeyMappings[GLFW_KEY_G] = Key_G;
    GameState->KeyMappings[GLFW_KEY_H] = Key_H;
    GameState->KeyMappings[GLFW_KEY_I] = Key_I;
    GameState->KeyMappings[GLFW_KEY_J] = Key_J;
    GameState->KeyMappings[GLFW_KEY_K] = Key_K;
    GameState->KeyMappings[GLFW_KEY_L] = Key_L;
    GameState->KeyMappings[GLFW_KEY_M] = Key_M;
    GameState->KeyMappings[GLFW_KEY_N] = Key_N;
    GameState->KeyMappings[GLFW_KEY_O] = Key_O;
    GameState->KeyMappings[GLFW_KEY_P] = Key_P;
    GameState->KeyMappings[GLFW_KEY_Q] = Key_Q;
    GameState->KeyMappings[GLFW_KEY_R] = Key_R;
    GameState->KeyMappings[GLFW_KEY_S] = Key_S;
    GameState->KeyMappings[GLFW_KEY_T] = Key_T;
    GameState->KeyMappings[GLFW_KEY_U] = Key_U;
    GameState->KeyMappings[GLFW_KEY_V] = Key_V;
    GameState->KeyMappings[GLFW_KEY_W] = Key_W;
    GameState->KeyMappings[GLFW_KEY_X] = Key_X;
    GameState->KeyMappings[GLFW_KEY_Y] = Key_Y;
    GameState->KeyMappings[GLFW_KEY_Z] = Key_Z;
    GameState->KeyMappings[GLFW_KEY_0] = Key_0;
    GameState->KeyMappings[GLFW_KEY_1] = Key_1;
    GameState->KeyMappings[GLFW_KEY_2] = Key_2;
    GameState->KeyMappings[GLFW_KEY_3] = Key_3;
    GameState->KeyMappings[GLFW_KEY_4] = Key_4;
    GameState->KeyMappings[GLFW_KEY_5] = Key_5;
    GameState->KeyMappings[GLFW_KEY_6] = Key_6;
    GameState->KeyMappings[GLFW_KEY_7] = Key_7;
    GameState->KeyMappings[GLFW_KEY_8] = Key_8;
    GameState->KeyMappings[GLFW_KEY_9] = Key_9;
    GameState->KeyMappings[GLFW_KEY_KP_ADD] = Key_Add;
    GameState->KeyMappings[GLFW_KEY_KP_SUBTRACT] = Key_Subtract;
    GameState->KeyMappings[GLFW_KEY_F1] = Key_F1;
    GameState->KeyMappings[GLFW_KEY_F2] = Key_F2;
    GameState->KeyMappings[GLFW_KEY_F3] = Key_F3;
    GameState->KeyMappings[GLFW_KEY_F4] = Key_F4;
    GameState->KeyMappings[GLFW_KEY_F5] = Key_F5;
    GameState->KeyMappings[GLFW_KEY_F6] = Key_F6;
    GameState->KeyMappings[GLFW_KEY_F7] = Key_F7;
    GameState->KeyMappings[GLFW_KEY_F8] = Key_F8;
    GameState->KeyMappings[GLFW_KEY_F9] = Key_F9;
    GameState->KeyMappings[GLFW_KEY_F10] = Key_F10;
    GameState->KeyMappings[GLFW_KEY_F11] = Key_F11;
    GameState->KeyMappings[GLFW_KEY_F12] = Key_F12;
    GameState->KeyMappings[GLFW_KEY_DELETE] = Key_Delete;
}

void InitMouseButtonMappings(game_state* GameState)
{
    integer_Map_Init(&GameState->MouseButtonMappings,HashIntKeys,7);
    GameState->MouseButtonMappings[GLFW_MOUSE_BUTTON_LEFT] = Mouse_Left;
    GameState->MouseButtonMappings[GLFW_MOUSE_BUTTON_RIGHT] = Mouse_Right;
    GameState->MouseButtonMappings[GLFW_MOUSE_BUTTON_MIDDLE] = Mouse_Middle;
}


#define GLFW_JOYSTICK_17 16
#define GLFW_JOYSTICK_18 17

void InitControllerMappings(game_state* GameState)
{
    integer_Map_Init(&GameState->ControllerMappings, HashInt, 257);
    GameState->ControllerMappings[GLFW_JOYSTICK_1] = Joystick_1;
    GameState->ControllerMappings[GLFW_JOYSTICK_2] = Joystick_2;
    GameState->ControllerMappings[GLFW_JOYSTICK_3] = Joystick_3;
    GameState->ControllerMappings[GLFW_JOYSTICK_4] = Joystick_4;
    GameState->ControllerMappings[GLFW_JOYSTICK_5] = Joystick_5;
    GameState->ControllerMappings[GLFW_JOYSTICK_6] = Joystick_6;
    GameState->ControllerMappings[GLFW_JOYSTICK_7] = Joystick_7;
    GameState->ControllerMappings[GLFW_JOYSTICK_8] = Joystick_8;
    GameState->ControllerMappings[GLFW_JOYSTICK_9] = Joystick_9;
    GameState->ControllerMappings[GLFW_JOYSTICK_10] = Joystick_10;
    GameState->ControllerMappings[GLFW_JOYSTICK_11] = Joystick_11;
    GameState->ControllerMappings[GLFW_JOYSTICK_12] = Joystick_12;
    GameState->ControllerMappings[GLFW_JOYSTICK_13] = Joystick_13;
    GameState->ControllerMappings[GLFW_JOYSTICK_14] = Joystick_14;
    GameState->ControllerMappings[GLFW_JOYSTICK_15] = Joystick_15;
    GameState->ControllerMappings[GLFW_JOYSTICK_16] = Joystick_16;
    GameState->ControllerMappings[GLFW_JOYSTICK_17] = Joystick_LeftTrigger;
    GameState->ControllerMappings[GLFW_JOYSTICK_18] = Joystick_RightTrigger;
}

void InitKeys(game_state* GameState)
{
    InitKeyMappings(GameState);
    InitMouseButtonMappings(GameState);
    InitControllerMappings(GameState);
}


static b32 ControllerPresent(game_state* GameState)
{
    int Present = glfwJoystickPresent(GLFW_JOYSTICK_1); 
    
    if(Present)
    {
        GameState->InputController.ControllerPresent = true;
        const char* Name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        
        if(strstr(Name, "Xbox") != 0)
        {
            GameState->InputController.ControllerType = Controller_Xbox;
        }
        else if(strstr(Name, "PS4") != 0 || strstr(Name, "Wireless") != 0)
        {
            GameState->InputController.ControllerType = Controller_PS4;
        }
    }
    
    GameState->InputController.ControllerPresent = Present;
    return Present;
}

static void ControllerKeyCallback(game_state* GameState, int Key, int Action)
{
    if(GameState)
    {
        if (Action == GLFW_PRESS)
        {
            GameState->InputController.AnyKeyPressed = true;
            if (GameState->InputController.JoystickKeysJustPressed[GameState->ControllerMappings[Key]] == Key_NotPressed)
            {
                GameState->InputController.JoystickKeysJustPressed[GameState->ControllerMappings[Key]] = Key_JustPressed;
            }
            else if(GameState->InputController.JoystickKeysJustPressed[GameState->ControllerMappings[Key]] == Key_JustPressed)
            {
                // NOTE(niels): Do we ever even get in here???
                GameState->InputController.JoystickKeysJustPressed[GameState->ControllerMappings[Key]] = Key_Invalid;
                
            }
            GameState->InputController.JoystickKeysDown[GameState->ControllerMappings[Key]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.JoystickKeysJustPressed[GameState->ControllerMappings[Key]] = Key_NotPressed;
            GameState->InputController.JoystickKeysDown[GameState->ControllerMappings[Key]] = false;
        }
    }
}

static void ControllerKeys(game_state* GameState, u32 Joystick)
{
    i32 Count;
    const unsigned char* ButtonState = glfwGetJoystickButtons(Joystick,&Count);
    
    for(i32 i = 0; i < Count; i++)
    {
        ControllerKeyCallback(GameState,i,ButtonState[i]);
    }
    
    const float* Axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &Count);
    
    for(int Index = 0; Index < NUM_AXES; Index++)
    {
        GameState->InputController.Axes[Index] = Axes[Index];
        
        if(Index == 5 && GameState->InputController.ControllerType == Controller_Xbox)
        {
            r32 LTAxis = Axes[4];
            r32 RTAxis = Axes[5];
            
            GameState->InputController.JoystickKeysDown[Joystick_LeftTrigger] = LTAxis > 0;
            
            if(LTAxis > 0  && GameState->InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] != Key_Invalid)
            {
                GameState->InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] = Key_JustPressed;
            }
            else if(LTAxis <= 0)
            {
                GameState->InputController.JoystickKeysJustPressed[Joystick_LeftTrigger] = Key_NotPressed;
            }
            
            GameState->InputController.JoystickKeysDown[Joystick_RightTrigger] = RTAxis > 0;
            
            if(RTAxis > 0 && GameState->InputController.JoystickKeysJustPressed[Joystick_RightTrigger] != Key_Invalid)
            {
                GameState->InputController.JoystickKeysJustPressed[Joystick_RightTrigger] = Key_JustPressed;
            }
            else if(RTAxis <= 0)
            {
                GameState->InputController.JoystickKeysJustPressed[Joystick_RightTrigger] = Key_NotPressed;
            }
        }
        
        if(Abs(Axes[Index]) > GameState->InputController.AxesUsedZone)
        {
            if(GameState->InputController.AxesJustPressed[Index] == Key_JustPressed)
            {
                GameState->InputController.AxesJustPressed[Index] = Key_Invalid;
            }
            else if(GameState->InputController.AxesJustPressed[Index] != Key_Invalid)
            {
                GameState->InputController.AxesJustPressed[Index] = Key_JustPressed;
            }
        }
        else if(Abs(Axes[Index]) <= GameState->InputController.ControllerDeadzone)
        {
            GameState->InputController.AxesJustPressed[Index] = Key_NotPressed;
        }
    }
}

static void CursorPositionCallback(GLFWwindow *Window, double XPos, double YPos)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    
    if (GameState)
    {
        GameState->InputController.MouseX = XPos;
        GameState->InputController.MouseY = YPos;
    }
}

void ScrollCallback(GLFWwindow* Window, double XOffset, double YOffset)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    if (GameState)
    {
        GameState->InputController.ScrollX = XOffset;
        GameState->InputController.ScrollY = YOffset;
    }
}

static void KeyCallback(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    SetInvalidKeys(&GameState->InputController);
    
    if (GameState)
    {
        if (Action == GLFW_PRESS)
        {
            GameState->InputController.AnyKeyPressed = true;
            if (GameState->InputController.KeysJustPressed[GameState->KeyMappings[Key]] == Key_NotPressed)
            {
                GameState->InputController.KeysJustPressed[GameState->KeyMappings[Key]] = Key_JustPressed;
            }
            
            GameState->InputController.KeysDown[GameState->KeyMappings[Key]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.KeysUp[GameState->KeyMappings[Key]] = true;
            GameState->InputController.KeysJustPressed[GameState->KeyMappings[Key]] = Key_NotPressed;
            GameState->InputController.KeysDown[GameState->KeyMappings[Key]] = false;
        }
    }
}

static void MouseButtonCallback(GLFWwindow *Window, int Button, int Action, int Mods)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    SetMouseInvalidKeys(&GameState->InputController);
    
    if (GameState)
    {
        if (Action == GLFW_PRESS)
        {
            GameState->InputController.AnyKeyPressed = true;
            if (GameState->InputController.MouseButtonJustPressed[GameState->MouseButtonMappings[Button]] == Key_NotPressed)
            {
                GameState->InputController.MouseButtonJustPressed[GameState->MouseButtonMappings[Button]] = Key_JustPressed;
            }
            else if (GameState->InputController.MouseButtonJustPressed[GameState->MouseButtonMappings[Button]] == Key_JustPressed)
            {
                // NOTE(niels): Do we ever even get in here???
                GameState->InputController.MouseButtonJustPressed[GameState->MouseButtonMappings[Button]] = Key_Invalid;
            }
            
            GameState->InputController.MouseButtonDown[GameState->MouseButtonMappings[Button]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.MouseButtonJustPressed[GameState->MouseButtonMappings[Button]] = Key_NotPressed;
            GameState->InputController.MouseButtonDown[GameState->MouseButtonMappings[Button]] = false;
        }
    }
}

void CharacterCallback(GLFWwindow *Window, unsigned int Codepoint)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    if (GameState->Console.Open)
    {
        if(GameState->Console.BufferIndex < CONSOLE_BUFFER_SIZE - 1)
            GameState->Console.Buffer[GameState->Console.BufferIndex++] = (char)Codepoint;
    }
    GameState->InputController.CurrentCharacter = (char)Codepoint;
}

#endif