static inline b32 get_mouse_button(Mouse_Code key, InputController* input_controller)
{
    return input_controller->MouseButtonDown[key];
}

static inline b32 get_mouse_button_down(Mouse_Code key, InputController* input_controller)
{
    return input_controller->MouseButtonJustPressed[key] == Key_JustPressed;
}

static inline b32 get_mouse_button_up(Mouse_Code key, InputController* input_controller)
{
    return input_controller->MouseButtonsUp[key];
}

static inline b32 get_key(Key_Code key, InputController* input_controller)
{
    if(key == Key_MouseLeft)
        return get_mouse_button(Mouse_Left, input_controller);
    if(key == Key_MouseRight)
        return get_mouse_button(Mouse_Right, input_controller);
    return input_controller->KeysDown[key];
}

static inline b32 get_key_down(Key_Code key, InputController* input_controller)
{
    if(key == Key_MouseLeft)
        return get_mouse_button_down(Mouse_Left, input_controller);
    if(key == Key_MouseRight)
        return get_mouse_button_down(Mouse_Right, input_controller);
    return input_controller->KeysJustPressed[key] == Key_JustPressed;
}


static inline b32 get_key_down(Key_Code key, InputController& input_controller)
{
    if(key == Key_MouseLeft)
        return get_mouse_button_down(Mouse_Left, &input_controller);
    if(key == Key_MouseRight)
        return get_mouse_button_down(Mouse_Right, &input_controller);
    return input_controller.KeysJustPressed[key] == Key_JustPressed;
}


static inline b32 get_key_up(Key_Code key, InputController* input_controller)
{
    return input_controller->KeysUp[key];
}

static inline b32 get_joystick_key(Controller_Code key, InputController* input_controller)
{
    return input_controller->JoystickKeysDown[key];
}

//@Incomplete: Needs direction bool
static inline b32 get_joystick_axis_x_down(InputController* input_controller, b32 for_left_direction, Stick stick = Stick_Left)
{
    i32 axis = stick == Stick_Left ? 0 : 2;
    b32 correct_direction = false;
    
    if(for_left_direction)
    {
        correct_direction = input_controller->Axes[axis] < 0;
    }
    else
    {
        
        correct_direction = input_controller->Axes[axis] > 0;
    }
    
    return correct_direction && input_controller->AxesJustPressed[axis] == Key_JustPressed;
}

static inline b32 get_joystick_axis_y_down(InputController* input_controller, b32 for_up_direction, Stick stick = Stick_Left)
{
    i32 axis = stick == Stick_Left ? 1 : 3;
    b32 correct_direction = false;
    
    if(for_up_direction)
    {
        switch(input_controller->ControllerType)
        {
            case Controller_None:
            {}
            break;
            case Controller_Xbox:
            correct_direction = input_controller->Axes[axis] > 0;
            break;
            case Controller_PS4:
            correct_direction = input_controller->Axes[axis] < 0;
            break;
        }
    }
    else
    {
        switch(input_controller->ControllerType)
        {
            case Controller_None:
            {}
            break;
            case Controller_Xbox:
            correct_direction = input_controller->Axes[axis] < 0;
            break;
            case Controller_PS4:
            correct_direction = input_controller->Axes[axis] > 0;
            break;
        }
    }
    
    return correct_direction && input_controller->AxesJustPressed[axis] == Key_JustPressed;
}

static inline b32 get_joystick_key_down(Controller_Code key, InputController* input_controller)
{
    return input_controller->JoystickKeysJustPressed[key] == Key_JustPressed;
}

static inline r32 get_input_x(InputController* input_controller, Stick stick = Stick_Left)
{
    i32 axis = stick == Stick_Left ? 0 : 2;
    
    r32 input_x = 0.0f;
    
    if(Abs(InputController->Axes[Axis]) > InputController->ControllerDeadzone)
        input_x = InputController->Axes[Axis]; 
    
    if (get_key(Key_Left, input_controller))
    {
        input_x += -1;
    }
    else if (get_key(Key_Right, input_controller))
    {
        input_x += 1;
    }
    else
        input_x += 0;
    
    return input_x;
}

static inline r32 get_input_y(InputController* input_controller, Stick stick = Stick_Left)
{
    i32 axis = stick == Stick_Left ? 1 : 3;
    
    r32 input_y = 0.0f;
    
    if(Abs(InputController->Axes[Axis]) > InputController->ControllerDeadzone)
    {
        switch(InputController->ControllerType)
        {
            case Controller_None:
            {}
            break;
            case Controller_Xbox:
            input_y = InputController->Axes[Axis]; // Might be another axis index for other controllers
            break;
            case Controller_PS4:
            input_y = -1 * InputController->Axes[Axis];
            break;
        }
    }
    
    if (get_key(Key_Up, input_controller))
    {
        input_y += 1;
    }
    else if (get_key(Key_Down, input_controller))
    {
        input_y += -1;
    }
    
    return input_y;
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
static void add_custom_mapping(InputController* input_controller, i32 custom_key, Key_Code keyboard_key, i32 p_s4_key, i32 xbox_key)
{
    auto& custom_key_mapping = input_controller->CustomMappings[custom_key];
    custom_key_mapping.KeyboardKey = keyboard_key;
    custom_key_mapping.PS4Key = p_s4_key;
    custom_key_mapping.XboxKey = xbox_key;
}

static b32 is_custom_key_down(InputController* input_controller, i32 custom_key)
{
    auto& custom_key_mapping = InputController->CustomMappings[custom_key];
    
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

static b32 is_custom_key_pressed(InputController* input_controller, i32 custom_key)
{
    auto& custom_key_mapping = InputController->CustomMappings[custom_key];
    
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