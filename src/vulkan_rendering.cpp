static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT obj_type, u64 obj, size_t location, i32 code, const char* layer_prefix, const char* Msg, void* user_data)
{
    Debug("%s\n", Msg);
    return VK_FALSE;
}

VkResult create_debug_report_callback_ext(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugReportCallbackEXT* callback)
{
    auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr)
    {
        return func(instance, create_info, allocator, callback);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_report_callback_ext(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* allocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr)
    {
        func(instance, callback, allocator);
    }
}

static void vk_cleanup(VkRenderState& render_state, Renderer& renderer)
{
    vkDestroySemaphore(render_state.device, render_state.render_finished_semaphore, nullptr);
    vkDestroySemaphore(render_state.device, render_state.image_available_semaphore, nullptr);
    vkDestroyCommandPool(render_state.device, render_state.command_pool, nullptr);
    vkDestroySwapchainKHR(render_state.device, render_state.swapchain, nullptr);
    
    for (u32 index = 0; index < render_state.swapchain_image_count; index++) {
        vkDestroyFramebuffer(render_state.device, render_state.swapchain_framebuffers[index], nullptr);
    }
    
    for(u32 index = 0; index < render_state.swapchain_image_count; index++)
    {
        vkDestroyImageView(render_state.device, render_state.swapchain_image_views[index], nullptr);
    }
    
    vkDestroyPipeline(render_state.device, render_state.graphics_pipeline, nullptr);
    vkDestroyPipelineLayout(render_state.device, render_state.pipeline_layout, nullptr);
    vkDestroyRenderPass(render_state.device, render_state.render_pass, nullptr);
    
    vkDestroyDevice(render_state.device, nullptr);
    destroy_debug_report_callback_ext(render_state.instance, render_state.callback, nullptr);
    vkDestroySurfaceKHR(render_state.instance, render_state.surface, nullptr);
    vkDestroyInstance(render_state.instance, nullptr);
    glfwDestroyWindow(render_state.window);
    glfwTerminate();
}

static b32 check_validation_layer_support()
{
    u32 layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    
    VkLayerProperties* available_layers = push_temp_array(layer_count, VkLayerProperties);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
    
    for(i32 index = 0; index < VALIDATION_LAYER_SIZE; index++)
    {
        b32 layer_found = false;
        
        for(u32 layer_prop_index = 0; layer_prop_index < layer_count; layer_prop_index++)
        {
            if(strcmp(validation_layers[index], available_layers[layer_prop_index].layerName) == 0)
            {
                layer_found = true;
                break;
            }
        }
        
        if(!layer_found)
        {
            return false;
        }
    }
    
    return true;
}

b32 is_queue_family_complete(QueueFamilyIndices& indices)
{
    return indices.graphics_family >= 0 && indices.present_family >= 0;
}

QueueFamilyIndices find_queue_families(VkSurfaceKHR surface, VkPhysicalDevice device)
{
    QueueFamilyIndices indices;
    indices.graphics_family = -1;
    indices.present_family = -1;
    
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    
    VkQueueFamilyProperties* queue_families = push_temp_array(queue_family_count, VkQueueFamilyProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);
    
    for(u32 index = 0; index < queue_family_count; index++)
    {
        auto& queue_family = queue_families[index];
        if(queue_family.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = (i32)index;
        }
        
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, index, surface, &present_support);
        
        if(queue_family.queueCount > 0 && present_support)
        {
            indices.present_family = (i32)index;
        }
        
        if(is_queue_family_complete(indices))
        {
            break;
        }
    }
    
    return indices;
}

SwapchainSupportDetails query_swapchain_support(VkSurfaceKHR surface, VkPhysicalDevice device)
{
    SwapchainSupportDetails details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);
    
    if (format_count != 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats);
    }
    
    u32 present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);
    
    if (present_mode_count != 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes);
    }
    
    return details;
}

