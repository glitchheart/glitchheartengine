b32 GetMouseButton(Mouse_Code Key, input_controller* InputController)
{
    return InputController->MouseButtonDown[Key];
}

b32 GetMouseButtonDown(Mouse_Code Key, input_controller* InputController)
{
    return InputController->MouseButtonJustPressed[Key] == Key_JustPressed;
}

b32 GetKey(Key_Code Key, input_controller* InputController)
{
    if(Key == Key_MouseLeft)
        return GetMouseButton(Mouse_Left, InputController);
    if(Key == Key_MouseRight)
        return GetMouseButton(Mouse_Right, InputController);
    return InputController->KeysDown[Key];
}

b32 GetKeyDown(Key_Code Key, input_controller* InputController)
{
    if(Key == Key_MouseLeft)
        return GetMouseButtonDown(Mouse_Left, InputController);
    if(Key == Key_MouseRight)
        return GetMouseButtonDown(Mouse_Right, InputController);
    return InputController->KeysJustPressed[Key] == Key_JustPressed;
}

b32 GetKeyUp(Key_Code Key, input_controller* InputController)
{
    return InputController->KeysUp[Key];
}

b32 GetJoystickKey(Controller_Code Key, input_controller* InputController)
{
    return InputController->JoystickKeysDown[Key];
}

//@Incomplete: Needs direction bool
b32 GetJoystickAxisXDown(input_controller* InputController, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 0 : 2;
    
    return InputController->AxesJustPressed[Axis] == Key_JustPressed;
}

b32 GetJoystickAxisYDown(input_controller* InputController, b32 ForUpDirection, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 1 : 3;
    b32 CorrectDirection = false;
    
    if(ForUpDirection)
    {
        switch(InputController->ControllerType)
        {
            case Controller_Xbox:
            CorrectDirection = InputController->Axes[Axis] > 0;
            break;
            case Controller_PS4:
            CorrectDirection = InputController->Axes[Axis] < 0;
            break;
        }
    }
    else
    {
        switch(InputController->ControllerType)
        {
            case Controller_Xbox:
            CorrectDirection = InputController->Axes[Axis] < 0;
            break;
            case Controller_PS4:
            CorrectDirection = InputController->Axes[Axis] > 0;
            break;
        }
    }
    
    return CorrectDirection && InputController->AxesJustPressed[Axis] == Key_JustPressed;
}

b32 GetJoystickKeyDown(Controller_Code Key, input_controller* InputController)
{
    return InputController->JoystickKeysJustPressed[Key] == Key_JustPressed;
}

void GetActionButtonsForQueue(input_controller* InputController)
{
    b32 ActionQueued = false;
    
    if(InputController->ActionRunning)
    {
        for(i32 Action = 0; Action < Action_Count; Action++)
        {
            if(InputController->ControllerPresent)
            {
                if(InputController->ControllerType == Controller_PS4)
                {
                    if(GetJoystickKeyDown(InputController->ActionButtonPS4ControllerBindings[Action], InputController))
                    {
                        InputController->NextAction = (Action_Button)Action;
                        InputController->HasQueuedAction = true;
                        ActionQueued = true;
                    }
                }
                else
                {
                    if(GetJoystickKeyDown(InputController->ActionButtonXboxControllerBindings[Action], InputController))
                    {
                        InputController->NextAction = (Action_Button)Action;
                        InputController->HasQueuedAction = true;
                        ActionQueued = true;
                    }
                }
            }
            
            if(!ActionQueued)
            {
                if(GetKeyDown(InputController->ActionButtonKeyboardBindings[Action], InputController))
                {
                    InputController->NextAction = (Action_Button)Action;
                    InputController->HasQueuedAction = true;
                }
            }
        }
    }
}

void ResetActionButtonQueue(input_controller* InputController)
{
    InputController->HasQueuedAction = false;
}

