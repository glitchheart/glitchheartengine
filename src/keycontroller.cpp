static inline b32 GetMouseButton(Mouse_Code Key, input_controller* InputController)
{
    return InputController->MouseButtonDown[Key];
}

static inline b32 GetMouseButtonDown(Mouse_Code Key, input_controller* InputController)
{
    return InputController->MouseButtonJustPressed[Key] == Key_JustPressed;
}

static inline b32 GetMouseButtonUp(Mouse_Code Key, input_controller* InputController)
{
    return InputController->MouseButtonsUp[Key];
}

static inline b32 GetKey(Key_Code Key, input_controller* InputController)
{
    if(Key == Key_MouseLeft)
        return GetMouseButton(Mouse_Left, InputController);
    if(Key == Key_MouseRight)
        return GetMouseButton(Mouse_Right, InputController);
    return InputController->KeysDown[Key];
}

static inline b32 GetKeyDown(Key_Code Key, input_controller* InputController)
{
    if(Key == Key_MouseLeft)
        return GetMouseButtonDown(Mouse_Left, InputController);
    if(Key == Key_MouseRight)
        return GetMouseButtonDown(Mouse_Right, InputController);
    return InputController->KeysJustPressed[Key] == Key_JustPressed;
}


static inline b32 GetKeyDown(Key_Code Key, input_controller& InputController)
{
    if(Key == Key_MouseLeft)
        return GetMouseButtonDown(Mouse_Left, &InputController);
    if(Key == Key_MouseRight)
        return GetMouseButtonDown(Mouse_Right, &InputController);
    return InputController.KeysJustPressed[Key] == Key_JustPressed;
}


static inline b32 GetKeyUp(Key_Code Key, input_controller* InputController)
{
    return InputController->KeysUp[Key];
}

static inline b32 GetJoystickKey(Controller_Code Key, input_controller* InputController)
{
    return InputController->JoystickKeysDown[Key];
}

//@Incomplete: Needs direction bool
static inline b32 GetJoystickAxisXDown(input_controller* InputController, b32 ForLeftDirection, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 0 : 2;
    b32 CorrectDirection = false;
    
    if(ForLeftDirection)
    {
        CorrectDirection = InputController->Axes[Axis] < 0;
    }
    else
    {
        
        CorrectDirection = InputController->Axes[Axis] > 0;
    }
    
    return CorrectDirection && InputController->AxesJustPressed[Axis] == Key_JustPressed;
}

static inline b32 GetJoystickAxisYDown(input_controller* InputController, b32 ForUpDirection, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 1 : 3;
    b32 CorrectDirection = false;
    
    if(ForUpDirection)
    {
        switch(InputController->ControllerType)
        {
            case Controller_None:
            {}
            break;
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
            case Controller_None:
            {}
            break;
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

static inline b32 GetJoystickKeyDown(Controller_Code Key, input_controller* InputController)
{
    return InputController->JoystickKeysJustPressed[Key] == Key_JustPressed;
}

static inline r32 GetInputX(input_controller* InputController, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 0 : 2;
    
    r32 InputX = 0.0f;
    
    if(Abs(InputController->Axes[Axis]) > InputController->ControllerDeadzone)
        InputX = InputController->Axes[Axis]; 
    
    if (GetKey(Key_Left, InputController))
    {
        InputX += -1;
    }
    else if (GetKey(Key_Right, InputController))
    {
        InputX += 1;
    }
    else
        InputX += 0;
    
    return InputX;
}

static inline r32 GetInputY(input_controller* InputController, Stick Stick = Stick_Left)
{
    i32 Axis = Stick == Stick_Left ? 1 : 3;
    
    r32 InputY = 0.0f;
    
    if(Abs(InputController->Axes[Axis]) > InputController->ControllerDeadzone)
    {
        switch(InputController->ControllerType)
        {
            case Controller_None:
            {}
            break;
            case Controller_Xbox:
            InputY = InputController->Axes[Axis]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            InputY = -1 * InputController->Axes[Axis];
            break;
        }
    }
    
    if (GetKey(Key_Up, InputController))
    {
        InputY += 1;
    }
    else if (GetKey(Key_Down, InputController))
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
#define JOYSTICK_AXIS_X_DOWN(...) GetJoystickAxisXDown(InputController ,##__VA_ARGS__)
#define JOYSTICK_AXIS_Y_DOWN(Up, ...) GetJoystickAxisYDown(InputController,Up , ##__VA_ARGS__)
#define MOUSE(Key) GetMouseButton(Key, InputController)
#define MOUSE_DOWN(Key) GetMouseButtonDown(Key, InputController)
#define MOUSE_UP(Key) GetMouseButtonUp(Key, InputController)
#define INPUT_X(...) GetInputX(InputController , ##__VA_ARGS__)
#define INPUT_Y(...) GetInputY(InputController , ##__VA_ARGS__)
#define CUSTOM_KEY_DOWN(Key) IsCustomKeyDown(InputController, Key)
#define CUSTOM_KEY(Key) IsCustomKeyPressed(InputController, Key)

// Use this to add key mappings in your game. The CustomKey int is the keys identifier for your game and when checking for key events later this will be used to automatically check with the correct mapped keyboard- or controller-keys
static void AddCustomMapping(input_controller* InputController, i32 CustomKey, Key_Code KeyboardKey, i32 PS4Key, i32 XboxKey)
{
    auto& CustomKeyMapping = InputController->CustomMappings[CustomKey];
    CustomKeyMapping.KeyboardKey = KeyboardKey;
    CustomKeyMapping.PS4Key = PS4Key;
    CustomKeyMapping.XboxKey = XboxKey;
}

static b32 IsCustomKeyDown(input_controller* InputController, i32 CustomKey)
{
    auto& CustomKeyMapping = InputController->CustomMappings[CustomKey];
    
    if(InputController->ControllerPresent)
    {
        if(InputController->ControllerType == Controller_PS4)
        {
            return JOYSTICK_KEY_DOWN(CustomKeyMapping.PS4Key);
        }
        else
        {
            return JOYSTICK_KEY_DOWN(CustomKeyMapping.XboxKey);
        }
    }
    else
    {
        return KEY_DOWN(CustomKeyMapping.KeyboardKey);
    }
}

static b32 IsCustomKeyPressed(input_controller* InputController, i32 CustomKey)
{
    auto& CustomKeyMapping = InputController->CustomMappings[CustomKey];
    
    if(InputController->ControllerPresent)
    {
        if(InputController->ControllerType == Controller_PS4)
        {
            return JOYSTICK_KEY(CustomKeyMapping.PS4Key);
        }
        else
        {
            return JOYSTICK_KEY(CustomKeyMapping.XboxKey);
        }
    }
    else
    {
        return KEY(CustomKeyMapping.KeyboardKey);
    }
}