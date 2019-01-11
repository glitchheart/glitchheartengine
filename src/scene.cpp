#define ENTITY_IS_VALID(handle) handle.handle > 0

namespace scene
{
    static Scene create_scene(Renderer *renderer, EntityTemplateState &template_state, i32 initial_entity_array_size);
    static SceneHandle create_scene_from_file(const char *scene_file_path, SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size);
    static SceneHandle create_scene(SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size);
    static void free_scene(SceneHandle scene);
    static void load_scene(SceneHandle handle, u64 scene_load_flags);
    
    // Scene handle
    static RenderComponent& add_render_component(SceneHandle scene, EntityHandle entity_handle, b32 cast_shadows);
    static TransformComponent& add_transform_component(SceneHandle scene, EntityHandle entity_handle);
    static ParticleSystemComponent& add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material);
    static EntityHandle register_entity(u64 comp_flags, SceneHandle scene, b32 savable);
    static void unregister_entity(EntityHandle handle, SceneHandle scene);
    static EntityTemplate _load_template(const char *path, SceneHandle scene);
    static EntityHandle register_entity_from_template_file(const char *path, SceneHandle scene, b32 savable);
    static void set_active(EntityHandle handle, b32 active, SceneHandle scene);
    static TransformComponent& get_transform_comp(EntityHandle handle, SceneHandle scene);
    static RenderComponent& get_render_comp(EntityHandle handle, SceneHandle scene);
    static ParticleSystemComponent& get_particle_system_comp(EntityHandle handle, SceneHandle scene);
    static LightComponent &get_light_comp(EntityHandle handle, SceneHandle scene);
    static Camera & get_scene_camera(SceneHandle handle);
    static EntityHandle pick_entity(i32 mouse_x, i32 mouse_y);
    
    // @Deprecated: Scene struct 
    static RenderComponent& _add_render_component(Scene &scene, EntityHandle entity_handle, b32 cast_shadows);
    static TransformComponent& _add_transform_component(Scene &scene, EntityHandle entity_handle);
    static ParticleSystemComponent& _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material);
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene, b32 savable);
    static void _unregister_entity(EntityHandle handle, Scene &scene);
    static EntityTemplate _load_template(const char *path, Scene &scene);
    static EntityHandle _register_entity_from_template_file(const char *path, Scene &scene);
    static void _set_active(EntityHandle handle, b32 active, Scene &scene);
    static TransformComponent& _get_transform_comp(EntityHandle handle, Scene &scene);
    static RenderComponent& _get_render_comp(EntityHandle handle, Scene &scene);
    static ParticleSystemComponent& _get_particle_system_comp(EntityHandle handle, Scene &scene);
    static LightComponent &_get_light_comp(EntityHandle handle, Scene &scene);
    
    i32 _unused_entity_handle(Scene &scene);
    i32 _pack_transform_components(Entity &entity, Scene &scene);
    i32 _pack_render_components(Entity &entity, Scene &scene);
    i32 _pack_particle_system_components(Entity &entity, Scene &scene);
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene);


    static void translate_x(TransformComponent& comp, r32 x)
    {
        rendering::translate_x(comp.transform, x);
    }

    static void translate_y(TransformComponent& comp, r32 y)
    {
        rendering::translate_y(comp.transform, y);
    }

    static void translate_z(TransformComponent& comp, r32 z)
    {
        rendering::translate_z(comp.transform, z);
    }

    static void translate(TransformComponent& comp, math::Vec3 translation)
    {
        rendering::translate(comp.transform, translation);
    }

    static void rotate(TransformComponent& comp, math::Vec3 rotation)
    {
        rendering::rotate(comp.transform, rotation);
    }

    static void rotate(TransformComponent& comp, math::Quat rotation)
    {
        rendering::rotate(comp.transform, rotation);
    }

    static void set_rotation(TransformComponent& comp, math::Vec3 rotation)
    {
        rendering::set_rotation(comp.transform, rotation);
    }

    static void set_rotation(TransformComponent& comp, math::Quat orientation)
    {
        rendering::set_rotation(comp.transform, orientation);
    }

    static void set_rotation_x(TransformComponent& comp, r32 x)
    {
        rendering::set_rotation_x(comp.transform, x);
    }

    static void set_rotation_y(TransformComponent& comp, r32 y)
    {
        rendering::set_rotation_y(comp.transform, y);
    }

    static void set_rotation_z(TransformComponent& comp, r32 z)
    {
        rendering::set_rotation_z(comp.transform, z);
    }

    static void scale(TransformComponent& comp, math::Vec3 scale)
    {
        rendering::scale(comp.transform, scale);
    }

    static void set_scale(TransformComponent& comp, math::Vec3 scale)
    {
        rendering::set_scale(comp.transform, scale);
    }

    static void set_scale_x(TransformComponent& comp, r32 x)
    {
        rendering::set_scale_x(comp.transform, x);
    }

    static void set_scale_y(TransformComponent& comp, r32 y)
    {
        rendering::set_scale_y(comp.transform, y);
    }

    static void set_scale_z(TransformComponent& comp, r32 z)
    {
        rendering::set_scale_z(comp.transform, z);
    }

    static void set_position(TransformComponent& comp, math::Vec3 position)
    {
        rendering::set_position(comp.transform, position);
    }

    static void set_position_x(TransformComponent& comp, r32 x)
    {
        rendering::set_position_x(comp.transform, x);
    }

    static void set_position_y(TransformComponent& comp, r32 y)
    {
        rendering::set_position_y(comp.transform, y);
    }

    static void set_position_z(TransformComponent& comp, r32 z)
    {
        rendering::set_position_z(comp.transform, z);
    }

    
	static void set_uniform_value(EntityHandle handle, const char* name, r32 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec2 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec3 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec4 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, i32 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Mat4 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, rendering::TextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        assert(value.handle != 0);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}

    static void set_uniform_value(EntityHandle handle, const char* name, rendering::MSTextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        assert(value.handle != 0);
        rendering::set_uniform_value(scene.manager->renderer, render.v2.material_handle, name, value);
	}
    
    static i32 _find_handle_in_range(i32 start, i32 end, i32 *handles)
    {
        i32 handle = -1;
        for(i32 i = start; i < end; i++)
        {
            if(handles[i] == -1)
            {
                handle = i;
                break;
            }
        }

        return handle;
    } 

    static i32 _find_next_free_internal_handle(SceneManager *scene_manager)
    {
        i32 handle = -1;

        if(scene_manager->_internal_scene_handles[scene_manager->current_internal_index] == -1)
        {
            handle = scene_manager->current_internal_index;
        }
        else
        {
            i32 found_handle = _find_handle_in_range(scene_manager->current_internal_index, global_max_scenes, scene_manager->_internal_scene_handles);
            if(found_handle == -1)
            {
                found_handle = _find_handle_in_range(0, scene_manager->current_internal_index, scene_manager->_internal_scene_handles);
            }
            handle = found_handle;
        }

        // Set the current index to look for a free handle
        scene_manager->current_internal_index = handle + 1;
        if(scene_manager->current_internal_index == global_max_scenes)
        {
            scene_manager->current_internal_index = 0;
        }
        
        return handle;
    }

    static SceneHandle create_scene(SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size = 1024)
    {
        i32 internal_handle = _find_next_free_internal_handle(scene_manager);
        assert(internal_handle != -1);

        i32 real_handle = scene_manager->scene_count++;
        scene_manager->_internal_scene_handles[internal_handle] = real_handle;
        Scene &scene = scene_manager->scenes[real_handle];
        scene = {};
        scene.memory_arena = {};
        
        scene.persistent = persistent;
        scene.scene_manager = scene_manager;
        scene.valid = true;
        scene.loaded = false;
        scene.template_state = &scene_manager->template_state;
        scene.renderer = scene_manager->renderer;
        scene.max_entity_count = initial_entity_array_size;
        scene.entity_count = 0;
        scene.current_internal_handle = 0;
        scene.transform_component_count = 0;
        scene.render_component_count = 0;
		
        auto &memory_arena = scene.memory_arena;
        scene.entities = push_array(&memory_arena, initial_entity_array_size, Entity);
        scene._internal_handles = push_array(&memory_arena, initial_entity_array_size, i32);
        scene.active_entities = push_array(&memory_arena, initial_entity_array_size, b32);
        scene.transform_components = push_array(&memory_arena, initial_entity_array_size, TransformComponent);
        scene.render_components = push_array(&memory_arena, initial_entity_array_size, RenderComponent);
        scene.light_components = push_array(&memory_arena, initial_entity_array_size, LightComponent);
        scene.particle_system_components = push_array(&memory_arena, initial_entity_array_size, ParticleSystemComponent);

        if(scene_manager->debug_cube.handle == 0)
        {
            scene_manager->debug_cube = rendering::create_cube(scene_manager->renderer, nullptr);
            scene_manager->debug_shader_handle = rendering::load_shader(scene_manager->renderer, "../assets/shaders/unlit.shd");
            scene_manager->debug_material = rendering::create_material(scene_manager->renderer, scene_manager->debug_shader_handle);
        }

        for(i32 index = 0; index < initial_entity_array_size; index++)
        {
            scene._internal_handles[index] = -1;
            scene.active_entities[index] = true;
            scene.transform_components[index].transform = rendering::create_transform(math::Vec3(0, 0, 0), math::Vec3(1, 1, 1), math::Vec3(0.0f));
            scene.transform_components[index].parent_handle = EMPTY_COMP_HANDLE;

            for(i32 i = 0; i < MAX_CHILDREN; i++)
            {
                scene.transform_components[index].child_handles[i] = EMPTY_COMP_HANDLE;
            }
            
        }

        SceneHandle handle;
        handle.handle = internal_handle + 1;
        handle.manager = scene_manager;
        return handle;
    }

    static void save_scene(const char *file_path, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        FILE *file = fopen(file_path, "w");

        if(file)
        {
            for(i32 i = 0; i < scene.entity_count; i++)
            {
                Entity &entity = scene.entities[i];
                if(entity.savable)
                {
                    if(i > 0)
                        fprintf(file, "\n"); // Spacing between object declarations
                    
                    TransformComponent &transform = get_transform_comp(entity.handle, scene_handle);
                    
                    fprintf(file, "obj\n");
                    fprintf(file, "template: %s\n", entity.template_path);
                    fprintf(file, "position: %f %f %f\n", transform.transform.position.x, transform.transform.position.y, transform.transform.position.z);
                    fprintf(file, "scale: %f %f %f\n", transform.transform.scale.x, transform.transform.scale.y, transform.transform.scale.z);
                    fprintf(file, "rotation: %f %f %f\n", transform.transform.euler_angles.x, transform.transform.euler_angles.y, transform.transform.euler_angles.z);
                }
            }
            fclose(file);
        }
    }

    static void parse_scene_object(FILE *file, SceneHandle scene)
    {
        EntityHandle handle = { -1 };
        
        char buffer[256];
        
        while(fgets(buffer, 256, file))
        {
            if(starts_with(buffer, "\n"))
                break;

            if(starts_with(buffer, "template"))
            {
                char template_path[256];
                sscanf(buffer, "template: %[^\n]", template_path);
                handle = register_entity_from_template_file(template_path, scene, true);
            }
            else if(starts_with(buffer, "position"))
            {
                assert(handle.handle != -1);
                TransformComponent &transform = get_transform_comp(handle, scene);

                math::Vec3 new_position;
                sscanf(buffer, "position: %f %f %f", &new_position.x, &new_position.y, &new_position.z);
                rendering::set_position(transform.transform, new_position);
            }
            else if(starts_with(buffer, "scale"))
            {
                assert(handle.handle != -1);
                TransformComponent &transform = get_transform_comp(handle, scene);

                math::Vec3 new_scale;
                sscanf(buffer, "scale: %f %f %f", &new_scale.x, &new_scale.y, &new_scale.z);
                rendering::set_scale(transform.transform, new_scale);
            }
            else if(starts_with(buffer, "rotation"))
            {
                assert(handle.handle != -1);
                TransformComponent &transform = get_transform_comp(handle, scene);

                math::Vec3 new_rotation;
                sscanf(buffer, "rotation: %f %f %f", &new_rotation.x, &new_rotation.y, &new_rotation.z);
                rendering::set_rotation(transform.transform, new_rotation);
            }
        }
    }
    
    static SceneHandle create_scene_from_file(const char *scene_file_path, SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size)
    {
        SceneHandle handle = create_scene(scene_manager, persistent, initial_entity_array_size);
        FILE *file = fopen(scene_file_path, "r");
        
        if(file)
        {
            char line_buffer[256];
            while(fgets(line_buffer, 256, file))
            {
                if(starts_with(line_buffer, "obj"))
                {
                    parse_scene_object(file, handle);
                }
            }
                
            fclose(file);
        }

        return handle;
    }
    
    struct InstancePair
    {
        rendering::MaterialHandle material_handle;
        rendering::BufferHandle buffer_handle;
        rendering::MaterialInstanceHandle material_instances[1024];
        i32 count;

        rendering::VertexAttribute attributes[8];
        i32 attribute_count;
    };
    
    static void allocate_instance_buffers(Scene &scene)
    {
        InstancePair instance_pairs[32];
        i32 pair_count = 0;
        
        Renderer *renderer = scene.renderer;

        // Find out how many instance buffers we need to allocate
        for(i32 i = 0; i < scene.render_component_count; i++)
        {
            RenderComponent &comp = scene.render_components[i];
            rendering::Material &material_instance = rendering::get_material_instance(comp.v2.material_handle, renderer);

            if(material_instance.instanced_vertex_attribute_count > 0)
            {
                b32 found = false;
            
                for(i32 j = 0; j < pair_count; j++)
                {
                    InstancePair &pair = instance_pairs[j];
                    if(pair.buffer_handle.handle == comp.v2.buffer_handle.handle && pair.material_handle.handle ==  material_instance.source_material.handle)
                    {
                        found = true;
                        pair.material_instances[pair.count++] = comp.v2.material_handle;
                    }
                }

                if(!found)
                {
                    InstancePair &pair = instance_pairs[pair_count++];
                    pair.attribute_count = 0;
                    pair.count = 0;
                    pair.buffer_handle = comp.v2.buffer_handle;
                    pair.material_handle = material_instance.source_material;
                    
                    for(i32 j = 0; j < material_instance.instanced_vertex_attribute_count; j++)
                    {
                        pair.attributes[pair.attribute_count++] = material_instance.instanced_vertex_attributes[j].attribute;
                    }
                    
                    pair.material_instances[pair.count++] = comp.v2.material_handle;
                }
            }
        }

        // Allocate the buffers
        for(i32 i = 0; i < pair_count; i++)
        {
            InstancePair &pair = instance_pairs[i];
            InstanceBufferData data = {};
            data.buffer_handle = pair.buffer_handle;
            data.source_material_handle = pair.material_handle;
            data.max_count = pair.count;
            // Allocate all buffers
            for(i32 j = 0; j < pair.attribute_count; j++)
            {
                rendering::InstanceBufferHandle instance_buffer_handle = rendering::allocate_instance_buffer(pair.attributes[j].type, math::next_power_of_two(pair.count), renderer);

                data.instance_buffer_handles[data.instance_buffer_count++] = instance_buffer_handle;
                
                // Update all materials with the correct buffer handle
                for(i32 k = 0; k < pair.count; k++)
                {
                    rendering::Material &material = get_material_instance(pair.material_instances[k], renderer);
                    material.instanced_vertex_attributes[j].instance_buffer_handle = instance_buffer_handle;
                }

            }

            scene.instance_buffer_data[scene.instance_buffer_data_count++] = data;
            assert(scene.instance_buffer_data_count < MAX_INSTANCE_BUFFER_HANDLES);
        }
    }

    static void free_instance_buffers(Scene &scene)
    {
        for(i32 i = 0; i < scene.instance_buffer_data_count; i++)
        {
            InstanceBufferData data = scene.instance_buffer_data[i];
            for(i32 j = 0; j < data.instance_buffer_count; j++)
                rendering::free_instance_buffer(data.instance_buffer_handles[j], scene.renderer);
        }
        scene.instance_buffer_data_count = 0;
    }

    static void free_scene(SceneHandle handle)
    {
        SceneManager *scene_manager = handle.manager;
        i32 internal_handle = scene_manager->_internal_scene_handles[handle.handle - 1];
        scene_manager->_internal_scene_handles[handle.handle - 1] = -1;
        Scene &scene = scene_manager->scenes[internal_handle];
        
        if(scene.valid)
        {
			scene.valid = false;
            scene.loaded = false;

			if (scene.entity_count > 0)
            {
                for(i32 i = 0; i < scene.render_component_count; i++)
                {
                    rendering::delete_material_instance(scene.renderer, scene.render_components[i].v2.material_handle);
                }
                
                scene.entity_count = 0;
                scene.transform_component_count = 0;
                scene.render_component_count = 0;
            
                for(i32 index = 0; index < scene.particle_system_component_count; index++)
                {
                    ParticleSystemComponent& ps_comp = scene.particle_system_components[index];
                    remove_particle_system(scene.renderer, ps_comp.handle);
                }
                
                scene.particle_system_component_count = 0;
                scene.light_component_count = 0;
                scene.current_internal_handle = 0;
                clear(&scene.memory_arena);
            }

            free_instance_buffers(scene);
            
            if(internal_handle < scene_manager->scene_count - 1)
            {
                scene_manager->scenes[internal_handle] = scene_manager->scenes[scene_manager->scene_count - 1];
                for(i32 i = 0; i < global_max_scenes; i++)
                {
                    if(scene_manager->_internal_scene_handles[i] == scene_manager->scene_count - 1)
                    {
                        scene_manager->_internal_scene_handles[i] = internal_handle;
                        break;
                    }
                }
            }
            
            scene_manager->scene_count--;
            scene_manager->scene_loaded = false;
        }
    }

    static void deactivate_particle_systems(SceneHandle handle)
    {
        Scene &scene = get_scene(handle);                
        for(i32 i = 0; i < scene.particle_system_component_count; i++)
        {
            stop_particle_system(scene.particle_system_components[i].handle, scene.renderer);
        }
    }
    
    static void activate_particle_systems(SceneHandle handle)
    {
        Scene &scene = get_scene(handle);                
        for(i32 i = 0; i < scene.particle_system_component_count; i++)
        {
            start_particle_system(scene.particle_system_components[i].handle, scene.renderer);
            pause_particle_system(scene.particle_system_components[i].handle, scene.renderer, false);
        }
    }
    
    static math::Ray cast_ray(Scene &scene, i32 mouse_x, i32 mouse_y)
    {
        i32 width = scene.renderer->window_width;
        i32 height = scene.renderer->window_height;
        Camera &camera = scene.camera;
        
        r32 x = (2.0f * mouse_x) / width - 1.0f;
        r32 y = 1.0f - (2.0f * mouse_y) / height;
        r32 z = 1.0f;
        
        math::Vec3 ray_nds = math::Vec3(x, y, z);
        math::Vec4 ray_clip = math::Vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
        math::Vec4 ray_eye = math::inverse(camera.projection_matrix) * ray_clip;
        ray_eye = math::Vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        math::Vec3 ray_wor = (inverse(camera.view_matrix) * ray_eye).xyz;
        ray_wor = math::normalize(ray_wor);
        
        math::Ray ray;
        ray.origin = camera.position;
        ray.direction = ray_wor;

        return ray;
    }

    static math::Ray cast_ray(SceneHandle handle, i32 mouse_x, i32 mouse_y)
    {
        return cast_ray(get_scene(handle), mouse_x, mouse_y);
    }

