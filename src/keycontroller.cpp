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
	    GameState->InputController.KeysDown[Key] = true;
	else if (Action == GLFW_RELEASE)
	    GameState->InputController.KeysDown[Key] = false;
    }
}

static bool IsKeyDown(int Key, game_state *GameState)
{
    return GameState->InputController.KeysDown[Key];
}