namespace rendering
{
	static void push_line_to_render_pass(Renderer *renderer, math::Vec3 p0, math::Vec3 p1, r32 thickness, math::Rgba color, Transform transform, MaterialInstanceHandle material_instance_handle, RenderPassHandle render_pass_handle, CommandType type = CommandType::WITH_DEPTH);
    static Transform create_transform(Transform t);
    static Transform create_transform(math::Vec3 position, math::Vec3 scale, math::Vec3 rotation);
    
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

    static void set_shader_values(Material &material, Shader &shader, Renderer *renderer)
    {
        material.array_count = 0;
        memcpy(material.instanced_vertex_attributes, shader.instanced_vertex_attributes, sizeof(VertexAttributeInstanced) * shader.instanced_vertex_attribute_count);
        material.instanced_vertex_attribute_count = shader.instanced_vertex_attribute_count;

        UniformValue *uniform_vals = nullptr;
        UniformArray *arrays = nullptr;

        i32 location_index = 0;

        // @Incomplete: Get shader uniforms
        for (i32 uni_i = 0; uni_i < shader.uniform_count; uni_i++)
        {
            Uniform u = shader.uniforms[uni_i];

            // If we're working with an array type we should add all the possible uniforms for the whole array
            if (u.is_array)
            {
                // First we push the array placeholder inside the uniforms array
                UniformValue value = {};
                value.uniform = u;
                value.array_index = material.array_count++;
                strncpy(value.name, u.name, strlen(u.name) + 1);
                buf_push(uniform_vals, value);

                UniformArray array = {};
                strncpy(array.name, u.name, strlen(u.name) + 1);
                array.entry_count = 0;
                array.max_size = u.array_size;

                if (u.type == ValueType::STRUCTURE)
                {
                    Structure &structure = shader.structures[u.structure_index];

                    i32 num_to_allocate = u.array_size;
                    array.entries = push_array(&renderer->mesh_arena, num_to_allocate, UniformEntry);

                    if (u.mapping_type == UniformMappingType::DIRECTIONAL_LIGHTS || u.mapping_type == UniformMappingType::POINT_LIGHTS)
                    {
                        material.lighting.receives_light = true;
                    }

                    for (i32 i = 0; i < u.array_size; i++)
                    {
                        UniformEntry entry = {};
                        entry.value_count = 0;

                        for (i32 j = 0; j < structure.uniform_count; j++)
                        {
                            Uniform struct_uni = structure.uniforms[j];
                            Uniform new_uniform = struct_uni;
                            new_uniform.location_index = location_index++;
                            UniformValue u_v = {};

                            strncpy(u_v.name, struct_uni.name, strlen(struct_uni.name) + 1);
                            sprintf(new_uniform.name, "%s[%d].%s", u.name, i, struct_uni.name);

                            u_v.uniform = new_uniform;
                            entry.values[entry.value_count++] = u_v;
                        }
                        
                        array.entries[array.entry_count++] = entry;
                    }
                }
                else
                {
                    i32 num_to_allocate = u.array_size;
                    array.entries = push_array(&renderer->mesh_arena, num_to_allocate, UniformEntry);

                    for (i32 i = 0; i < u.array_size; i++)
                    {
                        UniformEntry entry = {};
                        entry.value_count = 0;

                        UniformValue u_v = {};
                        Uniform new_uniform = u;
                        new_uniform.location_index = location_index++;
                        sprintf(new_uniform.name, "%s[%d]", u.name, i);
                        u_v.uniform = new_uniform;
                        entry.values[entry.value_count++] = u_v;
                        array.entries[array.entry_count++] = entry;
                    }
                }

                buf_push(arrays, array);
            }
            else
            {
                UniformValue u_v = {};
                strncpy(u_v.name, u.name, strlen(u.name) + 1);
                u_v.uniform = u;
                u_v.uniform.location_index = location_index++;
                buf_push(uniform_vals, u_v);
            }
        }

        size_t size = buf_len(uniform_vals);
        size_t array_size = buf_len(arrays);

        material.uniform_value_count = (i32)size;
        memcpy(material.uniform_values, uniform_vals, size * sizeof(UniformValue));
        material.array_count = (i32)array_size;
        memcpy(material.arrays, arrays, array_size * sizeof(UniformValue));

        buf_free(arrays);
        buf_free(uniform_vals);
    }

    static MaterialHandle copy_material(Renderer *renderer, MaterialHandle material_handle)
    {
        renderer->render.materials[renderer->render.material_count] = renderer->render.materials[material_handle.handle];
        return { renderer->render.material_count++ };
    }

    static MaterialHandle create_material(Renderer *renderer, const Material &to_copy)
    {
        renderer->render.materials[renderer->render.material_count] = to_copy;
        return {renderer->render.material_count++};
    }
    
    static MaterialHandle create_material(Renderer *renderer, ShaderHandle shader_handle)
    {
        Material &material = renderer->render.materials[renderer->render.material_count];
        material.shader = shader_handle;

        Shader &shader = renderer->render.shaders[shader_handle.handle];

        set_shader_values(material, shader, renderer);
        
        return {renderer->render.material_count++};
    }

    static Material create_material_copyable(Renderer *renderer, ShaderHandle shader_handle)
    {
        Material material;
        material.shader = shader_handle;

        Shader &shader = renderer->render.shaders[shader_handle.handle];
        
        set_shader_values(material, shader, renderer);
        return material;
    }
    
    static MaterialInstanceHandle create_material_instance(Renderer *renderer, MaterialHandle material_handle)
    {
        Material &material = renderer->render.materials[material_handle.handle];
        
        i32 real_handle = renderer->render.material_instance_count++;
        i32 internal_index = _find_next_internal_handle(renderer->render.current_material_instance_index, global_max_material_instances, renderer->render._internal_material_instance_handles);

        if(internal_index == -1)
            internal_index = _find_next_internal_handle(0, renderer->render.current_material_instance_index, renderer->render._internal_material_instance_handles);

        assert(internal_index != -1);

        // Update the internal handle array
        renderer->render._internal_material_instance_handles[internal_index] = real_handle;
        
        renderer->render.material_instances[real_handle] = material;
        renderer->render.material_instances[real_handle].source_material = material_handle;

        return { internal_index + 1 };
    }

    static void delete_material_instance(Renderer *renderer, MaterialInstanceHandle material_handle)
    {
        i32 real_handle = renderer->render._internal_material_instance_handles[material_handle.handle - 1];
        renderer->render._internal_material_instance_handles[material_handle.handle - 1] = -1;

        if(renderer->render.material_instance_count > 1)
        {
            renderer->render.material_instances[real_handle] = renderer->render.material_instances[renderer->render.material_instance_count - 1];

            for(i32 i = 0; i < global_max_material_instances; i++)
            {
                if(renderer->render._internal_material_instance_handles[i] == renderer->render.material_instance_count - 1)
                {
                    renderer->render._internal_material_instance_handles[i] = real_handle;
                }
            }
        }
        
        renderer->render.material_instance_count--;
    }
    
    static void set_fallback_shader(Renderer *renderer, const char *path)
    {
        renderer->render.fallback_shader = load_shader(renderer, path);
    }

    static void set_shadow_map_shader(Renderer *renderer, const char *path)
    {
        renderer->render.shadow_map_shader = load_shader(renderer, path);
        renderer->render.shadow_map_material = rendering::create_material(renderer, renderer->render.shadow_map_shader);
    }

    static void set_wireframe_shader(Renderer *renderer, const char *path)
    {
        renderer->render.wireframe_shader = load_shader(renderer, path);
        MaterialHandle material = rendering::create_material(renderer, renderer->render.wireframe_shader);
        renderer->render.wireframe_material = create_material_instance(renderer, material);
    }

    static void set_bounding_box_shader(Renderer *renderer, const char *path)
    {
        renderer->render.bounding_box_shader = load_shader(renderer, path);
        MaterialHandle material = rendering::create_material(renderer, renderer->render.bounding_box_shader);
        renderer->render.bounding_box_material = create_material_instance(renderer, material);
    }

    static void set_debug_line_shader(Renderer *renderer, const char *path)
    {
        renderer->render.line_shader = load_shader(renderer, path);
        MaterialHandle material = create_material(renderer, renderer->render.line_shader);
        renderer->render.line_material = create_material_instance(renderer, material);
    }

    // Reallocates the buffer with the new max count and copies over the old data
    static void realloc_instance_buffer(InstanceBufferHandle handle, i32 new_instance_max, Renderer *renderer)
    {
        i32 *max = nullptr;
        size_t type_size = 0;
        Buffer *buffer = nullptr;
        
        switch(handle.type)
        {
        case ValueType::FLOAT:
        {
            r32 *new_ptr = (r32*)malloc(sizeof(r32) * new_instance_max);
            memcpy(new_ptr, renderer->render.instancing.float_buffers[handle.handle - 1], renderer->render.instancing.float_buffer_counts[handle.handle - 1] * sizeof(r32));
            free(renderer->render.instancing.float_buffers[handle.handle - 1]);
            max = &renderer->render.instancing.float_buffer_max[handle.handle - 1];
            type_size = sizeof(r32);
            renderer->render.instancing.float_buffers[handle.handle - 1] = new_ptr;
            buffer = renderer->render.instancing.internal_float_buffers[handle.handle - 1];
        }
        break;
        case ValueType::FLOAT2:
        {
            math::Vec2 *new_ptr = (math::Vec2*)malloc(sizeof(math::Vec2) * new_instance_max);
            memcpy(new_ptr, renderer->render.instancing.float2_buffers[handle.handle - 1], renderer->render.instancing.float2_buffer_counts[handle.handle - 1] * sizeof(math::Vec2));
            free(renderer->render.instancing.float2_buffers[handle.handle - 1]);
            max = &renderer->render.instancing.float2_buffer_max[handle.handle - 1];
            type_size = sizeof(math::Vec2);
            renderer->render.instancing.float2_buffers[handle.handle - 1] = new_ptr;
            buffer = renderer->render.instancing.internal_float2_buffers[handle.handle - 1];
        }
        break;
        case ValueType::FLOAT3:
        {
            math::Vec3 *new_ptr = (math::Vec3*)malloc(sizeof(math::Vec3) * new_instance_max);
            memcpy(new_ptr, renderer->render.instancing.float3_buffers[handle.handle - 1], renderer->render.instancing.float3_buffer_counts[handle.handle - 1] * sizeof(math::Vec3));
            free(renderer->render.instancing.float3_buffers[handle.handle - 1]);
            max = &renderer->render.instancing.float3_buffer_max[handle.handle - 1];
            type_size = sizeof(math::Vec3);
            renderer->render.instancing.float3_buffers[handle.handle - 1] = new_ptr;
            buffer = renderer->render.instancing.internal_float3_buffers[handle.handle - 1];
        }
        break;
        case ValueType::FLOAT4:
        {
            math::Vec4 *new_ptr = (math::Vec4*)malloc(sizeof(math::Vec4) * new_instance_max);
            memcpy(new_ptr, renderer->render.instancing.float4_buffers[handle.handle - 1], renderer->render.instancing.float4_buffer_counts[handle.handle - 1] * sizeof(math::Vec4));
            free(renderer->render.instancing.float4_buffers[handle.handle - 1]);
            max = &renderer->render.instancing.float4_buffer_max[handle.handle - 1];
            type_size = sizeof(math::Vec4);
            renderer->render.instancing.float4_buffers[handle.handle - 1] = new_ptr;
            buffer = renderer->render.instancing.internal_float4_buffers[handle.handle - 1];
        }
        break;
        case ValueType::MAT4:
        {
            math::Mat4 *new_ptr = (math::Mat4*)malloc(sizeof(math::Mat4) * new_instance_max);
            memcpy(new_ptr, renderer->render.instancing.mat4_buffers[handle.handle - 1], renderer->render.instancing.mat4_buffer_counts[handle.handle - 1] * sizeof(math::Mat4));
            free(renderer->render.instancing.mat4_buffers[handle.handle - 1]);
            max = &renderer->render.instancing.mat4_buffer_max[handle.handle - 1];
            type_size = sizeof(math::Mat4);
            renderer->render.instancing.mat4_buffers[handle.handle - 1] = new_ptr;
            buffer = renderer->render.instancing.internal_mat4_buffers[handle.handle - 1];
        }
        break;
        default:
        assert(false);
        break;
        }
        
		rendering::BufferUsage usage = renderer->api_functions.get_buffer_usage(buffer);

		renderer->api_functions.delete_instance_buffer(buffer, renderer->api_functions.render_state, renderer);

        renderer->api_functions.create_instance_buffer(buffer, type_size * new_instance_max, usage, renderer->api_functions.render_state, renderer);
        
        *max = new_instance_max;
    }

