#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

/*
XBOX 

A 0
B 1
X 2
Y 3
LB 4
RB 5
DPAD DOWN 12
DPAD UP 10
DPAD RIGHT 11
DPAD LEFFT 13
START 7
BACK 6

LEFT TRIGGER Axis 4
RIGHT TRIGGER Axis 5

PS4

  Cross 1
 Circle 2
 Square 0
 Triangle 3
L1 4
R1 5
L2 6
R2 7
DPAD DOWN 16
DPAD UP 14
DPAD RIGHT 15
DPAD LEFT 17
 OPTION 9
 SHARE 8
*/

enum KeyMode
{
    KEY_NOT_PRESSED,
    KEY_JUST_PRESSED,
    KEY_INVALID
};

#define NUM_KEYS 124

#define Key_Left 1
#define Key_Right 2
#define Key_Up 3
#define Key_Down 4
#define Key_Escape 5
#define Key_F1 6
#define Key_F2 7
#define Key_F3 8
#define Key_F4 9
#define Key_F5 10
#define Key_F6 11
#define Key_F7 12
#define Key_F8 13
#define Key_F9 14
#define Key_F10 15
#define Key_F11 16
#define Key_F12 17
#define Key_F13 18
#define Key_F14 19
#define Key_F15 20
#define Key_F16 21
#define Key_F17 22
#define Key_F18 23
#define Key_F19 24
#define Key_F20 25
#define Key_F21 26
#define Key_F22 27
#define Key_F23 28
#define Key_F24 29
#define Key_F25 30
#define Key_A 31
#define Key_B 32
#define Key_C 33
#define Key_D 34
#define Key_E 35
#define Key_F 36
#define Key_G 37
#define Key_H 38
#define Key_I 39
#define Key_J 40
#define Key_K 41
#define Key_L 42
#define Key_M 43
#define Key_N 44
#define Key_O 45
#define Key_P 46
#define Key_Q 47
#define Key_R 48
#define Key_S 49
#define Key_T 50
#define Key_U 51
#define Key_V 52
#define Key_W 53
#define Key_X 54
#define Key_Y 55
#define Key_Z 56
#define Key_Space 57
#define Key_Tab 58
#define Key_Enter 59
#define Key_Backspace 60
#define Key_LeftShift 61
#define Key_LeftCtrl 62
#define Key_RightCtrl 63
#define Key_RightShift 64
#define Key_Delete 65
#define Key_0 66
#define Key_1 67
#define Key_2 68
#define Key_3 69
#define Key_4 70
#define Key_5 71
#define Key_6 72
#define Key_7 73
#define Key_8 74
#define Key_9 75
#define Key_Add 76
#define Key_Subtract 77
#define Key_MouseLeft 78
#define Key_MouseRight 79
#define Key_Unknown 80
#define Key_Apostrophe 81
#define Key_Comma 82
#define Key_Minus 83
#define Key_Period 84
#define Key_Slash 85
#define Key_SemiColon 86
#define Key_Equal 87
#define Key_LeftBracket 88
#define Key_RightBracket 89
#define Key_ 90
#define Key_GraveAccent 91
#define Key_World1 92
#define Key_World2 93
#define Key_Insert 94
#define Key_PageUp 95
#define Key_PageDown 96
#define Key_Home 97
#define Key_Backslash 98
#define Key_End 99
#define Key_CapsLock 100
#define Key_ScrollLock 101
#define Key_NumLock 102
#define Key_PrintScreen 103
#define Key_Pause 104
#define Key_LeftAlt 105
#define Key_RightAlt 106
#define Key_LeftSuper 107
#define Key_RightSuper 108
#define Key_Menu 109
#define Key_KP0 110
#define Key_KP1 111
#define Key_KP2 112
#define Key_KP3 113
#define Key_KP4 114
#define Key_KP5 115
#define Key_KP6 116
#define Key_KP7 117
#define Key_KP8 118
#define Key_KP9 119
#define Key_KPDecimal 183
#define Key_KPDivide 121
#define Key_KPMultiply 122
#define Key_KPEnter 123
#define Key_KPEqual 124
#define Key_Last Key_KPEqual

#define NUM_MOUSE_BUTTONS 3

#define Mouse_Left 0
#define Mouse_Right 1
#define Mouse_Middle 2


