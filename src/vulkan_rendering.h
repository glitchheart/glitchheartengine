#ifndef VULKAN_RENDERING_H
#define VULKAN_RENDERING_H

// @Incomplete: This is a temporary ifdef, until we understand how to include/install on OSX the vulkan sdk correctly
#ifndef __APPLE__
#define GLFW_INCLUDE_VULKAN
#else
#define _GLFW_VULKAN_STATIC
#include "vulkan/vulkan.h"
#endif
#include <GLFW/glfw3.h>

#define VALIDATION_LAYER_SIZE 2
const char* ValidationLayers[VALIDATION_LAYER_SIZE] =
{
    "VK_LAYER_LUNARG_standard_validation",
    "VK_LAYER_LUNARG_assistant_layer"
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
    VkImageView* SwapchainImageViews;
    u32 SwapchainImageCount;
    
    VkFramebuffer* SwapchainFramebuffers;
    
    VkCommandPool CommandPool;
    VkCommandBuffer* CommandBuffers;
    
    VkFormat SwapchainImageFormat;
    VkExtent2D SwapchainExtent;
    
    VkPipeline GraphicsPipeline;
    VkRenderPass RenderPass;
    VkPipelineLayout PipelineLayout;
    
    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    
    b32 HideCursor;
    b32 EnableValidationLayers;
    
    memory_arena Arena;
};

#endif



