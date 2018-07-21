static inline b32 get_mouse_button(MouseCode key, InputController* input_controller)
{
    return input_controller->mouse_button_down[key];
}

static inline b32 get_mouse_button_down(MouseCode key, InputController* input_controller)
{
    return input_controller->mouse_button_just_pressed[key] == KEY_JUST_PRESSED;
}

static inline b32 get_mouse_button_up(MouseCode key, InputController* input_controller)
{
    return input_controller->mouse_buttons_up[key];
}

static inline b32 get_key(KeyCode key, InputController* input_controller)
{
    if(key == Key_MouseLeft)
        return get_mouse_button(Mouse_Left, input_controller);
    if(key == Key_MouseRight)
        return get_mouse_button(Mouse_Right, input_controller);
    return input_controller->keys_down[key];
}

static inline b32 get_key_down(KeyCode key, InputController* input_controller)
{
    if(key == Key_MouseLeft)
        return get_mouse_button_down(Mouse_Left, input_controller);
    if(key == Key_MouseRight)
        return get_mouse_button_down(Mouse_Right, input_controller);
    return input_controller->keys_just_pressed[key] == KEY_JUST_PRESSED;
}


static inline b32 get_key_down(KeyCode key, InputController& input_controller)
{
    if(key == Key_MouseLeft)
        return get_mouse_button_down(Mouse_Left, &input_controller);
    if(key == Key_MouseRight)
        return get_mouse_button_down(Mouse_Right, &input_controller);
    return input_controller.keys_just_pressed[key] == KEY_JUST_PRESSED;
}


static inline b32 get_key_up(KeyCode key, InputController* input_controller)
{
    return input_controller->keys_up[key];
}

static inline b32 get_joystick_key(ControllerCode key, InputController* input_controller)
{
    return input_controller->joystick_keys_down[key];
}

//@Incomplete: Needs direction bool
static inline b32 get_joystick_axis_x_down(InputController* input_controller, b32 for_left_direction, Stick stick = STICK_LEFT)
{
    i32 axis = stick == STICK_LEFT ? 0 : 2;
    b32 correct_direction = false;
    
    if(for_left_direction)
    {
        correct_direction = input_controller->axes[axis] < 0;
    }
    else
    {
        correct_direction = input_controller->axes[axis] > 0;
    }
    
    return correct_direction && input_controller->axes_just_pressed[axis] == KEY_JUST_PRESSED;
}

static inline b32 get_joystick_axis_y_down(InputController* input_controller, b32 for_up_direction, Stick stick = STICK_LEFT)
{
    i32 axis = stick == STICK_LEFT ? 1 : 3;
    b32 correct_direction = false;
    
    if(for_up_direction)
    {
        switch(input_controller->controller_type)
        {
            case CONTROLLER_NONE:
            {}
            break;
            case CONTROLLER_XBOX:
            correct_direction = input_controller->axes[axis] > 0;
            break;
            case CONTROLLER_PS4:
            correct_direction = input_controller->axes[axis] < 0;
            break;
        }
    }
    else
    {
        switch(input_controller->controller_type)
        {
            case CONTROLLER_NONE:
            {}
            break;
            case CONTROLLER_XBOX:
            correct_direction = input_controller->axes[axis] < 0;
            break;
            case CONTROLLER_PS4:
            correct_direction = input_controller->axes[axis] > 0;
            break;
        }
    }
    
    return correct_direction && input_controller->axes_just_pressed[axis] == KEY_JUST_PRESSED;
}

static inline b32 get_joystick_key_down(ControllerCode key, InputController* input_controller)
{
    return input_controller->joystick_keys_just_pressed[key] == KEY_JUST_PRESSED;
}

