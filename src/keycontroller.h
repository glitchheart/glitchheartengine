#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include <map>

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
};

enum Mouse_Code
{
    Mouse_Left,
    Mouse_Right,
    Mouse_Middle
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
    JOYSTICK_16
};

struct input_controller
{
    std::map<int, bool> KeysDown =
    {
        { Key_Left, false },
        { Key_Right, false  },
        { Key_Up, false },
        { Key_Down, false },
        { Key_Escape, false },
        { Key_Tab, false },
        { Key_Enter, false },
        { Key_LeftCtrl, false },
        { Key_A, false },
        { Key_B, false },
        { Key_C, false },
        { Key_D, false },
        { Key_E, false },
        { Key_F, false },
        { Key_G, false },
        { Key_H, false },
        { Key_I, false },
        { Key_J, false },
        { Key_K, false },
        { Key_L, false },
        { Key_M, false },
        { Key_N, false },
        { Key_O, false },
        { Key_Q, false },
        { Key_R, false },
        { Key_S, false },
        { Key_T, false },
        { Key_U, false },
        { Key_V, false },
        { Key_X, false },
        { Key_Y, false },
        { Key_Z, false }
    };
    
    std::map<int, bool> MouseButtonDown = 
    {
        {Mouse_Left, false},
        {Mouse_Right, false},
        {Mouse_Middle, false}
    };
    
    
    std::map<int, bool> JoystickKeysDown =
    {
        {JOYSTICK_1, false},
        {JOYSTICK_2, false},
        {JOYSTICK_3, false},
        {JOYSTICK_4, false},
        {JOYSTICK_5, false},
        {JOYSTICK_6, false},
        {JOYSTICK_7, false},
        {JOYSTICK_8, false},
        {JOYSTICK_9, false},
        {JOYSTICK_10, false},
        {JOYSTICK_11, false},
        {JOYSTICK_12, false},
        {JOYSTICK_13, false},
        {JOYSTICK_14, false},
        {JOYSTICK_15, false},
        {JOYSTICK_16, false}
    };
    
    std::map<int, uint32> JoystickKeysJustPressed = 
    {
        {JOYSTICK_1, Key_NotPressed},
        {JOYSTICK_2, Key_NotPressed},
        {JOYSTICK_3, Key_NotPressed},
        {JOYSTICK_4, Key_NotPressed},
        {JOYSTICK_5, Key_NotPressed},
        {JOYSTICK_6, Key_NotPressed},
        {JOYSTICK_7, Key_NotPressed},
        {JOYSTICK_8, Key_NotPressed},
        {JOYSTICK_9, Key_NotPressed},
        {JOYSTICK_10, Key_NotPressed},
        {JOYSTICK_11, Key_NotPressed},
        {JOYSTICK_12, Key_NotPressed},
        {JOYSTICK_13, Key_NotPressed},
        {JOYSTICK_14, Key_NotPressed},
        {JOYSTICK_15, Key_NotPressed},
        {JOYSTICK_16, Key_NotPressed}
    };
    
    std::map<int, uint32> MouseButtonJustPressed =
    {
        {Mouse_Left, Key_NotPressed},
        {Mouse_Right, Key_NotPressed},
        {Mouse_Middle, Key_NotPressed}
    };
    
    std::map<int, uint32> KeysJustPressed =
    {
        { Key_Left, Key_NotPressed },
        { Key_Right, Key_NotPressed  },
        { Key_Up, Key_NotPressed },
        { Key_Down, Key_NotPressed },
        { Key_Escape, Key_NotPressed },
        { Key_Tab, Key_NotPressed },
        { Key_Enter, Key_NotPressed },
        { Key_LeftCtrl, Key_NotPressed },
        { Key_A, Key_NotPressed },
        { Key_B, Key_NotPressed },
        { Key_C, Key_NotPressed },
        { Key_D, Key_NotPressed },
        { Key_E, Key_NotPressed },
        { Key_F, Key_NotPressed },
        { Key_G, Key_NotPressed },
        { Key_H, Key_NotPressed },
        { Key_I, Key_NotPressed },
        { Key_J, Key_NotPressed },
        { Key_K, Key_NotPressed },
        { Key_L, Key_NotPressed },
        { Key_M, Key_NotPressed },
        { Key_N, Key_NotPressed },
        { Key_O, Key_NotPressed },
        { Key_Q, Key_NotPressed },
        { Key_R, Key_NotPressed },
        { Key_S, Key_NotPressed },
        { Key_T, Key_NotPressed },
        { Key_U, Key_NotPressed },
        { Key_V, Key_NotPressed },
        { Key_X, Key_NotPressed },
        { Key_Y, Key_NotPressed },
        { Key_Z, Key_NotPressed }
    };
    
    double MouseX;
    double MouseY;
    bool32 ControllerPresent;
};

#endif