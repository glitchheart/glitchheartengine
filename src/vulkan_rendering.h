#ifndef VULKAN_RENDERING_H
#define VULKAN_RENDERING_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VALIDATION_LAYER_SIZE 1
const char* ValidationLayers[VALIDATION_LAYER_SIZE] =
{
    "VK_LAYER_LUNARG_standard_validation"
};

struct vk_render_state
{
    GLFWwindow* Window;
    
    VkInstance Instance;
    VkDebugReportCallbackEXT Callback;
    b32 HideCursor;
    
};

#endif