#define Joystick_1 0
#define Joystick_2 1
#define Joystick_3 2
#define Joystick_4 3
#define Joystick_5 4
#define Joystick_6 5
#define Joystick_7 6
#define Joystick_8 7
#define Joystick_9 8
#define Joystick_10 9
#define Joystick_11 10
#define Joystick_12 11
#define Joystick_13 12
#define Joystick_14 13
#define Joystick_15 14
#define Joystick_16 15
#define Joystick_LeftTrigger 16
#define Joystick_RightTrigger 17

#define NUM_JOYSTICK_KEYS 18

#define PS4_Square Joystick_1
#define PS4_Cross Joystick_2
#define PS4_Circle Joystick_3
#define PS4_Triangle Joystick_4
#define PS4_L1 Joystick_5
#define PS4_R1 Joystick_6
#define PS4_L2 Joystick_7
#define PS4_R2 Joystick_8
#define PS4_Share Joystick_9
#define PS4_Option Joystick_10
#define PS4_LeftStick Joystick_11
#define PS4_RightStick Joystick_12
#define PS4_Home Joystick_13
#define PS4_TouchPad Joystick_14
#define PS4_DPadUp Joystick_15
#define PS4_DPadRight Joystick_16
#define PS4_DPadDown Joystick_LeftTrigger
#define PS4_DPadLeft Joystick_RightTrigger

#define Xbox_A Joystick_1
#define Xbox_B Joystick_2
#define Xbox_X Joystick_3
#define Xbox_Y Joystick_4
#define Xbox_LeftBumper Joystick_5
#define Xbox_RightBumper Joystick_6
#define Xbox_Back Joystick_7
#define Xbox_Start Joystick_8
#define Xbox_LeftStick Joystick_9
#define Xbox_RightStick Joystick_10
#define Xbox_DPadUp Joystick_11
#define Xbox_DPadRight Joystick_12
#define Xbox_DPadDown Joystick_13
#define Xbox_DPadLeft Joystick_14
#define Xbox_14 Joystick_15
#define Xbox_15 Joystick_16
#define Xbox_16 Joystick_LeftTrigger
#define Xbox_LeftTrigger Joystick_LeftTrigger
#define Xbox_RightTrigger Joystick_RightTrigger

using KeyCode = i32;
using ControllerCode = i32;
using XboxCode = i32;
using PS4Code = i32;
using MouseCode = i32;

enum Stick
{
    STICK_LEFT,
    STICK_RIGHT
};

enum ControllerAxisCode
{
    AXIS_1, // Left stick X (PS4 W10)
    AXIS_2, // Left stick Y (PS4 W10)
    AXIS_3, // Right stick X (PS4 W10)
    AXIS_4, // Right stick Y (PS4 W10)
    AXIS_5, // Left trigger (PS4 W10)
    AXIS_6  // Right trigger (PS4 W10)
};

#define NUM_AXES 6

enum ControllerType
{
    CONTROLLER_NONE,
    CONTROLLER_XBOX,
    CONTROLLER_PS4
};

struct CustomKeyMapping
{
    KeyCode keyboard_key[4];
    i32 keyboard_key_count;
    
    PS4Code ps4_key[4];
    i32 ps4_key_count;
    
    XboxCode xbox_key[4];
    i32 xbox_key_count;
};

struct InputController
{
    CustomKeyMapping custom_mappings[64];
    
    b32 any_key_pressed;
    
    b32 keys_down[NUM_KEYS];
    b32 keys_up[NUM_KEYS];
    KeyMode keys_just_pressed[NUM_KEYS];
    
    b32 mouse_button_down[NUM_MOUSE_BUTTONS];
    b32 mouse_buttons_up[NUM_MOUSE_BUTTONS];
    KeyMode mouse_button_just_pressed[NUM_MOUSE_BUTTONS];
    
    b32 joystick_keys_down[NUM_JOYSTICK_KEYS];
    KeyMode joystick_keys_just_pressed[NUM_JOYSTICK_KEYS];
    
    r32 axes[NUM_AXES];
    
    r32 axes_used_zone = 0.4f;
    KeyMode axes_just_pressed[NUM_AXES];
    
    r64 mouse_x;
    r64 mouse_y;
    r64 scroll_x;
    r64 scroll_y;
    
    r32 controller_deadzone = 0.2f;
    
    b32 controller_present = false;
    ControllerType controller_type = CONTROLLER_NONE;
    
    char current_character;
    b32 delete_character = false;
    
    //integer_map key_mappings;
    Map key_mappings;
    Map mouse_button_mappings;
    Map controller_mappings;
};

#endif
