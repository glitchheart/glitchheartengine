#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

enum Key_Mode
{
    Key_NotPressed,
    Key_JustPressed,
    Key_Invalid
};

enum Key_Code
{
    Key_Left,
    Key_Right,
    Key_Up,
    Key_Down,
    Key_Escape,
    Key_F1,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12,
    Key_A,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z,
    Key_Tab,
    Key_Enter,
    Key_Backspace,
    Key_LeftShift,
    Key_LeftCtrl,
    Key_RightCtrl,
    Key_RightShift,
    Key_Delete,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_Count
};

enum Mouse_Code
{
    Mouse_Left,
    Mouse_Right,
    Mouse_Middle,
    Mouse_Count
};

enum Controller_Code
{
    Joystick_1,
    Joystick_2,
    Joystick_3,
    Joystick_4,
    Joystick_5,
    Joystick_6,
    Joystick_7,
    Joystick_8,
    Joystick_9,
    Joystick_10,
    Joystick_11,
    Joystick_12,
    Joystick_13,
    Joystick_14,
    Joystick_15,
    Joystick_16,
    Joystick_Count
};

enum Action_Button
{
    Action_Attack,
    Action_Dash,
    Action_Interact,
    
    Action_Count
};

enum Controller_Axis_Code
{
    Axis_1, // Left stick X (PS4 W10)
    Axis_2, // Left stick Y (PS4 W10)
    Axis_3, // Right stick X (PS4 W10)
    Axis_4, // Right stick Y (PS4 W10)
    Axis_5, // Left trigger (PS4 W10)
    Axis_6  // Right trigger (PS4 W10)
};

#define NUM_AXES 6

enum Controller_Type
{
    Controller_Xbox,
    Controller_PS4
};

struct input_controller
{
    Key_Code ActionButtonKeyboardBindings[Action_Count] = 
    {
        Key_Z,
        Key_X,
        Key_E
    };
    
    Controller_Code ActionButtonXboxControllerBindings[Action_Count] = 
    {
        Joystick_3,
        Joystick_1,
        Joystick_2
    };
    
    Controller_Code ActionButtonPS4ControllerBindings[Action_Count] = 
    {
        Joystick_1,
        Joystick_2,
        Joystick_1
    };
    
    bool32 KeysDown[Key_Count];
    bool32 KeysUp[Key_Count];
    Key_Mode KeysJustPressed[Key_Count];
    
    bool32 MouseButtonDown[Mouse_Count];
    Key_Mode MouseButtonJustPressed[Mouse_Count];
    
    bool32 JoystickKeysDown[Joystick_Count];
    Key_Mode JoystickKeysJustPressed[Joystick_Count];
    
    real32 Axes[NUM_AXES];
    
    real64 MouseX;
    real64 MouseY;
    real64 ScrollX;
    real64 ScrollY;
    
    real32 ControllerDeadzone = 0.1f;
    bool32 ControllerPresent;
    Controller_Type ControllerType;
    
    char CurrentCharacter;
    bool32 DeleteCharacter = false;
};

#endif