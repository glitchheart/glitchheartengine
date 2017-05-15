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
    Key_LeftCtrl,
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
    JOYSTICK_1,
    JOYSTICK_2,
    JOYSTICK_3,
    JOYSTICK_4,
    JOYSTICK_5,
    JOYSTICK_6,
    JOYSTICK_7,
    JOYSTICK_8,
    JOYSTICK_9,
    JOYSTICK_10,
    JOYSTICK_11,
    JOYSTICK_12,
    JOYSTICK_13,
    JOYSTICK_14,
    JOYSTICK_15,
    JOYSTICK_16,
    Joystick_Count
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


struct input_controller
{
    bool32 KeysDown[Key_Count];
    bool32 KeysUp[Key_Count];
    Key_Mode KeysJustPressed[Key_Count];
    
    bool32 MouseButtonDown[Mouse_Count];
    Key_Mode MouseButtonJustPressed[Mouse_Count];
    
    bool32 JoystickKeysDown[Joystick_Count];
    Key_Mode JoystickKeysJustPressed[Joystick_Count];
    
    real64 MouseX;
    real64 MouseY;
    bool32 ControllerPresent;
};

#endif