#define EPSILON 0.00001f

    // b32 ray_intersects_plane(math::Vec3 plane_normal, math::Vec3 plane_position, math::Vec3 ray_origin, math::Vec3 ray_direction, math::Vec3 &intersection_point)
    // {
    //     r32 denom = math::dot(plane_normal, ray_direction);
    //     if(denom > EPSILON)
    //     {
    //         math::Vec3 dist_vec = plane_position - ray_origin;
    //         r32 t = -math::dot(dist_vec, plane_normal) / denom;
    //         intersection_point = ray_origin + ray_direction * t;
    //         return t >= 0;
    //     }

    //     return false;
    // }

    b32 ray_intersects_plane(math::Vec3 plane_normal, math::Vec3 plane_position, math::Vec3 ray_origin, math::Vec3 ray_direction, math::Vec3 &intersection_point)
    {
        // assuming vectors are all normalized
        float denom = math::dot(plane_normal, ray_direction);
        if (denom > 1e-6)
        {
            math::Vec3 p0l0 = plane_position - ray_origin;
            r32 t = math::dot(p0l0, plane_normal) / denom;
            intersection_point = ray_origin + t * ray_direction;
            return (t >= 0);
        }

        return false; 
        
    }

    math::Vec3 ray_vs_plane(math::Vec3 ray_origin, math::Vec3 ray_direction, math::Vec3 plane_normal, r32 plane_d)
    {
        r32 denominator = math::dot(plane_normal, ray_direction);
        r32 numerator = math::dot(plane_normal, ray_origin);
        r32 t = -((plane_d + numerator) / denominator);
        return ray_origin + t * ray_direction;
    }
    
    b32 ray_vs_plane(math::Vec3 *x_point, math::Vec3 p0, math::Vec3 dir, math::Vec3 plane_point, math::Vec3 plane_n)
    {
        math::Vec3 local_p0 = p0 - plane_point;

        r32 denom = math::dot(plane_n, dir);
        r32 epsilon = 0.0001f;
        if(ABS(denom) < epsilon) return false;

        r32 t = (math::dot(local_p0, plane_n) / denom);

        debug("t: %f\n", t);
        
        *x_point = p0 + dir * t;
        
        if(t < 0) return false;

        return true;
    }
    
    static b32 _is_gizmo(EntityHandle entity, SceneManager *manager)
    {
        
        // if(entity.handle == manager->gizmos.x_arrow.handle)
        // {
        //     manager->gizmos.selected_gizmo = Gizmos::X_ARROW;
        //     return true;
        // }

        // if(entity.handle == manager->gizmos.y_arrow.handle)
        // {
        //     manager->gizmos.selected_gizmo = Gizmos::Y_ARROW;
        //     return true;
        // }

        // if(entity.handle == manager->gizmos.z_arrow.handle)
        // {
        //     manager->gizmos.selected_gizmo = Gizmos::Z_ARROW;
        //     return true;
        // }

        return false;
    }

    static void deactivate_gizmo_arrows(SceneManager *manager)
    {
        manager->gizmos.active = false;
    }

    static EntityHandle get_root_entity(TransformComponent &current_transform, Scene &scene)
    {
        if(IS_COMP_HANDLE_VALID(current_transform.parent_handle))
        {
            TransformComponent& parent_transform = scene.transform_components[current_transform.parent_handle.handle];
            return get_root_entity(parent_transform, scene);
        }
        else
        {
            i32 internal_handle = scene._internal_handles[current_transform.entity.handle - 1];
            assert(internal_handle > -1);
            Entity &entity = scene.entities[internal_handle];
            
            if(!entity.selection_enabled)
                return { -1 };
            else
                return current_transform.entity;
        }
    }
    
    static EntityHandle pick_entity(SceneHandle handle, i32 mouse_x, i32 mouse_y)
    {
        Scene &scene = get_scene(handle);

        math::Ray ray = cast_ray(scene, mouse_x, mouse_y);
        
        r32 dist = 100000; // Just set a crazy max distance
        
        EntityHandle entity_handle = { -1 };

        if(IS_ENTITY_HANDLE_VALID(handle.manager->selected_entity))
        {
            TransformComponent &selected_transform = get_transform_comp(handle.manager->selected_entity, handle);
            
            math::BoundingBox box;
            math::Vec3 real_scale = math::Vec3(1, 1, 1) * handle.manager->gizmos.current_distance_to_camera;
            
            box.min = math::Vec3(selected_transform.transform.position.x - real_scale.x * 0.5f, selected_transform.transform.position.y - real_scale.y * 0.5f, selected_transform.transform.position.z - real_scale.z * 0.5f);
            box.max = math::Vec3(selected_transform.transform.position.x + real_scale.x * 0.5f, selected_transform.transform.position.y + real_scale.y * 0.5f, selected_transform.transform.position.z + real_scale.z * 0.5f);

            math::Vec3 intersection_point;
            if(aabb_ray_intersection(ray, box, &intersection_point))
            {
                return handle.manager->gizmos.scale_cubes[3];
            }
        }
        
        for(i32 i = 0; i < scene.entity_count; i++)
        {
            const scene::Entity &ent = scene.entities[i];

            if (scene.active_entities[i])
            {
                if (ent.comp_flags & scene::COMP_RENDER)
                {
                    scene::TransformComponent &transform = scene.transform_components[ent.transform_handle.handle];
                    scene::RenderComponent &render_comp = scene.render_components[ent.render_handle.handle];

                    // Ignore the entity if it is not selectable and it has no parent
                    if(!ent.selection_enabled && !IS_COMP_HANDLE_VALID(transform.parent_handle))
                        continue;
                        
                    math::BoundingBox box;
                    math::Vec3 real_scale = transform.transform.scale * render_comp.mesh_scale;
                    box.min = math::Vec3(transform.transform.position.x - real_scale.x * 0.5f, transform.transform.position.y - real_scale.y * 0.5f, transform.transform.position.z - real_scale.z * 0.5f);
                    box.max = math::Vec3(transform.transform.position.x + real_scale.x * 0.5f, transform.transform.position.y + real_scale.y * 0.5f, transform.transform.position.z + real_scale.z * 0.5f);

                    math::Vec3 intersection_point;
                    if(aabb_ray_intersection(ray, box, &intersection_point))
                    {

                        r32 new_dist = math::distance(scene.camera.position, intersection_point);
                    
                        if(new_dist < dist)
                        {
                            // Look for selectable parents
                            if(!ent.selection_enabled)
                            {
                                EntityHandle parent = get_root_entity(transform, scene);

                                // If a selectable parent was found
                                if(IS_ENTITY_HANDLE_VALID(parent))
                                    entity_handle = parent;
                                else
                                    continue;
                            }
                            else
                                entity_handle = ent.handle;
                            
                            dist = new_dist;
                        }
                    }
                }
            }
        }

        if(IS_ENTITY_HANDLE_VALID(entity_handle))
            handle.manager->gizmos.selected_gizmo = Gizmos::NONE;
        
        return entity_handle;
    }

    static void set_wireframe_enabled(b32 enabled, EntityHandle entity_handle, SceneHandle &handle)
    {
        Scene &scene = get_scene(handle);
        RenderComponent &render_comp = _get_render_comp(entity_handle, scene);
        render_comp.wireframe_enabled = enabled;
    }

    static void _register_gizmos(SceneHandle scene)
    {
        SceneManager *manager = scene.manager;
        manager->gizmos.active = true;
    }

    static void draw_gizmos(SceneManager *manager)
    {
        TranslationConstraint c = manager->gizmos.constraint;
        if(manager->gizmos.active)
        {
            r32 unit = manager->gizmos.current_distance_to_camera;
            
            TransformComponent &transform_comp = get_transform_comp(manager->selected_entity, manager->loaded_scene);
            rendering::Transform t = rendering::create_transform(transform_comp.transform.position, math::Vec3(1.0f), transform_comp.transform.orientation);
            math::Vec3 yellow(RGB_FLOAT(189), RGB_FLOAT(183), RGB_FLOAT(107));

            if(manager->gizmos.transformation_type == TransformationType::SCALE)
            {
                r32 scale_cube_size = 0.2f;
                
                set_uniform_value(manager->gizmos.scale_cubes[0], "color", c == TranslationConstraint::X ? yellow : math::Vec3(1.0f, 0.0f, 0.0f), manager->loaded_scene);
                set_uniform_value(manager->gizmos.scale_cubes[1], "color", c == TranslationConstraint::Y ? yellow : math::Vec3(0.0f, 1.0f, 0.0f), manager->loaded_scene);
                set_uniform_value(manager->gizmos.scale_cubes[2], "color", c == TranslationConstraint::Z ? yellow : math::Vec3(0.0f, 0.0f, 1.0f), manager->loaded_scene);
                set_uniform_value(manager->gizmos.scale_cubes[3], "color", manager->gizmos.scaling_mode == ScalingMode::ALL_AXIS ? yellow : math::Vec3(1.0f, 1.0f, 1.0f), manager->loaded_scene);
                
                TransformComponent &x_t = get_transform_comp(manager->gizmos.scale_cubes[0], manager->loaded_scene);
                set_position(x_t, transform_comp.transform.position + math::Vec3(unit, 0, 0));
                set_scale(x_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));

                TransformComponent &y_t = get_transform_comp(manager->gizmos.scale_cubes[1], manager->loaded_scene);
                set_position(y_t, transform_comp.transform.position + math::Vec3(0, unit, 0));
                set_scale(y_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));

                TransformComponent &z_t = get_transform_comp(manager->gizmos.scale_cubes[2], manager->loaded_scene);
                set_position(z_t, transform_comp.transform.position + math::Vec3(0, 0, unit));
                set_scale(z_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));
                
                TransformComponent &center_t = get_transform_comp(manager->gizmos.scale_cubes[3], manager->loaded_scene);
                set_position(center_t, transform_comp.transform.position);
                set_scale(center_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));
            }
                
            // Draw transform manipulators
            math::Vec3 line[6];
            math::Vec3 p1;
            math::Vec3 p2;
            line[0] = math::Vec3(0.0f, 0.0f, 0.0f);
            line[1] = math::Vec3(manager->gizmos.current_distance_to_camera, 0.0f, 0.0f);

            // X
            rendering::direct_update_buffer(manager->renderer, manager->gizmos.x_buffer, (r32*)line, 2, sizeof(math::Vec3) * 2);
            rendering::set_uniform_value(manager->renderer, manager->gizmos.x_material, "color", c == TranslationConstraint::X ? yellow : math::Vec3(1.0f, 0.0f, 0.0f));
            rendering::push_buffer_to_render_pass(manager->renderer, manager->gizmos.x_buffer, manager->gizmos.x_material, t, manager->gizmos.line_shader, {1}, rendering::CommandType::NO_DEPTH, rendering::PrimitiveType::LINES);

            // Y
            line[1] = math::Vec3(0.0f, manager->gizmos.current_distance_to_camera, 0.0f);
            rendering::direct_update_buffer(manager->renderer, manager->gizmos.y_buffer, (r32*)line, 2, sizeof(math::Vec3) * 2);
            rendering::set_uniform_value(manager->renderer, manager->gizmos.y_material, "color", c == TranslationConstraint::Y ? yellow : math::Vec3(0.0f, 1.0f, 0.0f));
            rendering::push_buffer_to_render_pass(manager->renderer, manager->gizmos.y_buffer, manager->gizmos.y_material, t, manager->gizmos.line_shader, {1}, rendering::CommandType::NO_DEPTH, rendering::PrimitiveType::LINES);

            // Z
            line[1] = math::Vec3(0.0f, 0.0f, manager->gizmos.current_distance_to_camera);
            rendering::direct_update_buffer(manager->renderer, manager->gizmos.z_buffer, (r32*)line, 2, sizeof(math::Vec3) * 2);
            rendering::set_uniform_value(manager->renderer, manager->gizmos.z_material, "color", c == TranslationConstraint::Z ? yellow : math::Vec3(0.0f, 0.0f, 1.0f));
            rendering::push_buffer_to_render_pass(manager->renderer, manager->gizmos.z_buffer, manager->gizmos.z_material, t, manager->gizmos.line_shader, {1}, rendering::CommandType::NO_DEPTH, rendering::PrimitiveType::LINES);
        }
    }

    // Finds the line between two lines with the minimum distance
    b32 line_vs_line(Line l1, Line l2, math::Vec3 *points)
    {
        r32 mua = 0.0f;
        r32 mub = 0.0f;
        
        math::Vec3 p1 = l1.start;
        math::Vec3 p2 = l1.end;
        math::Vec3 p3 = l2.start;
        math::Vec3 p4 = l2.end;
        
        math::Vec3 p13,p43,p21;
        r32 d1343,d4321,d1321,d4343,d2121;
        r32 numer,denom;

        p13.x = p1.x - p3.x;
        p13.y = p1.y - p3.y;
        p13.z = p1.z - p3.z;
        p43.x = p4.x - p3.x;
        p43.y = p4.y - p3.y;
        p43.z = p4.z - p3.z;
        
        if (ABS(p43.x) < EPSILON && ABS(p43.y) < EPSILON && ABS(p43.z) < EPSILON)
            return false;

        p21.x = p2.x - p1.x;
        p21.y = p2.y - p1.y;
        p21.z = p2.z - p1.z;

        if (ABS(p21.x) < EPSILON && ABS(p21.y) < EPSILON && ABS(p21.z) < EPSILON)
            return false;

        d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
        d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
        d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
        d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
        d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

        denom = d2121 * d4343 - d4321 * d4321;

        if (ABS(denom) < EPSILON)
            return false;
        numer = d1343 * d4321 - d1321 * d4343;

        mua = numer / denom;
        mub = (d1343 + d4321 * (mua)) / d4343;

        points[0].x = p1.x + mua * p21.x;
        points[0].y = p1.y + mua * p21.y;
        points[0].z = p1.z + mua * p21.z;
        points[1].x = p3.x + mub * p43.x;
        points[1].y = p3.y + mub * p43.y;
        points[1].z = p3.z + mub * p43.z;

        return true;
    }

    static Line line_from_ray(math::Ray ray)
    {
        Line l;
        l.start = ray.origin;
        l.end = ray.origin + ray.direction * 1000;
        return l;
    }
    
    static void update_transform(TransformComponent &transform, Camera &camera, SceneManager *manager, InputController *input_controller, r64 delta_time)
    {
        if(manager->dragging)
        {
            if(manager->gizmos.transformation_type == TransformationType::SCALE && manager->gizmos.scaling_mode == ScalingMode::ALL_AXIS)
            {
                math::Vec2 mouse_position = math::Vec2(input_controller->mouse_x, manager->renderer->window_height - input_controller->mouse_y);
                math::Vec2 mouse_offset = manager->gizmos.scale_mouse_offset - mouse_position;
                r32 diff = (mouse_offset.x + mouse_offset.y) * SCALE_SENSITIVITY;
                
                rendering::set_scale(transform.transform, manager->gizmos.initial_scale + math::Vec3(diff, diff, diff));
            }
            else
            {
                math::Vec3 points[2];
                Scene &scene = get_scene(manager->loaded_scene);
                math::Ray ray = cast_ray(scene, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);
            
                Line l1 = line_from_ray(ray);
            
                line_vs_line(l1, manager->gizmos.current_line, points);
                
                switch(manager->gizmos.constraint)
                {
                case TranslationConstraint::X:
                {
                    if(manager->gizmos.transformation_type == TransformationType::POSITION)
                    {
                        rendering::set_position_x(transform.transform, points[1].x - manager->gizmos.initial_offset.x);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::SCALE)
                    {
                        rendering::set_scale_x(transform.transform, points[1].x - manager->gizmos.initial_offset.x);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::ROTATION)
                    {
                        // @Incomplete
                    }
                }
                break;
                case TranslationConstraint::Y:
                {
                    if(manager->gizmos.transformation_type == TransformationType::POSITION)
                    {
                        rendering::set_position_y(transform.transform, points[1].y - manager->gizmos.initial_offset.y);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::SCALE)
                    {
                        rendering::set_scale_y(transform.transform, points[1].y - manager->gizmos.initial_offset.y);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::ROTATION)
                    {
                        // @Incomplete
                    }
                }
                break;
                case TranslationConstraint::Z:
                {
                    if(manager->gizmos.transformation_type == TransformationType::POSITION)
                    {
                        rendering::set_position_z(transform.transform, points[1].z - manager->gizmos.initial_offset.z);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::SCALE)
                    {
                        rendering::set_scale_z(transform.transform, points[1].z - manager->gizmos.initial_offset.z);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::ROTATION)
                    {
                        // @Incomplete
                    }
                }
                break;
                case Gizmos::NONE:
                    return;
                }
            }
        }
            
    }

    static void _select_gizmo(SceneHandle handle, EntityHandle gizmo)
    {
        //RenderComponent &render_selected = get_render_comp(gizmo, handle);
        //rendering::set_uniform_value(*handle.manager->renderer, render_selected.v2.material_handle, "color", math::Rgba(1.0f, 1.0f, 0.0f, 1.0f));
    }

    static void set_editor_camera_lock(b32 locked, SceneManager *scene_manager)
    {
        scene_manager->editor.lock_camera = locked;
    }

    static void editor_save(SceneManager *scene_manager)
    {
        // @Incomplete: Save scene-file
        if(scene_manager->callbacks.on_save)
            scene_manager->callbacks.on_save(scene_manager->loaded_scene);
    }
    
    static void set_selection_enabled(b32 enabled, SceneManager *scene_manager)
    {
        scene_manager->editor.selection_enabled = enabled;

        if(!scene_manager->editor.selection_enabled)
        {
            if(IS_ENTITY_HANDLE_VALID(scene_manager->selected_entity))
                scene::set_wireframe_enabled(false, scene_manager->selected_entity, scene_manager->loaded_scene);
            
            scene_manager->selected_entity = { -1 };
            scene_manager->gizmos.active = false;
        }
    }
    
    static void toggle_selection_enabled(SceneManager *scene_manager)
    {
        set_selection_enabled(!scene_manager->editor.selection_enabled, scene_manager);
    }

    static void set_transformation_type(TransformationType type, SceneManager *scene_manager)
    {
        scene_manager->gizmos.transformation_type = type;
        switch(type)
        {
        case TransformationType::POSITION:
        {
            for(i32 i = 0; i < 4; i++)
            {
                set_active(scene_manager->gizmos.scale_cubes[i], false, scene_manager->loaded_scene);
            }
        }
        break;
        case TransformationType::SCALE:
        {
            for(i32 i = 0; i < 4; i++)
            {
                set_active(scene_manager->gizmos.scale_cubes[i], true, scene_manager->loaded_scene);
            }
        }
        break;
        case TransformationType::ROTATION:
            assert(false);
            break;
        }
    }

    static void update_editor_camera(Camera &camera, Scene &scene, InputController *input_controller, r64 delta_time)
    {
        // Update camera
        if(KEY(Key_W))
        {
            translate_forward(camera, (r32)delta_time * 10.0f);
        }

        if(KEY(Key_S))
        {
            translate_forward(camera, (r32)-delta_time * 10.0f);
        }

        if(KEY(Key_A))
        {
            translate_right(camera, (r32)-delta_time * 10.0f);
        }

        if(KEY(Key_D))
        {
            translate_right(camera, (r32)delta_time * 10.0f);
        }

        if(KEY_DOWN(Key_LeftAlt))
            center(camera);
        else if(KEY_UP(Key_LeftAlt))
            free_roam(camera);
    
        if(MOUSE_DOWN(Mouse_Right))
            set_mouse_lock(true, *scene.renderer);
        else if(MOUSE_UP(Mouse_Right))
            set_mouse_lock(false, *scene.renderer);
    
        if(MOUSE(Mouse_Right))
        {
            rotate_around_x(camera, (r32)-input_controller->mouse_y_delta * 0.1f);
            rotate_around_y(camera, (r32)input_controller->mouse_x_delta * 0.1f);
        }
    }

    static void deselect_everything(SceneManager *manager)
    {
        if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
        {
            scene::set_wireframe_enabled(false, manager->selected_entity, manager->loaded_scene);
            manager->selected_entity = { -1 };
            manager->gizmos.active = false;
        }
    }

    static void select_entity(EntityHandle entity, SceneManager *manager)
    {
        if(IS_ENTITY_HANDLE_VALID(entity))
        {
            // Deselect the previously selected entity
            if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                scene::set_wireframe_enabled(false, manager->selected_entity, manager->loaded_scene);                        

            manager->selected_entity = entity;
            manager->gizmos.active = true;

            scene::set_wireframe_enabled(true, entity, manager->loaded_scene);
            manager->callbacks.on_entity_selected(entity, manager->loaded_scene);
        }
    }
    
    static void find_all_template_files(SceneManager *scene_manager)
    {
        DirectoryData data = {};
        platform.get_all_files_with_extension("../assets/templates/", "tmpl", &data, true);
        scene_manager->editor.template_files = data;
    }
    
    static void update_scene_editor(SceneHandle handle, InputController *input_controller, r64 delta_time)
    {
        Scene &scene = get_scene(handle);
        
        SceneManager *manager = handle.manager;
        
        if(KEY_DOWN(Key_E) && KEY(Key_LeftCtrl))
        {
            if(manager->mode == SceneMode::RUNNING)
            {
                find_all_template_files(manager);
                
                if(manager->callbacks.on_started_edit_mode)
                    manager->callbacks.on_started_edit_mode(handle);
                
                // @Note: Unnecessary?
                if(manager->callbacks.on_load)
                    manager->callbacks.on_load(handle);

                // Register all debug entities
                for(i32 i = 0; i < 4; i++)
                {
                    manager->gizmos.scale_cubes[i] = register_entity_from_template_file("../assets/templates/editor/scale_cube.tmpl", manager->loaded_scene, false);
                    TransformComponent &transform = get_transform_comp(manager->gizmos.scale_cubes[i], manager->loaded_scene);
                    set_position(transform, math::Vec3(0, 1 + i, 0));
                }
                
                manager->play_camera = scene.camera;
                manager->mode = SceneMode::EDITING;
            }
            else
            {
                // Disable wireframes
                if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                    scene::set_wireframe_enabled(false, manager->selected_entity, handle);
                
                // When exiting to running mode we should make sure to notify the game about it, to ensure that all
                // editor-specific entities are cleaned up before the game is running again.
                if(manager->callbacks.on_exited_edit_mode)
                    manager->callbacks.on_exited_edit_mode(handle);

                // @Incomplete: Instead of saving every time we exit the editor, we should control when saving occurs in the game-specific part of the editor
                // Tell the game to save everything that changed
                //if(manager->callbacks.on_save)
                //manager->callbacks.on_save(handle);

                scene.camera = manager->play_camera;

                // Disable gizmos
                manager->gizmos.active = false;

                manager->selected_entity = { -1 };
                manager->mode = SceneMode::RUNNING;
            }
        }

        if(manager->mode == SceneMode::EDITING)
        {
            //manager->dragging = MOUSE(Mouse_Left);

            // if(!manager->dragging)
            //_deselect_gizmos(handle);

            Camera &camera = scene.camera;
            
            if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
            {
                TransformComponent &t = get_transform_comp(manager->selected_entity, handle);
                
                update_transform(t, camera, manager, input_controller, delta_time);
                manager->gizmos.current_distance_to_camera = math::distance(camera.position, t.transform.position) * 0.1f;
                
                if(KEY_DOWN(Key_F))
                {
                    set_target(camera, t.transform.position);
                }
            }

            if(KEY_DOWN(Key_Escape))
            {
                deselect_everything(manager);
            }

            if(MOUSE_DOWN(Mouse_Left))
            {
                manager->dragging = true;
                
                if(manager->editor.selection_enabled)
                {
                    if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                    {
                        TransformComponent &t = get_transform_comp(manager->selected_entity, handle);

                        math::Vec3 pos = t.transform.position;
                        math::Vec3 start;

                        switch(manager->gizmos.transformation_type)
                        {
                        case TransformationType::POSITION:
                        {
                            start = t.transform.position;
                        }
                        break;
                        case TransformationType::SCALE:
                        {
                            start = t.transform.scale;
                        }
                        break;
                        case TransformationType::ROTATION:
                            assert(false);
                            break;
                        }
                        
                        manager->gizmos.initial_scale = t.transform.scale;
                    
                        TranslationConstraint constraint = TranslationConstraint::NONE;

                        math::Vec3 points[2];
                    
                        math::Ray ray = cast_ray(scene, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);

                        r32 max_distance = GIZMO_TOLERANCE * manager->gizmos.current_distance_to_camera;
                        r32 gizmo_size = manager->gizmos.current_distance_to_camera;
                        r32 current_distance = 1000.0f;
                        Line l1 = line_from_ray(ray);

                        // Check X axis
                        Line l2;
                        l2.start = pos;
                        l2.end = pos + math::Vec3(gizmo_size, 0, 0);
                
                        line_vs_line(l1, l2, points);
                        r32 x_dist = math::distance(points[0], points[1]);
                        if(x_dist < max_distance)
                        {
                            current_distance = x_dist;
                            constraint = TranslationConstraint::X;
                            manager->gizmos.current_line = l2;
                            manager->gizmos.initial_offset = points[1] - start;
                        }

                        // Check Y axis
                        l2.end = pos + math::Vec3(0, gizmo_size, 0);
                
                        line_vs_line(l1, l2, points);
                
                        r32 y_dist = math::distance(points[0], points[1]);
                        if(y_dist < current_distance && y_dist < max_distance)
                        {
                            current_distance = y_dist;
                            constraint = TranslationConstraint::Y;
                            manager->gizmos.current_line = l2;
                            manager->gizmos.initial_offset = points[1] - start;
                        }

                        // Check Z axis
                        l2.end = pos + math::Vec3(0, 0, gizmo_size);

                        line_vs_line(l1, l2, points);
                        r32 z_dist = math::distance(points[0], points[1]);
                        if(z_dist < current_distance && z_dist < max_distance)
                        {
                            current_distance = z_dist;
                            constraint = TranslationConstraint::Z;
                            manager->gizmos.current_line = l2;
                            manager->gizmos.initial_offset = points[1] - start;
                        }

                        manager->gizmos.constraint = constraint;
                    }

                    scene::EntityHandle entity = scene::pick_entity(handle, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);
                    
                    if(HANDLES_EQUAL(entity, manager->gizmos.scale_cubes[3]))
                    {
                        manager->gizmos.scaling_mode = ScalingMode::ALL_AXIS;
                        manager->gizmos.scale_mouse_offset = math::Vec2((r32)input_controller->mouse_x, (r32)manager->renderer->window_height - input_controller->mouse_y);
                    }
                    else if(manager->gizmos.constraint == TranslationConstraint::NONE)
                    {
                        select_entity(entity, manager);
                    }
                }
            }
            
            if(MOUSE_UP(Mouse_Left))
            {
                manager->gizmos.scaling_mode = ScalingMode::SINGLE_AXIS;
                if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                {
                    manager->callbacks.on_entity_updated(manager->selected_entity, handle);
                }
                
                manager->gizmos.constraint = TranslationConstraint::NONE;
                manager->dragging = false;
            }

            if(!manager->editor.lock_camera)
            {
                update_editor_camera(camera, scene, input_controller, delta_time);
            }
        }
    }
    
    static void load_scene(SceneHandle handle, u64 load_flags = 0)
    {
        SceneManager *scene_manager = handle.manager;
        if(scene_manager->scene_loaded)
        {
            scene::deactivate_particle_systems(scene_manager->loaded_scene);
            scene::Scene &loaded_scene = get_scene(scene_manager->loaded_scene);
            
            if(!loaded_scene.persistent || load_flags & SceneLoadFlags::FREE_CURRENT_SCENE)
            {
                free_instance_buffers(get_scene(scene_manager->loaded_scene));
                loaded_scene.loaded = false;
                free_scene(scene_manager->loaded_scene);
                scene_manager->loaded_scene = { -1 };
                scene_manager->scene_loaded = false;
            }
        }
        
        Scene &scene = get_scene(handle);
        assert(scene.valid);

        if(!scene.loaded)
        {
            allocate_instance_buffers(scene);
            scene.loaded = true;
        }

        activate_particle_systems(handle);
        
        scene_manager->scene_loaded = true;
        scene_manager->loaded_scene = handle;
    }
        
    i32 _unused_entity_handle(Scene &scene)
    {
        for(i32 index = scene.current_internal_handle; index < scene.max_entity_count; index++)
        {
            i32 i = scene._internal_handles[index];
            if(i == -1)
            {
                scene.current_internal_handle = index;
                return(index);
            }
        }
        
        // start from 0 afterwards
        for(i32 index = 0; index < scene.current_internal_handle; index++)
        {
            i32 i = scene._internal_handles[index];
            if(i == -1)
            {
                scene.current_internal_handle = index;
                return(index);
            }
        }
        
        debug("All entities are in use.");
        assert(false);
        
        return -1;
    }
    
    static RenderComponent& _add_render_component(Scene &scene, EntityHandle entity_handle, b32 cast_shadows = true)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_RENDER;
        
        entity.render_handle = { scene.render_component_count++ };
        scene::RenderComponent &comp = scene.render_components[entity.render_handle.handle];
        comp.v2.render_pass_count = 0;
        comp.casts_shadows = cast_shadows;
        
        return(comp);
    }

    static RenderComponent& add_render_component(SceneHandle handle, EntityHandle entity_handle, b32 cast_shadows = true)
    {
        Scene &scene = get_scene(handle);
        return _add_render_component(scene, entity_handle, cast_shadows);
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, rendering::ShaderHandle shader_handle, RenderComponent &comp)
    {
        comp.v2.render_passes[comp.v2.render_pass_count] = render_pass_handle;
        comp.v2.shader_handles[comp.v2.render_pass_count] = shader_handle;
        comp.v2.render_pass_count++;
    }

    static void add_to_render_pass(const char *pass_name, rendering::ShaderHandle shader_handle, RenderComponent &comp, Renderer *renderer)
    {
        rendering::RenderPassHandle render_pass_handle = rendering::get_render_pass_handle_for_name(pass_name, renderer);
        add_to_render_pass(render_pass_handle, shader_handle, comp);
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, RenderComponent &comp)
    {
        add_to_render_pass(render_pass_handle, comp.v2.shader_handles[0], comp);
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, EntityHandle entity, SceneHandle &scene)
    {
        RenderComponent &render_comp = get_render_comp(entity, scene);
        add_to_render_pass(render_pass_handle, render_comp);
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, rendering::ShaderHandle shader_handle, EntityHandle entity, SceneHandle &scene)
    {
        RenderComponent &render_comp = get_render_comp(entity, scene);
        add_to_render_pass(render_pass_handle, shader_handle, render_comp);
    }

    static void add_all_to_render_pass(rendering::RenderPassHandle render_pass_handle, SceneHandle &handle)
    {
        Scene &scene = scene::get_scene(handle);
        for(i32 i = 0; i < scene.render_component_count; i++)
        {
            add_to_render_pass(render_pass_handle, scene.render_components[i]);
        }
    }

    static void remove_from_render_pass(rendering::RenderPassHandle render_pass_handle, scene::EntityHandle entity, SceneHandle& scene)
    {
        RenderComponent &render_comp = get_render_comp(entity, scene);
        for(i32 i = 0; i < render_comp.v2.render_pass_count; i++)
        {
            if(render_comp.v2.render_passes[i].handle == render_pass_handle.handle)
            {
                render_comp.v2.render_passes[i] = render_comp.v2.render_passes[render_comp.v2.render_pass_count - 1];
                render_comp.v2.shader_handles[i] = render_comp.v2.shader_handles[render_comp.v2.render_pass_count - 1];
                render_comp.v2.render_pass_count--;
                break;
            }
        }
    }

    static TransformComponent& _add_transform_component(Scene &scene, EntityHandle entity_handle)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_TRANSFORM;
        entity.transform_handle = { scene.transform_component_count++ };
        scene::TransformComponent &comp = scene.transform_components[entity.transform_handle.handle];
        comp.transform = rendering::create_transform(math::Vec3(0.0f), math::Vec3(0.0f), math::Vec3(0.0f));
        comp.entity = entity_handle;
        return(comp);
    }

    static TransformComponent& add_transform_component(SceneHandle handle, EntityHandle entity_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_transform_component(scene, entity_handle);
    }

    
    static ParticleSystemComponent & _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_PARTICLES;
        
        entity.particle_system_handle = {scene.particle_system_component_count++};
        scene::ParticleSystemComponent &comp = scene.particle_system_components[entity.particle_system_handle.handle];
        
        comp.handle = create_particle_system(scene.renderer, max_particles, material);
        ParticleSystemInfo* info = get_particle_system_info(comp.handle, scene.renderer);
        assert(info);
        
        info->attributes = attributes;
        
        return(comp);
    }

    static ParticleSystemComponent & add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material)
    {
        Scene &scene = get_scene(handle);
        return _add_particle_system_component(scene, entity_handle, attributes, max_particles, material);
    }

    static LightComponent& _add_light_component(Scene &scene, EntityHandle entity_handle)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_LIGHT;
        entity.light_handle = { scene.light_component_count++ };
        scene::LightComponent &comp = scene.light_components[entity.light_handle.handle];
        
        return(comp);
    }
    
    static LightComponent & add_light_component(SceneHandle &handle, EntityHandle entity_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_light_component(scene, entity_handle);
    }

    // Returns a new valid "EntityHandle". "comp_flags" Specifies the components that the entity should contain.
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene, b32 savable = false)
    {
        i32 new_handle = _unused_entity_handle(scene) + 1;
        
        EntityHandle handle = { new_handle };
        scene._internal_handles[new_handle - 1] = scene.entity_count++;
        
        Entity &entity = scene.entities[scene._internal_handles[new_handle - 1]];
        entity.savable = savable;
        entity.selection_enabled = true;
        entity.handle = handle;
        entity.comp_flags = comp_flags;
        
        if(comp_flags & COMP_TRANSFORM)
        {
            _add_transform_component(scene, handle);
        }
        
        if(comp_flags & COMP_RENDER)
        {
            _add_render_component(scene, handle, true);
        }
        
        // if(comp_flags & COMP_PARTICLES)
        // {
        //     _add_particle_system_component(scene, handle, get_default_particle_system_attributes(), 0, {}, {});
        // }

        if(comp_flags & COMP_LIGHT)
        {
            _add_light_component(scene, handle);
        }
        
        return(handle);
    }

    static EntityHandle register_entity(u64 comp_flags, SceneHandle scene_handle, b32 savable = false)
    {
        Scene &scene = get_scene(scene_handle);
        return _register_entity(comp_flags, scene, savable);
    }
    
    static EntityTemplate _load_template(const char *path, Scene &scene)
    {
        EntityTemplate templ = {};
        
        FILE *file = fopen(path, "r");
        if(file)
        {
            sprintf(templ.file_path, "%s", path);

            i32 last_slash = 0;
            for(i32 i = 0; i < (i32)strlen(path); i++)
            {
                if(path[i] == '/')
                    last_slash = i;
            }

            i32 real_index = 0;
            for(i32 i = last_slash + 1; i < (i32)strlen(path); i++)
            {
                char c = path[i];
                if(c != '.')
                    templ.name[real_index++] = c;
                else
                {
                    templ.name[real_index++] = '\0';
                    break;
                }
            }
            
            char buffer[256];

            while(fgets(buffer, 256, file))
            {
                if(starts_with(buffer, "v2"))
                {
                }
                else if(starts_with(buffer, "-transform"))
                {
                    templ.comp_flags |= COMP_TRANSFORM;
                    templ.transform.position = math::Vec3();
                    templ.transform.scale = math::Vec3(1, 1, 1);
                    templ.transform.rotation = math::Vec3(0, 0, 0);
                    templ.transform.child_handle = EMPTY_TEMPLATE_HANDLE;
                    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "position"))
                        {
                            sscanf(buffer, "position: %f %f %f\n", &templ.transform.position.x, &templ.transform.position.y, &templ.transform.position.z);
                        }
                        else if(starts_with(buffer, "scale"))
                        {
                            sscanf(buffer, "scale: %f %f %f\n", &templ.transform.scale.x, &templ.transform.scale.y, &templ.transform.scale.z);
                        }
                        else if(starts_with(buffer, "rotation"))
                        {
                            sscanf(buffer, "rotation: %f %f %f\n", &templ.transform.rotation.x, &templ.transform.rotation.y, &templ.transform.rotation.z);
                        }
                        else if(starts_with(buffer, "child_templ"))
                        {
                            // @Incomplete
                            // Child template referenced
                        }
                    }
                }
                else if(starts_with(buffer, "-render"))
                {
                    templ.comp_flags |= COMP_RENDER;
                    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "shd:"))
                        {
                            if(starts_with(buffer, "shd_pass"))
                            {
                                // @Incomplete
                                // char pass_name[128];
                                // char shader_file[256];
                                // sscanf(buffer, "shd_pass: %s - %s", pass_name, shader_file);
                                // rendering::ShaderHandle shader_handle;

                                // rendering::ShaderHandle existing = rendering::get_shader_by_path(scene.renderer, shader_file);
                                // if(existing.handle != -1)
                                //     shader_handle = existing;
                                // else
                                //     shader_handle = rendering::load_shader(scene.renderer, shader_file);

                                // // Add the pass information
                                // strncpy(templ.render.v2.render_pass_names[templ.render.v2.render_pass_count], pass_name, strlen(pass_name) + 1);
                                // templ.render.v2.shader_handles[templ.render.v2.render_pass_count++] = shader_handle;
                            }
                            else
                            {
                                char shader_file[256];
                                sscanf(buffer, "shd: %s", shader_file);
                                
                                rendering::ShaderHandle shader_handle = rendering::load_shader(scene.renderer, shader_file);
                                templ.render.v2.material_handle = rendering::create_material(scene.renderer, shader_handle);

                                // Add the pass information
                                strncpy(templ.render.v2.render_pass_names[templ.render.v2.render_pass_count], STANDARD_PASS, strlen(STANDARD_PASS) + 1);
                                templ.render.v2.shader_handles[templ.render.v2.render_pass_count++] = shader_handle;
                            }
                        }
                        else if(starts_with(buffer, "ignore depth"))
                        {
                            sscanf(buffer, "ignore depth: %d\n", &templ.render.ignore_depth);
                        }
                        else if(starts_with(buffer, "receives shadows"))
                        {
                            //sscanf(buffer, "receives shadows: %d\n", &templ.render.receives_shadows);
                        }
                        else if(starts_with(buffer, "cast shadows"))
                        {
                            sscanf(buffer, "cast shadows: %d\n", &templ.render.casts_shadows);
                        }
                        else if(starts_with(buffer, "obj"))
                        {
                            char obj_file[256];
                            sscanf(buffer, "obj: %s", obj_file);
                           
                            templ.render.is_new_version = true;
                            templ.render.v2.buffer_handle = rendering::load_obj(scene.renderer, obj_file, &templ.render.v2.material_handle, &templ.render.mesh_scale);
                        }
                        else if(starts_with(buffer, "prim"))
                        {
                            char *prim_type = buffer + sizeof(char) * 6;

                            if(starts_with(prim_type, "cube"))
                            {
                                templ.render.is_new_version = true;
                                templ.render.v2.buffer_handle = rendering::create_cube(scene.renderer, &templ.render.mesh_scale);
                            }
                            else if(starts_with(prim_type, "plane"))
                            {
                                templ.render.is_new_version = true;
                                templ.render.v2.buffer_handle = rendering::create_plane(scene.renderer, &templ.render.mesh_scale);
                            }
                        }
                        else if(starts_with(buffer, "mtl"))
                        {
                            if(templ.render.material_handle.handle != 0)
                            {
                                debug("A material file was already loaded for this template file: %s\n", templ.file_path);
                            }

                            char mtl_file[256];
                            sscanf(buffer, "mtl: %s", mtl_file);
                            rendering::load_material_from_mtl(scene.renderer, templ.render.v2.material_handle, mtl_file);
                        }
                    }
                }
                else if(starts_with(buffer, "-particles"))
                {
                    templ.comp_flags |= COMP_PARTICLES;
		    
                    templ.particles.max_particles = 0;
                    templ.particles.color_over_lifetime.value_count = 0;
                    templ.particles.size_over_lifetime.value_count = 0;
                    templ.particles.speed_over_lifetime.value_count = 0;
                    templ.particles.material_handle = {};
                    templ.particles.shader_handle = {};
		    
                    ParticleSystemAttributes attributes = get_default_particle_system_attributes();
		    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "max_particles"))
                        {
                            sscanf(buffer, "max_particles: %d", &templ.particles.max_particles);
                        }
                        else if(starts_with(buffer, "shd"))
                        {
                            char shader_file[256];
                            sscanf(buffer, "shd: %s", shader_file);
                            rendering::ShaderHandle shader_handle = rendering::load_shader(scene.renderer, shader_file);
                            templ.particles.material_handle = rendering::create_material(scene.renderer, shader_handle);

                            // @Incomplete: Multiple passes?
                            // Add the pass information
                            templ.particles.shader_handle = shader_handle;
                        }
                        else if(starts_with(buffer, "started"))
                        {
                            sscanf(buffer, "started: %d", &templ.particles.started);
                        }
                        else if(starts_with(buffer, "start_size"))
                        {
                            if(attributes.start_size_type != StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
                            {
                                sscanf(buffer, "start_size: %f", &attributes.size.constant.start_size);
                                attributes.start_size_type = StartParameterType::CONSTANT;
                            }
                        }
                        else if(starts_with(buffer, "random_start_size"))
                        {
                            sscanf(buffer, "random_start_size: %f %f", &attributes.size.random_between_two_constants.s0, &attributes.size.random_between_two_constants.s1);
                            attributes.start_size_type = StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS;
                        }
                        else if(starts_with(buffer, "start_color"))
                        {
                            sscanf(buffer, "start_color: %f %f %f %f", &attributes.start_color.r, &attributes.start_color.g, &attributes.start_color.b, &attributes.start_color.a);
                        }
                        else if(starts_with(buffer, "start_speed"))
                        {
                            if(attributes.start_speed_type != StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
                            {
                                sscanf(buffer, "start_speed: %f", &attributes.speed.constant.start_speed);
                                attributes.start_speed_type = StartParameterType::CONSTANT;
                            }
                        }
                        else if(starts_with(buffer, "random_start_speed"))
                        {
                            sscanf(buffer, "random_start_speed: %f %f", &attributes.speed.random_between_two_constants.s0, &attributes.speed.random_between_two_constants.s1);
                            attributes.start_speed_type = StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS;
                        }
                        else if(starts_with(buffer, "life_time"))
                        {
                            if(attributes.start_life_time_type != StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
                            {
                                sscanf(buffer, "life_time: %lf", &attributes.life.constant.life_time);
                                attributes.start_life_time_type = StartParameterType::CONSTANT;
                            }
                        }
                        else if(starts_with(buffer, "random_life_time"))
                        {
                            sscanf(buffer, "random_life_time: %lf %lf", &attributes.life.random_between_two_constants.l0, &attributes.life.random_between_two_constants.l1);
                            attributes.start_life_time_type = StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS;
                        }
                        else if(starts_with(buffer, "per_second"))
                        {
                            sscanf(buffer, "per_second: %d", &attributes.particles_per_second);
                        }
                        else if(starts_with(buffer, "spread"))
                        {
                            sscanf(buffer, "spread: %f", &attributes.spread);
                        }
                        else if(starts_with(buffer, "direction"))
                        {
                            sscanf(buffer, "direction: %f %f %f", &attributes.direction.x, &attributes.direction.y, &attributes.direction.z);
                        }
                        else if(starts_with(buffer, "texture"))
                        {
                            char texture_path[128];
                            sscanf(buffer, "texture: %s", texture_path);
                            load_texture(texture_path, scene.renderer, LINEAR, REPEAT, TextureFormat::RGBA, attributes.texture_handle);
                        }
                        else if(starts_with(buffer, "gravity"))
                        {
                            sscanf(buffer, "gravity: %f", &attributes.gravity);
                        }
                        else if(starts_with(buffer, "emitter_func"))
                        {
                            char func_name[32];
                            sscanf(buffer, "emitter_func: %s", func_name);

                            attributes.emission_module.emitter_func_type = EmissionFuncType::RANDOM_DIRECTION;
                            if(starts_with(func_name, "2d_square_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE_2D_RANDOM;
                            }
                            else if(starts_with(func_name, "2d_square"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE_2D;
                            }
                            else if(starts_with(func_name, "random_dir"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::RANDOM_DIRECTION;
                            }
                            else if(starts_with(func_name, "dir"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::DIRECTION;
                            }
                            else if(starts_with(func_name, "square"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE;
                            }
                            else if(starts_with(func_name, "square_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE_RANDOM;
                            }
                            else if(starts_with(func_name, "disc"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::DISC;
                            }
                            else if(starts_with(func_name, "disc_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::DISC_RANDOM;
                            }
                            else if(starts_with(func_name, "circle"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::CIRCLE;
                            }
                            else if(starts_with(func_name, "circle_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::CIRCLE_RANDOM;
                            }
                        }
                        else if(starts_with(buffer, "emission_min"))
                        {
                            sscanf(buffer, "emission_min: %f", &attributes.emission_module.min);
                        }
                        else if(starts_with(buffer, "emission_max"))
                        {
                            sscanf(buffer, "emission_max: %f", &attributes.emission_module.max);
                        }
                        else if(starts_with(buffer, "prewarm"))
                        {
                            sscanf(buffer, "prewarm: %d", &attributes.prewarm);
                        }
                        else if(starts_with(buffer, "looping"))
                        {
                            sscanf(buffer, "looping: %d", &attributes.looping);
                        }
                        else if(starts_with(buffer, "duration"))
                        {
                            sscanf(buffer, "duration: %lf", &attributes.duration);
                        }
                        else if(starts_with(buffer, "base_position"))
                        {
                            sscanf(buffer, "base_position: %f %f %f", &attributes.base_position.x, &attributes.base_position.y, &attributes.base_position.z);
                        }
                        else if(starts_with(buffer, "size_key"))
                        {
                            math::Vec2 &value = templ.particles.size_over_lifetime.values[templ.particles.size_over_lifetime.value_count];
                            r64 &key = templ.particles.size_over_lifetime.keys[templ.particles.size_over_lifetime.value_count];
                            sscanf(buffer, "size_key: %lf %f %f", &key, &value.x, &value.y);
                            templ.particles.size_over_lifetime.value_count++;
                        }
                        else if(starts_with(buffer, "color_key"))
                        {
                            b32 keep_start_color = false;
                            for(size_t i = 0; i < strlen(buffer); i++)
                            {
                                if(buffer[i] == '-')
                                {
                                    keep_start_color = true;
                                    break;
                                }
                            }

                            math::Rgba &value = templ.particles.color_over_lifetime.values[templ.particles.color_over_lifetime.value_count];
                            r64 &key = templ.particles.color_over_lifetime.keys[templ.particles.color_over_lifetime.value_count];

                            if(keep_start_color)
                            {
                                sscanf(buffer, "color_key: %lf - %f", &key, &value.a);
                                value.r = attributes.start_color.r;
                                value.g = attributes.start_color.g;
                                value.b = attributes.start_color.b;
                            }
                            else
                            {
                                sscanf(buffer, "color_key: %lf %f %f %f %f", &key, &value.r, &value.g, &value.b, &value.a);
                            }
                            
                            templ.particles.color_over_lifetime.value_count++;
                        }
                        else if(starts_with(buffer, "speed_key"))
                        {
                            r32 &value = templ.particles.speed_over_lifetime.values[templ.particles.speed_over_lifetime.value_count];
                            r64 &key = templ.particles.speed_over_lifetime.keys[templ.particles.speed_over_lifetime.value_count];
                            sscanf(buffer, "speed_key: %lf %f", &key, &value);
                            templ.particles.speed_over_lifetime.value_count++;
                        }
                    }

                    templ.particles.attributes = attributes;
                }
            }
            
            fclose(file);
        }
        else
            assert(false);
        
        return(templ);
    }

#define EMPTY_TRANSFORM { math::Vec3(), math::Vec3(1, 1, 1), math::Vec3(), EMPTY_COMP_HANDLE, EMPTY_COMP_HANDLE };

    static inline void _set_entity_name(EntityHandle handle, const char *name, Scene& scene)
    {
        Entity &entity = scene.entities[scene._internal_handles[handle.handle - 1]];
        strcpy(entity.name, name);
    }

    static inline void _set_entity_template_path(EntityHandle handle, const char *template_path, Scene& scene)
    {
        Entity &entity = scene.entities[scene._internal_handles[handle.handle - 1]];
        strcpy(entity.template_path, template_path);
    }

    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene, b32 savable)
    {
        EntityHandle handle = _register_entity(templ.comp_flags, scene, savable);
        _set_entity_name(handle, templ.name, scene);
        _set_entity_template_path(handle, templ.file_path, scene);
        if(templ.comp_flags & COMP_TRANSFORM)
        {
            TransformComponent &transform = _get_transform_comp(handle, scene);
            transform.transform = rendering::create_transform(templ.transform.position, templ.transform.scale, templ.transform.rotation);
            // @Incomplete: Parent and child handles
        }
        
        if(templ.comp_flags & COMP_RENDER)
        {
            RenderComponent &render = _get_render_comp(handle, scene);
            if(templ.render.is_new_version)
            {
                render.is_new_version = true;
                render.ignore_depth = templ.render.ignore_depth;
                render.v2.buffer_handle = templ.render.v2.buffer_handle;
                render.v2.material_handle = rendering::create_material_instance(scene.renderer, templ.render.v2.material_handle);
                
                if(scene.loaded)
                {
                    // We have to look for the right instance buffers or allocate them
                    rendering::Material &material_instance = rendering::get_material_instance(render.v2.material_handle, scene.renderer);
                    if(material_instance.instanced_vertex_attribute_count > 0)
                    {
                        InstanceBufferData *data = nullptr;
                        
                        for(i32 i = 0; i < scene.instance_buffer_data_count; i++)
                        {
                            InstanceBufferData &current_data = scene.instance_buffer_data[i];
                            if(current_data.buffer_handle.handle == render.v2.buffer_handle.handle // The same buffer
                               && current_data.source_material_handle.handle == material_instance.source_material.handle) // The same source material)
                            {
                                data = &current_data;
                                break;
                            }
                        }

                        if(data)
                        {
                            data->max_count++;
                            
                            for(i32 i = 0; i < material_instance.instanced_vertex_attribute_count; i++)
                            {
                                // We got a match!
                                rendering::InstanceBufferHandle handle = data->instance_buffer_handles[i];
                                i32 max = rendering::get_instance_buffer_max(handle, scene.renderer);
                                
                                material_instance.instanced_vertex_attributes[i].instance_buffer_handle = handle;
                                    
                                if(data->max_count > max)
                                {
                                    i32 new_max = math::next_power_of_two(max + 1);
                                    realloc_instance_buffer(handle, new_max, scene.renderer);
                                }
                            }
                        }
                        else
                        {
                            // Allocate all the needed buffers
                            data = &scene.instance_buffer_data[scene.instance_buffer_data_count++];
                            data->max_count = 1;
                            data->buffer_handle = render.v2.buffer_handle;
                            data->source_material_handle = material_instance.source_material;
                            
                            for(i32 i = 0; i < material_instance.instanced_vertex_attribute_count; i++)
                            {
                                rendering::InstanceBufferHandle handle = rendering::allocate_instance_buffer(material_instance.instanced_vertex_attributes[i].attribute.type, math::next_power_of_two(1), scene.renderer);
                                material_instance.instanced_vertex_attributes[i].instance_buffer_handle = handle;
                                data->instance_buffer_handles[data->instance_buffer_count++] = handle;
                            }
                        }
                    }
                }
        
                
                render.casts_shadows = templ.render.casts_shadows;
                render.mesh_scale = templ.render.mesh_scale;
                
                for(i32 i = 0; i < templ.render.v2.render_pass_count; i++)
                {
                    add_to_render_pass(templ.render.v2.render_pass_names[i], templ.render.v2.shader_handles[i], render, scene.renderer);
                }
            }
            else
            {
                assert(false);
            }
        }
        
        if(templ.comp_flags & COMP_PARTICLES)
        {
            scene::ParticleSystemComponent &ps_comp = scene::_add_particle_system_component(scene, handle, templ.particles.attributes, templ.particles.max_particles, templ.particles.material_handle);

            ParticleSystemInfo *ps = get_particle_system_info(ps_comp.handle, scene.renderer);
            
            for(i32 i = 0; i < templ.particles.size_over_lifetime.value_count; i++)
            {
                r64 key = templ.particles.size_over_lifetime.keys[i];
                math::Vec2 value = templ.particles.size_over_lifetime.values[i];
                add_size_key(*ps, key, value);
            }

            for(i32 i = 0; i < templ.particles.color_over_lifetime.value_count; i++)
            {
                r64 key = templ.particles.color_over_lifetime.keys[i];
                math::Rgba value = templ.particles.color_over_lifetime.values[i];
                add_color_key(*ps, key, value);
            }

            for(i32 i = 0; i < templ.particles.speed_over_lifetime.value_count; i++)
            {
                r64 key = templ.particles.speed_over_lifetime.keys[i];
                r32 value = templ.particles.speed_over_lifetime.values[i];
                add_speed_key(*ps, key, value);
            }

            if(templ.comp_flags & COMP_TRANSFORM)
            {
                ps->transform = rendering::create_transform(ps->attributes.base_position + templ.transform.position,
                                                            templ.transform.scale, templ.transform.rotation);
            }

            if(templ.particles.attributes.texture_handle.handle != 0)
            {
                rendering::set_uniform_value(scene.renderer, ps->material_handle, "tex0", templ.particles.attributes.texture_handle);
            }

            ps_comp.render_pass = rendering::get_render_pass_handle_for_name(STANDARD_PASS, scene.renderer);
            
            if(templ.particles.started)
                start_particle_system(ps_comp.handle, scene.renderer);
        }
        
        return(handle);
    }
    
    static EntityHandle _register_entity_from_template_file(const char *path, Scene &scene, b32 savable = false)
    {
        EntityTemplateState *template_state = scene.template_state;
        
        EntityTemplate *templ = nullptr;
        i32 handle = -1;
        
        for(i32 i = 0; i < template_state->template_count; i++)
        {
            if(strcmp(template_state->templates[i].file_path, path) == 0)
            {
                handle = i;
                break;
            }
        }
        
        if(handle != -1) // The template was already loaded
        {
            templ = &template_state->templates[handle];
        }
        else
        {
            template_state->templates[template_state->template_count] = _load_template(path, scene);
            templ = &template_state->templates[template_state->template_count++];
        }
        
        assert(templ);
        
        return(_register_entity_with_template(*templ, scene, savable));
    }

    static EntityHandle register_entity_from_template_file(const char *path, SceneHandle scene_handle, b32 savable = false)
    {
        Scene &scene = get_scene(scene_handle);
        return _register_entity_from_template_file(path, scene, savable);
    }
    
    i32 _pack_transform_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_TRANSFORM)
        {
            for(i32 index = entity.transform_handle.handle; index < scene.transform_component_count - 1; index++)
            {
                scene.transform_components[index] = scene.transform_components[index + 1];
            }
            scene.transform_component_count--;
            return entity.transform_handle.handle;
        }
        return -1;
    }
    
    i32 _pack_render_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_RENDER)
        {
            for(i32 index = entity.render_handle.handle; index < scene.render_component_count - 1; index++)
            {
                scene.render_components[index] = scene.render_components[index + 1];
            }
            scene.render_component_count--;
            return entity.render_handle.handle;
        }
        return -1;
    }

    i32 _pack_light_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_LIGHT)
        {
            for(i32 index = entity.light_handle.handle; index < scene.light_component_count - 1; index++)
            {
                scene.light_components[index] = scene.light_components[index + 1];
            }
            scene.light_component_count--;
            return entity.light_handle.handle;
        }
        return -1;
    }

    i32 _pack_particle_system_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_PARTICLES)
        {
            ParticleSystemHandle ps_handle = scene.particle_system_components[entity.particle_system_handle.handle].handle;
            remove_particle_system(scene.renderer, ps_handle);
            for(i32 index = entity.particle_system_handle.handle; index < scene.particle_system_component_count - 1; index++)
            {
                scene.particle_system_components[index] = scene.particle_system_components[index + 1];
            }
            scene.particle_system_component_count--;
            return entity.particle_system_handle.handle;
        }
        return -1;
    }
    
    static void _unregister_entity(EntityHandle handle, Scene &scene)
    {
        if(handle.handle == 0 || scene._internal_handles[handle.handle - 1] == -1 && scene._internal_handles[handle.handle - 1] < scene.entity_count)
            return;
        
        i32 removed_handle = handle.handle;
        
        // Invalidate handle
        handle.handle = 0;
        
        // If only one entity was present, we should just zero everything...
        if(scene.entity_count == 1)
        {
            scene.entity_count = 0;
            scene.transform_component_count = 0;
            scene.render_component_count = 0;
            scene.particle_system_component_count = 0;
            scene.current_internal_handle = 0;
        }
        else
        {
            // Get the handle into the real entity array
            i32 real_handle = scene._internal_handles[removed_handle - 1];
            Entity &entity = scene.entities[real_handle];
            
            // Pack the components in scene by removing the unregistered entity's components and moving the rest to pack the arrays. If the returned handles are -1 the entity didn't have that component set.
            i32 transform_handle = _pack_transform_components(entity, scene);
            i32 render_handle = _pack_render_components(entity, scene);
            i32 particle_system_handle = _pack_particle_system_components(entity, scene);
            i32 light_component_handle = _pack_light_components(entity, scene);
            
            // Remember to reset the internal handle to -1 to indicate an unused index
            scene._internal_handles[removed_handle - 1] = -1;
            
            // Run through all existing entities and pack the array to remove the unregistered one. All component handles affected by the removal are decremented.
            for(i32 index = real_handle; index < scene.entity_count - 1; index++)
            {
                scene.entities[index] = scene.entities[index + 1];

                if(transform_handle != -1 && scene.entities[index].transform_handle.handle > transform_handle)
                {
                    scene.entities[index].transform_handle.handle--;
                }
                
                if(render_handle != -1 && scene.entities[index].render_handle.handle > render_handle)
                {
                    scene.entities[index].render_handle.handle--;
                }
                
                if(particle_system_handle != -1 && scene.entities[index].particle_system_handle.handle > particle_system_handle)
                {
                    scene.entities[index].particle_system_handle.handle--;
                }

                if(light_component_handle != -1 && scene.entities[index].light_handle.handle > light_component_handle)
                {
                    scene.entities[index].light_handle.handle--;
                }
                
                scene.active_entities[index] = scene.active_entities[index + 1];
            }
            
            scene.entity_count--;
            
            // We also have to update the internal handles if they were placed after the unregistered entity
            for(i32 internal_index = 0; internal_index < scene.max_entity_count; internal_index++)
            {
                i32 current_handle = scene._internal_handles[internal_index];
                if(current_handle > real_handle)
                {
                    scene._internal_handles[internal_index] = current_handle - 1;
                }
            }
        }
    }

    static void unregister_entity(EntityHandle entity, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        _unregister_entity(entity, scene);
    }

    static void place_entity_from_template(math::Vec3 position, const char* path, SceneManager *manager)
    {
        EntityHandle entity = register_entity_from_template_file(path, manager->loaded_scene, true);
        TransformComponent &transform = get_transform_comp(entity, manager->loaded_scene);
        rendering::set_position(transform.transform, position);
    }
    
    static void _set_active(EntityHandle handle, b32 active, Scene &scene)
    {
        if(handle.handle > 0)
        {
            i32 internal_handle = scene._internal_handles[handle.handle - 1];
            if(internal_handle > -1)
            {
                scene.active_entities[internal_handle] = active;
            }
        }
    }

    static void set_active(EntityHandle handle, b32 active, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        _set_active(handle, active, scene);
    }

    static void set_entity_selection_enabled(EntityHandle entity_handle, b32 enabled, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        i32 internal_handle = scene._internal_handles[entity_handle.handle - 1];
        assert(internal_handle > -1);
        
        Entity &entity = scene.entities[internal_handle];
        entity.selection_enabled = enabled;
    }

    static void set_entity_tag(const char *tag, EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        i32 internal_handle = scene._internal_handles[entity_handle.handle - 1];
        assert(internal_handle > -1);
        
        Entity &entity = scene.entities[internal_handle];
        strcpy(entity.tag, tag);
    }

    static const char * get_entity_tag(EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        i32 internal_handle = scene._internal_handles[entity_handle.handle - 1];
        assert(internal_handle > -1);
        
        const Entity &entity = scene.entities[internal_handle];
        return entity.tag;
    }
    
    static void set_entity_name(const char *name, EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        i32 internal_handle = scene._internal_handles[entity_handle.handle - 1];
        assert(internal_handle > -1);
        
        Entity &entity = scene.entities[internal_handle];
        strcpy(entity.name, name);
    }

    static const char * get_entity_name(EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        i32 internal_handle = scene._internal_handles[entity_handle.handle - 1];
        assert(internal_handle > -1);
        
        const Entity &entity = scene.entities[internal_handle];
        return entity.name;
    }
    
    // Returns a direct pointer to the TransformComponent of the specified entity
    static TransformComponent& _get_transform_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle > -1);
        
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_TRANSFORM);
        
        TransformComponent& comp = scene.transform_components[entity.transform_handle.handle];
        return(comp);
    }

    static TransformComponent& get_transform_comp(EntityHandle entity, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        return _get_transform_comp(entity, scene);
    }
     
    // @Note(Daniel): Should we really return a pointer here? A reference might suffice, since we don't ever use the null-value for anything....
    // Returns a direct pointer to the RenderComponent of the specified entity
    static RenderComponent& _get_render_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_RENDER);
        
        RenderComponent& comp = scene.render_components[entity.render_handle.handle];
        return(comp);
    }

    static RenderComponent& get_render_comp(EntityHandle entity, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        return _get_render_comp(entity, scene);
    }
    
    static ParticleSystemComponent& _get_particle_system_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_PARTICLES);
        
        ParticleSystemComponent& comp = scene.particle_system_components[entity.particle_system_handle.handle];
        return(comp);
    }

    static ParticleSystemComponent& get_particle_system_comp(EntityHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return _get_particle_system_comp(handle, scene);
    }
    
    static LightComponent &_get_light_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_LIGHT);
        
        LightComponent& comp = scene.light_components[entity.light_handle.handle];
        return(comp);
    }

    static LightComponent &get_light_comp(EntityHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return _get_light_comp(handle, scene);
    }
    
    static Camera & get_scene_camera(SceneHandle handle)
    {
        i32 real_handle = handle.manager->_internal_scene_handles[handle.handle - 1];
        return handle.manager->scenes[real_handle].camera;
    }
    