static inline r32 get_input_x(InputController* input_controller, Stick stick = STICK_LEFT)
{
    i32 axis = stick == STICK_LEFT ? 0 : 2;
    
    r32 input_x = 0.0f;
    
    if(ABS(input_controller->axes[axis]) > input_controller->controller_deadzone)
        input_x = input_controller->axes[axis]; 
    
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

static inline r32 get_input_y(InputController* input_controller, Stick stick = STICK_LEFT)
{
    i32 axis = stick == STICK_LEFT ? 1 : 3;
    
    r32 input_y = 0.0f;
    
    if(ABS(input_controller->axes[axis]) > input_controller->controller_deadzone)
    {
        switch(input_controller->controller_type)
        {
            case CONTROLLER_NONE:
            {}
            break;
            case CONTROLLER_XBOX:
            input_y = input_controller->axes[axis]; // Might be another axis index for other controllers
            break;
            case CONTROLLER_PS4:
            input_y = -1 * input_controller->axes[axis];
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

#define KEY(Key) get_key(Key, input_controller)
#define KEY_DOWN(Key) get_key_down(Key, input_controller)
#define KEY_UP(Key) get_key_up(Key, input_controller)
#define JOYSTICK_KEY(Key) get_joystick_key(Key, input_controller)
#define JOYSTICK_KEY_DOWN(Key) get_joystick_key_down(Key, input_controller)
#define JOYSTICK_AXIS_X_DOWN(Left,...) get_joystick_axis_x_down(input_controller, Left, ##__VA_ARGS__)
#define JOYSTICK_AXIS_Y_DOWN(Up, ...) get_joystick_axis_y_down(input_controller,Up , ##__VA_ARGS__)
#define MOUSE(Key) get_mouse_button(Key, input_controller)
#define MOUSE_DOWN(Key) get_mouse_button_down(Key, input_controller)
#define MOUSE_UP(Key) get_mouse_button_up(Key, input_controller)
#define INPUT_X(...) get_input_x(input_controller , ##__VA_ARGS__)
#define INPUT_Y(...) get_input_y(input_controller , ##__VA_ARGS__)
#define CUSTOM_KEY_DOWN(Key) is_custom_key_down(input_controller, Key)
#define CUSTOM_KEY(Key) is_custom_key_pressed(input_controller, Key)

// Use this to add key mappings in your game. The CustomKey int is the keys identifier for your game and when checking for key events later this will be used to automatically check with the correct mapped keyboard- or controller-keys
static void add_custom_mapping(InputController* input_controller, i32 custom_key, KeyCode keyboard_key, i32 ps4_key, i32 xbox_key)
{
    auto& custom_key_mapping = input_controller->custom_mappings[custom_key];
    custom_key_mapping.keyboard_key[custom_key_mapping.keyboard_key_count++] = keyboard_key;
    custom_key_mapping.ps4_key[custom_key_mapping.ps4_key_count++] = ps4_key;
    custom_key_mapping.xbox_key[custom_key_mapping.xbox_key_count++] = xbox_key;
}

static b32 is_custom_key_down(InputController* input_controller, i32 custom_key)
{
    auto& custom_key_mapping = input_controller->custom_mappings[custom_key];
    
    b32 result = false;
    
    if(input_controller->controller_present)
    {
        if(input_controller->controller_type == CONTROLLER_PS4)
        {
            
            for(i32 ps4_key = 0; ps4_key < custom_key_mapping.ps4_key_count; ps4_key++)
            {
                result |= JOYSTICK_KEY_DOWN(custom_key_mapping.ps4_key[ps4_key]);
            }
        }
        else
        {
            b32 result = false;
            for(i32 xbox_key = 0; xbox_key < custom_key_mapping.xbox_key_count; xbox_key++)
            {
                result |= JOYSTICK_KEY_DOWN(custom_key_mapping.xbox_key[xbox_key]);
            }
            return result;
        }
    }
    for(i32 keyboard_key = 0; keyboard_key < custom_key_mapping.keyboard_key_count; keyboard_key++)
    {
        result |= KEY_DOWN(custom_key_mapping.keyboard_key[keyboard_key]);
    }
    return result;
    
}

static b32 is_custom_key_pressed(InputController* input_controller, i32 custom_key)
{
    auto& custom_key_mapping = input_controller->custom_mappings[custom_key];
    b32 result = false;
    
    if(input_controller->controller_present)
    {
        if(input_controller->controller_type == CONTROLLER_PS4)
        {
            for(i32 ps4_key = 0; ps4_key < custom_key_mapping.ps4_key_count; ps4_key++)
            {
                result |= JOYSTICK_KEY(custom_key_mapping.ps4_key[ps4_key]);
            }
        }
        else
        {
            for(i32 xbox_key = 0; xbox_key < custom_key_mapping.xbox_key_count; xbox_key++)
            {
                result |= JOYSTICK_KEY(custom_key_mapping.xbox_key[xbox_key]);
            }
        }
    }
    for(i32 keyboard_key = 0; keyboard_key < custom_key_mapping.keyboard_key_count; keyboard_key++)
    {
        result |= KEY(custom_key_mapping.keyboard_key[keyboard_key]);
    }
    return result;
}
