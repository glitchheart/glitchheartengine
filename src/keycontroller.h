#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include <map>

enum Key_Mode
{
    Key_NotPressed,
    Key_JustPressed,
    Key_Invalid
};

struct input_controller
{
    std::map<int, bool> KeysDown =
	{
	    { GLFW_KEY_LEFT, false },
	    { GLFW_KEY_RIGHT, false },
	    { GLFW_KEY_UP, false },
	    { GLFW_KEY_DOWN, false },
	    { GLFW_KEY_ESCAPE, false },
	    { GLFW_KEY_W, false },
	    { GLFW_KEY_A, false },
	    { GLFW_KEY_S, false },
	    { GLFW_KEY_D, false },
	    { GLFW_KEY_TAB, false },
	    { GLFW_KEY_ENTER, false },
	    { GLFW_KEY_1, false },
	    { GLFW_KEY_2, false },
	    { GLFW_KEY_3, false },
	    { GLFW_KEY_4, false },
	    { GLFW_KEY_5, false },
	    { GLFW_KEY_B, false },
	    { GLFW_KEY_C, false },
	    { GLFW_KEY_E, false },
	    { GLFW_KEY_F, false },
	    { GLFW_KEY_G, false },
	    { GLFW_KEY_H, false },
	    { GLFW_KEY_I, false },
	    { GLFW_KEY_J, false },
	    { GLFW_KEY_K, false },
	    { GLFW_KEY_L, false },
	    { GLFW_KEY_M, false },
	    { GLFW_KEY_N, false },
	    { GLFW_KEY_O, false },
	    { GLFW_KEY_P, false },
	    { GLFW_KEY_Q, false },
	    { GLFW_KEY_R, false },
	    { GLFW_KEY_S, false },
	    { GLFW_KEY_T, false },
	    { GLFW_KEY_U, false },
	    { GLFW_KEY_V, false },
	    { GLFW_KEY_X, false },
	    { GLFW_KEY_Y, false },
	    { GLFW_KEY_Z, false } };

    std::map<int, uint32> KeysJustPressed =
	{
	    { GLFW_KEY_LEFT, Key_NotPressed },
	    { GLFW_KEY_RIGHT, Key_NotPressed },
	    { GLFW_KEY_UP, Key_NotPressed },
	    { GLFW_KEY_DOWN, Key_NotPressed },
	    { GLFW_KEY_ESCAPE, Key_NotPressed },
	    { GLFW_KEY_W, Key_NotPressed },
	    { GLFW_KEY_A, Key_NotPressed },
	    { GLFW_KEY_S, Key_NotPressed },
	    { GLFW_KEY_D, Key_NotPressed },
	    { GLFW_KEY_TAB, Key_NotPressed },
	    { GLFW_KEY_ENTER, Key_NotPressed },
	    { GLFW_KEY_1, Key_NotPressed },
	    { GLFW_KEY_2, Key_NotPressed },
	    { GLFW_KEY_3, Key_NotPressed },
	    { GLFW_KEY_4, Key_NotPressed },
	    { GLFW_KEY_5, Key_NotPressed },
	    { GLFW_KEY_B, Key_NotPressed },
	    { GLFW_KEY_C, Key_NotPressed },
	    { GLFW_KEY_E, Key_NotPressed },
	    { GLFW_KEY_F, Key_NotPressed },
	    { GLFW_KEY_G, Key_NotPressed },
	    { GLFW_KEY_H, Key_NotPressed },
	    { GLFW_KEY_I, Key_NotPressed },
	    { GLFW_KEY_J, Key_NotPressed },
	    { GLFW_KEY_K, Key_NotPressed },
	    { GLFW_KEY_L, Key_NotPressed },
	    { GLFW_KEY_M, Key_NotPressed },
	    { GLFW_KEY_N, Key_NotPressed },
	    { GLFW_KEY_O, Key_NotPressed },
	    { GLFW_KEY_P, Key_NotPressed },
	    { GLFW_KEY_Q, Key_NotPressed },
	    { GLFW_KEY_R, Key_NotPressed },
	    { GLFW_KEY_S, Key_NotPressed },
	    { GLFW_KEY_T, Key_NotPressed },
	    { GLFW_KEY_U, Key_NotPressed },
	    { GLFW_KEY_V, Key_NotPressed },
	    { GLFW_KEY_X, Key_NotPressed },
	    { GLFW_KEY_Y, Key_NotPressed },
	    { GLFW_KEY_Z, Key_NotPressed } };

    double MouseX;
    double MouseY;
};

#endif