#define SET_MAT_ARRAY_VALUE(type) static void set_uniform_array_value(EntityHandle handle, const char *array_name, i32 index, const char *variable_name, type value, SceneHandle &scene) \
    {                                                                   \
        RenderComponent &render = get_render_comp(handle, scene);       \
        rendering::set_uniform_array_value(scene.manager->renderer, render.v2.material_handle, array_name, index, variable_name, value); \
    }                                                                   \

    SET_MAT_ARRAY_VALUE(r32)
    SET_MAT_ARRAY_VALUE(math::Vec2)
    SET_MAT_ARRAY_VALUE(math::Vec3)
    SET_MAT_ARRAY_VALUE(math::Vec4)
    SET_MAT_ARRAY_VALUE(i32)
    SET_MAT_ARRAY_VALUE(math::Mat4)
    SET_MAT_ARRAY_VALUE(rendering::TextureHandle)


#define STANDARD_PASS_HANDLE { 1 }

    static Entity& _get_entity(EntityHandle handle, SceneHandle& scene_handle)
    {
        Scene& scene = get_scene(scene_handle);
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle > -1);
        
        Entity& entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_TRANSFORM);

        return entity;
    }

    static void recompute_transforms(TransformComponent& root, Scene& scene)
    {
        rendering::recompute_transform(root.transform);
        if(root.parent_handle.handle != -1)
        {
            math::Mat4 parent_model = scene.transform_components[root.parent_handle.handle].transform.model;
            root.transform.model = parent_model * root.transform.model;
        }

        for(i32 i = 0; i < root.child_count; i++)
        {
            TransformComponent& child = scene.transform_components[root.child_handles[i].handle];
            recompute_transforms(child, scene);
        }
    }

    static void add_child(TransformComponentHandle parent_handle, TransformComponentHandle child_handle, SceneHandle& scene_handle)
    {
        Scene& scene = get_scene(scene_handle);
        TransformComponent& parent = scene.transform_components[parent_handle.handle];
        assert(parent.child_count + 1 < MAX_CHILDREN);
        parent.child_handles[parent.child_count++] = child_handle;
        parent.transform.dirty = true;
        
        TransformComponent& child = scene.transform_components[child_handle.handle];
        child.parent_handle = parent_handle;
    }

    static void add_child(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene)
    {
        Entity& parent = _get_entity(parent_handle, scene);
        Entity& child = _get_entity(child_handle, scene);
        add_child(parent.transform_handle, child.transform_handle, scene);
    }

    static void remove_child(TransformComponentHandle parent_handle, TransformComponentHandle child_handle, SceneHandle& scene_handle)
    {
        Scene& scene = get_scene(scene_handle);
        TransformComponent& parent = scene.transform_components[parent_handle.handle];
        parent.transform.dirty = true;
        
        assert(parent.child_count + 1 < MAX_CHILDREN);

        for(i32 i = 0; i < parent.child_count; i++)
        {
            if(parent.child_handles[i].handle == child_handle.handle)
            {
                TransformComponent& child = scene.transform_components[child_handle.handle];
                child.parent_handle = {0};
                
                parent.child_handles[i] = parent.child_handles[parent.child_count - 1];
                parent.child_count--;
                break;
            }
        }
    }

    static void remove_child(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene)
    {
        Entity& parent = _get_entity(parent_handle, scene);
        Entity& child = _get_entity(child_handle, scene);
        remove_child(parent.transform_handle, child.transform_handle, scene);
    }

    static void add_parent(TransformComponentHandle child_handle, TransformComponentHandle parent_handle, SceneHandle& scene)
    {
        add_child(parent_handle, child_handle, scene);
    }

    static void add_parent(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene)
    {
        Entity& parent = _get_entity(parent_handle, scene);
        Entity& child = _get_entity(child_handle, scene);
        add_parent(parent.transform_handle, child.transform_handle, scene);
    }

    static void remove_parent(TransformComponentHandle child_handle, TransformComponentHandle parent_handle, SceneHandle& scene)
    {
        remove_child(parent_handle, child_handle, scene);
    }
    
    static void remove_parent(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene)
    {
        Entity& parent = _get_entity(parent_handle, scene);
        Entity& child = _get_entity(child_handle, scene);
        remove_parent(parent.transform_handle, child.transform_handle, scene);
    }
    
    static void push_scene_for_rendering(scene::Scene &scene, Renderer *renderer)
    {
        renderer->camera = scene.camera;

        for(i32 i = 0; i < renderer->render.pass_count; i++)
        {
            rendering::RenderPass &pass = renderer->render.passes[i];
            if(pass.use_scene_camera)
            {
                pass.camera = scene.camera;
            }
        }
    
        renderer->render.dir_light_count = 0;
        renderer->render.point_light_count = 0;
    
        QueuedRenderCommand *queued_commands = renderer->render.queued_commands;
        i32 normal_count = 0;

        i32 particles_to_push[64];
        i32 particles_count = 0;

        for(i32 ent_index = 0; ent_index < scene.entity_count; ent_index++)
        {
            const scene::Entity &ent = scene.entities[ent_index];

            if(scene.active_entities[ent_index])
            {
                TransformComponent* start_component = &scene.transform_components[ent.transform_handle.handle];
                
                if(start_component->transform.dirty)
                {
                    while(start_component->parent_handle.handle != -1)
                    {
                        TransformComponent& parent = scene.transform_components[start_component->parent_handle.handle];
                        if(parent.transform.dirty)
                        {
                            start_component = &parent;
                        }
                    }

                    assert(start_component);
                    recompute_transforms(*start_component, scene);
                }
            }
        }
        
    
        for(i32 ent_index = 0; ent_index < scene.entity_count; ent_index++)
        {
            const scene::Entity &ent = scene.entities[ent_index];
        
            if (scene.active_entities[ent_index])
            {
                scene::TransformComponent &transform = scene.transform_components[ent.transform_handle.handle];
                
                // Create a copy of the position, rotation and scale since we don't want the parents transform to change the child's transform. Only when rendering.
                math::Vec3 position = transform.transform.position;
            
                if(ent.comp_flags & scene::COMP_LIGHT)
                {
                    scene::LightComponent &light_comp = scene.light_components[ent.light_handle.handle];
                
                    switch(light_comp.type)
                    {
                    case scene::LightType::DIRECTIONAL:
                    renderer->render.directional_lights[renderer->render.dir_light_count++] = light_comp.dir_light;
                    break;
                    case scene::LightType::POINT:
                    light_comp.point_light.position = position;
                    renderer->render.point_lights[renderer->render.point_light_count++] = light_comp.point_light;
                    break;
                    default:
                    assert(false);
                    }
                }

                if (ent.comp_flags & scene::COMP_RENDER)
                {
                    scene::RenderComponent &render = scene.render_components[ent.render_handle.handle];
                
                    if(render.is_new_version)
                    {
                        QueuedRenderCommand *command = nullptr;
                        rendering::Material &instance = get_material_instance(render.v2.material_handle, renderer);
                    
                        for(i32 i = 0; i < normal_count; i++)
                        {
                            QueuedRenderCommand &cmd = queued_commands[i];
                            if(cmd.buffer_handle.handle == render.v2.buffer_handle.handle
                               && cmd.original_material.handle == instance.source_material.handle && cmd.ignore_depth == render.ignore_depth)
                            {
                                // It's a doozy
                                command = &cmd;
                                break;
                            }
                        }

                        if(!command)
                        {
                            command = &queued_commands[normal_count++];
                            command->ignore_depth = render.ignore_depth;
                            command->buffer_handle = render.v2.buffer_handle;
                            command->original_material = instance.source_material;
                            command->count = 0;
                        }

                        if(render.v2.render_pass_count > 0)
                        {
                            if(render.wireframe_enabled)
                            {
                                rendering::push_buffer_to_render_pass(renderer, render.v2.buffer_handle, renderer->render.wireframe_material, transform.transform, renderer->render.wireframe_shader, render.v2.render_passes[0], rendering::CommandType::NO_DEPTH);
                            }
                        
                            BatchedCommand &batch_command = command->commands[command->count];
                            batch_command.transform = transform.transform;
                            batch_command.material_handle = render.v2.material_handle;
                            batch_command.casts_shadows = render.casts_shadows;
                            batch_command.pass_count = render.v2.render_pass_count;
                        
                            for(i32 i = 0; i < render.v2.render_pass_count; i++)
                            {
                                batch_command.passes[i] = render.v2.render_passes[i];
                                batch_command.shader_handles[i] = render.v2.shader_handles[i];
                            }
                        
                            command->count++;
                        }
                    }
                }
                else if(ent.comp_flags & scene::COMP_PARTICLES)
                {
                    if(ent.particle_system_handle.handle != -1)
                    {
                        scene::ParticleSystemComponent &ps = scene.particle_system_components[ent.particle_system_handle.handle];
                    
                        i32 _internal_handle = renderer->particles._internal_handles[ps.handle.handle - 1];
                        ParticleSystemInfo& system = renderer->particles.particle_systems[_internal_handle];
                    
                        system.transform.position = system.attributes.base_position;
                    
                        if(ent.comp_flags & scene::COMP_TRANSFORM)
                        {
                            system.transform.position += transform.transform.position;
                            system.transform.scale = transform.transform.scale;
                            system.transform.orientation = transform.transform.orientation;
                            
                            if(transform.transform.dirty)
                            {
                                rendering::recompute_transform(system.transform);
                            }
                        }

                        if(system.simulating || system.paused)
                        {
                            particles_to_push[particles_count++] = ps.handle.handle;
                        }
                    }
                }
            }
        }

        for(i32 index = 0; index < normal_count; index++)
        {
            QueuedRenderCommand &queued_command = queued_commands[index];
        
            BatchedCommand &first_command = queued_command.commands[0];
            rendering::Material &material = get_material_instance(first_command.material_handle, renderer);

            if(material.lighting.receives_light)
                update_lighting_for_material(first_command, renderer);

            for(i32 batch_index = 0; batch_index < queued_command.count; batch_index++)
            {
                BatchedCommand &render_command = queued_command.commands[batch_index];

                rendering::Material &mat_instance = get_material_instance(render_command.material_handle, renderer);

                if (mat_instance.instanced_vertex_attribute_count == 0)
                {
                    // Just push the buffer as a normal draw call
                    for (i32 pass_index = 0; pass_index < render_command.pass_count; pass_index++)
                    {
                        rendering::push_buffer_to_render_pass(renderer, queued_command.buffer_handle, render_command.material_handle, render_command.transform, render_command.shader_handles[pass_index], render_command.passes[pass_index], queued_command.ignore_depth ? rendering::CommandType::NO_DEPTH : rendering::CommandType::WITH_DEPTH);
                    }
                    continue;
                }
                else
                {
                    // We can make one call instead
                    for (i32 i = 0; i < mat_instance.instanced_vertex_attribute_count; i++)
                    {
                        rendering::VertexAttributeInstanced &va = mat_instance.instanced_vertex_attributes[i];
                        rendering::VertexAttribute &attr = va.attribute;
                        
                        switch (attr.type)
                        {
                        case rendering::ValueType::FLOAT:
                        rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float_val, renderer);
                        break;
                        case rendering::ValueType::FLOAT2:
                        rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float2_val, renderer);
                        break;
                        case rendering::ValueType::FLOAT3:
                        {
                            math::Vec3 val = attr.float3_val;
                            
                            if(va.mapping_type != rendering::VertexAttributeMappingType::NONE)
                            {
                                if(va.mapping_type == rendering::VertexAttributeMappingType::POSITION)
                                {
                                    assert(false);
                                    // val = render_command.transform.position;
                                }
                                else if(va.mapping_type == rendering::VertexAttributeMappingType::ROTATION)
                                {
                                    assert(false);
                                    // val = render_command.transform.rotation;
                                }
                                else if(va.mapping_type == rendering::VertexAttributeMappingType::SCALE)
                                {
                                    assert(false);
                                    // val = render_command.transform.scale;
                                }
                                
                            }
                            rendering::add_instance_buffer_value(va.instance_buffer_handle, val, renderer);
                        }
                        break;
                        case rendering::ValueType::FLOAT4:
                        rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float4_val, renderer);
                        break;
                        case rendering::ValueType::MAT4:
                        {
                            math::Mat4 val = attr.mat4_val;
                            
                            if(va.mapping_type == rendering::VertexAttributeMappingType::MODEL)
                            {
                                rendering::Transform &transform = render_command.transform;
                                val = math::transpose(transform.model);
                            }
                            rendering::add_instance_buffer_value(va.instance_buffer_handle, val, renderer);
                        }
                        break;
                        default:
                        assert(false);
                        }
                    }
                }
            }
        
            // Push the command to the shadow buffer if it casts shadows
            if(first_command.casts_shadows)
            {
                rendering::push_instanced_buffer_to_shadow_pass(renderer, queued_command.count, queued_command.buffer_handle, material.instanced_vertex_attributes, material.instanced_vertex_attribute_count);
            }
        
            // Push the command to the correct render passes
            for (i32 pass_index = 0; pass_index < first_command.pass_count; pass_index++)
            {
                rendering::push_instanced_buffer_to_render_pass(renderer, queued_command.count, queued_command.buffer_handle, first_command.material_handle, first_command.shader_handles[pass_index], first_command.passes[pass_index], queued_command.ignore_depth ? rendering::CommandType::NO_DEPTH : rendering::CommandType::WITH_DEPTH);
            }
        }
    
        for(i32 i = 0; i < particles_count; i++)
        {
            i32 _internal_handle = renderer->particles._internal_handles[particles_to_push[i] - 1];
            ParticleSystemInfo& system = renderer->particles.particle_systems[_internal_handle];
            rendering::Material& particle_material = get_material_instance(system.material_handle, renderer);
            
            rendering::push_instanced_buffer_to_render_pass(renderer, system.particle_count, renderer->particles.quad_buffer, system.material_handle, particle_material.shader, rendering::get_render_pass_handle_for_name(STANDARD_PASS, renderer), rendering::CommandType::WITH_DEPTH);
        }

        draw_gizmos(scene.scene_manager);
    }
    
}
