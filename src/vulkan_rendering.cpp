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
    DestroyDebugReportCallbackEXT(RenderState.Instance, RenderState.Callback, nullptr);
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
    
    CreateInfo.enabledLayerCount = 0;
    u32 GlfwExtensionCount = 0;
    
    const char** GlfwExtensions;
    GlfwExtensions = glfwGetRequiredInstanceExtensions(&GlfwExtensionCount);
    
    auto Extensions = PushTempArray(GlfwExtensionCount + 1, const char*);
    for(u32 Index = 0; Index < GlfwExtensionCount; Index++)
    {
        Extensions[Index] = GlfwExtensions[Index];
    }
    
#if GLITCH_DEBUG
    if(!CheckValidationLayerSupport())
    {
        printf("No validation layers are supported\n");
        
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    else
    {
        Extensions[GlfwExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        CreateInfo.enabledLayerCount = (u32)VALIDATION_LAYER_SIZE;
        CreateInfo.ppEnabledLayerNames = ValidationLayers;
    }
    
    VkDebugReportCallbackCreateInfoEXT DebugCreateInfo = {};
    DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    DebugCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    DebugCreateInfo.pfnCallback = DebugCallback;
#endif
    
    CreateInfo.enabledExtensionCount = GlfwExtensionCount;
    CreateInfo.ppEnabledExtensionNames = Extensions;
    
    if(vkCreateInstance(&CreateInfo, nullptr, &RenderState.Instance) != VK_SUCCESS)
    {
        printf("Could not create VkInstance\n");
        VkCleanup(RenderState, Renderer);
        exit(EXIT_FAILURE);
    }
    
#if GLITCH_DEBUG
    if(CreateDebugReportCallbackEXT(RenderState.Instance, &DebugCreateInfo, nullptr, &RenderState.Callback) != VK_SUCCESS)
    {
        DEBUG_PRINT("Failed to set up debug callback\n");
    }
#endif
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


