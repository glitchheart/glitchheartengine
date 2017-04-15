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
	    { GLFW_KEY_TAB, false } 
	};

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
	    { GLFW_KEY_TAB, Key_NotPressed } };

    double MouseX;
    double MouseY;
};

#endif