#ifdef KEY_INIT
static void SetInvalidKeys(input_controller *InputController)
{
    for(uint32 KeyCode = 0; KeyCode < Key_Count; KeyCode++)
    {
        if(InputController->KeysJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController->KeysJustPressed[KeyCode] = Key_Invalid;
        }
    }
    
    if (InputController->KeysJustPressed[Key_Tab] == Key_JustPressed)
    {
        InputController->KeysJustPressed[Key_Tab] = Key_Invalid;
    }
}

static void SetControllerInvalidKeys(input_controller *InputController)
{
    for(uint32 KeyCode = 0; KeyCode < Joystick_Count; KeyCode++)
    {
        InputController->JoystickKeysJustPressed[KeyCode] = Key_Invalid;
    }
}

static void SetMouseInvalidKeys(input_controller *InputController)
{
    for(uint32 KeyCode = 0; KeyCode < Mouse_Count; KeyCode++)
    {
        InputController->MouseButtonJustPressed[KeyCode] = Key_Invalid;
    }
}
#endif

static bool32 GetKey(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysDown[Key];
}

static bool32 GetKeyDown(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysJustPressed[Key] == Key_JustPressed;
}

static bool32 GetKeyUp(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysUp[Key];
}

static bool32 GetJoystickKey(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysDown[Key];
}

static bool32 GetJoystickKeyDown(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysJustPressed[Key] == Key_JustPressed;
}

static bool32 GetMouseButton(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonDown[Key];
}

static bool32 GetMouseButtonDown(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonJustPressed[Key] == Key_JustPressed;
}
