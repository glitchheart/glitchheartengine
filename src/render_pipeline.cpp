namespace rendering
{
    static i32 _find_next_internal_handle(i32 start, i32 max, i32 *indices)
    {
        for(i32 i = start; i < max; i++)
        {
            if(indices[i] == -1)
                return i;
        }

        for(i32 i = 0; i < start; i++)
        {
            if(indices[i] == -1)
                return i;
        }
        
        // We hit max... Allocate more!
        assert(false);
        return -1;
    }


    static MaterialInstanceArrayHandle allocate_material_instance_array(Renderer *renderer, i32 size = 2056)
    {
        i32 internal_handle = _find_next_internal_handle(renderer->render.current_material_instance_array_index, MAX_MATERIAL_INSTANCE_ARRAYS, renderer->render._internal_material_instance_array_handles);
        if(internal_handle == -1)
        {
            internal_handle = _find_next_internal_handle(0, renderer->render.current_material_instance_array_index, renderer->render._internal_material_instance_array_handles);
        }

        assert(internal_handle != -1);

        // Set the currently used index for less overhead later when looking for the next free index
        renderer->render.current_material_instance_array_index = internal_handle + 1;
        if(renderer->render.current_material_instance_array_index == MAX_MATERIAL_INSTANCE_ARRAYS)
            renderer->render.current_material_instance_array_index = 0;
        
        i32 real_handle = renderer->render.material_instance_array_count++;
        renderer->render._internal_material_instance_array_handles[internal_handle] = real_handle;
        renderer->render.material_instance_arrays[real_handle] = (Material*)malloc(size * sizeof(Material));
        
        MaterialInstanceArrayHandle handle;
        handle.handle = internal_handle + 1;
        return handle;
    }

    static void free_material_instance_array(rendering::MaterialInstanceArrayHandle handle, Renderer *renderer)
    {
        i32 real_handle = renderer->render._internal_material_instance_array_handles[handle.handle - 1];

        free(renderer->render.material_instance_arrays[real_handle]);
        renderer->render.material_instance_arrays[real_handle] = nullptr;
        
        renderer->render._internal_material_instance_array_handles[handle.handle - 1] = -1;
        
        if(real_handle < renderer->render.material_instance_array_count - 1)
        {
            renderer->render.material_instance_arrays[real_handle] = renderer->render.material_instance_arrays[renderer->render.material_instance_array_count - 1];
            
            for(i32 i = 0; i < MAX_MATERIAL_INSTANCE_ARRAYS; i++)
            {
                if(renderer->render._internal_material_instance_array_handles[i] == renderer->render.material_instance_array_count - 1)
                {
                    renderer->render._internal_material_instance_array_handles[i] = real_handle;
                }
            }
        }
        
        renderer->render.material_instance_array_count--;
    }

    static i32 _find_and_update_next_free_buffer_index(i32 *current_index, b32 *free_buffers)
    {
        for(i32 i = *current_index; i < MAX_INSTANCE_BUFFERS; i++)
        {
            if(free_buffers[i])
            {
                *current_index = i;
                free_buffers[i] = false;
                return i;
            }
        }

        for(i32 i = 0; i < *current_index; i++)
        {            
            if(free_buffers[i])
            {
                *current_index = i;
                free_buffers[i] = false;
                return i;
            }
        }

        assert(false);
        
        return -1;
    }

    static InstanceBufferHandle allocate_instance_buffer(ValueType type, i32 instance_max, Renderer *renderer)
    {
        InstanceBufferHandle handle;
        handle.type = type;
        
        size_t type_size = 0;
        void **buf_ptr = nullptr;

        Buffer *buffer = nullptr;
        
        switch(type)
        {
        case ValueType::FLOAT:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float_buffers);
            type_size = sizeof(r32);
            buf_ptr = (void **)&renderer->render.instancing.float_buffers[index];
            buffer = renderer->render.instancing.internal_float_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::FLOAT2:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float2_buffers);
            type_size = sizeof(r32) * 2;
            buf_ptr = (void **)&renderer->render.instancing.float2_buffers[index];
            buffer = renderer->render.instancing.internal_float2_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::FLOAT3:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float3_buffers);
            type_size = sizeof(r32) * 3;
            buf_ptr = (void **)&renderer->render.instancing.float3_buffers[index];
            buffer = renderer->render.instancing.internal_float3_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::FLOAT4:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float4_buffers);
            type_size = sizeof(r32) * 4;
            buf_ptr = (void **)&renderer->render.instancing.float4_buffers[index];
            buffer = renderer->render.instancing.internal_float4_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::MAT4:
        { 
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_mat4_handle, renderer->render.instancing.free_mat4_buffers);
            type_size = sizeof(r32) * 16;
            buf_ptr = (void **)&renderer->render.instancing.mat4_buffers[index];
            buffer = renderer->render.instancing.internal_mat4_buffers[index];
            handle.handle = index + 1;
        }
        break;
        default:
            assert(false);
        }

        *buf_ptr = malloc(type_size * instance_max);
        renderer->api_functions.create_instance_buffer(buffer, type_size * instance_max, BufferUsage::DYNAMIC, renderer->api_functions.render_state, renderer);

        return handle;
    }
    
    static r32* get_float_buffer_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT);
        return renderer->render.instancing.float_buffers[buffer_handle.handle - 1];
    }

    static i32* get_float_buffer_count_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT);
        return &renderer->render.instancing.float_buffer_counts[buffer_handle.handle - 1];
    }

    static math::Vec2* get_float2_buffer_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT2);
        return renderer->render.instancing.float2_buffers[buffer_handle.handle - 1];
    }

    static i32* get_float2_buffer_count_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT2);
        return &renderer->render.instancing.float2_buffer_counts[buffer_handle.handle - 1];
    }

    static math::Vec3* get_float3_buffer_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT3);
        return renderer->render.instancing.float3_buffers[buffer_handle.handle - 1];
    }

    static i32* get_float3_buffer_count_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT3);
        return &renderer->render.instancing.float3_buffer_counts[buffer_handle.handle - 1];
    }

    static math::Vec4* get_float4_buffer_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT4);
        return renderer->render.instancing.float4_buffers[buffer_handle.handle - 1];
    }

    static i32* get_float4_buffer_count_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT4);
        return &renderer->render.instancing.float4_buffer_counts[buffer_handle.handle - 1];
    }

    static math::Mat4* get_mat4_buffer_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::MAT4);
        return renderer->render.instancing.mat4_buffers[buffer_handle.handle - 1];
    }

    static i32* get_mat4_buffer_count_pointer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::MAT4);
        return &renderer->render.instancing.mat4_buffer_counts[buffer_handle.handle - 1];
    }
    
    static void free_instance_buffer(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        i32 index = buffer_handle.handle - 1;
        Buffer *buffer = nullptr;
        
        switch(buffer_handle.type)
        {
        case ValueType::FLOAT:
        {
            buffer = renderer->render.instancing.internal_float_buffers[index];
            renderer->render.instancing.float_buffer_counts[index] = 0;
            renderer->render.instancing.free_float_buffers[index] = true;
        }
        break;
        case ValueType::FLOAT2:
        {
            buffer = renderer->render.instancing.internal_float2_buffers[index];
            renderer->render.instancing.float2_buffer_counts[index] = 0;
            renderer->render.instancing.free_float2_buffers[index] = true;
        }
        break;
        case ValueType::FLOAT3:
        {
            buffer = renderer->render.instancing.internal_float3_buffers[index];
            renderer->render.instancing.float3_buffer_counts[index] = 0;
            renderer->render.instancing.free_float3_buffers[index] = true;
        }
        break;
        case ValueType::FLOAT4:
        {
            buffer = renderer->render.instancing.internal_float4_buffers[index];
            renderer->render.instancing.float4_buffer_counts[index] = 0;
            renderer->render.instancing.free_float4_buffers[index] = true;
        }
        break;
        case ValueType::MAT4:
        {
            buffer = renderer->render.instancing.internal_mat4_buffers[index];
            renderer->render.instancing.mat4_buffer_counts[index] = 0;
            renderer->render.instancing.free_mat4_buffers[index] = true;
        }
        break;
        default:
        break;
        }

        renderer->api_functions.delete_instance_buffer(buffer, renderer->api_functions.render_state, renderer);
    }

    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, r32 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float_buffer_counts[index];
        renderer->render.instancing.float_buffers[index][(*count)++] = value;
    }

    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Vec2 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT2);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float2_buffer_counts[index];
        renderer->render.instancing.float2_buffers[index][(*count)++] = value;
    }
    
    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Vec3 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT3);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float3_buffer_counts[index];
        renderer->render.instancing.float3_buffers[index][(*count)++] = value;
    }
    
    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Vec4 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT4);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float4_buffer_counts[index];
        renderer->render.instancing.float4_buffers[index][(*count)++] = value;
    }

    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Mat4 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::MAT4);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.mat4_buffer_counts[index];
        renderer->render.instancing.mat4_buffers[index][(*count)++] = value;
    }
    
    // @Note: Creates a RenderPass with the specified FramebufferHandle
    static RenderPassHandle create_render_pass(const char *name, FramebufferHandle framebuffer, Renderer *renderer)
    {
        RenderPass &pass = renderer->render.passes[renderer->render.pass_count];
        pass.framebuffer = framebuffer;
        pass.use_scene_camera = true;
        pass.clipping_planes.type = ClippingPlaneType::NONE;
        pass.clipping_planes.plane = math::Vec4(0, 0, 0, 0);
        
        strncpy(pass.name, name, strlen(name) + 1);
        pass.commands.render_commands = push_array(&renderer->render.render_pass_arena, global_max_render_commands, RenderCommand);
        
        return { (renderer->render.pass_count++) + 1};
    }

    static void create_ui_render_pass(Renderer *renderer)
    {
        RenderPass& pass = renderer->render.ui.pass;
        pass = {};
        pass.framebuffer = renderer->render.final_framebuffer;
        pass.use_scene_camera = false;

        Camera camera = {};
        camera.zoom = 1.0f;
        camera.position = math::Vec3(0.0f);
        camera.orientation = math::Quat();
        camera.target = math::Vec3();
        camera.view_matrix = math::Mat4();

        camera.projection_matrix = math::ortho(0.0f, (r32)renderer->framebuffer_width, 0.0f,
                                               (r32)renderer->framebuffer_height, -500.0f, 500.0f);

        pass.camera = camera;
        strncpy(pass.name, "UI Pass", strlen("UI Pass") + 1);
        pass.ui.render_commands = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, UIRenderCommand);
        pass.ui.render_command_count = 0;
        pass.ui.transparent_commands = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, UIRenderCommand);
        pass.ui.transparent_command_count = 0;
        pass.ui.coords = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, CharacterData*);

        for(i32 i = 0; i < global_max_ui_commands; i++)
        {
            pass.ui.coords[i] = push_array(&renderer->render.render_pass_arena, 4096, CharacterData);
        }

        pass.ui.text_z_layers = push_array(&renderer->render.render_pass_arena, Z_LAYERS, i32*);
        for(i32 i = 0; i < Z_LAYERS; i++)
        {
            pass.ui.text_z_layers[i] = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, i32);
        }
        
        pass.ui.text_z_layer_counts = push_array(&renderer->render.render_pass_arena, Z_LAYERS, i32);

        pass.ui.ui_z_layers = push_array(&renderer->render.render_pass_arena, Z_LAYERS, i32*);
        for(i32 i = 0; i < Z_LAYERS; i++)
        {
            pass.ui.ui_z_layers[i] = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, i32);
        }
        
        pass.ui.ui_z_layer_counts = push_array(&renderer->render.render_pass_arena, Z_LAYERS, i32);

        
        pass.ui.text_commands = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, TextRenderCommand);
        pass.ui.text_command_count = 0;
    }

    static void set_uniforms_from_shader(Renderer *renderer, RenderPass& pass, ShaderHandle shader_handle)
    {
        Shader& shader = renderer->render.shaders[shader_handle.handle];

        for(i32 i = 0; i < shader.uniform_count; i++)
        {
            UniformValue& value = pass.post_processing.uniform_values[pass.post_processing.uniform_value_count++];
            value.uniform = shader.uniforms[i];
            value.uniform.location_index = i;
            strncpy(value.name, shader.uniforms[i].name, strlen(shader.uniforms[i].name) + 1);
        }
    }

    static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, r32 value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT);
                u_v.float_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, math::Vec2 value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT2);
                u_v.float2_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, math::Vec3 value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT3);
                u_v.float3_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, math::Vec4 value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT4);
                u_v.float4_val = value;
                break;
            }
        }
	}

    static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, TextureHandle value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::TEXTURE);
                assert(value.handle != 0);
                u_v.texture = value;
                break;
            }
        }
	}

    static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, MSTextureHandle value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::MS_TEXTURE);
                u_v.ms_texture = value;
                break;
            }
        }
	}


	static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, i32 value)
	{
        
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::INTEGER || u_v.uniform.type == ValueType::BOOL);
                if(u_v.uniform.type == ValueType::BOOL)
                {
                    u_v.boolean_val = value;
                    break;
                }
                else if(u_v.uniform.type == ValueType::INTEGER)
                {
                    u_v.integer_val = value;
                    break;
                }
            }
        }
	}

	static void set_uniform_value(Renderer *renderer, PostProcessingRenderPassHandle handle, const char* name, math::Mat4 value)
	{
        RenderPass &pass = renderer->render.post_processing_passes[handle.handle - 1];
        
		for(i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            UniformValue& u_v = pass.post_processing.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::MAT4);
                u_v.mat4_val = value;
                break;
            }
        }
	}

    static PostProcessingRenderPassHandle create_post_processing_render_pass(const char *name, FramebufferHandle framebuffer, Renderer *renderer, ShaderHandle shader)
    {
        RenderPass &pass = renderer->render.post_processing_passes[renderer->render.post_processing_pass_count];
        

        pass.framebuffer = framebuffer;
        pass.use_scene_camera = false;
        pass.clipping_planes.type = ClippingPlaneType::NONE;
        pass.clipping_planes.plane = math::Vec4(0, 0, 0, 0);

        pass.post_processing.shader_handle = shader;

        set_uniforms_from_shader(renderer, pass, shader);
        
        strncpy(pass.name, name, strlen(name) + 1);
//        pass.commands.render_commands = push_array(&renderer->render.render_pass_arena, global_max_render_commands, RenderCommand);
        return { (renderer->render.post_processing_pass_count++) + 1 };
    }

    // @Note: Generates and returns a FramebufferInfo with default values
    static FramebufferInfo generate_framebuffer_info()
    {
        FramebufferInfo info = {};
        info.color_attachments.enabled = false;
        info.color_attachments.count = 0;
        info.depth_attachment.enabled = false;
        info.depth_attachment.flags = 0;
        info.size_ratio = 1;

        return info;
    }

    // @Note: creates a framebuffer that can be used with a render pass
    // The handle returned maps directly to the graphics API specific framebuffer index
    static FramebufferHandle create_framebuffer(FramebufferInfo &info, Renderer *renderer, char* name = "")
    {
        FramebufferHandle handle = { (renderer->render.framebuffer_count++) + 1 };
        strncpy(info.name, name, strlen(name) + 1);
        renderer->render.framebuffers[handle.handle - 1] = info;

        assert(renderer->api_functions.create_framebuffer);
        renderer->api_functions.create_framebuffer(renderer->render.framebuffers[handle.handle - 1], renderer->api_functions.render_state, renderer);
        
        return handle;
    }

    static RenderPassHandle get_render_pass_handle_for_name(const char *name, Renderer *renderer)
    {
        for(i32 i = 0; i < renderer->render.pass_count; i++)
        {
            if(strcmp(renderer->render.passes[i].name, name) == 0)
            {
                return { i + 1 };
            }
        }

        return { -1 };
    }

    static TextureHandle get_texture_from_framebuffer(i32 texture_index, FramebufferHandle framebuffer, Renderer *renderer)
    {
        FramebufferInfo &info = renderer->render.framebuffers[framebuffer.handle - 1];
        if(texture_index < info.color_attachments.count)
        {
            assert((info.color_attachments.attachments[texture_index].flags & ColorAttachmentFlags::MULTISAMPLED) == 0);
            assert(info.color_attachments.attachments[texture_index].texture.handle != 0);
            return info.color_attachments.attachments[texture_index].texture;
        }
        
        assert(false);
        return { -1 };
    }

    static MSTextureHandle get_ms_texture_from_framebuffer(i32 texture_index, FramebufferHandle framebuffer, Renderer *renderer)
    {
        FramebufferInfo &info = renderer->render.framebuffers[framebuffer.handle - 1];
        if(texture_index < info.color_attachments.count)
        {
            assert(info.color_attachments.attachments[texture_index].flags & ColorAttachmentFlags::MULTISAMPLED);
            assert(info.color_attachments.attachments[texture_index].ms_texture.handle != 0);
            return info.color_attachments.attachments[texture_index].ms_texture;
        }
        
        assert(false);
        return { -1 };
    }
    
    static void add_color_attachment(ColorAttachmentType type, u64 flags, FramebufferInfo &info, u32 samples = 0)
    {
        ColorAttachment attachment;
        attachment.type = type;
        attachment.flags = flags;

        if(flags & ColorAttachmentFlags::MULTISAMPLED)
        {
            assert(samples > 0);
        }
        
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

    static void set_clipping_plane(math::Vec4 plane, RenderPassHandle render_pass_handle, Renderer *renderer)
    {
        RenderPass &pass = renderer->render.passes[render_pass_handle.handle - 1];
        pass.clipping_planes.plane = plane;
    }

    static void set_camera_for_render_pass(Camera &camera, RenderPassHandle render_pass_handle, Renderer *renderer)
    {
        renderer->render.passes[render_pass_handle.handle - 1].camera = camera;
        renderer->render.passes[render_pass_handle.handle - 1].use_scene_camera = false;
    }

    static math::Vec2i get_texture_size(rendering::TextureHandle handle, Renderer *renderer)
    {
        Texture* texture = renderer->render.textures[handle.handle - 1];
        return renderer->api_functions.get_texture_size(texture);
    }
}
