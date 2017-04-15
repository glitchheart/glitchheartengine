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
    if (GameState)
    {
        if (Action == GLFW_PRESS)
        {
            if(GameState->InputController.KeysJustPressed[Key] == Key_NotPressed)
            {
                GameState->InputController.KeysJustPressed[Key] = Key_JustPressed;
            }
            else if(GameState->InputController.KeysJustPressed[Key] == Key_JustPressed)
            {
                GameState->InputController.KeysJustPressed[Key] = Key_Invalid;
            }

            GameState->InputController.KeysDown[Key] = true;
        }
        else if (Action == GLFW_RELEASE)
        {
            GameState->InputController.KeysJustPressed[Key] = Key_NotPressed;
            GameState->InputController.KeysDown[Key] = false;
        }
    }
}

static bool GetKey(int Key, game_state* GameState)
{
    return GameState->InputController.KeysDown[Key];
}

static bool GetKeyDown(int Key, game_state* GameState)
{
    return GameState->InputController.KeysJustPressed[Key] == Key_JustPressed;
}

static void SetInvalidKeys(input_controller* InputController)
{
    if(InputController->KeysJustPressed[GLFW_KEY_TAB] == Key_JustPressed)
    {
        InputController->KeysJustPressed[GLFW_KEY_TAB] = Key_Invalid;
    }
}