    // Allocates a new instance buffer with the specified max count
    static InstanceBufferHandle allocate_instance_buffer(ValueType type, i32 instance_max, BufferUsage usage, Renderer *renderer)
    {
        InstanceBufferHandle handle;
        handle.type = type;
        
        size_t type_size = 0;
        void **buf_ptr = nullptr;

        Buffer *buffer = nullptr;
        i32 *max = nullptr;
        
        switch(type)
        {
        case ValueType::FLOAT:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float_buffers);
            type_size = sizeof(r32);
            buf_ptr = (void **)&renderer->render.instancing.float_buffers[index];
            max = &renderer->render.instancing.float_buffer_max[index];
            buffer = renderer->render.instancing.internal_float_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::FLOAT2:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float2_buffers);
            type_size = sizeof(r32) * 2;
            buf_ptr = (void **)&renderer->render.instancing.float2_buffers[index];
            max = &renderer->render.instancing.float2_buffer_max[index];
            buffer = renderer->render.instancing.internal_float2_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::FLOAT3:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float3_buffers);
            type_size = sizeof(r32) * 3;
            buf_ptr = (void **)&renderer->render.instancing.float3_buffers[index];
            max = &renderer->render.instancing.float3_buffer_max[index];
            buffer = renderer->render.instancing.internal_float3_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::FLOAT4:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_float_handle, renderer->render.instancing.free_float4_buffers);
            type_size = sizeof(r32) * 4;
            buf_ptr = (void **)&renderer->render.instancing.float4_buffers[index];
            max = &renderer->render.instancing.float4_buffer_max[index];
            buffer = renderer->render.instancing.internal_float4_buffers[index];
            handle.handle = index + 1;
        }
        break;
        case ValueType::MAT4:
        {
            i32 index = _find_and_update_next_free_buffer_index(&renderer->render.instancing.current_internal_mat4_handle, renderer->render.instancing.free_mat4_buffers);
            type_size = sizeof(r32) * 16;
            buf_ptr = (void **)&renderer->render.instancing.mat4_buffers[index];
            max = &renderer->render.instancing.mat4_buffer_max[index];
            buffer = renderer->render.instancing.internal_mat4_buffers[index];
            handle.handle = index + 1;
        }
        break;
        default:
            assert(false);
        }
        
        *max = instance_max;
        *buf_ptr = malloc(type_size * instance_max);
        renderer->api_functions.create_instance_buffer(buffer, type_size * instance_max, usage, renderer->api_functions.render_state, renderer);

        return handle;
    }
    
    static i32 get_instance_buffer_max(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        switch(buffer_handle.type)
        {
        case ValueType::FLOAT:
        return renderer->render.instancing.float_buffer_max[buffer_handle.handle - 1];
        case ValueType::FLOAT2:
        return renderer->render.instancing.float2_buffer_max[buffer_handle.handle - 1];
        case ValueType::FLOAT3:
        return renderer->render.instancing.float3_buffer_max[buffer_handle.handle - 1];
        case ValueType::FLOAT4:
        return renderer->render.instancing.float4_buffer_max[buffer_handle.handle - 1];
        case ValueType::MAT4:
        return renderer->render.instancing.mat4_buffer_max[buffer_handle.handle - 1];
        default:
        assert(false);
        break;
        }
        return -1;
    }

    static i32 get_instance_buffer_count(InstanceBufferHandle buffer_handle, Renderer *renderer)
    {
        switch(buffer_handle.type)
        {
        case ValueType::FLOAT:
        return renderer->render.instancing.float_buffer_counts[buffer_handle.handle - 1];
        case ValueType::FLOAT2:
        return renderer->render.instancing.float2_buffer_counts[buffer_handle.handle - 1];
        case ValueType::FLOAT3:
        return renderer->render.instancing.float3_buffer_counts[buffer_handle.handle - 1];
        case ValueType::FLOAT4:
        return renderer->render.instancing.float4_buffer_counts[buffer_handle.handle - 1];
        case ValueType::MAT4:
        return renderer->render.instancing.mat4_buffer_counts[buffer_handle.handle - 1];
        default:
        assert(false);
        break;
        }
        
        return -1;
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
        assert(*count < renderer->render.instancing.float_buffer_max[index]);
        renderer->render.instancing.float_buffers[index][(*count)++] = value;
        renderer->render.instancing.dirty_float_buffers[index] = true;
    }

    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Vec2 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT2);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float2_buffer_counts[index];
        assert(*count < renderer->render.instancing.float2_buffer_max[index]);
        renderer->render.instancing.float2_buffers[index][(*count)++] = value;
        renderer->render.instancing.dirty_float2_buffers[index] = true;
    }
    
    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Vec3 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT3);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float3_buffer_counts[index];
        assert(*count < renderer->render.instancing.float3_buffer_max[index]);
        renderer->render.instancing.float3_buffers[index][(*count)++] = value;
        renderer->render.instancing.dirty_float3_buffers[index] = true;
    }
    
    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Vec4 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::FLOAT4);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.float4_buffer_counts[index];
        assert(*count < renderer->render.instancing.float4_buffer_max[index]);
        renderer->render.instancing.float4_buffers[index][(*count)++] = value;
        renderer->render.instancing.dirty_float4_buffers[index] = true;
    }

    static void add_instance_buffer_value(InstanceBufferHandle buffer_handle, math::Mat4 value, Renderer *renderer)
    {
        assert(buffer_handle.type == ValueType::MAT4);
        i32 index = buffer_handle.handle - 1;
        i32 *count = &renderer->render.instancing.mat4_buffer_counts[index];
        assert(*count < renderer->render.instancing.mat4_buffer_max[index]);
        renderer->render.instancing.mat4_buffers[index][(*count)++] = value;
        renderer->render.instancing.dirty_mat4_buffers[index] = true;
    }
    
    // @Note: Creates a RenderPass with the specified FramebufferHandle
    static RenderPassHandle create_render_pass(const char *name, FramebufferHandle framebuffer, Renderer *renderer, u64 settings = 0)
    {
        assert(renderer->render.pass_count < global_max_render_commands);
        RenderPass &pass = renderer->render.passes[renderer->render.pass_count];
        pass.has_clear_color = false;
        pass.type = RenderPassType::NORMAL;
        pass.framebuffer = framebuffer;
        pass.use_scene_camera = true;
        pass.clipping_planes.type = ClippingPlaneType::NONE;
        pass.clipping_planes.plane = math::Vec4(0, 0, 0, 0);
        pass.settings = settings;
        
        strncpy(pass.name, name, strlen(name) + 1);
        pass.commands.render_commands = push_array(&renderer->render.render_pass_arena, global_max_render_commands, RenderCommand);
        pass.commands.depth_free_commands = push_array(&renderer->render.render_pass_arena, global_max_depth_free_commands, RenderCommand);
        
        return { (renderer->render.pass_count++) + 1};
    }

    static void set_read_draw_render_passes(RenderPassHandle read_from_pass, RenderPassHandle draw_to_pass, Renderer *renderer)
    {
        RenderPass &read = renderer->render.passes[read_from_pass.handle - 1];
        RenderPass &draw = renderer->render.passes[draw_to_pass.handle - 1];
        draw.type = RenderPassType::READ_DRAW;
        draw.read_framebuffer = read.framebuffer;
    }

    static void set_render_pass_clear_color(RenderPassHandle handle, math::Rgba clear_color, Renderer *renderer)
    {
        RenderPass &pass = renderer->render.passes[handle.handle - 1];
        pass.has_clear_color = true;
        pass.clear_color = clear_color;
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
        camera.target = math::Vec3();
        camera.view_matrix = math::Mat4();

        camera.projection_matrix = math::ortho(0.0f, (r32)renderer->framebuffer_width, 0.0f,
                                               (r32)renderer->framebuffer_height, -500.0f, 500.0f);

        pass.camera = camera;
        strncpy(pass.name, "UI Pass", strlen("UI Pass") + 1);
        pass.ui.render_commands = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, UIRenderCommand);
        pass.ui.render_command_count = 0;
        // pass.ui.transparent_commands = push_array(&renderer->render.render_pass_arena, global_max_ui_commands, UIRenderCommand);
        // pass.ui.transparent_command_count = 0;
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
        info.depth_attachments.enabled = false;
        info.depth_attachments.count = 0;
        info.size_ratio = 1;

        return info;
    }

    // @Note: creates a framebuffer that can be used with a render pass
    // The handle returned maps directly to the graphics API specific framebuffer index
    static FramebufferHandle create_framebuffer(FramebufferInfo &info, Renderer *renderer, char* name = "")
    {
        assert(renderer->render.framebuffer_count < global_max_framebuffers);
        FramebufferHandle handle = { (renderer->render.framebuffer_count++) + 1 };
        strncpy(info.name, name, strlen(name) + 1);
        renderer->render.framebuffers[handle.handle - 1] = info;

        assert(renderer->api_functions.create_framebuffer);
        renderer->api_functions.create_framebuffer(renderer->render.framebuffers[handle.handle - 1], renderer->api_functions.render_state, renderer);
        
        return handle;
    }

    static void reload_framebuffer(FramebufferHandle handle, i32 width, i32 height, Renderer* renderer)
    {
        renderer->api_functions.reload_framebuffer(handle, renderer->api_functions.render_state, renderer, width, height);
    }

    static FramebufferHandle get_read_framebuffer_from_pass(RenderPassHandle pass_handle, Renderer *renderer)
    {
        assert(pass_handle.handle > 0);
        RenderPass& pass = renderer->render.passes[pass_handle.handle - 1];
        return pass.read_framebuffer;
    }

    static FramebufferHandle get_write_framebuffer_from_pass(RenderPassHandle pass_handle, Renderer *renderer)
    {
        assert(pass_handle.handle > 0);
        RenderPass& pass = renderer->render.passes[pass_handle.handle - 1];
        return pass.framebuffer;
    }

    static FramebufferInfo& get_framebuffer(FramebufferHandle handle, Renderer* renderer)
    {
        assert(handle.handle > 0);
        return renderer->render.framebuffers[handle.handle - 1];
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

    static TextureHandle get_depth_texture_from_framebuffer(i32 index, FramebufferHandle framebuffer, Renderer *renderer)
    {
        FramebufferInfo &info = renderer->render.framebuffers[framebuffer.handle - 1];
        return info.depth_attachments.attachments[index].texture;
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
    
    static void add_color_attachment(AttachmentType type, u64 flags, FramebufferInfo &info, u32 samples = 0)
    {
        ColorAttachment attachment = {};
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

    static void add_depth_attachment(AttachmentType type, u64 flags, FramebufferInfo &info, u32 samples = 0)
    {
        DepthAttachment attachment = {};
        attachment.type = type;
        attachment.flags = flags;
        
        
        if(flags & DepthAttachmentFlags::DEPTH_MULTISAMPLED)
        {
            assert(samples > 0);
        }

        info.depth_attachments.enabled = true;
        attachment.samples = samples;
        info.depth_attachments.attachments[info.depth_attachments.count++] = attachment;
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

    static void set_bloom_shader(Renderer *renderer, const char *path)
    {
        renderer->render.bloom_shader = load_shader(renderer, path);
    }

    static void set_blur_shader(Renderer *renderer, const char *path)
    {
        renderer->render.blur_shader = load_shader(renderer, path);
    }

    static void set_hdr_shader(Renderer *renderer, const char *path)
    {
        renderer->render.hdr_shader = load_shader(renderer, path);
    }

    static void set_final_framebuffer(Renderer *renderer, FramebufferHandle framebuffer)
    {
        renderer->render.final_framebuffer = framebuffer;
    }

    static void set_light_space_matrices(Renderer *renderer, math::Mat4 projection_matrix, math::Mat4 view_matrix)
    {
        renderer->render.shadow_view_position = math::translation(view_matrix);
        renderer->render.light_space_matrix = projection_matrix * view_matrix;
        renderer->render.light_view_matrix = view_matrix;
    }

    static math::Vec3 projection_to_view_space(math::Mat4 p, math::Vec3 position)
    {
        r32 s_x = p.m11;
        r32 s_y = p.m22;
        r32 s_z = p.m33;
        r32 t_z = p.m34;

        r32 z = t_z / (position.z - s_z);
        r32 x = (position.x * z) / s_x;
        r32 y = (position.y * z) / s_y;

        return math::Vec3(x, y, z);
    }

    static void calculate_light_space_matrices(Renderer *renderer, Camera camera, math::Vec3 direction)
    {
        direction = math::normalize(direction);
        math::Vec3 right = math::normalize(math::cross(math::Vec3(0, 1, 0), direction));
        math::Vec3 up = math::normalize(math::cross(direction, right));
        
        math::Mat4 light_view_matrix = math::Mat4(math::Vec4(right, 0.0f), math::Vec4(up, 0.0f),
                                                  math::Vec4(direction, 0.0f), math::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

        r32 z_near = renderer->render.shadow_settings.z_near;
        r32 z_far = renderer->render.shadow_settings.z_far;
        r32 fov = renderer->render.shadow_settings.fov;

        math::Mat4 inv_view = math::inverse(camera.view_matrix);
        r32 aspect_ratio = (float)renderer->framebuffer_width / (float)renderer->framebuffer_height;
        
        math::Mat4 p = math::perspective(aspect_ratio, DEGREE_IN_RADIANS * fov, z_near, z_far);

        // @Note: NDC Corners in OpenGL
        math::Vec3 near_top_left = inv_view * projection_to_view_space(p, math::Vec3(-1, 1, -1));
        math::Vec3 near_top_right = inv_view * projection_to_view_space(p, math::Vec3(1, 1, -1));
        math::Vec3 near_bottom_left = inv_view * projection_to_view_space(p, math::Vec3(-1, -1, -1));
        math::Vec3 near_bottom_right = inv_view * projection_to_view_space(p, math::Vec3(1, -1, -1));

        math::Vec3 far_top_left = inv_view * projection_to_view_space(p, math::Vec3(-1, 1, 1));
        math::Vec3 far_top_right = inv_view * projection_to_view_space(p, math::Vec3(1, 1, 1));
        math::Vec3 far_bottom_left = inv_view * projection_to_view_space(p, math::Vec3(-1, -1, 1));
        math::Vec3 far_bottom_right = inv_view * projection_to_view_space(p, math::Vec3(1, -1, 1));

        math::Vec3 points[8];

        points[0] = light_view_matrix * near_top_left;
        points[1] = light_view_matrix * near_top_right;
        points[2] = light_view_matrix * near_bottom_left;
        points[3] = light_view_matrix * near_bottom_right;
        points[4] = light_view_matrix * far_top_left;
        points[5] = light_view_matrix * far_top_right;
        points[6] = light_view_matrix * far_bottom_left;
        points[7] = light_view_matrix * far_bottom_right;

        r32 min_x = points[0].x;
        r32 max_x = points[0].x;
        r32 min_y = points[0].y;
        r32 max_y = points[0].y;
        r32 min_z = points[0].z;
        r32 max_z = points[0].z;
        
        for(i32 i = 1; i < 8; i++)
        {
            if(points[i].x < min_x)
            {
                min_x = points[i].x;
            }
            else if(points[i].x > max_x)
            {
                max_x = points[i].x;
            }

            if(points[i].y < min_y)
            {
                min_y = points[i].y;
            }
            else if(points[i].y > max_y)
            {
                max_y = points[i].y;
            }

            if(points[i].z < min_z)
            {
                min_z = points[i].z;
            }
            else if(points[i].z > max_z)
            {
                max_z = points[i].z;
            }
        }
        r32 increment = 15.0f;

        min_x -= increment;
        min_y -= increment;
        min_z -= increment;

        max_x += increment;
        max_y += increment;
        max_z += increment;

        math::Mat4 projection = math::Mat4(1.0f);

        projection.m11 = 2.0f / (max_x - min_x);
        projection.m22 = 2.0f / (max_y - min_y);
        projection.m33 = 2.0f / (max_z - min_z);
        projection.m14 = -((max_x + min_x)/(max_x - min_x));
        projection.m24 = -((max_y + min_y)/(max_y - min_y));
        projection.m34 = -((max_z + min_z)/(max_z - min_z));
                
        rendering::set_light_space_matrices(renderer, projection, light_view_matrix);
    }
        
    static inline Material &get_material_instance(MaterialInstanceHandle handle, Renderer *renderer)
    {
        i32 real_handle = renderer->render._internal_material_instance_handles[handle.handle - 1];
        return renderer->render.material_instances[real_handle];
    }

    static UniformValue *get_array_variable_mapping(MaterialInstanceHandle handle, const char *array_name, UniformMappingType type, Renderer *renderer)
    {
        Material &material = get_material_instance(handle, renderer);
        ;

        for (i32 i = 0; i < material.array_count; i++)
        {
            if (strcmp(material.arrays[i].name, array_name) == 0)
            {
                UniformEntry &first_entry = material.arrays[i].entries[0];

                for (i32 j = 0; j < first_entry.value_count; j++)
                {
                    if (first_entry.values[j].uniform.mapping_type == type)
                    {
                        return &first_entry.values[j];
                    }
                }
                break;
            }
        }

        return nullptr;
    }

    static UniformValue *mapping(Material &material, UniformMappingType type)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            if (material.uniform_values[i].uniform.mapping_type == type)
            {
                return &material.uniform_values[i];
            }
        }

        return nullptr;
    }

    static VertexAttributeInstanced *attrib_mapping(Material &material, VertexAttributeMappingType type)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            if (material.instanced_vertex_attributes[i].mapping_type == type)
            {
                return &material.instanced_vertex_attributes[i];
            }
        }

        return nullptr;
    }

    static UniformValue *get_mapping(MaterialInstanceHandle handle, UniformMappingType type, Renderer *renderer)
    {
        Material &material = get_material_instance(handle, renderer);
        return mapping(material, type);
    }
    
    static VertexAttributeInstanced *get_attrib_mapping(MaterialInstanceHandle handle, VertexAttributeMappingType type, Renderer *renderer)
    {
        Material &material = get_material_instance(handle, renderer);
        return attrib_mapping(material, type);
    }

    static UniformValue *get_value(Material &material, ValueType type, const char *name)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &value = material.uniform_values[i];

            if (value.uniform.type == type && strcmp(value.uniform.name, name) == 0)
            {
                return &value;
            }
        }

        return nullptr;
    }

    static void set_old_material_values(Material &new_material, Material &old_material)
    {
        for (i32 uniform_index = 0; uniform_index < new_material.uniform_value_count; uniform_index++)
        {
            UniformValue &value = new_material.uniform_values[uniform_index];

            if (value.uniform.is_array)
            {
                // @Incomplete: NOT SUPPORTED

                // Uniform &u = value.uniform;

                // if(u.type == ValueType::STRUCTURE)
                // {

                // }
                // else
                // {
                //     // UniformArray &array = new_material
                //     // for(i32 i = 0; i < array.entry_count; i++)
                //     // {
                //     //     UniformArrayEntry &entry = array.entries[i];
                //     //     if(Uniform
                //     // }
                // }
            }
            else
            {
                if (UniformValue *old_value = get_value(old_material, value.uniform.type, value.uniform.name))
                {
                    switch (value.uniform.type)
                    {
                    case ValueType::FLOAT:
					value.float_val = old_value->float_val;
					break;
                    case ValueType::FLOAT2:
					value.float2_val = old_value->float2_val;
					break;
                    case ValueType::FLOAT3:
					value.float3_val = old_value->float3_val;
					break;
                    case ValueType::FLOAT4:
					value.float4_val = old_value->float4_val;
					break;
                    case ValueType::INTEGER:
					value.integer_val = old_value->integer_val;
					break;
                    case ValueType::BOOL:
					value.boolean_val = old_value->boolean_val;
					break;
                    case ValueType::MAT4:
					value.mat4_val = old_value->mat4_val;
					break;
                    case ValueType::TEXTURE:
					value.texture = old_value->texture;
					break;
                    case ValueType::MS_TEXTURE:
					value.ms_texture = old_value->ms_texture;
					break;
                    case ValueType::STRUCTURE:
					// @Incomplete
					break;
                    default:
					assert(false);
                    }
                }
            }
        }
    }

    static void get_updated_material(Material *new_material, Material &current_material, Shader &shader, Renderer *renderer)
    {
        set_shader_values(*new_material, shader, renderer);
        set_old_material_values(*new_material, current_material);
    }

    static void update_materials_with_shader(Renderer *renderer, Shader &shader)
    {
        for (i32 i = 0; i < renderer->render.material_count; i++)
        {
            Material &material = renderer->render.materials[i];
            if (material.shader.handle == shader.index)
            {
                Material new_material = {};
                new_material.shader = material.shader;
                get_updated_material(&new_material, material, shader, renderer);
                memcpy(new_material.instanced_vertex_attributes, material.instanced_vertex_attributes, material.instanced_vertex_attribute_count * sizeof(VertexAttributeInstanced));
                new_material.instanced_vertex_attribute_count = material.instanced_vertex_attribute_count;
                renderer->render.materials[i] = new_material;
            }
        }

        for (i32 i = 0; i < renderer->render.material_instance_count; i++)
        {
            Material &material = renderer->render.material_instances[i];
            if (material.shader.handle == shader.index)
            {
                Material new_material = {};
                new_material.shader = material.shader;
                get_updated_material(&new_material, material, shader, renderer);
                memcpy(new_material.instanced_vertex_attributes, material.instanced_vertex_attributes, material.instanced_vertex_attribute_count * sizeof(VertexAttributeInstanced));
                new_material.instanced_vertex_attribute_count = material.instanced_vertex_attribute_count;
                renderer->render.material_instances[i] = new_material;
            }
        }
    }

    static void load_texture(Renderer *renderer, TextureFiltering filtering, TextureWrap wrap, unsigned char *data, i32 width, i32 height, TextureFormat format, TextureUsage usage, TextureHandle &handle)
    {
        if (handle.handle == 0)
        {
            handle.handle = renderer->render.texture_count++ + 1;
            assert(handle.handle != 0);
        }

        Texture* texture = renderer->render.textures[handle.handle - 1];

        renderer->api_functions.load_texture(texture, filtering, wrap, format, width, height, data, renderer->api_functions.render_state, renderer, usage);
    }

    static void load_texture(const char *full_texture_path, Renderer *renderer, TextureFiltering filtering, TextureWrap wrap, TextureFormat format, TextureUsage usage, TextureHandle &handle)
    {
        if(handle.handle == 0)
        {
            handle.handle = renderer->render.texture_count++ + 1;
            assert(handle.handle != 0);
        }

        PlatformFile png_file = platform.open_file(full_texture_path, POF_READ | POF_OPEN_EXISTING | POF_IGNORE_ERROR);

        if(png_file.handle)
        {
            platform.seek_file(png_file, 0, SO_END);
            i32 size = platform.tell_file(png_file);
            platform.seek_file(png_file, 0, SO_SET);

            TemporaryMemory temp_mem = begin_temporary_memory(&renderer->texture_arena);
            stbi_uc* tex_data = push_size(&renderer->texture_arena, size + 1, stbi_uc);
            platform.read_file(tex_data, size, 1, png_file);

            i32 width;
            i32 height;
            
            unsigned char* image_data = stbi_load_from_memory(tex_data, size, &width, &height, 0, STBI_rgb_alpha);
            platform.close_file(png_file);
            end_temporary_memory(temp_mem);

            assert(renderer->api_functions.load_texture);

            Texture* texture = renderer->render.textures[handle.handle - 1];
            
            renderer->api_functions.load_texture(texture, filtering, wrap, format, width, height, image_data, renderer->api_functions.render_state, renderer, usage);

            stbi_image_free(image_data);
        }
        else
        {
            printf("Texture could not be loaded: %s\n", full_texture_path);
            assert(false);
        }
    }

    static void update_buffer(rendering::BufferHandle handle, BufferType buffer_type, BufferUsage usage, void* data, size_t count, size_t size, Renderer *renderer)
    {
        assert(renderer->render._internal_buffer_handles);

        i32 _internal_handle = renderer->render._internal_buffer_handles[handle.handle - 1];
        
        Buffer* buffer = renderer->render.buffers[_internal_handle];
        
        renderer->api_functions.update_buffer(buffer, buffer_type, data, count, size, usage, renderer->api_functions.render_state, renderer);
    }

    static size_t size_for_type(ValueType type)
    {
        switch (type)
        {
        default:
        case ValueType::INVALID:
        case ValueType::TEXTURE:
        case ValueType::MS_TEXTURE:
		assert(false);
        case ValueType::FLOAT:
		return sizeof(r32);
        case ValueType::FLOAT2:
		return sizeof(r32) * 2;
        case ValueType::FLOAT3:
		return sizeof(r32) * 3;
        case ValueType::FLOAT4:
		return sizeof(r32) * 4;
        case ValueType::INTEGER:
        case ValueType::BOOL:
		return sizeof(i32);
        case ValueType::MAT4:
		return sizeof(r32) * 16;
        }
    }

    static math::Vec2i get_scale(Renderer *renderer)
    {
        return {renderer->framebuffer_width, renderer->framebuffer_height};
    }
    

    static math::Vec3 to_ui(Renderer *renderer, math::Vec2 coord)
    {
        math::Vec2i scale = get_scale(renderer);
        math::Vec3 res;
        res.x = ((r32)coord.x / (r32)scale.x) * UI_COORD_DIMENSION;
        res.y = ((r32)coord.y / (r32)scale.y) * UI_COORD_DIMENSION;
        res.z = 0.0f;
        return res;
    }

    math::Vec2 from_ui(Renderer *renderer, math::Vec3 coord)
    {
        math::Vec2i scale = get_scale(renderer);
        math::Vec2 res(0.0f);
        res.x = (((r32)coord.x / (r32)UI_COORD_DIMENSION) * scale.x);
        res.y = (((r32)coord.y / (r32)UI_COORD_DIMENSION) * scale.y);
        return res;
    }

    r32 from_ui(Renderer *renderer, i32 scale, r32 coord)
    {
        return ((r32)coord / (r32)UI_COORD_DIMENSION) * (r32)scale;
    }

    r32 to_ui(Renderer *renderer, i32 scale, r32 coord)
    {
        return (coord / (r32)scale) * (r32)UI_COORD_DIMENSION;
    }

    static math::Vec2 get_text_size(const char *text, TrueTypeFontInfo font)
    {
        math::Vec2 size;
        r32 placeholder_y = 0.0;

        i32 lines = 1;

        r32 current_width = 0.0f;
    
        for(u32 i = 0; i < strlen(text); i++)
        {
            if(text[i] != '\n' && text[i] != '\r')
            {
                stbtt_aligned_quad quad;
                stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                                    text[i] - font.first_char, &size.x, &placeholder_y, &quad, 1);
        
                if(MAX(quad.y1, quad.y0) - MIN(quad.y0, quad.y1) > size.y)
                {
                    size.y = quad.y1 - quad.y0;
                }
        
                i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
                current_width += (r32)kerning * font.scale;
            }
            else
            {
                if(size.x > current_width)
                    current_width = size.x;

                size.x = 0.0f;
                lines++;
            }
        }

        return math::Vec2(current_width, size.y * lines * (lines));
    }

    static TrueTypeFontInfo get_tt_font_info(Renderer *renderer, i32 handle)
    {
        assert(handle >= 0 && handle < renderer->tt_font_count);
        return renderer->tt_font_infos[handle];
    }

    static LineData get_line_size_data(const char *text, TrueTypeFontInfo font)
    {
        math::Vec2 size;
        r32 placeholder_y = 0.0;
    
        LineData line_data = {};
        line_data.total_height = 0.0f;
        line_data.line_count = 1;

        r32 min_y = 0.0f;
        r32 max_y = 0.0f;
    
        line_data.line_spacing = (r32)font.size + font.line_gap * font.scale;

        i32 x0, x1, y0, y1;
        stbtt_GetFontBoundingBox(&font.info, &x0, &y0, &x1, &y1);
    
        for(u32 i = 0; i < strlen(text); i++)
        {
            if(text[i] != '\n' && text[i] != '\r')
            {
                stbtt_aligned_quad quad;
                stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                                    text[i] - font.first_char, &line_data.line_sizes[line_data.line_count - 1].x, &placeholder_y, &quad, 1);

                if(MAX(quad.y1, quad.y0) - MIN(quad.y0, quad.y1) > size.y)
                {
                    line_data.line_sizes[line_data.line_count - 1].y = quad.y1 - quad.y0;
                    size.y = quad.y1 - quad.y0;
                }
        
                i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
                line_data.line_sizes[line_data.line_count - 1].x += (r32)kerning * font.scale;
            }
            else
            {
                line_data.line_count++;
            }
        }

        if(line_data.line_count == 1)
        {
            min_y = y0 * font.scale;
            max_y = y1 * font.scale;
            line_data.total_height = max_y - min_y;
        }
        else
            line_data.total_height = (line_data.line_count - 1) * line_data.line_spacing;
    
        return line_data;
    }

    static math::Vec2 get_text_size_scaled(Renderer *renderer, const char* text, TrueTypeFontInfo font, u64 scaling_flags = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        LineData line_data = get_line_size_data(text, font);
        math::Vec2 font_size = math::Vec2(line_data.line_sizes[0].x, line_data.total_height);
        math::Vec2 result(0.0f);
    
        math::Vec2i scale = get_scale(renderer);
    
        result.x = (font_size.x / (r32)scale.x) * UI_COORD_DIMENSION;
    
        if(scaling_flags & UIScalingFlag::KEEP_ASPECT_RATIO)
        {
            r32 ratio = font_size.y / font_size.x;
            result.y = font_size.x * ratio;
        }
        else
        {
            result.y = (font_size.y / (r32)scale.y) * UI_COORD_DIMENSION;
        }
    
        return result;
    }
    