VkSurfaceFormatKHR choose_swap_surface_format(VkSurfaceFormatKHR* formats, i32 format_count)
{
    if(formats)
    {
        if(format_count == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
        }
    }
    
    for(i32 format_index = 0; format_index < format_count; format_index++)
    {
        auto available_format = formats[format_index];
        if(available_format.format == VK_FORMAT_B8G8R8A8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_format;
        }
    }
    
    return formats[0];
}

VkPresentModeKHR choose_swap_present_mode(VkPresentModeKHR* present_modes, i32 present_mode_count)
{
    VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;
    
    for (i32 present_mode_index = 0; present_mode_index < present_mode_count; present_mode_index++)
    {
        auto available_present_mode = present_modes[present_mode_index];
        if (available_present_mode == VK_PRESENT_MODE_FIFO_KHR)
        {
            return available_present_mode;
        }
        else if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            best_mode = available_present_mode;
        } 
        else if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR && best_mode != VK_PRESENT_MODE_MAILBOX_KHR)
        {
            best_mode = available_present_mode;
        }
    }
    
    return best_mode;
}

VkExtent2D choose_swap_extent(VkRenderState& render_state, VkSurfaceCapabilitiesKHR Capabilities)
{
    
    VkExtent2D ActualExtent = {(u32)render_state.window_width, (u32)render_state.window_height};
    
    ActualExtent.width = Max(Capabilities.minImageExtent.width, Min(Capabilities.maxImageExtent.width, ActualExtent.width));
    ActualExtent.height = Max(Capabilities.minImageExtent.height, Min(Capabilities.maxImageExtent.height, ActualExtent.height));
    
    return ActualExtent;
}

static b32 check_device_extension_support(VkPhysicalDevice device)
{
    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
    
    VkExtensionProperties* extension_properties = push_temp_array(extension_count, VkExtensionProperties);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, extension_properties);
    
    i32 extension_size = 0;
    for(u32 index = 0; index < extension_count; index++)
    {
        for(i32 required_index = 0; required_index < DEVICE_EXTENSIONS_SIZE; required_index++)
        {
            if(strcmp(device_extensions[required_index], extension_properties[index].extensionName) == 0)
                extension_size++;
        }
    }
    
    return extension_size == DEVICE_EXTENSIONS_SIZE;
}

static b32 is_device_suitable(VkRenderState& render_state)
{
    auto extensions_supported = check_device_extension_support(render_state.physical_device);
    
    bool swapchain_adequate = false;
    if(extensions_supported)
    {
        render_state.swapchain_support_details = query_swapchain_support(render_state.surface, render_state.physical_device);
        swapchain_adequate = render_state.swapchain_support_details.format_count > 0 && render_state.swapchain_support_details.present_mode_count > 0;
    }
    
    // render_state.DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
    
    return render_state.device_features.geometryShader && is_queue_family_complete(render_state.queue_family_indices) && extensions_supported && swapchain_adequate;
}

static VkDeviceQueueCreateInfo create_device_queue(u32 family_index)
{
    VkDeviceQueueCreateInfo queue_create_info = {};
    
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = family_index;
    queue_create_info.queueCount = 1;
    
    r32 queue_priority = 1.0f;
    queue_create_info.pQueuePriorities = &queue_priority;
    return queue_create_info;
}

