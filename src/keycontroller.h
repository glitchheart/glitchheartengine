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
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
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
    
    std::map<int, uint32> KeysJustPressed =
    {
        { Key_Left, Key_NotPressed },
        { Key_Right, Key_NotPressed  },
        { Key_Up, Key_NotPressed },
        { Key_Down, Key_NotPressed },
        { Key_Escape, Key_NotPressed },
        { Key_Tab, Key_NotPressed },
        { Key_Enter, Key_NotPressed },
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
};

#endif