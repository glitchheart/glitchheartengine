#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include <GLFW/glfw3.h>
#include <map>

static double MouseX;
static double MouseY;

static std::map<int, bool> KeysDown =
{
	{ GLFW_KEY_LEFT, false },
	{ GLFW_KEY_RIGHT, false },
	{ GLFW_KEY_UP, false },
	{ GLFW_KEY_DOWN, false },
	{ GLFW_KEY_ESCAPE, false }
};

static void CursorPositionCallback(GLFWwindow* Window, double XPos, double YPos)
{
	MouseX = XPos;
	MouseY = YPos;
}

static void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
{
	if(Action == GLFW_PRESS)
		KeysDown[Key] = true;
	else if(Action == GLFW_RELEASE)
		KeysDown[Key] = false;
}

static bool IsKeyDown(int Key)
{
	return KeysDown[Key];
}
#endif