static void create_swapchain(VkRenderState& render_state)
{
    VkSurfaceFormatKHR surface_format = choose_swap_surface_format(render_state.swapchain_support_details.formats, render_state.swapchain_support_details.format_count);
    
    VkPresentModeKHR present_mode = choose_swap_present_mode(render_state.swapchain_support_details.present_modes, render_state.swapchain_support_details.present_mode_count);
    VkExtent2D extent = choose_swap_extent(render_state, render_state.swapchain_support_details.capabilities);
    
    u32 image_count = render_state.swapchain_support_details.capabilities.minImageCount + 1;
    if(render_state.swapchain_support_details.capabilities.maxImageCount > 0 && image_count > render_state.swapchain_support_details.capabilities.maxImageCount)
    {
        image_count = render_state.swapchain_support_details.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR swapchain_create_info = {};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.surface = render_state.surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    u32 queue_family_indices[] = {(u32) render_state.queue_family_indices.graphics_family, (u32) render_state.queue_family_indices.present_family};
    
    if (render_state.queue_family_indices.graphics_family != render_state.queue_family_indices.present_family)
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchain_create_info.queueFamilyIndexCount = 2;
        swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount = 0; // Optional
        swapchain_create_info.pQueueFamilyIndices = nullptr; // Optional
    }
    
    swapchain_create_info.preTransform = render_state.swapchain_support_details.capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_TRUE;
    swapchain_create_info.oldSwapchain = VK_NULL_HANDLE;
    
    if (vkCreateSwapchainKHR(render_state.device, &swapchain_create_info, nullptr, &render_state.swapchain) != VK_SUCCESS) 
    {
        Debug("Unable to create swapchain\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    vkGetSwapchainImagesKHR(render_state.device, render_state.swapchain, &image_count, nullptr);
    render_state.swapchain_images = push_array(&render_state.arena, image_count, VkImage);
    vkGetSwapchainImagesKHR(render_state.device, render_state.swapchain, &image_count, render_state.swapchain_images);
    render_state.swapchain_image_count = image_count;
    render_state.swapchain_image_format = surface_format.format;
    render_state.swapchain_extent = extent;
}

static void create_swapchain_image_views(VkRenderState& render_state)
{
    render_state.swapchain_image_views = push_array(&render_state.arena, render_state.swapchain_image_count, VkImageView);
    
    for(u32 index = 0; index < render_state.swapchain_image_count; index++)
    {
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = render_state.swapchain_images[index];
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = render_state.swapchain_image_format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        
        if (vkCreateImageView(render_state.device, &create_info, nullptr, &render_state.swapchain_image_views[index]) != VK_SUCCESS)
        {
            Debug("Failed to create image views\n");
        }
    }
}

static VkBool32 create_shader_module(VkRenderState& render_state, const char* ShaderPath, VkShaderModule& shader_module)
{
    FILE* shader_file = fopen(ShaderPath, "rb");
    
    if(shader_file)
    {
        u32 size = 0;
        fseek(shader_file, 0, SEEK_END);
        size = (u32)ftell(shader_file);
        fseek(shader_file, 0, SEEK_SET);
        
        u32* buffer = push_temp_size(size, u32);
        fread(buffer, 1, (size_t)size, shader_file);
        
        VkShaderModuleCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = size;
        create_info.pCode = buffer;
        
        fclose(shader_file);
        
        if (vkCreateShaderModule(render_state.device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            Debug("Failed to create shader module\n");
            return VK_FALSE;
        }
        
    }
    else
    {
        Debug("The file '%s' could not be opened\n", ShaderPath);
        return VK_FALSE;
    }
    
    return VK_TRUE;
}

static void create_graphics_pipeline(VkRenderState& render_state)
{
    VkShaderModule vertex_shader_module;
    VkShaderModule fragment_shader_module;
    
    if(!create_shader_module(render_state, "../engine_assets/shaders/vulkan/vertex.spv", vertex_shader_module))
    {
        Debug("Could not create vertex shader module\n");
    }
    
    if(!create_shader_module(render_state, "../engine_assets/shaders/vulkan/fragment.spv", fragment_shader_module))
    {
        Debug("Could not create vertex shader module\n");
    }
    
    VkPipelineShaderStageCreateInfo vertex_shader_stage_info = {};
    vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = vertex_shader_module;
    vertex_shader_stage_info.pName = "main";
    
    VkPipelineShaderStageCreateInfo fragment_shader_stage_info = {};
    fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = fragment_shader_module;
    fragment_shader_stage_info.pName = "main";
    
    VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_stage_info, fragment_shader_stage_info};
    
    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 0;
    vertex_input_info.pVertexBindingDescriptions = nullptr;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.pVertexAttributeDescriptions = nullptr;
    
    VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (r32) render_state.swapchain_extent.width;
    viewport.height = (r32) render_state.swapchain_extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = render_state.swapchain_extent;
    
    VkPipelineViewportStateCreateInfo viewport_state = {};
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &scissor;
    
    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    
    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;
    
    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_TRUE;
    color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    
    VkPipelineColorBlendStateCreateInfo color_blending = {};
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &color_blend_attachment;
    color_blending.blendConstants[0] = 0.0f;
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f;
    color_blending.blendConstants[3] = 0.0f;
    
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
    
    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;
    pipeline_layout_info.pPushConstantRanges = 0;
    
    if (vkCreatePipelineLayout(render_state.device, &pipeline_layout_info, nullptr, &render_state.pipeline_layout) != VK_SUCCESS)
    {
        Debug("Failed to create pipeline layout");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    VkGraphicsPipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pDepthStencilState = nullptr;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = nullptr;
    pipeline_info.renderPass = render_state.render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.basePipelineIndex = -1;
    pipeline_info.layout = render_state.pipeline_layout;
    
    if (vkCreateGraphicsPipelines(render_state.device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &render_state.graphics_pipeline) != VK_SUCCESS) {
        Debug("failed to create graphics pipeline\n");
    }
    
    vkDestroyShaderModule(render_state.device, fragment_shader_module, nullptr);
    vkDestroyShaderModule(render_state.device, vertex_shader_module, nullptr);
}

static void create_render_pass(VkRenderState& render_state)
{
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = render_state.swapchain_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;
    
    if (vkCreateRenderPass(render_state.device, &render_pass_info, nullptr, &render_state.render_pass) != VK_SUCCESS) 
    {
        Debug("failed to create render pass\n");
    }
}

static void create_framebuffers(VkRenderState& render_state)
{
    render_state.swapchain_framebuffers = push_array(&render_state.arena, render_state.swapchain_image_count, VkFramebuffer);
    
    for(size_t index = 0; index < render_state.swapchain_image_count; index++) 
    {
        VkImageView attachments[] = 
        {
            render_state.swapchain_image_views[index]
        };
        
        VkFramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = render_state.render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = render_state.swapchain_extent.width;
        framebuffer_info.height = render_state.swapchain_extent.height;
        framebuffer_info.layers = 1;
        
        if(vkCreateFramebuffer(render_state.device, &framebuffer_info, nullptr, &render_state.swapchain_framebuffers[index]) != VK_SUCCESS) 
        {
            Debug("failed to create framebuffer\n");
        }
    }
}

static void create_command_pool(VkRenderState& render_state)
{
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = (u32)render_state.queue_family_indices.graphics_family;
    pool_info.flags = 0;
    
    if (vkCreateCommandPool(render_state.device, &pool_info, nullptr, &render_state.command_pool) != VK_SUCCESS) 
    {
        Debug("failed to create command pool\n");
    }
}

static void create_command_buffers(VkRenderState& render_state)
{
    auto command_buffer_count = render_state.swapchain_image_count;
    render_state.command_buffers = push_array(&render_state.arena, command_buffer_count, VkCommandBuffer);
    
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = render_state.command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = (uint32_t) command_buffer_count;
    
    if (vkAllocateCommandBuffers(render_state.device, &alloc_info, render_state.command_buffers) != VK_SUCCESS) 
    {
        Debug("failed to allocate command buffers\n");
    }
    
    for(size_t index = 0; index < command_buffer_count; index++) 
    {
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
        begin_info.pInheritanceInfo = nullptr; // Optional
        
        vkBeginCommandBuffer(render_state.command_buffers[index], &begin_info);
        
        VkRenderPassBeginInfo render_pass_info = {};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = render_state.render_pass;
        render_pass_info.framebuffer = render_state.swapchain_framebuffers[index];
        
        render_pass_info.renderArea.offset = {0, 0};
        render_pass_info.renderArea.extent = render_state.swapchain_extent;
        
        VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        render_pass_info.clearValueCount = 1;
        render_pass_info.pClearValues = &clear_color;
        
        vkCmdBeginRenderPass(render_state.command_buffers[index], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindPipeline(render_state.command_buffers[index], VK_PIPELINE_BIND_POINT_GRAPHICS, render_state.graphics_pipeline);
        
        vkCmdDraw(render_state.command_buffers[index], 3, 1, 0, 0);
        
        vkCmdEndRenderPass(render_state.command_buffers[index]);
        
        if (vkEndCommandBuffer(render_state.command_buffers[index]) != VK_SUCCESS) 
        {
            Debug("failed to record command buffer\n");
        }
    }
    
}

static void create_semaphores(VkRenderState& render_state)
{
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    if (vkCreateSemaphore(render_state.device, &semaphore_info, nullptr, &render_state.image_available_semaphore) != VK_SUCCESS ||
        vkCreateSemaphore(render_state.device, &semaphore_info, nullptr, &render_state.render_finished_semaphore) != VK_SUCCESS) 
    {
        Debug("failed to create semaphores\n");
    }
}

static void initialize_vulkan(VkRenderState& render_state, Renderer& renderer, ConfigData& config_data)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    render_state.window = glfwCreateWindow(config_data.screen_width, config_data.screen_height, config_data.title, nullptr, nullptr);
    
    if(!render_state.window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    /*u32 ExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
    
    Debug("Extensions supported: %d\n", ExtensionCount);
    */
    
    render_state.window_width = config_data.screen_width;
    render_state.window_height = config_data.screen_height;
    
    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = config_data.title;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Glitchheart Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = VK_API_VERSION_1_0;
    
    VkInstanceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    
#if GLITCH_DEBUG
    render_state.enable_validation_layers = true;
#else
    render_state.EnableValidationLayers = false;
#endif
    
    if(render_state.enable_validation_layers && !check_validation_layer_support())
    {
        Debug("No validation layers are supported\n");
        
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    create_info.enabledLayerCount = 0;
    u32 glfw_extension_count = 0;
    
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    
    auto extensions = push_temp_array(glfw_extension_count + 1, const char*);
    for(u32 index = 0; index < glfw_extension_count; index++)
    {
        extensions[index] = glfw_extensions[index];
    }
    
    if(render_state.enable_validation_layers)
    {
        extensions[glfw_extension_count++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        create_info.enabledLayerCount = (u32)VALIDATION_LAYER_SIZE;
        create_info.ppEnabledLayerNames = validation_layers;
    }
    
    
    create_info.enabledExtensionCount = glfw_extension_count;
    create_info.ppEnabledExtensionNames = extensions;
    
    if(vkCreateInstance(&create_info, nullptr, &render_state.instance) != VK_SUCCESS)
    {
        Debug("Could not create VkInstance\n");
        vk_cleanup(render_state, renderer);
        exit(EXIT_FAILURE);
    }
    
#if GLITCH_DEBUG
    VkDebugReportCallbackCreateInfoEXT debug_create_info = {};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    debug_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    debug_create_info.pfnCallback = debug_callback;
    
    if(create_debug_report_callback_ext(render_state.instance, &debug_create_info, nullptr, &render_state.callback) != VK_SUCCESS)
    {
        Debug("Failed to set up debug callback\n");
    }
#endif
    
    if(glfwCreateWindowSurface(render_state.instance, render_state.window, nullptr, &render_state.surface) != VK_SUCCESS)
    {
        Debug("Unable to create window surface\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(render_state.instance, &device_count, nullptr);
    
    if(device_count == 0)
    {
        Debug("No physical devices found\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    render_state.physical_device = VK_NULL_HANDLE;
    
    VkPhysicalDevice* devices = push_temp_array(device_count, VkPhysicalDevice);
    vkEnumeratePhysicalDevices(render_state.instance, &device_count, devices);
    
    // @Incomplete:(Niels): Find "most" suitable device (integrated/dedicated)
    render_state.physical_device = devices[0];
    
    vkGetPhysicalDeviceProperties(render_state.physical_device, &render_state.device_properties);
    vkGetPhysicalDeviceFeatures(render_state.physical_device, &render_state.device_features);
    
    render_state.queue_family_indices = find_queue_families(render_state.surface, render_state.physical_device);
    
    if(!is_device_suitable(render_state))
    {
        Debug("Device is not suitable");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    VkDeviceQueueCreateInfo* queue_create_infos = push_temp_array(2, VkDeviceQueueCreateInfo);
    queue_create_infos[0] = create_device_queue((u32)render_state.queue_family_indices.graphics_family);
    queue_create_infos[1] = create_device_queue((u32)render_state.queue_family_indices.present_family);
    
    u32 queue_count = 2;
    
    if(render_state.queue_family_indices.graphics_family == render_state.queue_family_indices.present_family)
    {
        queue_count = 1;
    }
    
    VkDeviceCreateInfo device_create_info = {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.queueCreateInfoCount = queue_count;
    device_create_info.pEnabledFeatures = &render_state.device_features;
    
    device_create_info.enabledExtensionCount = DEVICE_EXTENSIONS_SIZE;
    device_create_info.ppEnabledExtensionNames = device_extensions;
    
    if(render_state.enable_validation_layers)
    {
        device_create_info.enabledLayerCount = (u32)VALIDATION_LAYER_SIZE;
        device_create_info.ppEnabledLayerNames = validation_layers;
    }
    else
    {
        device_create_info.enabledLayerCount = 0;
    }
    
    if(vkCreateDevice(render_state.physical_device, &device_create_info, nullptr, &render_state.device) != VK_SUCCESS)
    {
        Debug("Unable to create physical device\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    vkGetDeviceQueue(render_state.device, (u32)render_state.queue_family_indices.graphics_family, 0, &render_state.graphics_queue);
    vkGetDeviceQueue(render_state.device, (u32)render_state.queue_family_indices.present_family, 0, &render_state.present_queue);
    
    create_swapchain(render_state);
    create_swapchain_image_views(render_state);
    create_render_pass(render_state);
    create_graphics_pipeline(render_state);
    create_framebuffers(render_state);
    create_command_pool(render_state);
    create_command_buffers(render_state);
    create_semaphores(render_state);
}

static void toggle_cursor(VkRenderState& render_state)
{
    render_state.hide_cursor = !render_state.hide_cursor;
    
    if(render_state.hide_cursor)
    {
        glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else
    {
        glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

static void draw_frame(VkRenderState& render_state)
{
    u32 image_index;
    
    vkQueueWaitIdle(render_state.present_queue);
    
    vkAcquireNextImageKHR(render_state.device, render_state.swapchain, ULONG_MAX, render_state.image_available_semaphore, VK_NULL_HANDLE, &image_index);
    
    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore wait_semaphores[] = {render_state.image_available_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &render_state.command_buffers[image_index];
    
    VkSemaphore signal_semaphores[] = {render_state.render_finished_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;
    
    if (vkQueueSubmit(render_state.graphics_queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS)
    {
        Debug("failed to submit draw command buffer\n");
    }
    
    VkPresentInfoKHR present_info = {};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    
    VkSwapchainKHR swapchains[] = {render_state.swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;
    
    vkQueuePresentKHR(render_state.present_queue, &present_info);
}

static void vk_render(VkRenderState& render_state, Renderer& renderer)
{
    while(!glfwWindowShouldClose(render_state.window))
    {
        glfwPollEvents();
        draw_frame(render_state);
    }
    
    vkDeviceWaitIdle(render_state.device);
    
    vk_cleanup(render_state, renderer);
}