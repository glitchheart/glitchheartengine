#ifdef KEY_INIT
static void SetInvalidKeys(input_controller *InputController)
{
    InputController->AnyKeyPressed = false;
    for(u32 KeyCode = 0; KeyCode < NUM_KEYS; KeyCode++)
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
    for(u32 KeyCode = 0; KeyCode < NUM_JOYSTICK_KEYS; KeyCode++)
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
    for(u32 KeyCode = 0; KeyCode < NUM_MOUSE_BUTTONS; KeyCode++)
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

b32 GetMouseButton(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonDown[Key];
}

b32 GetMouseButtonDown(Mouse_Code Key, game_state* GameState)
{
    return GameState->InputController.MouseButtonJustPressed[Key] == Key_JustPressed;
}

b32 GetKey(Key_Code Key, game_state *GameState)
{
    if(Key == Key_MouseLeft)
        return GetMouseButton(Mouse_Left, GameState);
    if(Key == Key_MouseRight)
        return GetMouseButton(Mouse_Right, GameState);
    return GameState->InputController.KeysDown[Key];
}

b32 GetKeyDown(Key_Code Key, game_state *GameState)
{
    if(Key == Key_MouseLeft)
        return GetMouseButtonDown(Mouse_Left, GameState);
    if(Key == Key_MouseRight)
        return GetMouseButtonDown(Mouse_Right, GameState);
    return GameState->InputController.KeysJustPressed[Key] == Key_JustPressed;
}

b32 GetKeyUp(Key_Code Key, game_state *GameState)
{
    return GameState->InputController.KeysUp[Key];
}

b32 GetJoystickKey(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysDown[Key];
}

//@Incomplete: Needs direction bool
b32 GetJoystickAxisXDown(game_state* GameState, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 0 : 2;
    
    return GameState->InputController.AxesJustPressed[Axis] == Key_JustPressed;
}

b32 GetJoystickAxisYDown(game_state* GameState, b32 ForUpDirection, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 1 : 3;
    b32 CorrectDirection = false;
    
    if(ForUpDirection)
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            CorrectDirection = GameState->InputController.Axes[Axis] > 0;
            break;
            case Controller_PS4:
            CorrectDirection = GameState->InputController.Axes[Axis] < 0;
            break;
        }
    }
    else
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            CorrectDirection = GameState->InputController.Axes[Axis] < 0;
            break;
            case Controller_PS4:
            CorrectDirection = GameState->InputController.Axes[Axis] > 0;
            break;
        }
    }
    
    return CorrectDirection && GameState->InputController.AxesJustPressed[Axis] == Key_JustPressed;
}

b32 GetJoystickKeyDown(Controller_Code Key, game_state* GameState)
{
    return GameState->InputController.JoystickKeysJustPressed[Key] == Key_JustPressed;
}

void GetActionButtonsForQueue(game_state* GameState)
{
    auto InputController = &GameState->InputController;
    b32 ActionQueued = false;
    
    if(GameState->InputController.ActionRunning)
    {
        for(i32 Action = 0; Action < Action_Count; Action++)
        {
            if(InputController->ControllerPresent)
            {
                if(InputController->ControllerType == Controller_PS4)
                {
                    if(GetJoystickKeyDown(InputController->ActionButtonPS4ControllerBindings[Action], GameState))
                    {
                        InputController->NextAction = (Action_Button)Action;
                        GameState->InputController.HasQueuedAction = true;
                        ActionQueued = true;
                    }
                }
                else
                {
                    if(GetJoystickKeyDown(InputController->ActionButtonXboxControllerBindings[Action], GameState))
                    {
                        InputController->NextAction = (Action_Button)Action;
                        GameState->InputController.HasQueuedAction = true;
                        ActionQueued = true;
                    }
                }
            }
            
            if(!ActionQueued)
            {
                if(GetKeyDown(InputController->ActionButtonKeyboardBindings[Action], GameState))
                {
                    InputController->NextAction = (Action_Button)Action;
                    GameState->InputController.HasQueuedAction = true;
                }
            }
        }
    }
}

void ResetActionButtonQueue(game_state* GameState)
{
    GameState->InputController.HasQueuedAction = false;
}

b32 GetActionButtonDown(Action_Button ActionButton, game_state* GameState)
{
    if(GameState->InputController.HasQueuedAction)
    {
        if((Action_Button)GameState->InputController.NextAction == ActionButton)
        {
            GameState->InputController.HasQueuedAction = false;
            return true;
        }
        else return false;
    }
    else
    {
        b32 ButtonDown = false;
        if(GameState->InputController.ControllerPresent)
        {
            switch(GameState->InputController.ControllerType)
            {
                case Controller_Xbox:
                {
                    ButtonDown = ButtonDown ||  GetJoystickKeyDown(GameState->InputController.ActionButtonXboxControllerBindings[ActionButton], GameState);
                }
                case Controller_PS4:
                {
                    ButtonDown = ButtonDown || GetJoystickKeyDown(GameState->InputController.ActionButtonPS4ControllerBindings[ActionButton], GameState);
                }
            }
        }
        
        ButtonDown = ButtonDown ||  GetKeyDown(GameState->InputController.ActionButtonKeyboardBindings[ActionButton], GameState);
        
        return ButtonDown;
    }
}

b32 GetActionButton(Action_Button ActionButton, game_state* GameState)
{
    b32 Button = false;
    if(GameState->InputController.ControllerPresent)
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            {
                Button = Button || GetJoystickKey(GameState->InputController.ActionButtonXboxControllerBindings[ActionButton], GameState);
            }
            case Controller_PS4:
            {
                Button = Button || GetJoystickKey(GameState->InputController.ActionButtonPS4ControllerBindings[ActionButton], GameState);
            }
        }
    }
    
    Button = Button || GetKey(GameState->InputController.ActionButtonKeyboardBindings[ActionButton], GameState);
    
    return Button;
}

float GetInputX(game_state* GameState, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 0 : 2;
    
    r32 InputX = 0.0f;
    
    if(Abs(GameState->InputController.Axes[Axis]) > GameState->InputController.ControllerDeadzone)
        InputX = GameState->InputController.Axes[Axis]; 
    
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
    i32 Axis = Stick == Stick_Left ? 1 : 3;
    
    r32 InputY = 0.0f;
    
    if(Abs(GameState->InputController.Axes[Axis]) > GameState->InputController.ControllerDeadzone)
    {
        switch(GameState->InputController.ControllerType)
        {
            case Controller_Xbox:
            InputY = GameState->InputController.Axes[Axis]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            InputY = -1 * GameState->InputController.Axes[Axis];
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
