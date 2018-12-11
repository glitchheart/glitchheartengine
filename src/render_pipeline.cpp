namespace rendering
{
    // @Note: Creates a RenderPass with the specified FramebufferHandle
    static RenderPassHandle create_render_pass(const char *name, FramebufferHandle framebuffer, Renderer &renderer)
    {
        RenderPass &pass = renderer.render.passes[renderer.render.pass_count];
        pass.framebuffer = framebuffer;
        pass.use_scene_camera = true;
        pass.clipping_planes.type = ClippingPlaneType::NONE;
        pass.clipping_planes.plane = math::Vec4(0, 0, 0, 0);
        
        strncpy(pass.name, name, strlen(name) + 1);
        pass.commands.render_commands = push_array(&renderer.render.render_pass_arena, global_max_render_commands, RenderCommand);
        
        return { (renderer.render.pass_count++) + 1};
    }

    static PostProcessingRenderPassHandle create_post_processing_render_pass(const char *name, FramebufferHandle framebuffer, Renderer &renderer)
    {
        RenderPass &pass = renderer.render.post_processing_passes[renderer.render.post_processing_pass_count];
        pass.commands.render_commands = push_array(&renderer.render.render_pass_arena, global_max_render_commands, RenderCommand);
        return { (renderer.render.post_processing_pass_count++) + 1 };
    }


    // @Note: Generates and returns a FramebufferInfo with default values
    static FramebufferInfo generate_framebuffer_info()
    {
        FramebufferInfo info = {};
        info.color_attachments.enabled = false;
        info.color_attachments.count = 0;
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

    static RenderPassHandle get_render_pass_handle_for_name(const char *name, Renderer &renderer)
    {
        for(i32 i = 0; i < renderer.render.pass_count; i++)
        {
            if(strcmp(renderer.render.passes[i].name, name) == 0)
            {
                return { i + 1 };
            }
        }

        return { -1 };
    }

    static TextureHandle get_texture_from_framebuffer(i32 texture_index, FramebufferHandle framebuffer, Renderer &renderer)
    {
        FramebufferInfo &info = renderer.render.framebuffers[framebuffer.handle - 1];
        if(texture_index < info.color_attachments.count)
        {
            return info.color_attachments.attachments[texture_index].texture;
        }
        
        assert(false);
        return { -1 };
    }

    static void add_color_attachment(ColorAttachmentType type, u64 flags, FramebufferInfo &info, u32 samples = 0)
    {
        ColorAttachment attachment;
        attachment.type = type;
        attachment.flags = flags;
        attachment.samples = samples;
        info.color_attachments.enabled = true;
        info.color_attachments.attachments[info.color_attachments.count++] = attachment;
    }

    static void add_depth_attachment(u64 flags, FramebufferInfo &info, u32 samples = 0)
    {
        info.depth_attachment.enabled = true;
        info.depth_attachment.flags = flags;
        info.depth_attachment.samples = samples;
    }

    static void set_clipping_plane(math::Vec4 plane, RenderPassHandle render_pass_handle, Renderer &renderer)
    {
        RenderPass &pass = renderer.render.passes[render_pass_handle.handle - 1];
        pass.clipping_planes.plane = plane;
    }

    static void set_camera_for_render_pass(Camera &camera, RenderPassHandle render_pass_handle, Renderer &renderer)
    {
        renderer.render.passes[render_pass_handle.handle - 1].camera = camera;
        renderer.render.passes[render_pass_handle.handle - 1].use_scene_camera = false;
    }
}