b32 GetActionButtonDown(Action_Button ActionButton, input_controller* InputController)
{
    if(InputController->HasQueuedAction)
    {
        if((Action_Button)InputController->NextAction == ActionButton)
        {
            InputController->HasQueuedAction = false;
            return true;
        }
        else return false;
    }
    else
    {
        b32 ButtonDown = false;
        if(InputController->ControllerPresent)
        {
            switch(InputController->ControllerType)
            {
                case Controller_Xbox:
                {
                    ButtonDown = ButtonDown ||  GetJoystickKeyDown(InputController->ActionButtonXboxControllerBindings[ActionButton], InputController);
                }
                break;
                case Controller_PS4:
                {
                    ButtonDown = ButtonDown || GetJoystickKeyDown(InputController->ActionButtonPS4ControllerBindings[ActionButton], InputController);
                }
            }
        }
        
        ButtonDown = ButtonDown ||  GetKeyDown(InputController->ActionButtonKeyboardBindings[ActionButton], InputController);
        
        return ButtonDown;
    }
}

b32 GetActionButton(Action_Button ActionButton, input_controller* InputController)
{
    b32 Button = false;
    if(InputController->ControllerPresent)
    {
        switch(InputController->ControllerType)
        {
            case Controller_Xbox:
            {
                Button = Button || GetJoystickKey(InputController->ActionButtonXboxControllerBindings[ActionButton], InputController);
            }
            break;
            case Controller_PS4:
            {
                Button = Button || GetJoystickKey(InputController->ActionButtonPS4ControllerBindings[ActionButton], InputController);
            }
        }
    }
    
    Button = Button || GetKey(InputController->ActionButtonKeyboardBindings[ActionButton], InputController);
    
    return Button;
}

float GetInputX(input_controller* InputController, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 0 : 2;
    
    r32 InputX = 0.0f;
    
    if(Abs(InputController->Axes[Axis]) > InputController->ControllerDeadzone)
        InputX = InputController->Axes[Axis]; 
    
    if (GetKey(Key_A, InputController))
    {
        InputX += -1;
    }
    else if (GetKey(Key_D, InputController))
    {
        InputX += 1;
    }
    else
        InputX += 0;
    
    return InputX;
}

float GetInputY(input_controller* InputController, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 1 : 3;
    
    r32 InputY = 0.0f;
    
    if(Abs(InputController->Axes[Axis]) > InputController->ControllerDeadzone)
    {
        switch(InputController->ControllerType)
        {
            case Controller_Xbox:
            InputY = InputController->Axes[Axis]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            InputY = -1 * InputController->Axes[Axis];
            break;
        }
    }
    
    if (GetKey(Key_W, InputController))
    {
        InputY += 1;
    }
    else if (GetKey(Key_S, InputController))
    {
        InputY += -1;
    }
    
    return InputY;
}

#define KEY(Key) GetKey(Key, InputController)
#define KEY_DOWN(Key) GetKeyDown(Key, InputController)
#define KEY_UP(Key) GetKeyUp(Key, InputController)
#define JOYSTICK_KEY(Key) GetJoystickKey(Key, InputController)
#define JOYSTICK_KEY_DOWN(Key) GetJoystickKeyDown(Key, InputController)
#define JOYSTICK_AXIS_X_DOWN(...) GetJoystickAxisXDown(InputController,__VA_ARGS__)
#define JOYSTICK_AXIS_Y_DOWN(Up, ...) GetJoystickAxisYDown(InputController,Up, __VA_ARGS__)
#define MOUSE(Key) GetMouseButton(Key, InputController)
#define MOUSE_DOWN(Key) GetMouseButtonDown(Key, InputController)
#define ACTION_DOWN(Action) GetActionButtonDown(Action, InputController)
#define ACTION(Action) GetActionButton(Action, InputController)
#define INPUT_X(...) GetInputX(InputController,__VA_ARGS__)
#define INPUT_Y(...) GetInputX(InputController,__VA_ARGS__)