// Gets an array of text widths for each character
// Remember to free
    static TextLengthInfo get_char_widths_scaled(Renderer *renderer, const char* text, TrueTypeFontInfo font, MemoryArena* arena)
    {
        TextLengthInfo info = {};
    
        info.length = strlen(text);
        info.widths = push_array(arena, info.length, r32);//(r32*)calloc(info.length, sizeof(r32));
    
        r32 placeholder_y = 0.0f;
    
        math::Vec2i scale = get_scale(renderer);
    
        for(size_t i = 0; i < info.length; i++)
        {
            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                                text[i] - font.first_char, &info.widths[i], &placeholder_y, &quad, 1);
        
            i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
        
            info.widths[i] += (r32)kerning * font.scale;
            info.widths[i] = ((r32)info.widths[i] / (r32)scale.x) * UI_COORD_DIMENSION;
        }
    
        return info;
    }


    static void load_font(Renderer *renderer, const char *path, i32 size, FontHandle &handle)
    {
        i32 index = handle.handle;

        if (index == 0)
        {
            handle.handle = renderer->tt_font_count++;
            index = handle.handle;
        }
        
        TrueTypeFontInfo &font_info = renderer->tt_font_infos[index];
        char buf[256];
        strncpy(buf, path, strlen(path) + 1);

        if(font_info.ttf_buffer)
        {
            free(font_info.ttf_buffer);
        }

        TextureHandle texture = font_info.texture;
        
        font_info = {};
        
        strncpy(font_info.path, buf, strlen(buf) + 1);

        RenderPass& pass = renderer->render.ui.pass;

        FramebufferInfo& framebuffer = renderer->render.framebuffers[pass.framebuffer.handle - 1];

        font_info.resolution_loaded_for.width = (i32)framebuffer.width;
        font_info.resolution_loaded_for.height = (i32)framebuffer.height;

        font_info.oversample_x = 1;
        font_info.oversample_y = 1;
        font_info.first_char = ' ';
        font_info.char_count = '~' - ' ';
        font_info.load_size = size;

        font_info.size = (i32)from_ui(renderer, (i32)framebuffer.height, (r32)font_info.load_size);

        i32 count_per_line = (i32)math::ceil(math::sqrt((r32)font_info.char_count));
        font_info.atlas_width = math::multiple_of_number(font_info.size * count_per_line, 4);
        font_info.atlas_height = math::multiple_of_number(font_info.size * count_per_line, 4);

        font_info.ttf_buffer = (unsigned char*)malloc(1<<20);
        
        TemporaryMemory temp_mem = begin_temporary_memory(&renderer->font_arena);

        unsigned char *temp_bitmap = push_array(&renderer->font_arena, font_info.atlas_width * font_info.atlas_height, unsigned char);

        fread(font_info.ttf_buffer, 1, 1 << 20, fopen(font_info.path, "rb"));

        stbtt_InitFont(&font_info.info, font_info.ttf_buffer, 0);
        font_info.scale = stbtt_ScaleForPixelHeight(&font_info.info, 15);
        stbtt_GetFontVMetrics(&font_info.info, &font_info.ascent, &font_info.descent, &font_info.line_gap);
        font_info.baseline = (i32)(font_info.ascent * font_info.scale);

        stbtt_pack_context context;
        if (!stbtt_PackBegin(&context, temp_bitmap, font_info.atlas_width, font_info.atlas_height, 0, 1, nullptr))
            printf("Failed to initialize font");

        stbtt_PackSetOversampling(&context, font_info.oversample_x, font_info.oversample_y);
        if (!stbtt_PackFontRange(&context, font_info.ttf_buffer, 0, (r32)font_info.size, font_info.first_char, font_info.char_count, font_info.char_data))
            printf("Failed to pack font");

        stbtt_PackEnd(&context);       

        load_texture(renderer, TextureFiltering::LINEAR, TextureWrap::CLAMP_TO_EDGE, temp_bitmap, font_info.atlas_width, font_info.atlas_height, TextureFormat::RED, TextureUsage::DYNAMIC, texture);

        font_info.texture = texture;

        font_info.line_height = font_info.size + font_info.line_gap * font_info.scale;

        r32 largest_character = 0;

        for (i32 i = 0; i < font_info.char_count; i++)
        {
            char str[2];
            str[0] = (char)(font_info.first_char + i);
            str[1] = '\0';
            math::Vec2 char_size = get_text_size(str, font_info);
            if (char_size.y > largest_character)
            {
                largest_character = char_size.y;
            }
        }

        font_info.largest_character_height = largest_character;

        end_temporary_memory(temp_mem);
    }

    static RegisterBufferInfo create_register_buffer_info()
    {
        RegisterBufferInfo info = {};
        info.data = {};
        info.vertex_attribute_count = 0;
        info.stride = 0;
        return info;
    }

    static void add_vertex_attrib(ValueType type, RegisterBufferInfo &info)
    {
        VertexAttribute attribute = {};
        attribute.type = type;
        info.vertex_attributes[info.vertex_attribute_count++] = attribute;

        info.stride += size_for_type(type);
    }

    static void load_material_from_mtl(Renderer *renderer, Material &material, const char *file_path)
    {
        // @Incomplete: We need a better way to do this!
        // Find the directory of the file
        size_t index = 0;
        for (size_t i = 0; i < strlen(file_path); i++)
        {
            if (file_path[i] == '/')
            {
                index = i + 1;
            }
        }

        TemporaryMemory temp_block = begin_temporary_memory(&renderer->temp_arena);

        char *dir = push_string(temp_block.arena, index);
        strncpy(dir, file_path, index);

        dir[index] = 0;
        FILE *file = fopen(file_path, "r");

        if (file)
        {
            char buffer[256];

            if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
            {
                u->float4_val = math::Rgba(1, 1, 1, 1);
            }

            if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_COLOR))
            {
                u->float4_val = math::Rgba(0, 0, 0, 1);
            }

            if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_COLOR))
            {
                u->float4_val = math::Rgba(0.0f);
            }

            if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_EXPONENT))
            {
                u->float_val = 1000;
            }

            while (fgets(buffer, sizeof(buffer), file))
            {
                if (starts_with(buffer, "newmtl"))
                {
                    // @Incomplete: Save name
                }
                else if (starts_with(buffer, "illum")) // illumination
                {
                }
                else if (starts_with(buffer, "Ka")) // ambient color
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_COLOR))
                    {
                        sscanf(buffer, "Ka %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if (starts_with(buffer, "Kd")) // diffuse color
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
                    {
                        sscanf(buffer, "Kd %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                    else if(VertexAttributeInstanced *va = attrib_mapping(material, VertexAttributeMappingType::DIFFUSE_COLOR))
                    {
                        sscanf(buffer, "Kd %f %f %f", &va->attribute.float4_val.r, &va->attribute.float4_val.g, &va->attribute.float4_val.b);
                        va->attribute.float4_val.a = 1.0f;
                    }
                }
                else if (starts_with(buffer, "Ks")) // specular color
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_COLOR))
                    {
                        sscanf(buffer, "Ks %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if (starts_with(buffer, "Ns")) // specular exponent
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_EXPONENT))
                    {
                        sscanf(buffer, "Ns %f", &u->float_val);
                    }
                }
                else if (starts_with(buffer, "d"))
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::DISSOLVE))
                    {
                        sscanf(buffer, "d %f", &u->float_val);
                    }
                }
                else if (starts_with(buffer, "map_Ka")) // ambient map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ka %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                    }
                }
                else if (starts_with(buffer, "map_Kd")) // diffuse map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Kd %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                    }
                }
                else if (starts_with(buffer, "map_Ks")) // specular map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ks %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                    }
                }
                else if (starts_with(buffer, "map_Ns")) // specular intensity map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_INTENSITY_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ns %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                    }
                }
            }

            fclose(file);
        }
        else
        {
            error("Can't load .mtl file", file_path);
        }

        end_temporary_memory(temp_block);
    }

    static void generate_vertex_buffer(r32 *vertex_buffer, Vertex *vertices, i32 vertex_count, i32 vertex_size, b32 has_normals, b32 has_uvs)
    {
        i32 vertex_data_count = vertex_size;

        for (i32 i = 0; i < vertex_count; i++)
        {
            i32 increment_by = 1;
            i32 base_index = i * vertex_data_count;
            Vertex vertex = vertices[i];
            vertex_buffer[base_index] = vertex.position.x;
            vertex_buffer[base_index + increment_by++] = vertex.position.y;
            vertex_buffer[base_index + increment_by++] = vertex.position.z;

            if (has_normals)
            {
                vertex_buffer[base_index + increment_by++] = vertex.normal.x;
                vertex_buffer[base_index + increment_by++] = vertex.normal.y;
                vertex_buffer[base_index + increment_by++] = vertex.normal.z;
            }

            if (has_uvs)
            {
                vertex_buffer[base_index + increment_by++] = vertex.uv.x;
                vertex_buffer[base_index + increment_by++] = vertex.uv.y;
            }
        }
    }

    static i32 _find_unused_handle(Renderer *renderer)
    {
        for (i32 index = renderer->render._current_internal_buffer_handle; index < global_max_custom_buffers; index++)
        {
            if (renderer->render._internal_buffer_handles[index] == -1)
            {
                renderer->render._current_internal_buffer_handle = index;
                return index;
            }
        }

        for (i32 index = 0; index < global_max_custom_buffers; index++)
        {
            if (renderer->render._internal_buffer_handles[index] == -1)
            {
                renderer->render._current_internal_buffer_handle = index;
                return index;
            }
        }

        assert(false);

        return -1;
    }

    static void generate_index_buffer(u16 *index_buffer, Face *faces, i32 face_count)
    {
        i32 face_data_count = 3;

        for (i32 i = 0; i < face_count; i++)
        {
            i32 base_index = i * face_data_count;
            Face face = faces[i];
            index_buffer[base_index] = face.indices[0];
            index_buffer[base_index + 1] = face.indices[1];
            index_buffer[base_index + 2] = face.indices[2];
        }
    }

    static BufferHandle register_buffer(Renderer *renderer, RegisterBufferInfo info)
    {
        assert(renderer->render.buffer_count + 1 < global_max_custom_buffers);
        assert(renderer->render._internal_buffer_handles);

        i32 unused_handle = _find_unused_handle(renderer) + 1;

        renderer->render._internal_buffer_handles[unused_handle - 1] = renderer->render.buffer_count++;

        Buffer *buffer = renderer->render.buffers[renderer->render._internal_buffer_handles[unused_handle - 1]];
        
        renderer->api_functions.create_buffer(buffer, info, renderer->api_functions.render_state, renderer);

        return {unused_handle};
    }

    static BufferHandle create_line_buffer(Renderer *renderer)
    {
       assert(renderer->render.buffer_count + 1 < global_max_custom_buffers);

        // @Note: Untextured
        i32 vertex_size = 3;

        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = BufferUsage::DYNAMIC;
        add_vertex_attrib(ValueType::FLOAT3, info);
        r32 vertices[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        info.data.vertex_count = 4;
        info.data.vertex_buffer_size = info.data.vertex_count * vertex_size * (i32)sizeof(r32);

        info.data.vertex_buffer = push_size(&renderer->buffer_arena, info.data.vertex_buffer_size, r32);

        for (i32 i = 0; i < 12; i++)
        {
            info.data.vertex_buffer[i] = vertices[i];
        }

        info.data.index_buffer_size = 0;
        info.data.index_buffer_count = 0;

        return {register_buffer(renderer, info).handle}; 
    }

    static BufferHandle create_bounding_box_buffer(Renderer *renderer)
    {
        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = rendering::BufferUsage::STATIC;
        add_vertex_attrib(rendering::ValueType::FLOAT3, info);

        BufferHandle buffer = register_buffer(renderer, info);
        
        math::Vec3* vertices = nullptr;

        buf_push(vertices, math::Vec3(-0.5, -0.5, -0.5));
        buf_push(vertices, math::Vec3(0.5, -0.5, -0.5));
        buf_push(vertices, math::Vec3(0.5, 0.5, -0.5));
        buf_push(vertices, math::Vec3(-0.5, 0.5, -0.5));
        buf_push(vertices, math::Vec3(-0.5, -0.5, 0.5));
        buf_push(vertices, math::Vec3(0.5, -0.5, 0.5));
        buf_push(vertices, math::Vec3(0.5, 0.5, 0.5));
        buf_push(vertices, math::Vec3(-0.5, 0.5, 0.5));

        u16 *indices = nullptr;

        buf_push(indices, 0);
        buf_push(indices, 1);
        buf_push(indices, 2);
        buf_push(indices, 3);

        buf_push(indices, 4);
        buf_push(indices, 5);
        buf_push(indices, 6);
        buf_push(indices, 7);

        buf_push(indices, 0);
        buf_push(indices, 4);
        buf_push(indices, 1);
        buf_push(indices, 5);
        buf_push(indices, 2);
        buf_push(indices, 6);
        buf_push(indices, 3);
        buf_push(indices, 7);

        // @Incomplete: Change to static?
        update_buffer(buffer, BufferType::VERTEX, BufferUsage::DYNAMIC, (r32*)vertices, buf_len(vertices), buf_len(vertices) * sizeof(math::Vec3), renderer);

        update_buffer(buffer, BufferType::INDEX, BufferUsage::DYNAMIC, (r32*)indices, buf_len(indices), buf_len(indices) * sizeof(u16), renderer);

        buf_free(vertices);
        buf_free(indices);

        return buffer;
    }

    static void update_line_buffer(Renderer* renderer, BufferHandle buffer, math::Vec3* _vertices, size_t n)
    {
        math::Vec3* lines = nullptr;
        math::Vec3* vertices = nullptr;

        for(size_t i = 0; i < n; i++)
        {
            math::Vec3 v = _vertices[i];
            buf_push(vertices, v);
        }

        math::Vec3 adj_0 = 2.0f * vertices[0] - vertices[1];
        buf_push(lines, adj_0);

        buf_push(lines, vertices[0]);
        buf_push(lines, vertices[1]);
        buf_push(lines, vertices[2]);
        buf_push(lines, vertices[3]);

        math::Vec3 adj_n = 2.0f * vertices[n-1] - vertices[n-2];

        buf_push(vertices, adj_n);
        buf_push(vertices, adj_n);

        n = buf_len(vertices) - 3;

        for(size_t i = 1; i < n; i++)
        {
            buf_push(lines, vertices[i - 1]);
            buf_push(lines, vertices[i]);
            buf_push(lines, vertices[i + 1]);
            buf_push(lines, vertices[i + 2]);
        }
        
        update_buffer(buffer, BufferType::VERTEX, BufferUsage::DYNAMIC, (r32*)lines, buf_len(lines), buf_len(lines) * sizeof(math::Vec3), renderer);

        buf_free(lines);
        buf_free(vertices);
    }

    static math::Vec3* generate_grid_buffer(Renderer* renderer, i32 width, i32 height, r32 unit_size)
    {
        math::Vec3* points = nullptr;
        r32 real_grid_width = width * unit_size;
        r32 real_grid_height = height * unit_size;
        r32 half_size = 0.5f * unit_size;

        math::Vec3 bottom_right;
        bottom_right.x = real_grid_width * 0.5f - half_size;
        bottom_right.y = 0.0f;
        bottom_right.z = -real_grid_height * 0.5f - half_size;
        
        math::Vec3 bottom_left;
        bottom_left.x = -real_grid_width * 0.5f - half_size;
        bottom_left.y = 0.0f;
        bottom_left.z = -real_grid_height * 0.5f - half_size;

        math::Vec3 top_left;
        top_left.x = -real_grid_width * 0.5f - half_size;
        top_left.y = 0.0f;
        top_left.z = real_grid_height * 0.5f - half_size;

        math::Vec3 top_right;
        top_right.x = real_grid_width * 0.5f - half_size;
        top_right.y = 0.0f;
        top_right.z = real_grid_height * 0.5f - half_size;

        buf_push(points, bottom_left);
        buf_push(points, top_left);
        buf_push(points, top_right);
        buf_push(points, bottom_right);
        buf_push(points, bottom_left);
        buf_push(points, bottom_right);

        for(i32 i = 1; i < width; i++)
        {
            math::Vec3 origin;
            origin.x = -real_grid_width * 0.5f + i - half_size;
            origin.y = 0.0f;
            origin.z = -real_grid_height * 0.5f - half_size;

            math::Vec3 end;
            end.x = -real_grid_width * 0.5f + i - half_size;
            end.y = 0.0f;
            end.z = real_grid_height * 0.5f - half_size;

            buf_push(points, origin);
            buf_push(points, end);
            buf_push(points, origin);
        }

        buf_push(points, bottom_left);

        for(i32 i = 1; i < height; i++)
        {
            math::Vec3 origin;
            origin.x = -real_grid_width * 0.5f - half_size;
            origin.y = 0.0f;
            origin.z = -real_grid_height * 0.5f + i - half_size;

            math::Vec3 end;
            end.x = real_grid_width * 0.5f - half_size;
            end.y = 0.0f;
            end.z = -real_grid_height * 0.5f + i - half_size;

            buf_push(points, origin);
            buf_push(points, end);
            buf_push(points, origin);
        }
        return points;
    }

    static BufferHandle create_vertex_buffer(Renderer *renderer, r32 *vertices, i32 size, i32 vertex_count)
    {
        assert(renderer->render.buffer_count + 1 < global_max_custom_buffers);

        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = BufferUsage::STATIC;
        add_vertex_attrib(ValueType::FLOAT3, info);

        info.data.vertex_count = vertex_count;
        info.data.vertex_buffer_size = size;

        info.data.vertex_buffer = push_size(&renderer->buffer_arena, info.data.vertex_buffer_size, r32);
        memcpy(info.data.vertex_buffer, vertices, (size_t)info.data.vertex_buffer_size);

        info.data.index_buffer_size = 0;
        info.data.index_buffer_count = 0;

        return {register_buffer(renderer, info).handle};
    }

    static BufferHandle create_quad_buffer(Renderer *renderer, u64 anchor = 0, b32 uvs = false)
    {
        assert(renderer->render.buffer_count + 1 < global_max_custom_buffers);

        // @Note: Untextured
        i32 vertex_size = 2;

        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = BufferUsage::STATIC;
        add_vertex_attrib(ValueType::FLOAT2, info);

        math::Vec2 pivot = math::Vec2(0.5f);

        if (anchor & UIAlignment::BOTTOM)
        {
            pivot.y = 0.0f;
        }
        else if (anchor & UIAlignment::TOP)
        {
            pivot.y = 1.0f;
        }

        if (anchor & UIAlignment::LEFT)
        {
            pivot.x = 0.0f;
        }
        else if (anchor & UIAlignment::RIGHT)
        {
            pivot.x = 1.0f;
        }

        TemporaryMemory temp_mem = begin_temporary_memory(&renderer->buffer_arena);

        if (uvs)
        {
            add_vertex_attrib(ValueType::FLOAT2, info);
            vertex_size += 2;
            r32 vertices[16] =
                {
                    0.0f - pivot.x, 0.0f - pivot.y, 0.0f, 1.0f,
                    1.0f - pivot.x, 0.0f - pivot.y, 1.0f, 1.0f,
                    1.0f - pivot.x, 1.0f - pivot.y, 1.0f, 0.0f,
                    0.0f - pivot.x, 1.0f - pivot.y, 0.0f, 0.0f};

            info.data.vertex_count = 4;
            info.data.vertex_buffer_size = info.data.vertex_count * vertex_size * (i32)sizeof(r32);
            info.data.vertex_buffer = push_size(&renderer->buffer_arena, info.data.vertex_buffer_size, r32);

            for (i32 i = 0; i < info.data.vertex_count * vertex_size; i++)
            {
                info.data.vertex_buffer[i] = vertices[i];
            }
        }
        else
        {
            r32 vertices[8] =
                {
                    0.0f - pivot.x,
                    0.0f - pivot.y,
                    1.0f - pivot.x,
                    0.0f - pivot.y,
                    1.0f - pivot.x,
                    1.0f - pivot.y,
                    0.0f - pivot.x,
                    1.0f - pivot.y,
                };

            info.data.vertex_count = 4;
            info.data.vertex_buffer_size = info.data.vertex_count * vertex_size * (i32)sizeof(r32);
            info.data.vertex_buffer = push_size(&renderer->buffer_arena, info.data.vertex_buffer_size, r32);

            for (i32 i = 0; i < info.data.vertex_count * vertex_size; i++)
            {
                info.data.vertex_buffer[i] = vertices[i];
            }
        }

        u16 quad_indices[6] =
            {
                0, 1, 2,
                0, 2, 3
            };

        i32 index_count = 6;
        info.data.index_buffer_size = index_count * (i32)sizeof(u16);
        info.data.index_buffer_count = index_count;

        info.data.index_buffer = push_size(&renderer->buffer_arena, info.data.index_buffer_size, u16);

        for (i32 i = 0; i < index_count; i++)
        {
            info.data.index_buffer[i] = quad_indices[i];
        }

        rendering::BufferHandle handle = register_buffer(renderer, info);

        end_temporary_memory(temp_mem);

        return handle;
    }

	
	// struct RegisterBufferInfo
	// {
	// 	VertexAttribute vertex_attributes[16];
	// 	i32 vertex_attribute_count;

	// 	size_t stride;

	// 	BufferUsage usage;
		
	// 	BufferData data;

	// 	RegisterBufferInfo(const RegisterBufferInfo& other)
    //     {
    //         memcpy(vertex_attributes, other.vertex_attributes, sizeof(VertexAttribute) * other.vertex_attribute_count);
    //         vertex_attribute_count = other.vertex_attribute_count;
    //         stride = other.stride;
    //         usage = other.usage;
    //         data = other.data;
    //     }

	// 	RegisterBufferInfo() {}
	// };

    static BufferHandle create_buffers_from_mesh(Renderer *renderer, Mesh &mesh, u64 vertex_data_flags, b32 has_normals, b32 has_uvs)
    {
        assert(renderer->render.buffer_count + 1 < global_max_custom_buffers);
        i32 vertex_size = 3;

        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = BufferUsage::STATIC;
        add_vertex_attrib(ValueType::FLOAT3, info);

        if (has_normals)
        {
            add_vertex_attrib(ValueType::FLOAT3, info);
            vertex_size += 3;
        }

        if (has_uvs)
        {
            add_vertex_attrib(ValueType::FLOAT2, info);
            vertex_size += 2;
        }

        info.data.vertex_count = mesh.vertex_count;
        info.data.vertex_buffer_size = mesh.vertex_count * vertex_size * (i32)sizeof(r32);

        info.data.vertex_buffer = push_size(&renderer->mesh_arena, info.data.vertex_buffer_size, r32);
        generate_vertex_buffer(info.data.vertex_buffer, mesh.vertices, mesh.vertex_count, vertex_size, has_normals, has_uvs);

        i32 index_count = mesh.face_count * 3;
        info.data.index_buffer_size = index_count * (i32)sizeof(u16);
        info.data.index_buffer_count = index_count;

        // @Robustness:(Niels): How do we make sure that this is cleared if the mesh is removed?
        // Or will that never happen? Maybe use malloc/free instead? Or maybe at some point
        // we really __should__ create a more general purpose allocator ourselves...
        info.data.index_buffer = push_size(&renderer->mesh_arena, info.data.index_buffer_size, u16);
        generate_index_buffer(info.data.index_buffer, mesh.faces, mesh.face_count);

        return {register_buffer(renderer, info).handle};
    }
    
    static BufferHandle create_plane(Renderer *renderer, math::Vec3 *scale = nullptr, math::BoundingBox *box = nullptr)
    {
        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;

        Mesh mesh;
        mesh = {};
        mesh.vertices = push_array(&renderer->mesh_arena, sizeof(plane_vertices) / sizeof(r32) / 3, Vertex);
        mesh.faces = push_array(&renderer->mesh_arena, sizeof(plane_indices) / sizeof(u16) / 3, Face);
        mesh.vertex_count = sizeof(plane_vertices) / sizeof(r32) / 3;

        for (i32 i = 0; i < mesh.vertex_count; i++)
        {
            Vertex &vertex = mesh.vertices[i];
            vertex.position = math::Vec3(plane_vertices[i * 3], plane_vertices[i * 3 + 1], plane_vertices[i * 3 + 2]);
            vertex.normal = math::Vec3(plane_normals[i * 3], plane_normals[i * 3 + 1], plane_normals[i * 3 + 2]);
            vertex.uv = math::Vec2(plane_uvs[i * 2], plane_uvs[i * 2 + 1]);
            
            min_x = MIN(min_x, vertex.position.x);
            min_y = MIN(min_y, vertex.position.y);
            min_z = MIN(min_z, vertex.position.z);
            max_x = MAX(max_x, vertex.position.x);
            max_y = MAX(max_y, vertex.position.y);
            max_z = MAX(max_z, vertex.position.z);
        }

        mesh.face_count = sizeof(plane_indices) / sizeof(u16) / 3;

        for (i32 i = 0; i < mesh.face_count; i++)
        {
            Face &face = mesh.faces[i];

            face.indices[0] = plane_indices[i * 3];
            face.indices[1] = plane_indices[i * 3 + 1];
            face.indices[2] = plane_indices[i * 3 + 2];
        }

        if(scale)
        {
            *scale = math::Vec3(max_x - min_x, 0.00001f, max_z - min_z); 
        }

        if(box)
        {
            math::BoundingBox new_box = {};
            new_box.min = math::Vec3(min_x, min_y, min_z);
            new_box.max = math::Vec3(max_x, max_y, max_z);
            *box = new_box;
        }
        
        return {create_buffers_from_mesh(renderer, mesh, 0, true, true)};
    }

    static BufferHandle create_cube(Renderer *renderer, math::Vec3 *scale, math::BoundingBox *box = nullptr)
    {
        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;

        Mesh mesh;
        mesh = {};
        mesh.vertices = push_array(&renderer->mesh_arena, sizeof(cube_vertices) / sizeof(r32) / 3, Vertex);
        mesh.faces = push_array(&renderer->mesh_arena, sizeof(cube_indices) / sizeof(u16) / 3, Face);

        mesh.vertex_count = sizeof(cube_vertices) / sizeof(r32) / 3;

        for (i32 i = 0; i < mesh.vertex_count; i++)
        {
            Vertex &vertex = mesh.vertices[i];
            vertex.position = math::Vec3(cube_vertices[i * 3], cube_vertices[i * 3 + 1], cube_vertices[i * 3 + 2]);
            vertex.normal = math::Vec3(cube_normals[i * 3], cube_normals[i * 3 + 1], cube_normals[i * 3 + 2]);
            vertex.uv = math::Vec2(cube_uvs[i * 2], cube_uvs[i * 2 + 1]);

            min_x = MIN(min_x, vertex.position.x);
            min_y = MIN(min_y, vertex.position.y);
            min_z = MIN(min_z, vertex.position.z);
            max_x = MAX(max_x, vertex.position.x);
            max_y = MAX(max_y, vertex.position.y);
            max_z = MAX(max_z, vertex.position.z);
        }

        mesh.face_count = sizeof(cube_indices) / sizeof(u16) / 3;

        for (i32 i = 0; i < mesh.face_count; i++)
        {
            Face &face = mesh.faces[i];

            face.indices[0] = cube_indices[i * 3];
            face.indices[1] = cube_indices[i * 3 + 1];
            face.indices[2] = cube_indices[i * 3 + 2];
        }

        if(scale)
        {
            *scale = math::Vec3(max_x - min_x, max_y - min_y, max_z - min_z); 
        }

        if(box)
        {
            math::BoundingBox new_box = {};
            new_box.min = math::Vec3(min_x, min_y, min_z);
            new_box.max = math::Vec3(max_x, max_y, max_z);
            *box = new_box;
        }
        
        return {create_buffers_from_mesh(renderer, mesh, 0, true, true)};
    }

    static b32 vertex_equals(Vertex &v1, Vertex &v2, b32 with_normals, b32 with_uvs)
    {
        return v1.position.x == v2.position.x && v1.position.y == v2.position.y && v1.position.z == v2.position.z && ((v1.uv.x == v2.uv.x && v1.uv.y == v2.uv.y) || !with_uvs) && ((v1.normal.x == v2.normal.x && v1.normal.y == v2.normal.y && v1.normal.z == v2.normal.z) || !with_normals);
    }

    static i32 check_for_identical_vertex(Vertex &vertex, math::Vec2 uv, math::Vec3 normal, b32 with_normals, b32 with_uvs, Vertex *final_vertices, i32 current_size, b32 *should_add)
    {
        vertex.uv = uv;
        vertex.normal = normal;

        for (i32 index = 0; index < current_size; index++)
        {
            Vertex &existing = final_vertices[index];

            if (vertex_equals(existing, vertex, with_normals, with_uvs))
            {
                return (i32)index;
            }
        }

        *should_add = true;

        return current_size;
    }
    
    static b32 mtl_has_texture(char *source)
    {
        char buffer[256];

        while (read_line(buffer, 256, &source))
        {
            if(starts_with(buffer, "newmtl"))
            {
                return false;
            }
            if(starts_with(buffer, "map_Kd"))
            {
                return true;
            }
        }

        return false;
    }
    
	static b32 has_new_mtl(char *source)
	{
		char buffer[256];
		while (read_line(buffer, 256, &source))
		{
			if (starts_with(buffer, "newmtl"))
			{
				return true;
			}
		}

		return false;
	}

	static i32 _create_temp_material(Material *materials, i32 index, Material material)
	{
		materials[index] = material;
		return index;
	}

    static void load_materials_from_mtl(Material *temp_materials, i32 *temp_count, PassMaterial *pass_materials, i32 pass_material_count, MaterialPair *pairs, i32 *mat_pair_count, const char *file_path, Renderer *renderer)
    {
        size_t index = 0;
        for (size_t i = 0; i < strlen(file_path); i++)
        {
            if (file_path[i] == '/')
            {
                index = i + 1;
            }
        }

        TemporaryMemory temp_block = begin_temporary_memory(&renderer->temp_arena);

        char *dir = push_string(temp_block.arena, index);
        strncpy(dir, file_path, index);

        dir[index] = 0;
        
        FILE *file = fopen(file_path, "r");
        
        if(file)
        {
            MaterialPair *current = nullptr;
            
            char *source = read_file_into_buffer(file);
			char *copy = source;

			b32 has_newmtl = has_new_mtl(copy);
			
			if (!has_newmtl)
			{
			}

			char buffer[256];
            while (read_line(buffer, 256, &source))
            {
                if (starts_with(buffer, "newmtl") || !has_newmtl)
                {
					if (!has_newmtl)
						source = copy;
					
                    char *ptr = source;
                    
                    b32 has_texture = mtl_has_texture(ptr);

                    MaterialPair pair;
                    pair.has_texture = has_texture;

                    pair.pass_count = 0;
                    
                    for(i32 i = 0; i < pass_material_count; i++)
                    {
                        PassMaterial &pass = pass_materials[i];
                        switch(pass.pass_type)
                        {
                        case PassType::NONE:
                        case PassType::STANDARD:
                        {
                            pair.passes[pair.pass_count] = _create_temp_material(temp_materials, *temp_count, pass.material);
							(*temp_count)++;
							strcpy(pair.pass_names[pair.pass_count], pass.pass_name);
                            pair.pass_count++;
                        }
                        break;
                        case PassType::SHADOWS:
                        case PassType::NO_UVS:
                        {
                            if(!has_texture)
                            {
                                pair.passes[pair.pass_count] = _create_temp_material(temp_materials, *temp_count, pass.material);
								(*temp_count)++;
                                strcpy(pair.pass_names[pair.pass_count], pass.pass_name);
                                pair.pass_count++;
                            }
                        }
                        break;
                        case PassType::SHADOWS_WITH_UVS:
                        case PassType::WITH_UVS:
                        {
                            if(has_texture)
                            {
                                pair.passes[pair.pass_count] = _create_temp_material(temp_materials, *temp_count, pass.material);
								(*temp_count)++;
								strcpy(pair.pass_names[pair.pass_count], pass.pass_name);
                                pair.pass_count++;
                            }
                        }
                        break;
                        }
                    }
                    
					if (!has_newmtl)
						strcpy(pair.name, "glitch_default");
					else
						sscanf(buffer, "newmtl %[^\n]", pair.name);
					
					has_newmtl = true;

                    pairs[(*mat_pair_count)++] = pair;
                    
                    current = &pairs[*mat_pair_count - 1];

                    for(i32 i = 0; i < pair.pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
                        {
                            u->float4_val = math::Rgba(1, 1, 1, 1);
                        }

                        if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_COLOR))
                        {
                            u->float4_val = math::Rgba(0, 0, 0, 1);
                        }

                        if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_COLOR))
                        {
                            u->float4_val = math::Rgba(0.0f);
                        }

                        if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_EXPONENT))
                        {
                            u->float_val = 1000;
                        }
                    }
                }
                else if (starts_with(buffer, "illum")) // illumination
                {
                }
                else if (starts_with(buffer, "Ka")) // ambient color
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_COLOR))
                        {
                            sscanf(buffer, "Ka %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                            u->float4_val.a = 1.0f;
                        }
                    }
                }
                else if (starts_with(buffer, "Kd")) // diffuse color
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
                        {
                            sscanf(buffer, "Kd %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                            u->float4_val.a = 1.0f;
                        }
                        else if(VertexAttributeInstanced *va = attrib_mapping(material, VertexAttributeMappingType::DIFFUSE_COLOR))
                        {
                            sscanf(buffer, "Kd %f %f %f", &va->attribute.float4_val.r, &va->attribute.float4_val.g, &va->attribute.float4_val.b);
                            va->attribute.float4_val.a = 1.0f;
                        }
                    }
                }
                else if (starts_with(buffer, "Ks")) // specular color
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_COLOR))
                        {
                            sscanf(buffer, "Ks %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                            u->float4_val.a = 1.0f;
                        }
                    }
                }
                else if (starts_with(buffer, "Ns")) // specular exponent
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_EXPONENT))
                        {
                            sscanf(buffer, "Ns %f", &u->float_val);
                        }
                    }
                }
                else if (starts_with(buffer, "d"))
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::DISSOLVE))
                        {
                            sscanf(buffer, "d %f", &u->float_val);
                        }
                    }
                }
                else if (starts_with(buffer, "map_Ka")) // ambient map
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_TEX))
                        {
                            char name[64];
                            sscanf(buffer, "map_Ka %s", name);

                            if (name[0] == '.' || name[1] == ':')
                                load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                            else
                                load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        }
                    }
                }
                else if (starts_with(buffer, "map_bump")) // ambient map
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::BUMP_TEX))
                        {
                            char name[64];
                            sscanf(buffer, "map_bump %s", name);

                            if (name[0] == '.' || name[1] == ':')
                                load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                            else
                                load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        }
                    }
                }
                else if (starts_with(buffer, "map_Kd")) // diffuse map
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_TEX))
                        {
                            char name[64];
                            sscanf(buffer, "map_Kd %s", name);

                            if (name[0] == '.' || name[1] == ':')
                                load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                            else
                                load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        }
                    }
                }
                else if (starts_with(buffer, "map_Ks")) // specular map
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_TEX))
                        {
                            char name[64];
                            sscanf(buffer, "map_Ks %s", name);

                            if (name[0] == '.' || name[1] == ':')
                                load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                            else
                                load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        }
                    }
                }
                else if (starts_with(buffer, "map_Ns")) // specular intensity map
                {
                    for(i32 i = 0; i < current->pass_count; i++)
                    {
                        i32 handle = current->passes[i];
                        rendering::Material &material = temp_materials[handle];
                        
                        if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_INTENSITY_TEX))
                        {
                            char name[64];
                            sscanf(buffer, "map_Ns %s", name);

                            if (name[0] == '.' || name[1] == ':')
                                load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                            else
                                load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, u->texture);
                        }
                    }
                }
            }

            fclose(file);
        }
        else
        {
            error("Can't load .mtl file", file_path);
        }

        end_temporary_memory(temp_block);
    }
    
	struct _VertexPtrs
	{
		Vertex *vertices;
		Vertex *final_vertices;
		math::Vec3 *normals;
		math::Vec2 *uvs;
		Face *faces;

		i32 vertex_count;
		i32 final_vertex_count;
		i32 face_count;
		i32 normal_count;
		i32 uv_count;
	};

	static void parse_obj_object(const char *geometry_name, _VertexPtrs *vertex_ptrs, char **source, MeshObjectData *obj_data, MaterialPair *pairs, i32 mat_count, Renderer *renderer)
    {
        b32 with_uvs = false;
        b32 with_normals = false;

        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;
        
        char buffer[256];

        char *last_ptr = *source;
        
        while (read_line_from_buffer(buffer, 256, source))
        {
            if(starts_with(buffer, "g"))
            {
                char name[256];
                sscanf(buffer, "g %[^\n]", name);
                if(strcmp(name, geometry_name) != 0) 
                {
                    *source = last_ptr;
                    break;
                }
            }
            else if (starts_with(buffer, "usemtl")) // Used material for geometry
            {
                if(pairs)
                {
                    char name[32];
                    sscanf(buffer, "usemtl %[^\n\r]", name);
                    
                    // for(i32 i = 0; i < mat_count; i++)
                    // {
                    //     const MaterialPair &pair = pairs[i];
                    //     if(strcmp(pair.name, name) == 0)
                    //     {
                    //         obj_data->pair_index = i;
                    //         break;
                    //     }
                    // }
                }
            }
            else if (starts_with(buffer, "vn")) // vertex normal
            {
                with_normals = true;
                math::Vec3 normal(0.0f);
                sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                vertex_ptrs->normals[vertex_ptrs->normal_count++] = normal;
            }
            else if (starts_with(buffer, "vt")) // vertex uv
            {
                with_uvs = true;
                math::Vec2 uv(0.0f);
                sscanf(buffer, "vt %f %f", &uv.x, &uv.y);
                uv.y = 1.0f - uv.y;
                vertex_ptrs->uvs[vertex_ptrs->uv_count++] = uv;
            }
			else if (starts_with(buffer, "v ")) // vertex
			{
				Vertex vertex = {};
				sscanf(buffer, "v %f %f %f", &vertex.position.x, &vertex.position.y, &vertex.position.z);

				min_x = MIN(min_x, vertex.position.x);
				min_y = MIN(min_y, vertex.position.y);
				min_z = MIN(min_z, vertex.position.z);
				max_x = MAX(max_x, vertex.position.x);
				max_y = MAX(max_y, vertex.position.y);
				max_z = MAX(max_z, vertex.position.z);

				vertex_ptrs->vertices[vertex_ptrs->vertex_count++] = vertex;
			}
            else if (starts_with(buffer, "f")) // face
            {
                Face face = {};
                math::Vec3i normal_indices = {};
                math::Vec3i uv_indices = {};

                if (with_uvs && with_normals)
                {
                    sscanf(buffer, "f %hd/%d/%d %hd/%d/%d %hd/%d/%d", &face.indices[0], &uv_indices.x, &normal_indices.x, &face.indices[1], &uv_indices.y, &normal_indices.y, &face.indices[2], &uv_indices.z, &normal_indices.z);
                }
                else if (with_uvs)
                {
                    sscanf(buffer, "f %hd/%d %hd/%d %hd/%d", &face.indices[0], &uv_indices.x, &face.indices[1], &uv_indices.y, &face.indices[2], &uv_indices.z);
                }

                else if (with_normals)
                {
                    sscanf(buffer, "f %hd//%d %hd//%d %hd//%d", &face.indices[0], &normal_indices.x, &face.indices[1], &normal_indices.y, &face.indices[2], &normal_indices.z);
                }

                // The obj-format was made by geniuses and therefore the indices are not 0-indexed. Such wow.
                face.indices[0] -= 1;
                face.indices[1] -= 1;
                face.indices[2] -= 1;

                b32 should_add = false;
                Vertex v1 = vertex_ptrs->vertices[face.indices[0]];
                math::Vec2 uv1(0.0f);
                math::Vec3 n1(0.0f);

                if (with_uvs)
                {
                    uv1 = vertex_ptrs->uvs[uv_indices.x - 1];
                }

                if (with_normals)
                {
                    n1 = vertex_ptrs->normals[normal_indices.x - 1];
                }

                face.indices[0] = (u16)check_for_identical_vertex(v1, uv1, n1, with_normals, with_uvs, vertex_ptrs->final_vertices, vertex_ptrs->final_vertex_count, &should_add);

                if (should_add)
                {
					vertex_ptrs->final_vertices[vertex_ptrs->final_vertex_count++] = v1;
                }

                should_add = false;
                Vertex &v2 = vertex_ptrs->vertices[face.indices[1]];
                math::Vec2 uv2(0.0f);
                math::Vec3 n2(0.0f);

                if (with_uvs)
                {
                    uv2 = vertex_ptrs->uvs[uv_indices.y - 1];
                }

                if (with_normals)
                {
                    n2 = vertex_ptrs->normals[normal_indices.y - 1];
                }

                face.indices[1] = (u16)check_for_identical_vertex(v2, uv2, n2, with_normals, with_uvs, vertex_ptrs->final_vertices, vertex_ptrs->final_vertex_count, &should_add);

                if (should_add)
                {
                    vertex_ptrs->final_vertices[vertex_ptrs->final_vertex_count++] = v2;
                }

                should_add = false;
                Vertex &v3 = vertex_ptrs->vertices[face.indices[2]];

                math::Vec2 uv3(0.0f);
                math::Vec3 n3(0.0f);

                if (with_uvs)
                {
                    uv3 = vertex_ptrs->uvs[uv_indices.z - 1];
                }

                if (with_normals)
                {
                    n3 = vertex_ptrs->normals[normal_indices.z - 1];
                }

                face.indices[2] = (u16)check_for_identical_vertex(v3, uv3, n3, with_normals, with_uvs, vertex_ptrs->final_vertices, vertex_ptrs->final_vertex_count, &should_add);

                if (should_add)
                {
					vertex_ptrs->final_vertices[vertex_ptrs->final_vertex_count++] = v3;
                }

				vertex_ptrs->faces[vertex_ptrs->face_count++] = face;
            }

            last_ptr = *source;
        }

        Mesh mesh;
        mesh.vertices = push_array(&renderer->mesh_arena, vertex_ptrs->final_vertex_count, Vertex);
        mesh.faces = push_array(&renderer->mesh_arena, vertex_ptrs->face_count, Face);
        mesh.vertex_count = vertex_ptrs->final_vertex_count;
        mesh.face_count = vertex_ptrs->face_count;

        memcpy(mesh.vertices, vertex_ptrs->final_vertices, mesh.vertex_count * sizeof(Vertex));
        memcpy(mesh.faces, vertex_ptrs->faces, mesh.face_count * sizeof(Face));

		vertex_ptrs->final_vertex_count = 0;
		vertex_ptrs->face_count = 0;

        obj_data->mesh_scale = math::Vec3(max_x - min_x, max_y - min_y, max_z - min_z);
        math::BoundingBox box = {};
        box.min = math::Vec3(min_x, min_y, min_z);
        box.max = math::Vec3(max_x, max_y, max_z);
        obj_data->bounding_box = box;
        obj_data->buffer = create_buffers_from_mesh(renderer, mesh, 0, with_normals, with_uvs);
    }

    static void parse_obj_object(const char *geometry_name, _VertexPtrs *vertex_ptrs, char **source, MeshObjectData *obj_data, Renderer *renderer)
    {
        b32 with_uvs = false;
        b32 with_normals = false;

        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;
        
        char buffer[256];

        char *last_ptr = *source;
        
        while (read_line_from_buffer(buffer, 256, source))
        {
            if(starts_with(buffer, "g"))
            {
                char name[256];
                sscanf(buffer, "g %[^\n]", name);
                if(strcmp(name, geometry_name) != 0) 
                {
                    *source = last_ptr;
                    break;
                }
            }
            else if (starts_with(buffer, "usemtl")) // Used material for geometry
            {
                obj_data->use_material = true;
                sscanf(buffer, "usemtl %[^\n\r]", obj_data->material_name);
            }
            else if (starts_with(buffer, "vn")) // vertex normal
            {
                with_normals = true;
                math::Vec3 normal(0.0f);
                sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                vertex_ptrs->normals[vertex_ptrs->normal_count++] = normal;
            }
            else if (starts_with(buffer, "vt")) // vertex uv
            {
                with_uvs = true;
                math::Vec2 uv(0.0f);
                sscanf(buffer, "vt %f %f", &uv.x, &uv.y);
                uv.y = 1.0f - uv.y;
                vertex_ptrs->uvs[vertex_ptrs->uv_count++] = uv;
            }
			else if (starts_with(buffer, "v ")) // vertex
			{
				Vertex vertex = {};
				sscanf(buffer, "v %f %f %f", &vertex.position.x, &vertex.position.y, &vertex.position.z);

				min_x = MIN(min_x, vertex.position.x);
				min_y = MIN(min_y, vertex.position.y);
				min_z = MIN(min_z, vertex.position.z);
				max_x = MAX(max_x, vertex.position.x);
				max_y = MAX(max_y, vertex.position.y);
				max_z = MAX(max_z, vertex.position.z);

				vertex_ptrs->vertices[vertex_ptrs->vertex_count++] = vertex;
			}
            else if (starts_with(buffer, "f")) // face
            {
                Face face = {};
                math::Vec3i normal_indices = {};
                math::Vec3i uv_indices = {};

                if (with_uvs && with_normals)
                {
                    sscanf(buffer, "f %hd/%d/%d %hd/%d/%d %hd/%d/%d", &face.indices[0], &uv_indices.x, &normal_indices.x, &face.indices[1], &uv_indices.y, &normal_indices.y, &face.indices[2], &uv_indices.z, &normal_indices.z);
                }
                else if (with_uvs)
                {
                    sscanf(buffer, "f %hd/%d %hd/%d %hd/%d", &face.indices[0], &uv_indices.x, &face.indices[1], &uv_indices.y, &face.indices[2], &uv_indices.z);
                }

                else if (with_normals)
                {
                    sscanf(buffer, "f %hd//%d %hd//%d %hd//%d", &face.indices[0], &normal_indices.x, &face.indices[1], &normal_indices.y, &face.indices[2], &normal_indices.z);
                }

                // The obj-format was made by geniuses and therefore the indices are not 0-indexed. Such wow.
                face.indices[0] -= 1;
                face.indices[1] -= 1;
                face.indices[2] -= 1;

                b32 should_add = false;
                Vertex v1 = vertex_ptrs->vertices[face.indices[0]];
                math::Vec2 uv1(0.0f);
                math::Vec3 n1(0.0f);

                if (with_uvs)
                {
                    uv1 = vertex_ptrs->uvs[uv_indices.x - 1];
                }

                if (with_normals)
                {
                    n1 = vertex_ptrs->normals[normal_indices.x - 1];
                }

                face.indices[0] = (u16)check_for_identical_vertex(v1, uv1, n1, with_normals, with_uvs, vertex_ptrs->final_vertices, vertex_ptrs->final_vertex_count, &should_add);

                if (should_add)
                {
					vertex_ptrs->final_vertices[vertex_ptrs->final_vertex_count++] = v1;
                }

                should_add = false;
                Vertex &v2 = vertex_ptrs->vertices[face.indices[1]];
                math::Vec2 uv2(0.0f);
                math::Vec3 n2(0.0f);

                if (with_uvs)
                {
                    uv2 = vertex_ptrs->uvs[uv_indices.y - 1];
                }

                if (with_normals)
                {
                    n2 = vertex_ptrs->normals[normal_indices.y - 1];
                }

                face.indices[1] = (u16)check_for_identical_vertex(v2, uv2, n2, with_normals, with_uvs, vertex_ptrs->final_vertices, vertex_ptrs->final_vertex_count, &should_add);

                if (should_add)
                {
                    vertex_ptrs->final_vertices[vertex_ptrs->final_vertex_count++] = v2;
                }

                should_add = false;
                Vertex &v3 = vertex_ptrs->vertices[face.indices[2]];

                math::Vec2 uv3(0.0f);
                math::Vec3 n3(0.0f);

                if (with_uvs)
                {
                    uv3 = vertex_ptrs->uvs[uv_indices.z - 1];
                }

                if (with_normals)
                {
                    n3 = vertex_ptrs->normals[normal_indices.z - 1];
                }

                face.indices[2] = (u16)check_for_identical_vertex(v3, uv3, n3, with_normals, with_uvs, vertex_ptrs->final_vertices, vertex_ptrs->final_vertex_count, &should_add);

                if (should_add)
                {
					vertex_ptrs->final_vertices[vertex_ptrs->final_vertex_count++] = v3;
                }

				vertex_ptrs->faces[vertex_ptrs->face_count++] = face;
            }

            last_ptr = *source;
        }

        Mesh mesh;
        mesh.vertices = push_array(&renderer->mesh_arena, vertex_ptrs->final_vertex_count, Vertex);
        mesh.faces = push_array(&renderer->mesh_arena, vertex_ptrs->face_count, Face);
        mesh.vertex_count = vertex_ptrs->final_vertex_count;
        mesh.face_count = vertex_ptrs->face_count;

        memcpy(mesh.vertices, vertex_ptrs->final_vertices, mesh.vertex_count * sizeof(Vertex));
        memcpy(mesh.faces, vertex_ptrs->faces, mesh.face_count * sizeof(Face));

		vertex_ptrs->final_vertex_count = 0;
		vertex_ptrs->face_count = 0;

        obj_data->mesh_scale = math::Vec3(max_x - min_x, max_y - min_y, max_z - min_z);
        math::BoundingBox box = {};
        box.min = math::Vec3(min_x, min_y, min_z);
        box.max = math::Vec3(max_x, max_y, max_z);
        obj_data->bounding_box = box;
        obj_data->buffer = create_buffers_from_mesh(renderer, mesh, 0, with_normals, with_uvs);
    }
    
	struct MeshDataCounts
	{
		i32 vertex_count;
		i32 normal_count;
		i32 uv_count;
		i32 face_count;
	};

	static MeshDataCounts get_data_counts(char *source)
	{
		MeshDataCounts counts = {};
		
		char buffer[256];

		while (read_line(buffer, 256, &source))
		{
			if (starts_with(buffer, "vn"))
			{
				counts.normal_count++;
			}
			else if (starts_with(buffer, "vt"))
			{
				counts.uv_count++;
			}
			else if (starts_with(buffer, "v "))
			{
				counts.vertex_count++;
			}
			else if (starts_with(buffer, "f"))
			{
				counts.face_count++;
			}
		}

		return counts;
	}

	static MeshObjectInfo load_obj(Renderer *renderer, const char *file_path, rendering::PassMaterial *pass_materials, i32 pass_material_count, rendering::MaterialPair *pairs = nullptr, i32 count = 0)
	{
		MeshObjectInfo obj_info = {};
	    
        obj_info.object_count = 0;
        
        FILE *file = fopen(file_path, "rb");

        if(file)
        {
            MaterialPair std_pairs[64];
            i32 pair_count = count;
            
            MaterialPair *mat_pairs;
            
            if(count > 0)
            {
                mat_pairs = pairs;
            }
            else
            {
                mat_pairs = std_pairs;
            }
            
            char *source = read_file_into_buffer(file);
			char *source_copy = source;

			MeshDataCounts data_counts = get_data_counts(source_copy);
			_VertexPtrs ptrs;

			ptrs.vertex_count = 0;
			ptrs.final_vertex_count = 0;
			ptrs.normal_count = 0;
			ptrs.uv_count = 0;
			ptrs.face_count = 0;

			ptrs.vertices = (Vertex*)malloc(sizeof(Vertex) * data_counts.vertex_count);
			
			ptrs.normals = (math::Vec3*)malloc(sizeof(math::Vec3) * data_counts.normal_count);
			ptrs.uvs = (math::Vec2*)malloc(sizeof(math::Vec2) * data_counts.uv_count);
			ptrs.faces = (Face*)malloc(sizeof(Face) * data_counts.face_count);
            ptrs.final_vertices = (Vertex*)malloc(sizeof(Vertex) * 3 * data_counts.face_count);
            
			char buffer[256];
            
            while (read_line(buffer, 256, &source))
            {
                if (starts_with(buffer, "mtllib")) // Material file
                {
                    char mtl_file_name[32];
                    
                    // Read the material file-name
                    sscanf(buffer, "mtllib %s", mtl_file_name);

                    // Find the directory of the file
                    size_t index = 0;
                    for (size_t i = 0; i < strlen(file_path); i++)
                    {
                        if (file_path[i] == '/')
                        {
                            index = i + 1;
                        }
                    }

                    auto temp_block = begin_temporary_memory(&renderer->temp_arena);

                    char *dir = push_string(temp_block.arena, index);
                    strncpy(dir, file_path, index);

                    dir[index] = 0;
                    //char *material_file_path = concat(dir, mtl_file_name, &renderer->temp_arena);
                    //load_materials_from_mtl(pass_materials, pass_material_count, mat_pairs, &pair_count, material_file_path, renderer);

					end_temporary_memory(temp_block);
                }
                else if (starts_with(buffer, "g")) // we're starting with new geometry
                {
                    char name[256];
                    sscanf(buffer, "g %[^\n]", name);
                    //read_line(buffer, 256, &source);
                    
                    parse_obj_object(name, &ptrs, &source, &obj_info.data[obj_info.object_count++], mat_pairs, pair_count, renderer);
                }
            }

            free(source_copy);
            free(ptrs.vertices);
			free(ptrs.final_vertices);
			free(ptrs.normals);
			free(ptrs.uvs);
			free(ptrs.faces);
        }
        else
        {
            debug("File '%s' could not be loaded\n", file_path);
        }

        return obj_info;
    }

    
	static MeshObjectInfo load_obj(Renderer *renderer, const char *file_path)
	{
		MeshObjectInfo obj_info = {};
        obj_info.object_count = 0;
        
        FILE *file = fopen(file_path, "rb");

        if(file)
        {
            char *source = read_file_into_buffer(file);
			char *source_copy = source;

			MeshDataCounts data_counts = get_data_counts(source_copy);
			_VertexPtrs ptrs;

			ptrs.vertex_count = 0;
			ptrs.final_vertex_count = 0;
			ptrs.normal_count = 0;
			ptrs.uv_count = 0;
			ptrs.face_count = 0;

			ptrs.vertices = (Vertex*)malloc(sizeof(Vertex) * data_counts.vertex_count);
			
			ptrs.normals = (math::Vec3*)malloc(sizeof(math::Vec3) * data_counts.normal_count);
			ptrs.uvs = (math::Vec2*)malloc(sizeof(math::Vec2) * data_counts.uv_count);
			ptrs.faces = (Face*)malloc(sizeof(Face) * data_counts.face_count);
            ptrs.final_vertices = (Vertex*)malloc(sizeof(Vertex) * 3 * data_counts.face_count);
            
			char buffer[256];
            
            while (read_line(buffer, 256, &source))
            {
                if (starts_with(buffer, "mtllib")) // Material file
                {
                    char mtl_file_name[32];
                    
                    // Read the material file-name
                    sscanf(buffer, "mtllib %s", mtl_file_name);

                    // Find the directory of the file
                    size_t index = 0;
                    for (size_t i = 0; i < strlen(file_path); i++)
                    {
                        if (file_path[i] == '/')
                        {
                            index = i + 1;
                        }
                    }

                    auto temp_block = begin_temporary_memory(&renderer->temp_arena);

                    char *dir = push_string(temp_block.arena, index);
                    strncpy(dir, file_path, index);

                    dir[index] = 0;
                    char *material_file_path = concat(dir, mtl_file_name, &renderer->temp_arena);

                    obj_info.has_mtl = true;
                    strcpy(obj_info.mtl_file_path, material_file_path);
                    
					end_temporary_memory(temp_block);
                }
                else if (starts_with(buffer, "g")) // we're starting with new geometry
                {
                    char name[256];
                    sscanf(buffer, "g %[^\n]", name);
                    parse_obj_object(name, &ptrs, &source, &obj_info.data[obj_info.object_count++], renderer);
                }
            }

            free(source_copy);
            free(ptrs.vertices);
			free(ptrs.final_vertices);
			free(ptrs.normals);
			free(ptrs.uvs);
			free(ptrs.faces);
        }
        else
        {
            debug("File '%s' could not be loaded\n", file_path);
        }

        return obj_info;
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, r32 value)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::FLOAT);
                mapping.float_val = value;
                return;
            }
        }
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, math::Vec2 value)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::FLOAT2);
                mapping.float2_val = value;
                return;
            }
        }
    }
    
    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, math::Vec3 value)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::FLOAT3);
                mapping.float3_val = value;
                return;
            }
        }
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, math::Vec4 value)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::FLOAT4);
                mapping.float4_val = value;
                return;
            }
        }
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, math::Mat4 value)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::MAT4);
                mapping.mat4_val = value;
                return;
            }
        }
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, TextureHandle texture)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::TEXTURE);
                mapping.texture = texture;
                return;
            }
        }
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, MSTextureHandle texture)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::MS_TEXTURE);
                mapping.ms_texture = texture;
                return;
            }
        }
    }

    static void set_custom_mapped_uniform_value(Renderer *renderer, const char *name, i32 value)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping &mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, name) == 0)
            {
                assert(mapping.type == ValueType::INTEGER || mapping.type == ValueType::BOOL);
                if (mapping.type == ValueType::BOOL)
                {
                    mapping.boolean_val = value;
                    return;
                }
                else if (mapping.type == ValueType::INTEGER)
                {
                    mapping.integer_val = value;
                    return;
                }
            }
        }
    }
    
    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, r32 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::FLOAT);
                va.float_val = value;
                return;
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT);
                u_v.float_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, r32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, math::Vec2 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::FLOAT2);
                va.float2_val = value;
                return;
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT2);
                u_v.float2_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, math::Vec2 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, math::Vec3 value)
    {
		for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
		{
			VertexAttribute& va = material.instanced_vertex_attributes[i].attribute;
			if (strcmp(va.name, name) == 0)
			{
				assert(va.type == ValueType::FLOAT3);
				va.float3_val = value;
				return;
			}
		}

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT3);
                u_v.float3_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, math::Vec3 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, math::Vec4 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::FLOAT4);
                va.float4_val = value;
                return;
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT4);
                u_v.float4_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, math::Vec4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, i32 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::INTEGER || va.type == ValueType::BOOL);
                if (va.type == ValueType::BOOL)
                {
                    va.boolean_val = value;
                    return;
                }
                else if (va.type == ValueType::INTEGER)
                {
                    va.integer_val = value;
                    return;
                }
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::INTEGER || u_v.uniform.type == ValueType::BOOL);
                if (u_v.uniform.type == ValueType::BOOL)
                {
                    u_v.boolean_val = value;
                    break;
                }
                else if (u_v.uniform.type == ValueType::INTEGER)
                {
                    u_v.integer_val = value;
                    break;
                }
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, i32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, math::Mat4 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::MAT4);
                if (va.type == ValueType::MAT4)
                {
                    va.mat4_val = value;
                    return;
                }
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::MAT4);
                u_v.mat4_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, math::Mat4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, TextureHandle value)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::TEXTURE);
                assert(value.handle != 0);
                u_v.texture = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, TextureHandle value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer *renderer, Material &material, const char *name, MSTextureHandle value)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::MS_TEXTURE);
                u_v.ms_texture = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer *renderer, MaterialInstanceHandle handle, const char *name, MSTextureHandle value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_array_value(Renderer *renderer, Material &material, const char *array_name, i32 index, const char *variable_name, r32 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];

            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT);
                        u_v.float_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, r32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer *renderer, Material &material, const char *array_name, i32 index, const char *variable_name, math::Vec2 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT2);
                        u_v.float2_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec2 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer *renderer, Material &material, const char *array_name, i32 index, const char *variable_name, math::Vec3 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT3);
                        u_v.float3_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec3 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer *renderer, Material& material, const char *array_name, i32 index, const char *variable_name, math::Vec4 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];
                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT4);
                        u_v.float4_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer *renderer, Material &material, const char *array_name, i32 index, const char *variable_name, i32 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        if (u_v.uniform.type == ValueType::BOOL)
                        {
                            u_v.boolean_val = value;
                            break;
                        }
                        else if (u_v.uniform.type == ValueType::INTEGER)
                        {
                            u_v.integer_val = value;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, i32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer *renderer, Material& material, const char *array_name, i32 index, const char *variable_name, math::Mat4 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::MAT4);
                        u_v.mat4_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Mat4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

