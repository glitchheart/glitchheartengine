#ifndef KEYCONTROLLER_H
#define KEYCONTROLLER_H

#include <GLFW/glfw3.h>
#include <map>

static std::map<int, bool> keysDown =
{
	{ GLFW_KEY_LEFT, false },
	{ GLFW_KEY_RIGHT, false },
	{ GLFW_KEY_UP, false },
	{ GLFW_KEY_DOWN, false },
	{ GLFW_KEY_ESCAPE, false }
};

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
		keysDown[key] = true;
	else if(action == GLFW_RELEASE)
		keysDown[key] = false;
}

static bool is_key_down(int key)
{
	return keysDown[key];
}
#endif