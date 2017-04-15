#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include <map>

struct input_controller
{
    std::map<int, bool> KeysDown =
	{
	    {GLFW_KEY_LEFT, false},
	    {GLFW_KEY_RIGHT, false},
	    {GLFW_KEY_UP, false},
	    {GLFW_KEY_DOWN, false},
	    {GLFW_KEY_ESCAPE, false},
	    {GLFW_KEY_W, false},
	    {GLFW_KEY_A, false},
	    {GLFW_KEY_S, false},
	    {GLFW_KEY_D, false},
	    {GLFW_KEY_TAB, false},
		{GLFW_KEY_ENTER, false},
		{GLFW_KEY_1, false},
		{GLFW_KEY_2, false},
		{GLFW_KEY_3, false},
		{GLFW_KEY_4, false},
		{GLFW_KEY_5, false} };
    double MouseX;
    double MouseY;
};



#endif