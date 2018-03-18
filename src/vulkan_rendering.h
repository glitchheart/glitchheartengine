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

struct QueueFamilyIndices
{
    i32 graphics_family;
    i32 present_family;
};

#define PRESENT_MODES 4
#define SURFACE_FORMATS 32

struct SwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR formats[SURFACE_FORMATS];
    i32 format_count;
    
    VkPresentModeKHR present_modes[PRESENT_MODES];
    i32 present_mode_count;
};

struct VkRenderState
{
    GLFWwindow* window;
    
    i32 window_width;
    i32 window_height;
    
    VkInstance instance;
    VkDebugReportCallbackEXT callback;
    VkPhysicalDevice physical_device;
    VkPhysicalDeviceFeatures device_features;
    VkPhysicalDeviceProperties device_properties;
    
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    
    SwapchainSupportDetails swapchain_support_details;
    QueueFamilyIndices queue_family_indices;
    
    VkImage* swapchain_images;
    VkImageView* swapchain_image_views;
    u32 swapchain_image_count;
    
    VkFramebuffer* swapchain_framebuffers;
    
    VkCommandPool command_pool;
    VkCommandBuffer* command_buffers;
    
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_extent;
    
    VkPipeline graphics_pipeline;
    VkRenderPass render_pass;
    VkPipelineLayout pipeline_layout;
    
    VkSemaphore image_available_semaphore;
    VkSemaphore render_finished_semaphore;
    
    b32 hide_cursor;
    b32 enable_validation_layers;
    
    MemoryArena arena;
};

#endif



