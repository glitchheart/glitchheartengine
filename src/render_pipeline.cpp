namespace rendering
{
    // @Note: Creates a RenderPass with the specified FramebufferHandle
    static RenderPassHandle create_render_pass(FramebufferHandle framebuffer, Renderer &renderer)
    {
        RenderPass &pass = renderer.render.passes[renderer.render.pass_count];
        pass.render_commands = push_array(&renderer.render.render_pass_arena, global_max_render_commands, RenderCommand);
        return { renderer.render.pass_count++ };
    }

    static PostProcessingRenderPassHandle create_post_processing_render_pass(FramebufferHandle framebuffer, Renderer &renderer)
    {
        RenderPass &pass = renderer.render.post_processing_passes[renderer.render.post_processing_pass_count];
        pass.render_commands = push_array(&renderer.render.render_pass_arena, global_max_render_commands, RenderCommand);
        return { renderer.render.post_processing_pass_count++ };
    }


    // @Note: Generates and returns a FramebufferInfo with default values
    static FramebufferInfo generate_framebuffer_info()
    {
        FramebufferInfo info = {};
        info.color_attachment.enabled = false;
        info.color_attachment.flags = 0;
        info.depth_attachment.enabled = false;
        info.depth_attachment.flags = 0;

        return info;
    }

    // @Note: creates a framebuffer that can be used with a render pass
    // The handle returned maps directly to the graphics API specific framebuffer index
    static FramebufferHandle create_framebuffer(FramebufferInfo &info, Renderer &renderer)
    {
        FramebufferHandle handle = { (renderer.render.framebuffer_count++) + 1 };
        renderer.render.framebuffers[handle.handle - 1] = info;
        return handle;
    }

    static void test(Renderer &renderer)
    {
        FramebufferInfo info = generate_framebuffer_info();
        info.width = 1024;
        info.height = 720;
        info.color_attachment.enabled = true;
        info.color_attachment.flags = ColorAttachmentFlags::TEXTURE_BUFFER | ColorAttachmentFlags::RGBA_8;
        rendering::FramebufferHandle framebuffer_handle = rendering::create_framebuffer(info, renderer);
        rendering::RenderPassHandle render_pass_handle = rendering::create_render_pass(framebuffer_handle, renderer);
    }
}
