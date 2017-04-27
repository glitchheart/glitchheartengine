static void SetInvalidKeys(input_controller *InputController)
{
    for (auto const &Pair : InputController->KeysJustPressed)
    {
        if (Pair.second == Key_JustPressed)
        {
            InputController->KeysJustPressed[Pair.first] = Key_Invalid;
        }
    }
    
    if (InputController->KeysJustPressed[Key_Tab] == Key_JustPressed)
    {
        InputController->KeysJustPressed[Key_Tab] = Key_Invalid;
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
