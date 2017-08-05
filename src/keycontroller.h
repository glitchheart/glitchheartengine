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

enum Key_Mode
{
    Key_NotPressed,
    Key_JustPressed,
    Key_Invalid
};

#define NUM_KEYS 124

#define Key_Left 0
#define Key_Right 1
#define Key_Up 2
#define Key_Down 3
#define Key_Escape 4
#define Key_F1 5
#define Key_F2 6
#define Key_F3 7
#define Key_F4 8
#define Key_F5 9
#define Key_F6 10
#define Key_F7 11
#define Key_F8 12
#define Key_F9 13
#define Key_F10 14
#define Key_F11 15
#define Key_F12 16
#define Key_F13 17
#define Key_F14 18
#define Key_F15 19
#define Key_F16 20
#define Key_F17 21
#define Key_F18 22
#define Key_F19 23
#define Key_F20 24
#define Key_F21 25
#define Key_F22 26
#define Key_F23 27
#define Key_F24 28
#define Key_F25 29
#define Key_A 30
#define Key_B 31
#define Key_C 32
#define Key_D 33
#define Key_E 34
#define Key_F 35
#define Key_G 36
#define Key_H 37
#define Key_I 38
#define Key_J 39
#define Key_K 40
#define Key_L 41
#define Key_M 42
#define Key_N 43
#define Key_O 44
#define Key_P 45
#define Key_Q 46
#define Key_R 47
#define Key_S 48
#define Key_T 49
#define Key_U 50
#define Key_V 51
#define Key_W 52
#define Key_X 53
#define Key_Y 54
#define Key_Z 55
#define Key_Space 56
#define Key_Tab 57
#define Key_Enter 58
#define Key_Backspace 59
#define Key_LeftShift 60
#define Key_LeftCtrl 61
#define Key_RightCtrl 62
#define Key_RightShift 63
#define Key_Delete 64
#define Key_0 65
#define Key_1 66
#define Key_2 67
#define Key_3 68
#define Key_4 69
#define Key_5 70
#define Key_6 71
#define Key_7 72
#define Key_8 73
#define Key_9 74
#define Key_Add 75
#define Key_Subtract 76
#define Key_MouseLeft 77
#define Key_MouseRight 78
#define Key_Unknown 79
#define Key_Apostrophe 80
#define Key_Comma 81
#define Key_Minus 82
#define Key_Period 83
#define Key_Slash 84
#define Key_SemiColon 85
#define Key_Equal 86
#define Key_LeftBracket 87
#define Key_RightBracket 88
#define Key_ 89
#define Key_GraveAccent 90
#define Key_World1 91
#define Key_World2 92
#define Key_Insert 93
#define Key_PageUp 94
#define Key_PageDown 95
#define Key_Home 96
#define Key_Backslash 97
#define Key_End 98
#define Key_CapsLock 99
#define Key_ScrollLock 100
#define Key_NumLock 101
#define Key_PrintScreen 102
#define Key_Pause 103
#define Key_LeftAlt 104
#define Key_RightAlt 105
#define Key_LeftSuper 106
#define Key_RightSuper 107
#define Key_Menu 108
#define Key_KP0 109
#define Key_KP1 110
#define Key_KP2 111
#define Key_KP3 112
#define Key_KP4 113
#define Key_KP5 114
#define Key_KP6 115
#define Key_KP7 116
#define Key_KP8 117
#define Key_KP9 118
#define Key_KPDecimal 119
#define Key_KPDivide 120
#define Key_KPMultiply 121
#define Key_KPEnter 122
#define Key_KPEqual 123
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
#define PS4_DPadRigt Joystick_16
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
#define Xbox_LeftTrigger Joystick_RightTrigger
#define Xbox_RightTrigger 18

using Key_Code = i32;
using Controller_Code = i32;
using Xbox_Code = i32;
using PS4_Code = i32;
using Mouse_Code = i32;

enum Action_Button
{
    Action_Attack,
    Action_Use,
    Action_Dash,
    Action_Interact,
    Action_Checkpoint,
    Action_Menu,
    Action_Target,
    Action_SwitchTarget,
    Action_Throw,
    Action_Count
};

enum Stick
{
    Stick_Left,
    Stick_Right
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
        Key_MouseLeft,
        Key_Space,
        Key_MouseRight,
        Key_Q,
        Key_E,
        Key_Escape,
        Key_8,
        Key_C,
        Key_U
    };
    
    Xbox_Code ActionButtonXboxControllerBindings[Action_Count] = 
    {
        Xbox_RightBumper,
        Xbox_X,
        Xbox_B,
        Xbox_A,
        Xbox_LeftBumper,
        Xbox_Start,
        Xbox_Back,
        Xbox_Back,
        Xbox_Back
    };
    
    PS4_Code ActionButtonPS4ControllerBindings[Action_Count] = 
    {
        PS4_R1,
        PS4_Square,
        PS4_Circle,
        PS4_Cross,
        PS4_L1,
        PS4_Option,
        PS4_Share,
        PS4_Share
    };
    
    b32 ActionRunning = false;
    b32 HasQueuedAction = false;
    Action_Button NextAction;
    
    b32 AnyKeyPressed;
    
    b32 KeysDown[NUM_KEYS];
    b32 KeysUp[NUM_KEYS];
    Key_Mode KeysJustPressed[NUM_KEYS];
    
    b32 MouseButtonDown[NUM_MOUSE_BUTTONS];
    Key_Mode MouseButtonJustPressed[NUM_MOUSE_BUTTONS];
    
    b32 JoystickKeysDown[NUM_JOYSTICK_KEYS];
    Key_Mode JoystickKeysJustPressed[NUM_JOYSTICK_KEYS];
    
    r32 Axes[NUM_AXES];
    
    r32 AxesUsedZone = 0.4f;
    Key_Mode AxesJustPressed[NUM_AXES];
    
    r64 MouseX;
    r64 MouseY;
    r64 ScrollX;
    r64 ScrollY;
    
    r32 ControllerDeadzone = 0.2f;
    b32 ControllerPresent;
    Controller_Type ControllerType;
    
    char CurrentCharacter;
    b32 DeleteCharacter = false;
    
    integer_map KeyMappings;
    integer_map MouseButtonMappings;
    integer_map ControllerMappings;
    
};

#endif