// @Incomplete: Add MSTexture support for arrays
    static void set_uniform_array_value(Renderer *renderer, Material &material, const char *array_name, i32 index, const char *variable_name, rendering::TextureHandle value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::TEXTURE);
                        u_v.texture = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer *renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, TextureHandle value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void recompute_transform(Transform& transform)
    {
        transform.model = math::Mat4(1.0f);
        transform.model = math::scale(transform.model, transform.scale);
        transform.model = math::to_matrix(transform.orientation) * transform.model;
        transform.model = math::translate(transform.model, transform.position);
        transform.dirty = false;
    }

    static Transform create_transform(Transform t)
    {
        Transform result = {};
        result.position = t.position;
        result.scale = t.scale;
        result.orientation = t.orientation;
        result.euler_angles = t.euler_angles;

        result.model = t.model;
        
        if(t.dirty)
        {
            recompute_transform(result);
        }

        return result;
    }

    static Transform create_transform(math::Vec3 position, math::Vec3 scale, math::Vec3 rotation)
    {
        Transform t = {};
        t.position = position;
        t.scale = scale;
        t.orientation = math::rotate(math::Quat(), rotation);
        t.euler_angles = rotation;

        recompute_transform(t);
        return t;
    }

    static Transform create_transform(math::Vec3 position, math::Vec3 scale, math::Quat orientation)
    {
        Transform t = {};
        t.position = position;
        t.scale = scale;
        t.orientation = orientation;
        t.euler_angles = math::to_euler_angles(t.orientation);
        t.model = math::Mat4(1.0);

        recompute_transform(t);
        return t;
    }

    static void translate_x(Transform& transform, r32 x)
    {
        transform.dirty |= x != 0.0f;
        transform.position.x += x;
    }

    static void translate_y(Transform& transform, r32 y)
    {
        transform.dirty |= y != 0.0f;
        transform.position.y += y;
    }

    static void translate_z(Transform& transform, r32 z)
    {
        transform.dirty |= z != 0.0f;
        transform.position.z += z;
    }

    static void translate(Transform& transform, math::Vec3 translation)
    {
        transform.dirty |= translation.x != 0.0f || translation.y != 0.0f || translation.z != 0.0f;
        transform.position += translation;
    }

    static void rotate(Transform& transform, math::Vec3 rotation)
    {
        transform.dirty |= rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f;
        transform.orientation = math::rotate(transform.orientation, rotation);
        transform.euler_angles = rotation;
    }

    static void rotate(Transform& transform, math::Quat rotation)
    {
        transform.dirty |= rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f || rotation.w != 0.0f;
        transform.orientation = math::rotate(transform.orientation, rotation);
        transform.euler_angles = math::to_euler_angles(transform.orientation);
    }

    static void set_rotation(Transform& transform, math::Vec3 rotation)
    {
        transform.dirty |= transform.euler_angles.x != rotation.x ||
            transform.euler_angles.y != rotation.y ||
            transform.euler_angles.z != rotation.z;
        transform.euler_angles = rotation;
        transform.orientation = math::rotate(math::Quat(), rotation);
    }

    static void set_rotation(Transform& transform, math::Quat orientation)
    {
        transform.dirty |= transform.orientation.x != orientation.x ||
            transform.orientation.y != orientation.y ||
            transform.orientation.z != orientation.z ||
            transform.orientation.w != orientation.w;
        transform.orientation = orientation;
        transform.euler_angles = math::to_euler_angles(orientation);
    }

    static void set_rotation_x(Transform& transform, r32 x)
    {
        transform.dirty |= transform.euler_angles.x != x;
        transform.euler_angles.x = x;
        transform.orientation = math::rotate(math::Quat(), transform.euler_angles);
    }

    static void set_rotation_y(Transform& transform, r32 y)
    {
        transform.dirty |= transform.euler_angles.y != y;
        transform.euler_angles.y = y;
        transform.orientation = math::rotate(math::Quat(), transform.euler_angles);
    }

    static void set_rotation_z(Transform& transform, r32 z)
    {
        transform.dirty |= transform.euler_angles.z != z;
        transform.euler_angles.x = z;
        transform.orientation = math::rotate(math::Quat(), transform.euler_angles);
    }

    static void scale(Transform& transform, math::Vec3 scale)
    {
        transform.dirty |= scale.x != 0.0f || scale.y != 0.0f || scale.z != 0.0f;
        transform.scale *= scale;
    }

    static void set_scale(Transform& transform, math::Vec3 scale)
    {
        transform.dirty |= transform.scale.x != scale.x ||
            transform.scale.y != scale.y ||
            transform.scale.z != scale.z;
        
        transform.scale = scale;
    }

    static void set_scale_x(Transform& transform, r32 x)
    {
        transform.dirty |= x != transform.scale.x;
        transform.scale.x = x;
    }

    static void set_scale_y(Transform& transform, r32 y)
    {
        transform.dirty |= y != transform.scale.y;
        transform.scale.y = y;
    }

    static void set_scale_z(Transform& transform, r32 z)
    {
        transform.dirty |= z != transform.scale.z;
        transform.scale.z = z;
    }
    
    static void set_position(Transform& transform, math::Vec3 position)
    {
        transform.dirty |= transform.position.x != position.x || transform.position.y != position.y || transform.position.z != position.z;
        transform.position = position;
    }

    static void set_position_x(Transform& transform, r32 x)
    {
        transform.dirty |= transform.position.x != x;
        transform.position.x = x;
    }

    static void set_position_y(Transform& transform, r32 y)
    {
        transform.dirty |= transform.position.y != y;
        transform.position.y = y;
    }

    static void set_position_z(Transform& transform, r32 z)
    {
        transform.dirty |= transform.position.z != z;
        transform.position.z = z;
    }
    
    static void push_shadow_buffer(Renderer *renderer, BufferHandle buffer_handle, Transform &transform)
    {
        ShadowCommand shadow_command = {};
        shadow_command.buffer = buffer_handle;
        shadow_command.transform = transform;
        renderer->render.shadow_commands[renderer->render.shadow_command_count++] = shadow_command;
    }

    static void push_line_to_render_pass(Renderer *renderer, math::Vec3 p0, math::Vec3 p1, r32 thickness, math::Rgba color, Transform transform, MaterialInstanceHandle material_instance_handle, RenderPassHandle render_pass_handle, CommandType type)
    {
        RenderPass &pass = renderer->render.passes[render_pass_handle.handle - 1];
        assert(pass.commands.render_command_count < global_max_render_commands);

        RenderCommand render_command = {};
        render_command.type = RenderCommandType::LINE;
        render_command.material = material_instance_handle;
        render_command.transform = transform;

        render_command.line.p0 = p0;
        render_command.line.p1 = p1;
        render_command.line.thickness = thickness;
        render_command.line.color = color;

        if(type == CommandType::WITH_DEPTH)
            pass.commands.render_commands[pass.commands.render_command_count++] = render_command;
        else
            pass.commands.depth_free_commands[pass.commands.depth_free_command_count++] = render_command;
    }

    static void push_buffer_to_render_pass(Renderer *renderer, BufferHandle buffer_handle, MaterialInstanceHandle material_instance_handle, Transform &transform, RenderPassHandle render_pass_handle, CommandType type = CommandType::WITH_DEPTH, PrimitiveType primitive_type = PrimitiveType::TRIANGLES)
    {
        RenderPass &pass = renderer->render.passes[render_pass_handle.handle - 1];
        assert(pass.commands.render_command_count < global_max_render_commands);

        RenderCommand render_command = {};
        render_command.type = RenderCommandType::BUFFER;
        render_command.buffer.primitive_type = primitive_type;
        render_command.buffer.buffer = buffer_handle;
        render_command.material = material_instance_handle;
        render_command.transform = transform;

        if(type == CommandType::WITH_DEPTH)
            pass.commands.render_commands[pass.commands.render_command_count++] = render_command;
        else
            pass.commands.depth_free_commands[pass.commands.depth_free_command_count++] = render_command;
    }

    static void push_instanced_buffer_to_render_pass(Renderer *renderer, i32 count, BufferHandle buffer_handle, MaterialInstanceHandle material_instance_handle, RenderPassHandle render_pass_handle, CommandType type = CommandType::WITH_DEPTH, PrimitiveType primitive_type = PrimitiveType::TRIANGLES)
    {
        RenderCommand render_command = {};
        render_command.type = RenderCommandType::BUFFER;
        render_command.buffer.primitive_type = primitive_type;
        render_command.count = count;
        render_command.buffer.buffer = buffer_handle;
        render_command.material = material_instance_handle;
        RenderPass &pass = renderer->render.passes[render_pass_handle.handle - 1];

        if(type == CommandType::WITH_DEPTH)
            pass.commands.render_commands[pass.commands.render_command_count++] = render_command;
        else
            pass.commands.depth_free_commands[pass.commands.depth_free_command_count++] = render_command;
    }

    static void push_instanced_buffer_to_render_pass(Renderer *renderer, i32 count, BufferHandle buffer_handle, MaterialInstanceHandle material_instance_handle, RenderPassHandle render_pass_handle)
    {
    }

    static math::Vec2 get_relative_size(Renderer *renderer, math::Vec2 size, u64 scaling_flags = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec2 scaled_size;

        if (scaling_flags & UIScalingFlag::SCALE_WITH_WIDTH)
        {
            scaled_size.x = (size.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
            scaled_size.y = (size.y / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
        }
        else if (scaling_flags & UIScalingFlag::SCALE_WITH_HEIGHT)
        {
            scaled_size.x = (size.x / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
            scaled_size.y = (size.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
        }
        else
        {
            scaled_size.x = (size.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;

            if (scaling_flags & UIScalingFlag::KEEP_ASPECT_RATIO)
            {
                r32 ratio = size.y / size.x;
                scaled_size.y = scaled_size.x * ratio;
            }
            else
            {
                scaled_size.y = (size.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
            }
        }

        return scaled_size;
    }

// @Note Gets info about UI position for rendering things relative to each other
// We often want to be able to render things next to each other perfectly on different scales
// This function should help with that
// Parameters:
// renderer:      The renderer
// position:      The position of the original quad
// relative_size: The size of the original quad
// size:          The size of the thing you want to render next to the original
// relative:      The relative flag (top, bottom, left, right)
// centered:      Whether or not the original quad was centered (need to know this for origin etc.)
// scaling_flags: How do we scale these UI elements?
// origin:        The origin
    static RelativeUIQuadInfo get_relative_info(Renderer *renderer, math::Vec2 position, math::Vec2 relative_size, math::Vec2 size, RelativeFlag relative, u64 scaling_flags = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec3 pos;
        pos.x = (position.x / UI_COORD_DIMENSION) * resolution_scale.x;
        pos.y = (position.y / UI_COORD_DIMENSION) * resolution_scale.y;
        pos.z = 0.0f;

        math::Vec3 scaled_size = math::Vec3(get_relative_size(renderer, relative_size, scaling_flags), 0.0f);

        math::Vec3 relative_pos = math::Vec3(pos.x, pos.y, 0.0f);

        math::Vec3 new_size = math::Vec3(get_relative_size(renderer, size, scaling_flags), 0.0f);

        switch (relative)
        {
        case RELATIVE_TOP:
        {
            relative_pos.y += (i32)scaled_size.y;
        }
        break;
        case RELATIVE_LEFT:
        {
            relative_pos.x -= (i32)scaled_size.x + new_size.x;
        }
        break;
        case RELATIVE_RIGHT:
        {
            relative_pos.x += (i32)scaled_size.x;
        }
        break;
        case RELATIVE_BOTTOM:
        {
            relative_pos.y -= (i32)scaled_size.y;
        }
        break;
        }

        math::Vec2 ui_position = math::Vec2(0, 0);
        ui_position.x = ((relative_pos.x / (r32)resolution_scale.x) * UI_COORD_DIMENSION);
        ui_position.y = ((relative_pos.y / (r32)resolution_scale.y) * UI_COORD_DIMENSION);

        return {math::Vec2(relative_pos.x, relative_pos.y), math::Vec2(new_size.x, new_size.y), ui_position};
    }

    static void calculate_current_x_from_line_data(r32 *x, math::Vec2 text_size, u64 alignment_flags)
    {
        if (alignment_flags & UIAlignment::RIGHT)
        {
            *x -= text_size.x;
        }
        else if ((alignment_flags & UIAlignment::LEFT) == 0)
        {
            *x -= text_size.x / 2.0f;
        }
    }

    static math::Rect scale_clip_rect(Renderer *renderer, math::Rect clip_rect, u64 ui_scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        math::Vec2i resolution_scale = get_scale(renderer);
        math::Rect scaled_clip_rect;

        scaled_clip_rect.x = (clip_rect.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
        scaled_clip_rect.y = (clip_rect.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;

        r32 clip_ratio = clip_rect.height / clip_rect.width;
        scaled_clip_rect.width = (clip_rect.width / UI_COORD_DIMENSION) * (r32)resolution_scale.x;

        if (ui_scaling_flag & UIScalingFlag::KEEP_ASPECT_RATIO)
        {
            scaled_clip_rect.height = scaled_clip_rect.width * clip_ratio;
        }
        else
        {
            scaled_clip_rect.height = (clip_rect.height / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
        }

        if(scaled_clip_rect.x < 0.0f)
        {
            scaled_clip_rect.width = scaled_clip_rect.width + scaled_clip_rect.x;
            scaled_clip_rect.x = 0.0f;
        }

        if(scaled_clip_rect.y < 0.0f)
        {
            scaled_clip_rect.height = scaled_clip_rect.height + scaled_clip_rect.y;
            scaled_clip_rect.y = 0.0f;
        }

        if(isnan(scaled_clip_rect.width) || scaled_clip_rect.width < 0.0f)
        {
            scaled_clip_rect.width = 0.0f;
        }

        if(isnan(scaled_clip_rect.height) || scaled_clip_rect.height < 0.0f)
        {
            scaled_clip_rect.height = 0.0f;
        }

        return scaled_clip_rect;
    }

    static void generate_text_coordinates(Renderer* renderer, const char *text, TrueTypeFontInfo &font_info, math::Vec3 position, u64 alignment_flags, FramebufferInfo &framebuffer, CharacterData **coords)
    {
        // @Note: Compute the coord buffer
        i32 n = 0;

        LineData line_data = get_line_size_data(text, font_info);

        r32 start_x = position.x;
        r32 y = position.y;
        r32 x = position.x;
        i32 current_line = 0;

        if (alignment_flags & UIAlignment::TOP)
        {
            y -= line_data.total_height;
        }
        else if ((alignment_flags & UIAlignment::BOTTOM) == 0)
        {
            y -= line_data.total_height / 2.0f; 
        }

        y = framebuffer.height - y;

        calculate_current_x_from_line_data(&x, line_data.line_sizes[current_line], alignment_flags);

        // @Note: Compute baseling
        i32 x0, x1, y0, y1;
        stbtt_GetFontBoundingBox(&font_info.info, &x0, &y0, &x1, &y1);
        r32 baseline = font_info.scale * -y0;

        for (u32 i = 0; i < strlen(text); i++)
        {
            char c = text[i];

            if (c == '\n')
            {
                current_line++;

                y += font_info.line_height;
                x = start_x;

                if (current_line != line_data.line_count)
                {
                    calculate_current_x_from_line_data(&x, line_data.line_sizes[current_line], alignment_flags);
                }

                continue;
            }

            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(font_info.char_data, font_info.atlas_width, font_info.atlas_height, text[i] - font_info.first_char, &x, &y, &quad, 1);

            r32 x_min = quad.x0;
            r32 x_max = quad.x1;
            r32 y_min = framebuffer.height - (quad.y0 - baseline);
            r32 y_max = framebuffer.height - (quad.y1 - baseline);

            (*coords)[n++] = {x_max, y_max, quad.s1, quad.t1};
            (*coords)[n++] = {x_max, y_min, quad.s1, quad.t0};
            (*coords)[n++] = {x_min, y_min, quad.s0, quad.t0};
            (*coords)[n++] = {x_min, y_max, quad.s0, quad.t1};
            (*coords)[n++] = {x_max, y_max, quad.s1, quad.t1};
            (*coords)[n++] = {x_min, y_min, quad.s0, quad.t0};

            i32 kerning = stbtt_GetCodepointKernAdvance(&font_info.info, text[i] - font_info.first_char, text[i + 1] - font_info.first_char);
            x += (r32)kerning * font_info.scale;
        }
    }

    static void push_text(Renderer *renderer, CreateTextCommandInfo info, const char *text)
    {
        math::Vec2 size = get_text_size_scaled(renderer, text, get_tt_font_info(renderer, info.font.handle), 0);
        
        if(info.alignment_flags & UIAlignment::LEFT)
        {
            if(info.position.x + size.x < 0.0f || info.position.x > 1000.0f)
            {
                return;
            }
        }
        else
        {
            if(info.position.x < 0.0f || info.position.x - size.x > 1000.0f)
            {
                return;
            }
        }
        
        if(info.alignment_flags & UIAlignment::TOP)
        {
            if(info.position.y - size.y > 1000.0f || info.position.y < 0.0f)
            {
                return;
            }
        }
        else
        {
            if(info.position.y + size.y < 0.0f || info.position.y > 1000.0f)
            {
                return;
            }
        }
        
        RenderPass &pass = renderer->render.ui.pass;
        TextRenderCommand &command = pass.ui.text_commands[pass.ui.text_command_count];

        command.font = info.font;

        FramebufferInfo *framebuffer = &renderer->render.framebuffers[pass.framebuffer.handle - 1];
        TrueTypeFontInfo &font_info = renderer->tt_font_infos[info.font.handle];

        if (font_info.resolution_loaded_for.width != (i32)framebuffer->width || font_info.resolution_loaded_for.height != (i32)framebuffer->height)
        {
            load_font(renderer, font_info.path, font_info.load_size, info.font);
        }

        framebuffer = &renderer->render.framebuffers[pass.framebuffer.handle - 1];

        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec3 pos;
        pos.x = (info.position.x / UI_COORD_DIMENSION) * resolution_scale.x;
        pos.y = (info.position.y / UI_COORD_DIMENSION) * resolution_scale.y;
        pos.z = 0.0f;

        math::Rect scaled_clip_rect = scale_clip_rect(renderer, info.clip_rect, 0);

        if(scaled_clip_rect.width == 0.0f || scaled_clip_rect.height == 0.0f)
        {
            info.clip = false;
        }

        command.clip = info.clip;
        command.clip_rect = scaled_clip_rect;
        command.text_length = strlen(text);

        assert(info.z_layer < Z_LAYERS);

        if(info.has_world_position)
        {
            command.material = renderer->render.materials[renderer->render.ui.font3d_material.handle];

            set_uniform_value(renderer, command.material, "color", info.color);
            set_uniform_value(renderer, command.material, "z", (r32)info.z_layer);
            set_uniform_value(renderer, command.material, "tex", font_info.texture);
            set_uniform_value(renderer, command.material, "projection", info.projection_matrix);
            set_uniform_value(renderer, command.material, "view", info.view_matrix);
            set_uniform_value(renderer, command.material, "position", info.world_position);

            pass.ui.text_z_layers[info.z_layer][pass.ui.text_z_layer_counts[info.z_layer]++] = pass.ui.text_command_count;

            command.shader_handle = command.material.shader;
        }
        else
        {
            command.material = renderer->render.materials[renderer->render.ui.font_material.handle];

            set_uniform_value(renderer, command.material, "color", info.color);
            set_uniform_value(renderer, command.material, "z", (r32)info.z_layer);
            set_uniform_value(renderer, command.material, "tex", font_info.texture);

            pass.ui.text_z_layers[info.z_layer][pass.ui.text_z_layer_counts[info.z_layer]++] = pass.ui.text_command_count;

            command.shader_handle = command.material.shader;
        }

        CharacterData *coords = pass.ui.coords[pass.ui.text_command_count];

        if(info.has_world_position)
            pos = math::Vec2(0.0f);
        
        generate_text_coordinates(renderer, text, font_info, pos, info.alignment_flags, *framebuffer, &coords);

        command.buffer = {pass.ui.text_command_count++};
    }

    static void push_buffer_to_ui_pass(Renderer *renderer, BufferHandle buffer_handle, ShaderHandle shader, CreateUICommandInfo info)
    {
        UIRenderCommand render_command = {};
        render_command.material = {};        
        render_command.material.array_count = 0;
        render_command.material.lighting.receives_light = false;
        render_command.buffer = buffer_handle;

        if (info.texture_handle.handle != 0)
        {
            render_command.material = renderer->render.materials[renderer->render.ui.textured_material.handle];
            set_uniform_value(renderer, render_command.material, "tex0", info.texture_handle);
            set_uniform_value(renderer, render_command.material, "color", info.color);
        }
        else
        {
            render_command.material = renderer->render.materials[renderer->render.ui.material.handle];
            set_uniform_value(renderer, render_command.material, "color", info.color);
        }

        set_uniform_value(renderer, render_command.material, "position", info.transform.position);
        set_uniform_value(renderer, render_command.material, "scale", info.transform.scale);
        set_uniform_value(renderer, render_command.material, "rotation", info.transform.rotation);

        render_command.shader_handle = render_command.material.shader;
        render_command.clip_rect = info.clip_rect;
        render_command.clip = info.clip;
        RenderPass &pass = renderer->render.ui.pass;
        
        assert(info.z_layer < Z_LAYERS);
        i32 *z_layer = pass.ui.ui_z_layers[info.z_layer];
        i32 z_index = pass.ui.ui_z_layer_counts[info.z_layer]++;
        z_layer[z_index] = pass.ui.render_command_count;
        pass.ui.render_commands[pass.ui.render_command_count++] = render_command;
    }

    static void update_buffer(Renderer *renderer, BufferHandle handle, BufferData new_data, BufferUsage usage = BufferUsage::DYNAMIC)
    {
        update_buffer(handle, BufferType::VERTEX, usage, new_data.vertex_buffer, (size_t)new_data.vertex_count, (size_t)new_data.vertex_buffer_size, renderer);
        update_buffer(handle, BufferType::INDEX, usage, new_data.index_buffer, (size_t)new_data.index_buffer_count, (size_t)new_data.index_buffer_size, renderer);
    }

    static CreateUICommandInfo create_ui_command_info()
    {
        CreateUICommandInfo info = {};
        info.transform.position = math::Vec2(0.0f);
        info.transform.rotation = math::Vec3(0.0f);
        info.transform.scale = math::Vec2(0.0f);

        info.z_layer = 0;
        info.color = math::Rgba(1.0f);
        info.clip_rect = math::Rect(0.0f);
        info.clip = true;
        info.anchor_flag = 0;

        info.scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO;
        info.texture_handle = {0};

        return info;
    }

    static CreateTextCommandInfo create_text_command_info()
    {
        CreateTextCommandInfo info = {};
        info.position = math::Vec2(0.0f);
        info.rotation = math::Vec3(0.0f);
        info.scale = math::Vec2(0.0f);

        info.z_layer = 0;
        info.color = math::Rgba(1.0f);
        info.clip_rect = math::Rect(0.0f);
        info.clip = false;
        info.alignment_flags = UIAlignment::LEFT; // @Incomplete: Default to centered instead?

        return info;
    }

    static void push_ui_quad(Renderer *renderer, CreateUICommandInfo info)
    {
        CreateUICommandInfo scaled_info = info;
        math::Vec2i resolution_scale = get_scale(renderer);
        math::Vec2 absolute_scale = math::Vec2(ABS(info.transform.scale.x), ABS(info.transform.scale.y));

        if(info.anchor_flag & UIAlignment::LEFT)
        {
            if(info.transform.position.x + (absolute_scale.x) < 0.0f || info.transform.position.x > 1000.0f)
            {
                return;
            }
        }
        else if(info.anchor_flag & UIAlignment::RIGHT)
        {
            if(info.transform.position.x < 0.0f || info.transform.position.x - (absolute_scale.x) < 0.0f)
            {
                return;
            }
        }
        else
        {
            if(info.transform.position.x - (absolute_scale.x / 2.0f) < 0.0f || info.transform.position.x + (absolute_scale.x / 2.0f) > 1000.0f)
            {
                return;
            }
        }
        
        if(info.anchor_flag & UIAlignment::TOP)
        {
            if(info.transform.position.y - (absolute_scale.y) > 1000.0f || info.transform.position.y < 0.0f)
            {
                return;
            }
        }
        else if(info.anchor_flag & UIAlignment::BOTTOM)
        {
            if(info.transform.position.y + (absolute_scale.y) < 0.0f || info.transform.position.y > 1000.0f)
            {
                return;
            }
        }
        else
        {
            if(info.transform.position.y - (absolute_scale.y) / 2.0f < 0.0f || info.transform.position.y + (absolute_scale.y / 2.0f) > 1000.0f)
            {
                return;
            }
        }
        
        math::Vec2 pos;
        pos.x = (info.transform.position.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
        pos.y = (info.transform.position.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
        
        scaled_info.transform.position = pos;
        scaled_info.z_layer = info.z_layer;

        scaled_info.transform.scale = get_relative_size(renderer, info.transform.scale, info.scaling_flag);
        scaled_info.clip_rect = scale_clip_rect(renderer, info.clip_rect, 0);

        if(scaled_info.clip_rect.width == 0.0f || scaled_info.clip_rect.height == 0.0f)
        {
            info.clip = false;
        }
        
        scaled_info.clip = info.clip;

        scaled_info.transform.rotation = info.transform.rotation;
        scaled_info.color = info.color;
        scaled_info.anchor_flag = info.anchor_flag;

        u64 anchor = info.anchor_flag;

        BufferHandle buffer = {};
        ShaderHandle shader = {};

        if (anchor & UIAlignment::TOP)
        {
            r32 overflow = pos.y - scaled_info.transform.scale.y;
            if(overflow < 0.0f)
            {
                scaled_info.transform.scale.y -= overflow;
            }
            if (anchor & UIAlignment::LEFT)
            {
                buffer = renderer->render.ui.top_left_textured_quad_buffer;
            }
            else if (anchor & UIAlignment::RIGHT)
            {
                buffer = renderer->render.ui.top_right_textured_quad_buffer;
            }
            else
            {
                buffer = renderer->render.ui.top_x_centered_textured_quad_buffer;
            }
        }
        else if (anchor & UIAlignment::BOTTOM)
        {
            if (anchor & UIAlignment::LEFT)
            {
                buffer = renderer->render.ui.bottom_left_textured_quad_buffer;
            }
            else if (anchor & UIAlignment::RIGHT)
            {
                buffer = renderer->render.ui.bottom_right_textured_quad_buffer;
            }
            else
            {
                buffer = renderer->render.ui.bottom_x_centered_textured_quad_buffer;
            }
        }
        else
        {
            if (anchor & UIAlignment::LEFT)
            {
                buffer = renderer->render.ui.left_y_centered_textured_quad_buffer;
            }
            else if (anchor & UIAlignment::RIGHT)
            {
                buffer = renderer->render.ui.right_y_centered_textured_quad_buffer;
            }
            else
            {
                buffer = renderer->render.ui.centered_textured_quad_buffer;
            }
        }

        push_buffer_to_ui_pass(renderer, buffer, shader, scaled_info);
    }
}
