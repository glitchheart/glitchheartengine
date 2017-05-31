#ifdef KEY_INIT
static void SetInvalidKeys(input_controller *InputController)
{
    for(uint32 KeyCode = 0; KeyCode < Key_Count; KeyCode++)
    {
        if(InputController->KeysJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController->KeysJustPressed[KeyCode] = Key_Invalid;
        }
        InputController->KeysUp[KeyCode] = false;
    } 
}

static void SetControllerInvalidKeys(input_controller *InputController)
{
    for(uint32 KeyCode = 0; KeyCode < Joystick_Count; KeyCode++)
    {
        if(InputController->JoystickKeysJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController->JoystickKeysJustPressed[KeyCode] = Key_Invalid;
        }
    }
}

static void SetMouseInvalidKeys(input_controller *InputController)
{
    for(uint32 KeyCode = 0; KeyCode < Mouse_Count; KeyCode++)
    {
        if(InputController->MouseButtonJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController->MouseButtonJustPressed[KeyCode] = Key_Invalid;
        }
    }
    InputController->ScrollX = 0;
    InputController->ScrollY = 0;
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

static bool32 GetActionButtonDown(Action_Button ActionButton, game_state* GameState)
{
    if(GameState->InputController.ControllerPresent)
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            return GetJoystickKeyDown(GameState->InputController.ActionButtonXboxControllerBindings[ActionButton], GameState);
            break;
            case Controller_PS4:
            return GetJoystickKeyDown(GameState->InputController.ActionButtonPS4ControllerBindings[ActionButton], GameState);
            break;
        }
    }
    else
    {
        return GetKeyDown(GameState->InputController.ActionButtonKeyboardBindings[ActionButton], GameState);
    }
}

static float GetInputX(game_state* GameState)
{
    if(GameState->InputController.ControllerPresent)
    {
        if(Abs(GameState->InputController.Axes[0]) < GameState->InputController.ControllerDeadzone)
            return 0;
        return GameState->InputController.Axes[0]; // Might be another axis index for other controllers
    }
    else
    {
        if (GetKey(Key_Left, GameState))
        {
            return -1;
        }
        else if (GetKey(Key_Right, GameState))
        {
            return 1;
        }
        else
            return 0;
    }
}

static float GetInputY(game_state* GameState)
{
    if(GameState->InputController.ControllerPresent)
    {
        if(Abs(GameState->InputController.Axes[1]) < GameState->InputController.ControllerDeadzone)
            return 0;
        
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            return -1 * GameState->InputController.Axes[1]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            return GameState->InputController.Axes[1]; // Might be another axis index for other controllers
            break;
        }
    }
    else
    {
        if (GetKey(Key_Up, GameState))
        {
            return -1;
        }
        else if (GetKey(Key_Down, GameState))
        {
            return 1;
        }
        else
            return 0;
    }
}
