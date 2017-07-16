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
 Cirlce 2
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
    Key_Space,
    Key_Tab,
    Key_Enter,
    Key_Backspace,
    Key_LeftShift,
    Key_LeftCtrl,
    Key_RightCtrl,
    Key_RightShift,
    Key_Delete,
    Key_0,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,
    Key_Add,
    Key_Subtract,
    Key_MouseLeft,
    Key_MouseRight,
    Key_Count
};

enum Mouse_Code
{
    Mouse_Left,
    Mouse_Right,
    Mouse_Middle,
    Mouse_Count
};

enum Xbox_Code
{
    Xbox_A,
    Xbox_B,
    Xbox_X,
    Xbox_Y,
    Xbox_LeftBumper,
    Xbox_RightBumper,
    Xbox_Back,
    Xbox_Start,
    Xbox_LeftStick,
    Xbox_RightStick,
    Xbox_DPadUp,
    Xbox_DPadRight,
    Xbox_DPadDown,
    Xbox_DPadLeft,
    Xbox_14,
    Xbox_15,
    Xbox_16,
    Xbox_LeftTrigger,
    Xbox_RightTrigger,
    Xbox_Count
};

enum PS4_Code
{
    PS4_Square,
    PS4_Cross,
    PS4_Circle,
    PS4_Triangle,
    PS4_L1,
    PS4_R1,
    PS4_L2,
    PS4_R2,
    PS4_Select,
    PS4_Start,
    PS4_LeftStick,
    PS4_RightStick,
    PS4_Home,
    PS4_TouchPad,
    PS4_DPadUp,
    PS4_DPadRigt,
    PS4_DPadDown,
    PS4_DPadLeft,
    PS4_Count
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
    Joystick_LeftTrigger,
    Joystick_RightTrigger,
    Joystick_Count
};

enum Action_Button
{
    Action_Attack,
    Action_Use,
    Action_Dash,
    Action_Interact,
    Action_Target,
    Action_SwitchTarget,
    Action_Throw,
    Action_Checkpoint,
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
        Key_7,
        Key_8,
        Key_C
    };
    
    Xbox_Code ActionButtonXboxControllerBindings[Action_Count] = 
    {
        Xbox_RightBumper,
        Xbox_X,
        Xbox_B,
        Xbox_A
    };
    
    PS4_Code ActionButtonPS4ControllerBindings[Action_Count] = 
    {
        PS4_R1,
        PS4_Square,
        PS4_Circle,
        PS4_Cross,
    };
    
    b32 ActionRunning = false;
    b32 HasQueuedAction = false;
    Action_Button NextAction;
    
    b32 AnyKeyPressed;
    
    b32 KeysDown[Key_Count];
    b32 KeysUp[Key_Count];
    Key_Mode KeysJustPressed[Key_Count];
    
    b32 MouseButtonDown[Mouse_Count];
    Key_Mode MouseButtonJustPressed[Mouse_Count];
    
    b32 JoystickKeysDown[Joystick_Count];
    Key_Mode JoystickKeysJustPressed[Joystick_Count];
    
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
};

#endif