static void SetInvalidKeys(input_controller *InputController)
{
    for (auto const &Pair : InputController->KeysJustPressed)
    {
        if (Pair.second == Key_JustPressed)
        {
            InputController->KeysJustPressed[Pair.first] = Key_Invalid;
        }
        
        InputController->KeysUp[Pair.first] = false;
    }
    
    if (InputController->KeysJustPressed[Key_Tab] == Key_JustPressed)
    {
        InputController->KeysJustPressed[Key_Tab] = Key_Invalid;
    }
}

static void SetControllerInvalidKeys(input_controller *InputController)
{
    for (auto const &Pair : InputController->JoystickKeysJustPressed)
    {
        if (Pair.second == Key_JustPressed)
        {
            InputController->JoystickKeysJustPressed[Pair.first] = Key_Invalid;
        }
    }
}

static void SetMouseInvalidKeys(input_controller *InputController)
{
    for (auto const &Pair : InputController->MouseButtonJustPressed)
    {
        if (Pair.second == Key_JustPressed)
        {
            InputController->MouseButtonJustPressed[Pair.first] = Key_Invalid;
        }
    }
}

static bool GetKey(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysDown[Key];
}

static bool GetKeyDown(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysJustPressed[Key] == Key_JustPressed;
}

static bool GetKeyUp(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysUp[Key];
}

static bool GetJoystickKey(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysDown[Key];
}

static bool GetJoystickKeyDown(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysJustPressed[Key] == Key_JustPressed;
}

static bool GetMouseButton(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonDown[Key];
}

static bool GetMouseButtonDown(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonJustPressed[Key] == Key_JustPressed;
}