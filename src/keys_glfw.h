
std::map<uint32, Key_Code> KeyMappings = 
{
    { GLFW_KEY_LEFT, Key_Left },
    { GLFW_KEY_RIGHT, Key_Right },
    { GLFW_KEY_UP, Key_Up },
    { GLFW_KEY_DOWN, Key_Down },
    { GLFW_KEY_ESCAPE, Key_Escape },
    { GLFW_KEY_TAB, Key_Tab },
    { GLFW_KEY_BACKSPACE, Key_Backspace },
    { GLFW_KEY_LEFT_SHIFT, Key_LeftShift },
    { GLFW_KEY_ENTER, Key_Enter },
    { GLFW_KEY_LEFT_CONTROL, Key_LeftCtrl },
    { GLFW_KEY_A, Key_A },
    { GLFW_KEY_B, Key_B },
    { GLFW_KEY_C, Key_C },
    { GLFW_KEY_D, Key_D },
    { GLFW_KEY_E, Key_E },
    { GLFW_KEY_F, Key_F },
    { GLFW_KEY_G, Key_G },
    { GLFW_KEY_H, Key_H },
    { GLFW_KEY_I, Key_I },
    { GLFW_KEY_J, Key_J },
    { GLFW_KEY_K, Key_K },
    { GLFW_KEY_L, Key_L },
    { GLFW_KEY_M, Key_M },
    { GLFW_KEY_N, Key_N },
    { GLFW_KEY_O, Key_O },
    { GLFW_KEY_Q, Key_Q },
    { GLFW_KEY_R, Key_R },
    { GLFW_KEY_S, Key_S },
    { GLFW_KEY_T, Key_T },
    { GLFW_KEY_U, Key_U },
    { GLFW_KEY_V, Key_V },
    { GLFW_KEY_W, Key_W },
    { GLFW_KEY_X, Key_X },
    { GLFW_KEY_Y, Key_Y },
    { GLFW_KEY_Z, Key_Z },
    { GLFW_KEY_F1, Key_F1 },
    { GLFW_KEY_F2, Key_F2 }
};

std::map<uint32, Mouse_Code> MouseButtonMappings = 
{
    { GLFW_MOUSE_BUTTON_LEFT, Mouse_Left },
    { GLFW_MOUSE_BUTTON_RIGHT, Mouse_Right },
    { GLFW_MOUSE_BUTTON_MIDDLE, Mouse_Middle }
};

std::map<uint32, Controller_Code> ControllerMappings =
{
    {GLFW_JOYSTICK_1, JOYSTICK_1},
    {GLFW_JOYSTICK_2, JOYSTICK_2},
    {GLFW_JOYSTICK_3, JOYSTICK_3},
    {GLFW_JOYSTICK_4, JOYSTICK_4},
    {GLFW_JOYSTICK_5, JOYSTICK_5},
    {GLFW_JOYSTICK_6, JOYSTICK_6},
    {GLFW_JOYSTICK_7, JOYSTICK_7},
    {GLFW_JOYSTICK_8, JOYSTICK_8},
    {GLFW_JOYSTICK_9, JOYSTICK_9},
    {GLFW_JOYSTICK_10, JOYSTICK_10},
    {GLFW_JOYSTICK_11, JOYSTICK_11},
    {GLFW_JOYSTICK_12, JOYSTICK_12},
    {GLFW_JOYSTICK_13, JOYSTICK_13},
    {GLFW_JOYSTICK_14, JOYSTICK_14},
    {GLFW_JOYSTICK_15, JOYSTICK_15},
    {GLFW_JOYSTICK_16, JOYSTICK_16},
};

static void ControllerKeyCallback(game_state* GameState, int Key, int Action)
{
    if(GameState)
    {
        // NOTE(niels): This didn't seem to do anything? SetInvalidKeys(&GameState->InputController);
        if (Action == GLFW_PRESS)
        {
            printf("Key: %d\n",Key);
            if (GameState->InputController.JoystickKeysJustPressed[ControllerMappings[Key]] == Key_NotPressed)
            {
                GameState->InputController.JoystickKeysJustPressed[ControllerMappings[Key]] = Key_JustPressed;
            }
            else if(GameState->InputController.JoystickKeysJustPressed[ControllerMappings[Key]] == Key_JustPressed)
            {
                // NOTE(niels): Do we ever even get in here???
                GameState->InputController.JoystickKeysJustPressed[ControllerMappings[Key]] = Key_Invalid;
                
            }
            GameState->InputController.JoystickKeysDown[ControllerMappings[Key]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.JoystickKeysJustPressed[ControllerMappings[Key]] = Key_NotPressed;
            GameState->InputController.JoystickKeysDown[ControllerMappings[Key]] = false;
        }
    }
}

static void ControllerKeys(game_state* GameState, uint32 Joystick)
{
    int32 Count;
    const unsigned char* ButtonState = glfwGetJoystickButtons(Joystick,&Count);
    for(int32 i = 0; i < Count; i++)
    {
        ControllerKeyCallback(GameState,i,ButtonState[i]);
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

static void KeyCallback(GLFWwindow *Window, int Key, int Scancode, int Action, int Mods)
{
    game_state *GameState = (game_state *)glfwGetWindowUserPointer(Window);
    SetInvalidKeys(&GameState->InputController);
    
    if (GameState)
    {
        if (Action == GLFW_PRESS)
        {
            if (GameState->InputController.KeysJustPressed[KeyMappings[Key]] == Key_NotPressed)
            {
                GameState->InputController.KeysJustPressed[KeyMappings[Key]] = Key_JustPressed;
            }
            else if (GameState->InputController.KeysJustPressed[KeyMappings[Key]] == Key_JustPressed)
            {
                // NOTE(niels): Do we ever even get in here???
                GameState->InputController.KeysJustPressed[KeyMappings[Key]] = Key_Invalid;
            }
            
            GameState->InputController.KeysDown[KeyMappings[Key]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.KeysUp[KeyMappings[Key]] = true;
            GameState->InputController.KeysJustPressed[KeyMappings[Key]] = Key_NotPressed;
            GameState->InputController.KeysDown[KeyMappings[Key]] = false;
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
            if (GameState->InputController.MouseButtonJustPressed[MouseButtonMappings[Button]] == Key_NotPressed)
            {
                GameState->InputController.MouseButtonJustPressed[MouseButtonMappings[Button]] = Key_JustPressed;
            }
            else if (GameState->InputController.MouseButtonJustPressed[MouseButtonMappings[Button]] == Key_JustPressed)
            {
                // NOTE(niels): Do we ever even get in here???
                GameState->InputController.MouseButtonJustPressed[MouseButtonMappings[Button]] = Key_Invalid;
            }
            
            GameState->InputController.MouseButtonDown[MouseButtonMappings[Button]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.MouseButtonJustPressed[MouseButtonMappings[Button]] = Key_NotPressed;
            GameState->InputController.MouseButtonDown[MouseButtonMappings[Button]] = false;
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
}