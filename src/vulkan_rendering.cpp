static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT Flags, VkDebugReportObjectTypeEXT ObjType, u64 Obj, size_t Location, i32 Code, const char* LayerPrefix, const char* Msg, void* UserData)
{
    DEBUG_PRINT("%s\n", Msg);
    return VK_FALSE;
}

VkResult CreateDebugReportCallbackEXT(VkInstance Instance, const VkDebugReportCallbackCreateInfoEXT* CreateInfo, const VkAllocationCallbacks* Allocator, VkDebugReportCallbackEXT* Callback)
{
    auto Func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");
    if (Func != nullptr)
    {
        return Func(Instance, CreateInfo, Allocator, Callback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(VkInstance Instance, VkDebugReportCallbackEXT Callback, const VkAllocationCallbacks* Allocator) {
    auto Func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(Instance, "vkDestroyDebugReportCallbackEXT");
    if (Func != nullptr)
    {
        Func(Instance, Callback, Allocator);
    }
}

static void VkCleanup(vk_render_state& RenderState, renderer& Renderer)
{
    vkDestroySwapchainKHR(RenderState.Device, RenderState.Swapchain, nullptr);
    
    for(u32 Index = 0; Index < RenderState.SwapchainImageCount; Index++)
    {
        vkDestroyImageView(RenderState.Device, RenderState.SwapchainImageViews[Index], nullptr);
    }
    
    vkDestroyPipelineLayout(RenderState.Device, RenderState.PipelineLayout, nullptr);
    
    vkDestroyDevice(RenderState.Device, nullptr);
    DestroyDebugReportCallbackEXT(RenderState.Instance, RenderState.Callback, nullptr);
    vkDestroySurfaceKHR(RenderState.Instance, RenderState.Surface, nullptr);
    vkDestroyInstance(RenderState.Instance, nullptr);
    glfwDestroyWindow(RenderState.Window);
    glfwTerminate();
}

static b32 CheckValidationLayerSupport()
{
    u32 LayerCount;
    vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
    
    VkLayerProperties* AvailableLayers = PushTempArray(LayerCount, VkLayerProperties);
    vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers);
    
    for(i32 Index = 0; Index < VALIDATION_LAYER_SIZE; Index++)
    {
        b32 LayerFound = false;
        
        for(u32 LayerPropIndex = 0; LayerPropIndex < LayerCount; LayerPropIndex++)
        {
            if(strcmp(ValidationLayers[Index], AvailableLayers[LayerPropIndex].layerName) == 0)
            {
                LayerFound = true;
                break;
            }
        }
        
        if(!LayerFound)
        {
            return false;
        }
    }
    
    return true;
}

b32 IsQueueFamilyComplete(queue_family_indices& Indices)
{
    return Indices.GraphicsFamily >= 0 && Indices.PresentFamily >= 0;
}

queue_family_indices FindQueueFamilies(VkSurfaceKHR Surface, VkPhysicalDevice Device)
{
    queue_family_indices Indices;
    Indices.GraphicsFamily = -1;
    Indices.PresentFamily = -1;
    
    u32 QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);
    
    VkQueueFamilyProperties* QueueFamilies = PushTempArray(QueueFamilyCount, VkQueueFamilyProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies);
    
    for(u32 Index = 0; Index < QueueFamilyCount; Index++)
    {
        auto& QueueFamily = QueueFamilies[Index];
        if(QueueFamily.queueCount > 0 && QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            Indices.GraphicsFamily = (i32)Index;
        }
        
        VkBool32 PresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, Index, Surface, &PresentSupport);
        
        if(QueueFamily.queueCount > 0 && PresentSupport)
        {
            Indices.PresentFamily = (i32)Index;
        }
        
        if(IsQueueFamilyComplete(Indices))
        {
            break;
        }
    }
    
    return Indices;
}

