std::map<uint32, Key_Code> KeyMappings = 
{
    { GLFW_KEY_LEFT, Key_Left },
    { GLFW_KEY_RIGHT, Key_Right },
    { GLFW_KEY_UP, Key_Up },
    { GLFW_KEY_DOWN, Key_Down },
    { GLFW_KEY_ESCAPE, Key_Escape },
    { GLFW_KEY_TAB, Key_Tab },
    { GLFW_KEY_BACKSPACE, Key_Backspace },
    { GLFW_KEY_ENTER, Key_Enter },
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
    { GLFW_KEY_Z, Key_Z }
};


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
                GameState->InputController.KeysJustPressed[KeyMappings[Key]] = Key_Invalid;
            }
            
            GameState->InputController.KeysDown[KeyMappings[Key]] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.KeysJustPressed[KeyMappings[Key]] = Key_NotPressed;
            GameState->InputController.KeysDown[KeyMappings[Key]] = false;
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