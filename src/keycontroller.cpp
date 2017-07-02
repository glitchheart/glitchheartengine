#ifdef KEY_INIT
static void SetInvalidKeys(input_controller *InputController)
{
    InputController->AnyKeyPressed = false;
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
    InputController->AnyKeyPressed = false;
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
    InputController->AnyKeyPressed = false;
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

bool32 GetMouseButton(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonDown[Key];
}

bool32 GetMouseButtonDown(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonJustPressed[Key] == Key_JustPressed;
}

bool32 GetKey(Key_Code Key, game_state *GameState)
{
    if(Key == Key_MouseLeft)
        return GetMouseButton(Mouse_Left, GameState);
    if(Key == Key_MouseRight)
        return GetMouseButton(Mouse_Right, GameState);
    return GameState->InputController.KeysDown[Key];
}

bool32 GetKeyDown(Key_Code Key, game_state *GameState)
{
    if(Key == Key_MouseLeft)
        return GetMouseButtonDown(Mouse_Left, GameState);
    if(Key == Key_MouseRight)
        return GetMouseButtonDown(Mouse_Right, GameState);
    return GameState->InputController.KeysJustPressed[Key] == Key_JustPressed;
}

bool32 GetKeyUp(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysUp[Key];
}

bool32 GetJoystickKey(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysDown[Key];
}

bool32 GetJoystickKeyDown(Controller_Code Key, game_state* GameState)
{
    /*if(GameState->InputController.ControllerType == Xbox)
    {
        switch(Key)
        {
            case Joystick_
        }
    }*/
    
    return GameState->InputController.JoystickKeysJustPressed[Key] == Key_JustPressed;
}

bool32 GetActionButtonDown(Action_Button ActionButton, game_state* GameState)
{
    if(GameState->InputController.ControllerPresent)
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            {
                return GetJoystickKeyDown(GameState->InputController.ActionButtonXboxControllerBindings[ActionButton], GameState);
            }
            case Controller_PS4:
            {
                return GetJoystickKeyDown(GameState->InputController.ActionButtonPS4ControllerBindings[ActionButton], GameState);
            }
        }
    }
    else
    {
        return GetKeyDown(GameState->InputController.ActionButtonKeyboardBindings[ActionButton], GameState);
    }
    return 0;
}

float GetInputX(game_state* GameState, Stick Stick = Stick_Left)
{
    int32 Axis = Stick == Stick_Left ? 0 : 2;
    if(GameState->InputController.ControllerPresent)
    {
        if(Abs(GameState->InputController.Axes[Axis]) < GameState->InputController.ControllerDeadzone)
            return 0;
        return GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
    }
    else
    {
        if (GetKey(Key_A, GameState))
        {
            return -1;
        }
        else if (GetKey(Key_D, GameState))
        {
            return 1;
        }
        else
            return 0;
    }
}

float GetInputY(game_state* GameState, Stick Stick = Stick_Left)
{
    int32 Axis = Stick == Stick_Left ? 1 : 3;
    if(GameState->InputController.ControllerPresent)
    {
        if(Abs(GameState->InputController.Axes[Axis]) < GameState->InputController.ControllerDeadzone)
            return 0;
        
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            return GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            return -1 * GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
            break;
        }
    }
    else
    {
        if (GetKey(Key_W, GameState))
        {
            return 1;
        }
        else if (GetKey(Key_S, GameState))
        {
            return -1;
        }
    }
    return 0;
}