swapchain_support_details QuerySwapchainSupport(VkSurfaceKHR Surface, VkPhysicalDevice Device)
{
    swapchain_support_details Details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Details.Capabilities);
    
    u32 FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);
    
    if (FormatCount != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Details.Formats);
    }
    
    u32 PresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, nullptr);
    
    if (PresentModeCount != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Details.PresentModes);
    }
    
    return Details;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(VkSurfaceFormatKHR* Formats, i32 FormatCount)
{
    if(Formats)
    {
        if(FormatCount == 1 && Formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }
    }
    
    for(i32 FormatIndex = 0; FormatIndex < FormatCount; FormatIndex++)
    {
        auto AvailableFormat = Formats[FormatIndex];
        if(AvailableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return AvailableFormat;
        }
    }
    
    return Formats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(VkPresentModeKHR* PresentModes, i32 PresentModeCount)
{
    VkPresentModeKHR BestMode = VK_PRESENT_MODE_FIFO_KHR;
    
    for (i32 PresentModeIndex = 0; PresentModeIndex < PresentModeCount; PresentModeIndex++)
    {
        auto AvailablePresentMode = PresentModes[PresentModeIndex];
        if (AvailablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
        {
            return AvailablePresentMode;
        }
        else if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            BestMode = AvailablePresentMode;
        } 
        else if (AvailablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR && BestMode != VK_PRESENT_MODE_MAILBOX_KHR)
        {
            BestMode = AvailablePresentMode;
        }
    }
    
    return BestMode;
}

VkExtent2D ChooseSwapExtent(vk_render_state& RenderState, VkSurfaceCapabilitiesKHR Capabilities)
{
    
    VkExtent2D ActualExtent = {(u32)RenderState.WindowWidth, (u32)RenderState.WindowHeight};
    
    ActualExtent.width = Max(Capabilities.minImageExtent.width, Min(Capabilities.maxImageExtent.width, ActualExtent.width));
    ActualExtent.height = Max(Capabilities.minImageExtent.height, Min(Capabilities.maxImageExtent.height, ActualExtent.height));
    
    return ActualExtent;
}

static b32 CheckDeviceExtensionSupport(VkPhysicalDevice Device)
{
    u32 ExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(Device, nullptr, &ExtensionCount, nullptr);
    
    VkExtensionProperties* ExtensionProperties = PushTempArray(ExtensionCount, VkExtensionProperties);
    vkEnumerateDeviceExtensionProperties(Device, nullptr, &ExtensionCount, ExtensionProperties);
    
    i32 ExtensionSize = 0;
    for(u32 Index = 0; Index < ExtensionCount; Index++)
    {
        for(i32 RequiredIndex = 0; RequiredIndex < DEVICE_EXTENSIONS_SIZE; RequiredIndex++)
        {
            if(strcmp(DeviceExtensions[RequiredIndex], ExtensionProperties[Index].extensionName) == 0)
                ExtensionSize++;
        }
    }
    
    return ExtensionSize == DEVICE_EXTENSIONS_SIZE;
}

static b32 IsDeviceSuitable(vk_render_state& RenderState)
{
    auto ExtensionsSupported = CheckDeviceExtensionSupport(RenderState.PhysicalDevice);
    
    bool SwapchainAdequate = false;
    if(ExtensionsSupported)
    {
        RenderState.SwapchainSupportDetails = QuerySwapchainSupport(RenderState.Surface, RenderState.PhysicalDevice);
        SwapchainAdequate = RenderState.SwapchainSupportDetails.FormatCount > 0 && RenderState.SwapchainSupportDetails.PresentModeCount > 0;
    }
    
    // RenderState.DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    
    return RenderState.DeviceFeatures.geometryShader && IsQueueFamilyComplete(RenderState.QueueFamilyIndices) && ExtensionsSupported && SwapchainAdequate;
}

static VkDeviceQueueCreateInfo CreateDeviceQueue(u32 FamilyIndex)
{
    VkDeviceQueueCreateInfo QueueCreateInfo = {};
    
    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.queueFamilyIndex = FamilyIndex;
    QueueCreateInfo.queueCount = 1;
    
    r32 QueuePriority = 1.0f;
    QueueCreateInfo.pQueuePriorities = &QueuePriority;
    return QueueCreateInfo;
}

static void CreateSwapchain(vk_render_state& RenderState)
{
    VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(RenderState.SwapchainSupportDetails.Formats, RenderState.SwapchainSupportDetails.FormatCount);
    
    VkPresentModeKHR PresentMode = ChooseSwapPresentMode(RenderState.SwapchainSupportDetails.PresentModes, RenderState.SwapchainSupportDetails.PresentModeCount);
    VkExtent2D Extent = ChooseSwapExtent(RenderState, RenderState.SwapchainSupportDetails.Capabilities);
    
    u32 ImageCount = RenderState.SwapchainSupportDetails.Capabilities.minImageCount + 1;
    if(RenderState.SwapchainSupportDetails.Capabilities.maxImageCount > 0 && ImageCount > RenderState.SwapchainSupportDetails.Capabilities.maxImageCount)
    {
        ImageCount = RenderState.SwapchainSupportDetails.Capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR SwapchainCreateInfo = {};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = RenderState.Surface;
    SwapchainCreateInfo.minImageCount = ImageCount;
    SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapchainCreateInfo.imageExtent = Extent;
    SwapchainCreateInfo.imageArrayLayers = 1;
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    u32 QueueFamilyIndices[] = {(u32) RenderState.QueueFamilyIndices.GraphicsFamily, (u32) RenderState.QueueFamilyIndices.PresentFamily};
    
    if (RenderState.QueueFamilyIndices.GraphicsFamily != RenderState.QueueFamilyIndices.PresentFamily)
    {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfo.queueFamilyIndexCount = 2;
        SwapchainCreateInfo.pQueueFamilyIndices = QueueFamilyIndices;
    }
    else
    {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        SwapchainCreateInfo.queueFamilyIndexCount = 0; // Optional
        SwapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    
    SwapchainCreateInfo.preTransform = RenderState.SwapchainSupportDetails.Capabilities.currentTransform;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.presentMode = PresentMode;
    SwapchainCreateInfo.clipped = VK_TRUE;
    SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
    
    if (vkCreateSwapchainKHR(RenderState.Device, &SwapchainCreateInfo, nullptr, &RenderState.Swapchain) != VK_SUCCESS) 
    {
        printf("Unable to create swapchain\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    vkGetSwapchainImagesKHR(RenderState.Device, RenderState.Swapchain, &ImageCount, nullptr);
    RenderState.SwapchainImages = PushArray(&RenderState.Arena, ImageCount, VkImage);
    vkGetSwapchainImagesKHR(RenderState.Device, RenderState.Swapchain, &ImageCount, RenderState.SwapchainImages);
    RenderState.SwapchainImageCount = ImageCount;
    RenderState.SwapchainImageFormat = SurfaceFormat.format;
    RenderState.SwapchainExtent = Extent;
}

static void CreateSwapchainImageViews(vk_render_state& RenderState)
{
    RenderState.SwapchainImageViews = PushArray(&RenderState.Arena, RenderState.SwapchainImageCount, VkImageView);
    
    for(u32 Index = 0; Index < RenderState.SwapchainImageCount; Index++)
    {
        VkImageViewCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        CreateInfo.image = RenderState.SwapchainImages[Index];
        CreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        CreateInfo.format = RenderState.SwapchainImageFormat;
        CreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        CreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        CreateInfo.subresourceRange.baseMipLevel = 0;
        CreateInfo.subresourceRange.levelCount = 1;
        CreateInfo.subresourceRange.baseArrayLayer = 0;
        CreateInfo.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(RenderState.Device, &CreateInfo, nullptr, &RenderState.SwapchainImageViews[Index]) != VK_SUCCESS)
        {
            printf("Failed to create image views\n");
        }
    }
}

static VkBool32 CreateShaderModule(vk_render_state& RenderState, const char* ShaderPath, VkShaderModule& ShaderModule)
{
    FILE* ShaderFile = fopen(ShaderPath, "rb");
    
    if(ShaderFile)
    {
        u32 Size = 0;
        fseek(ShaderFile, 0, SEEK_END);
        Size = (u32)ftell(ShaderFile);
        fseek(ShaderFile, 0, SEEK_SET);
        
        u32* Buffer = PushTempSize(Size, u32);
        fread(Buffer, 1, (size_t)Size, ShaderFile);
        
        VkShaderModuleCreateInfo CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        CreateInfo.codeSize = Size;
        CreateInfo.pCode = Buffer;
        
        fclose(ShaderFile);
        
        if (vkCreateShaderModule(RenderState.Device, &CreateInfo, nullptr, &ShaderModule) != VK_SUCCESS)
        {
            printf("Failed to create shader module\n");
            return VK_FALSE;
        }
        
    }
    else
    {
        printf("The file '%s' could not be opened\n", ShaderPath);
        return VK_FALSE;
    }
    
    return VK_TRUE;
}

static void CreateGraphicsPipeline(vk_render_state& RenderState)
{
    VkShaderModule VertexShaderModule;
    VkShaderModule FragmentShaderModule;
    
    if(!CreateShaderModule(RenderState, "../engine_assets/shaders/vulkan/vertex.spv", VertexShaderModule))
    {
        printf("Could not create vertex shader module\n");
    }
    
    if(!CreateShaderModule(RenderState, "../engine_assets/shaders/vulkan/fragment.spv", FragmentShaderModule))
    {
        printf("Could not create vertex shader module\n");
    }
    
    VkPipelineShaderStageCreateInfo VertexShaderStageInfo = {};
    VertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    VertexShaderStageInfo.module = VertexShaderModule;
    VertexShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo FragmentShaderStageInfo = {};
    FragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    FragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    FragmentShaderStageInfo.module = FragmentShaderModule;
    FragmentShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo ShaderStages[] = { VertexShaderStageInfo, FragmentShaderStageInfo};
    
    VkPipelineVertexInputStateCreateInfo VertexInputInfo = {};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexBindingDescriptionCount = 0;
    VertexInputInfo.pVertexBindingDescriptions = nullptr;
    VertexInputInfo.vertexAttributeDescriptionCount = 0;
    VertexInputInfo.pVertexAttributeDescriptions = nullptr;
    
    VkPipelineInputAssemblyStateCreateInfo InputAssembly = {};
    InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    InputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkViewport Viewport = {};
    Viewport.x = 0.0f;
    Viewport.y = 0.0f;
    Viewport.width = (r32) RenderState.SwapchainExtent.width;
    Viewport.height = (r32) RenderState.SwapchainExtent.height;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;
    
    VkRect2D Scissor = {};
    Scissor.offset = {0, 0};
    Scissor.extent = RenderState.SwapchainExtent;
    
    VkPipelineViewportStateCreateInfo ViewportState = {};
    ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportState.viewportCount = 1;
    ViewportState.pViewports = &Viewport;
    ViewportState.scissorCount = 1;
    ViewportState.pScissors = &Scissor;
    
    VkPipelineRasterizationStateCreateInfo Rasterizer = {};
    Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Rasterizer.depthClampEnable = VK_FALSE;
    Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    Rasterizer.lineWidth = 1.0f;
    Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    Rasterizer.depthBiasEnable = VK_FALSE;
    Rasterizer.depthBiasConstantFactor = 0.0f;
    Rasterizer.depthBiasClamp = 0.0f;
    Rasterizer.depthBiasSlopeFactor = 0.0f;
    
    VkPipelineMultisampleStateCreateInfo Multisampling = {};
    Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Multisampling.sampleShadingEnable = VK_FALSE;
    Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    Multisampling.minSampleShading = 1.0f;
    Multisampling.pSampleMask = nullptr;
    Multisampling.alphaToCoverageEnable = VK_FALSE;
    Multisampling.alphaToOneEnable = VK_FALSE;
    
    VkPipelineColorBlendAttachmentState ColorBlendAttachment = {};
    ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    ColorBlendAttachment.blendEnable = VK_TRUE;
    ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo ColorBlending = {};
    ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    ColorBlending.logicOpEnable = VK_FALSE;
    ColorBlending.logicOp = VK_LOGIC_OP_COPY;
    ColorBlending.attachmentCount = 1;
    ColorBlending.pAttachments = &ColorBlendAttachment;
    ColorBlending.blendConstants[0] = 0.0f;
    ColorBlending.blendConstants[1] = 0.0f;
    ColorBlending.blendConstants[2] = 0.0f;
    ColorBlending.blendConstants[3] = 0.0f;
    
    /*
    VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_LINE_WIDTH
};

VkPipelineDynamicStateCreateInfo dynamicState = {};
dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
dynamicState.dynamicStateCount = 2;
dynamicState.pDynamicStates = dynamicStates;
    */
    
    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.setLayoutCount = 0;
    PipelineLayoutInfo.pSetLayouts = nullptr;
    PipelineLayoutInfo.pushConstantRangeCount = 0;
    PipelineLayoutInfo.pPushConstantRanges = 0;
    
    if (vkCreatePipelineLayout(RenderState.Device, &PipelineLayoutInfo, nullptr, &RenderState.PipelineLayout) != VK_SUCCESS)
    {
        printf("Failed to create pipeline layout");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    vkDestroyShaderModule(RenderState.Device, FragmentShaderModule, nullptr);
    vkDestroyShaderModule(RenderState.Device, VertexShaderModule, nullptr);
}

static void InitializeVulkan(vk_render_state& RenderState, renderer& Renderer, config_data& ConfigData)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    RenderState.Window = glfwCreateWindow(ConfigData.ScreenWidth, ConfigData.ScreenHeight, ConfigData.Title, nullptr, nullptr);
    
    if(!RenderState.Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    /*u32 ExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
    
    printf("Extensions supported: %d\n", ExtensionCount);
    */
    
    RenderState.WindowWidth = ConfigData.ScreenWidth;
    RenderState.WindowHeight = ConfigData.ScreenHeight;
    
    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = ConfigData.Title;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Glitchheart Engine";
    AppInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pApplicationInfo = &AppInfo;
    
#if GLITCH_DEBUG
    RenderState.EnableValidationLayers = true;
#else
    RenderState.EnableValidationLayers = false;
#endif
    
    if(RenderState.EnableValidationLayers && !CheckValidationLayerSupport())
    {
        printf("No validation layers are supported\n");
        
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    CreateInfo.enabledLayerCount = 0;
    u32 GlfwExtensionCount = 0;
    
    const char** GlfwExtensions;
    GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
    
    auto Extensions = PushTempArray(GlfwExtensionCount + 1, const char*);
    for(u32 Index = 0; Index < GlfwExtensionCount; Index++)
    {
        Extensions[Index] = GlfwExtensions[Index];
    }
    
    if(RenderState.EnableValidationLayers)
    {
        Extensions[GlfwExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        CreateInfo.enabledLayerCount = (u32)VALIDATION_LAYER_SIZE;
        CreateInfo.ppEnabledLayerNames = ValidationLayers;
    }
    
    
    CreateInfo.enabledExtensionCount = GlfwExtensionCount;
    CreateInfo.ppEnabledExtensionNames = Extensions;
    
    if(vkCreateInstance(&CreateInfo, nullptr, &RenderState.Instance) != VK_SUCCESS)
    {
        printf("Could not create VkInstance\n");
        VkCleanup(RenderState, Renderer);
        exit(EXIT_FAILURE);
    }
    
#if GLITCH_DEBUG
    VkDebugReportCallbackCreateInfoEXT DebugCreateInfo = {};
    DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    DebugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    DebugCreateInfo.pfnCallback = DebugCallback;
    
    if(CreateDebugReportCallbackEXT(RenderState.Instance, &DebugCreateInfo, nullptr, &RenderState.Callback) != VK_SUCCESS)
    {
        DEBUG_PRINT("Failed to set up debug callback\n");
    }
#endif
    
    if(glfwCreateWindowSurface(RenderState.Instance, RenderState.Window, nullptr, &RenderState.Surface) != VK_SUCCESS)
    {
        printf("Unable to create window surface\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    u32 DeviceCount = 0;
    vkEnumeratePhysicalDevices(RenderState.Instance, &DeviceCount, nullptr);
    
    if(DeviceCount == 0)
    {
        printf("No physical devices found\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    RenderState.PhysicalDevice = VK_NULL_HANDLE;
    
    VkPhysicalDevice* Devices = PushTempArray(DeviceCount, VkPhysicalDevice);
    vkEnumeratePhysicalDevices(RenderState.Instance, &DeviceCount, Devices);
    
    // @Incomplete:(Niels): Find "most" suitable device (integrated/dedicated)
    RenderState.PhysicalDevice = Devices[0];
    
    vkGetPhysicalDeviceProperties(RenderState.PhysicalDevice, &RenderState.DeviceProperties);
    vkGetPhysicalDeviceFeatures(RenderState.PhysicalDevice, &RenderState.DeviceFeatures);
    
    RenderState.QueueFamilyIndices = FindQueueFamilies(RenderState.Surface, RenderState.PhysicalDevice);
    
    if(!IsDeviceSuitable(RenderState))
    {
        printf("Device is not suitable");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    VkDeviceQueueCreateInfo* QueueCreateInfos = PushTempArray(2, VkDeviceQueueCreateInfo);
    QueueCreateInfos[0] = CreateDeviceQueue((u32)RenderState.QueueFamilyIndices.GraphicsFamily);
    QueueCreateInfos[1] = CreateDeviceQueue((u32)RenderState.QueueFamilyIndices.PresentFamily);
    
    u32 QueueCount = 2;
    
    if(RenderState.QueueFamilyIndices.GraphicsFamily == RenderState.QueueFamilyIndices.PresentFamily)
    {
        QueueCount = 1;
    }
    
    VkDeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos;
    DeviceCreateInfo.queueCreateInfoCount = QueueCount;
    DeviceCreateInfo.pEnabledFeatures = &RenderState.DeviceFeatures;
    
    DeviceCreateInfo.enabledExtensionCount = DEVICE_EXTENSIONS_SIZE;
    DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions;
    
    if(RenderState.EnableValidationLayers)
    {
        DeviceCreateInfo.enabledLayerCount = (u32)VALIDATION_LAYER_SIZE;
        DeviceCreateInfo.ppEnabledLayerNames = ValidationLayers;
    }
    else
    {
        DeviceCreateInfo.enabledLayerCount = 0;
    }
    
    if(vkCreateDevice(RenderState.PhysicalDevice, &DeviceCreateInfo, nullptr, &RenderState.Device) != VK_SUCCESS)
    {
        printf("Unable to create physical device\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    vkGetDeviceQueue(RenderState.Device, (u32)RenderState.QueueFamilyIndices.GraphicsFamily, 0, &RenderState.GraphicsQueue);
    vkGetDeviceQueue(RenderState.Device, (u32)RenderState.QueueFamilyIndices.PresentFamily, 0, &RenderState.PresentQueue);
    
    CreateSwapchain(RenderState);
    CreateSwapchainImageViews(RenderState);
}

static void ToggleCursor(vk_render_state& RenderState)
{
    RenderState.HideCursor = !RenderState.HideCursor;
    
    if(RenderState.HideCursor)
    {
        glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

static void VkRender(vk_render_state& RenderState, renderer& Renderer)
{
    while(!glfwWindowShouldClose(RenderState.Window))
    {
        glfwPollEvents();
    }
    
    VkCleanup(RenderState, Renderer);
}