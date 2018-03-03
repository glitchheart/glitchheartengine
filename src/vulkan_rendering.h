#ifndef VULKAN_RENDERING_H
#define VULKAN_RENDERING_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define VALIDATION_LAYER_SIZE 1
const char* ValidationLayers[VALIDATION_LAYER_SIZE] =
{
    "VK_LAYER_LUNARG_standard_validation"
};

#define DEVICE_EXTENSIONS_SIZE 1
const char* DeviceExtensions[DEVICE_EXTENSIONS_SIZE] = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct queue_family_indices
{
    i32 GraphicsFamily;
    i32 PresentFamily;
};

#define PRESENT_MODES 4
#define SURFACE_FORMATS 32

struct swapchain_support_details
{
    VkSurfaceCapabilitiesKHR Capabilities;
    VkSurfaceFormatKHR Formats[SURFACE_FORMATS];
    i32 FormatCount;
    
    VkPresentModeKHR PresentModes[PRESENT_MODES];
    i32 PresentModeCount;
};

struct vk_render_state
{
    GLFWwindow* Window;
    
    i32 WindowWidth;
    i32 WindowHeight;
    
    VkInstance Instance;
    VkDebugReportCallbackEXT Callback;
    VkPhysicalDevice PhysicalDevice;
    VkPhysicalDeviceFeatures DeviceFeatures;
    VkPhysicalDeviceProperties DeviceProperties;
    
    VkDevice Device;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    
    VkSurfaceKHR Surface;
    VkSwapchainKHR Swapchain;
    
    swapchain_support_details SwapchainSupportDetails;
    queue_family_indices QueueFamilyIndices;
    
    VkImage* SwapchainImages;
    u32 SwapchainImageCount;
    
    VkFormat SwapchainImageFormat;
    VkExtent2D SwapchainExtent;
    
    b32 HideCursor;
    b32 EnableValidationLayers;
    
    
    memory_arena Arena;
};

#endif



