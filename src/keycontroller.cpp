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
    
    real32 InputX = 0.0f;
    
    if(Abs(GameState->InputController.Axes[Axis]) < GameState->InputController.ControllerDeadzone)
        InputX = 0;
    else
        InputX = GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
    
    if (GetKey(Key_A, GameState))
    {
        InputX += -1;
    }
    else if (GetKey(Key_D, GameState))
    {
        InputX += 1;
    }
    else
        InputX += 0;
    
    return InputX;
}

float GetInputY(game_state* GameState, Stick Stick = Stick_Left)
{
    int32 Axis = Stick == Stick_Left ? 1 : 3;
    
    real32 InputY = 0.0f;
    
    if(Abs(GameState->InputController.Axes[Axis]) < GameState->InputController.ControllerDeadzone)
    {
        InputY = 0.0f;
    }
    else
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            InputY = GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            InputY = -1 * GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
            break;
        }
    }
    
    if (GetKey(Key_W, GameState))
    {
        InputY += 1;
    }
    else if (GetKey(Key_S, GameState))
    {
        InputY += -1;
    }
    
    return InputY;
}
