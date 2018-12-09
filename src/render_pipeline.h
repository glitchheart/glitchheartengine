namespace rendering
{
    struct FramebufferHandle
    {
        i32 handle;
    };

    enum FramebufferType
    {
        NORMAL,
        POST_PROCESSING
    };

    enum ColorAttachmentFlags
    {
        RENDER_BUFFER = 1 << 0,
        TEXTURE_BUFFER = 1 << 1,
        RGBA_8 = 1 << 2,
        RGBA_16 = 1 << 3,
        RGBA_32 = 1 << 4
    };

    enum DepthAttachmentFlags
    {
    };
    
    struct FramebufferInfo
    {
        FramebufferType type;
        
        u32 width;
        u32 height;
        
        struct
        {
            b32 enabled;
            u64 flags;
            
        } color_attachment;
        struct
        {
            b32 enabled;
            u64 flags;
        } depth_attachment;
    };

    // @Note: When rendering a scene, multiple render passes can be used.
    // Each render component can register itself to a render pass
    // Each render pass has it's own render commands
    // Each render pass has a framebuffer handle
    // When reaching the graphics renderer, each render pass binds it's framebuffer and goes through all render commands
    // A render pass describes what framebuffer to render to and the commands that should be rendered
    struct RenderPass
    {
        FramebufferHandle framebuffer;
        rendering::RenderCommand *render_commands;
        i32 render_command_count;
    };

    struct RenderPassHandle
    {
        i32 handle;
    };

    struct PostProcessingRenderPassHandle
    {
        i32 handle;
    };
}
