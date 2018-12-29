#define ENTITY_IS_VALID(handle) handle.handle > 0

namespace scene
{
    static Scene create_scene(Renderer &renderer, EntityTemplateState &template_state, i32 initial_entity_array_size);
    static SceneHandle create_scene(SceneManager *scene_manager, i32 initial_entity_array_size);
    static void free_scene(SceneHandle scene);
    static void load_scene(SceneHandle handle, u64 scene_load_flags);
    
    // Scene handle
    static RenderComponent& add_render_component(SceneHandle scene, EntityHandle entity_handle, b32 cast_shadows);
    static TransformComponent& add_transform_component(SceneHandle scene, EntityHandle entity_handle);
    static ParticleSystemComponent& add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material, rendering::MaterialInstanceArrayHandle array_handle);
    static EntityHandle register_entity(u64 comp_flags, SceneHandle scene);
    static void unregister_entity(EntityHandle handle, SceneHandle scene);
    static EntityTemplate _load_template(const char *path, SceneHandle scene);
    static EntityHandle register_entity_from_template_file(const char *path, SceneHandle scene);
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
    static ParticleSystemComponent& _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material, rendering::MaterialInstanceArrayHandle array_handle);
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene);
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

        }

        // Set the current index to look for a free handle
        scene_manager->current_internal_index = handle + 1;
        if(scene_manager->current_internal_index == global_max_scenes)
        {
            scene_manager->current_internal_index = 0;
        }
        
        return handle;
    }

    static SceneHandle create_scene(SceneManager *scene_manager, i32 initial_entity_array_size = 1024)
    {
        i32 internal_handle = _find_next_free_internal_handle(scene_manager);
        assert(internal_handle != -1);

        i32 real_handle = scene_manager->scene_count++;
        scene_manager->_internal_scene_handles[internal_handle] = real_handle;
        Scene &scene = scene_manager->scenes[real_handle];
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

        scene.material_array_handle = rendering::allocate_material_instance_array(*scene_manager->renderer, initial_entity_array_size);
        
        if(scene_manager->debug_cube.handle == 0)
        {
            scene_manager->debug_cube = rendering::create_cube(*scene_manager->renderer, nullptr);
            scene_manager->debug_shader_handle = rendering::load_shader(*scene_manager->renderer, "../assets/shaders/unlit.shd");
            scene_manager->debug_material = rendering::create_material(*scene_manager->renderer, scene_manager->debug_shader_handle);
        }

        scene_manager->debug_material_instance = rendering::create_material_instance(*scene_manager->renderer, scene_manager->debug_material, scene.material_array_handle);

        for(i32 index = 0; index < initial_entity_array_size; index++)
        {
            scene._internal_handles[index] = -1;
            scene.active_entities[index] = true;
            scene.transform_components[index].position = math::Vec3(0, 0, 0);
            scene.transform_components[index].scale = math::Vec3(1, 1, 1);
            scene.transform_components[index].rotation = math::Vec3(0, 0, 0);
            scene.transform_components[index].parent_handle = EMPTY_COMP_HANDLE;
            scene.transform_components[index].child_handle = EMPTY_COMP_HANDLE;
        }

        SceneHandle handle;
        handle.handle = internal_handle + 1;
        handle.manager = scene_manager;
        return handle;
    }

    struct InstancePair
    {
        rendering::MaterialHandle material_handle;
        rendering::BufferHandle buffer_handle;
        rendering::MaterialInstanceHandle material_instances[1024];
        i32 count;

        rendering::ValueType types[8];
        i32 type_count;
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
            rendering::Material &material_instance = rendering::get_material_instance(comp.v2.material_handle, *renderer);

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
                    pair.type_count = 0;
                    pair.count = 0;
                    pair.buffer_handle = comp.v2.buffer_handle;
                    pair.material_handle = material_instance.source_material;
                    
                    for(i32 j = 0; j < material_instance.instanced_vertex_attribute_count; j++)
                    {
                        pair.types[pair.type_count++] = material_instance.instanced_vertex_attributes[j].attribute.type;
                    }
                    
                    pair.material_instances[pair.count++] = comp.v2.material_handle;
                }
            }
        }

        // Allocate the buffers
        for(i32 i = 0; i < pair_count; i++)
        {
            InstancePair &pair = instance_pairs[i];

            // Allocate all buffers
            for(i32 j = 0; j < pair.type_count; j++)
            {
                rendering::InstanceBufferHandle instance_buffer_handle = rendering::allocate_instance_buffer(pair.types[j], math::next_power_of_two(pair.count), *renderer);

                // Update all materials with the correct buffer handle
                for(i32 k = 0; k < pair.count; k++)
                {
                    rendering::Material &material = get_material_instance(pair.material_instances[k], *renderer);
                    material.instanced_vertex_attributes[j].instance_buffer_handle = instance_buffer_handle;
                }

                assert(scene.instance_buffer_count < MAX_INSTANCE_BUFFER_HANDLES);
                scene.instance_buffer_handles[scene.instance_buffer_count++] = instance_buffer_handle;
            }
        }
    }

    static void free_instance_buffers(Scene &scene)
    {
        for(i32 i = 0; i < scene.instance_buffer_count; i++)
        {
            rendering::free_instance_buffer(scene.instance_buffer_handles[i], *scene.renderer);
        }
        scene.instance_buffer_count = 0;
    }

    static void free_scene(SceneHandle handle)
    {
        SceneManager *scene_manager = handle.manager;
        i32 internal_handle = scene_manager->_internal_scene_handles[handle.handle - 1];
        Scene &scene = scene_manager->scenes[internal_handle];
        if(scene.valid)
        {
			scene.valid = false;
            scene.loaded = false;

			if (scene.entity_count > 0)
            {
                scene.entity_count = 0;
                scene.transform_component_count = 0;
                scene.render_component_count = 0;
            
                for(i32 index = 0; index < scene.particle_system_component_count; index++)
                {
                    ParticleSystemComponent& ps_comp = scene.particle_system_components[index];
                    remove_particle_system(*scene.renderer, ps_comp.handle);
                }
                
                rendering::free_material_instance_array(scene.material_array_handle, *scene.scene_manager->renderer);
                scene.particle_system_component_count = 0;
                scene.light_component_count = 0;
                scene.current_internal_handle = 0;
                clear(&scene.memory_arena);
            }

            free_instance_buffers(scene);
            
            scene_manager->_internal_scene_handles[handle.handle - 1] = -1;
            
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
            stop_particle_system(scene.particle_system_components[i].handle, *scene.renderer);
        }
    }

    
    static void activate_particle_systems(SceneHandle handle)
    {
        Scene &scene = get_scene(handle);                
        for(i32 i = 0; i < scene.particle_system_component_count; i++)
        {
            start_particle_system(scene.particle_system_components[i].handle, *scene.renderer);
        }
    }
    static math::Ray cast_ray_new(Scene &scene, i32 mouse_x, i32 mouse_y)
    {
        i32 width = scene.renderer->window_width;
        i32 height = scene.renderer->window_height;
        Camera &camera = scene.camera;

        r32 x = ((2.0f * ((r32)mouse_x / (r32)width)) - 1.0f) / camera.projection_matrix.a;
        r32 y = -((2.0f * ((r32)mouse_y / (r32)height)) - 1.0f) / camera.projection_matrix.f;
        r32 z = -1.0f;

        math::Ray ray;
        ray.direction = math::normalize(math::Vec3(x, y, z) * camera.view_matrix);
        ray.origin = camera.position;

        return ray;
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
        
        if(entity.handle == manager->gizmos.x_arrow.handle)
        {
            manager->gizmos.selected_gizmo = Gizmos::X_ARROW;
            return true;
        }

        if(entity.handle == manager->gizmos.y_arrow.handle)
        {
            manager->gizmos.selected_gizmo = Gizmos::Y_ARROW;
            return true;
        }

        if(entity.handle == manager->gizmos.z_arrow.handle)
        {
            manager->gizmos.selected_gizmo = Gizmos::Z_ARROW;
            return true;
        }

        return false;
    }

    static void deactivate_gizmo_arrows(SceneManager *manager)
    {
        set_active(manager->gizmos.x_arrow, false, manager->loaded_scene);
        set_active(manager->gizmos.y_arrow, false, manager->loaded_scene);
        set_active(manager->gizmos.z_arrow, false, manager->loaded_scene);
    }
    
    static EntityHandle pick_entity(SceneHandle handle, i32 mouse_x, i32 mouse_y)
    {
        Scene &scene = get_scene(handle);

        math::Ray ray = cast_ray(scene, mouse_x, mouse_y);
        math::Ray new_ray = cast_ray_new(scene, mouse_x, mouse_y);
        
        r32 dist = 100000; // Just set a crazy max distance
        
        EntityHandle entity_handle = { -1 };
        
        for(i32 i = 0; i < scene.entity_count; i++)
        {
            const scene::Entity &ent = scene.entities[i];
            
            if (scene.active_entities[i])
            {
                if (ent.comp_flags & scene::COMP_RENDER)
                {
                    scene::TransformComponent &transform = scene.transform_components[ent.transform_handle.handle];
                    scene::RenderComponent &render_comp = scene.render_components[ent.render_handle.handle];
                    
                    math::BoundingBox box;
                    math::Vec3 real_scale = transform.scale * render_comp.mesh_scale;
                    box.min = math::Vec3(transform.position.x - real_scale.x * 0.5f, transform.position.y - real_scale.y * 0.5f, transform.position.z - real_scale.z * 0.5f);
                    box.max = math::Vec3(transform.position.x + real_scale.x * 0.5f, transform.position.y + real_scale.y * 0.5f, transform.position.z + real_scale.z * 0.5f);

                    math::Vec3 intersection_point;
                    if(aabb_ray_intersection(ray, box, &intersection_point))
                    {
                        if(_is_gizmo(ent.handle, handle.manager))
                            return ent.handle;
                        
                        r32 new_dist = math::distance(scene.camera.position, intersection_point);
                    
                        if(new_dist < dist)
                        {
                            //printf("Found: %s\n", ent.name);
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

        manager->gizmos.x_arrow = scene::register_entity_from_template_file("../assets/templates/editor/gizmos/x_arrow.tmpl", scene);
        manager->gizmos.y_arrow = scene::register_entity_from_template_file("../assets/templates/editor/gizmos/y_arrow.tmpl", scene);
        manager->gizmos.z_arrow = scene::register_entity_from_template_file("../assets/templates/editor/gizmos/z_arrow.tmpl", scene);
        
        scene::TransformComponent &x_t = scene::get_transform_comp(manager->gizmos.x_arrow, scene);
        scene::TransformComponent &y_t = scene::get_transform_comp(manager->gizmos.y_arrow, scene);
        scene::TransformComponent &z_t = scene::get_transform_comp(manager->gizmos.z_arrow, scene);
        
        x_t.position = math::Vec3(0.5f, 2, 0);
        y_t.position = math::Vec3(0, 2.5f, 0);
        z_t.position = math::Vec3(0, 2, 0.5f);

        set_active(manager->gizmos.x_arrow, false, scene);
        set_active(manager->gizmos.y_arrow, false, scene);
        set_active(manager->gizmos.z_arrow, false, scene);
    }

    static void _set_gizmo_arrow(math::Vec3 position, SceneHandle scene)
    {
        SceneManager *manager = scene.manager;
        
        TransformComponent &x_t = scene::get_transform_comp(manager->gizmos.x_arrow, scene);
        TransformComponent &y_t = scene::get_transform_comp(manager->gizmos.y_arrow, scene);
        TransformComponent &z_t = scene::get_transform_comp(manager->gizmos.z_arrow, scene);

        x_t.position = position + math::Vec3(0.5f, 0, 0);
        y_t.position = position + math::Vec3(0, 0.5f, 0);
        z_t.position = position + math::Vec3(0, 0, 0.5f);
        
        set_active(manager->gizmos.x_arrow, true, scene);
        set_active(manager->gizmos.y_arrow, true, scene);
        set_active(manager->gizmos.z_arrow, true, scene);
    }

    struct IntersectionData
    {
        b32 intersected;
        math::Vec3 intersection_point;
        math::Vec3 direction;;
    };

    r32 closest_distance_between_lines_2(math::Ray l1, math::Ray l2, Renderer &renderer)
    {
        math::Vec3 dp = l2.origin - l1.origin;
        r32 v12 = math::dot(l1.direction, l1.direction);
        r32 v22 = math::dot(l2.direction, l2.direction);
        r32 v1v2 = math::dot(l1.direction, l2.direction);

        r32 det = v1v2 * v1v2 - v12 * v22;

        if(ABS(det) > 0.00001f)
        {
            r32 inv_det = 1.f / det;

            r32 dpv1 = dot(dp, l1.direction);
            r32 dpv2 = dot(dp, l2.direction);
            
            r32 t = inv_det * (v22 * dpv1 - v1v2 * dpv2);
            r32 s = inv_det * (v1v2 * dpv1 - v12 * dpv2);

            math::Vec3 p1 = l1.origin + l1.direction * t;
            math::Vec3 p2 = l2.origin + l2.direction * s;
            
            rendering::push_line_to_render_pass(renderer, p1, p2, rendering::get_render_pass_handle_for_name(STANDARD_PASS, renderer), rendering::CommandType::NO_DEPTH);
 
            math::norm(dp + p2 - p1);
            return t;
        }
        else
        {
            math::Vec3 a = math::cross(dp, l1.direction);
            //return std::sqrt(dot(a, a) / v12);
            return 0.0f;
        }
    }
    
    r32 closest_distance_between_lines_new(math::Ray l1, math::Ray l2, Renderer &renderer)
    {
        r32 a = math::dot(l1.direction, l1.direction);
        r32 b = math::dot(l1.direction, l2.direction);
        math::Vec3 r = l1.origin - l2.origin;
        r32 c = math::dot(l1.direction, r);
        r32 e = math::dot(l2.direction, l2.direction);
        r32 f = math::dot(l2.direction, r);

        r32 d = a * e - b * b;

        if(ABS(d) >= 0.00001f)
        {
            r32 s = (b * f - c * e) / d;
            r32 t = (a * f - b * c) / d;

            math::Vec3 p1 = (l1.origin + l1.direction * s);
            math::Vec3 p2 = (l2.origin + l2.direction * t);
            
            rendering::push_line_to_render_pass(renderer, p1, p2, rendering::get_render_pass_handle_for_name(STANDARD_PASS, renderer), rendering::CommandType::NO_DEPTH);
        
            debug("p1: %f %f %f\n", p1.x, p1.y, p1.z);
            debug("p2: %f %f %f\n", p2.x, p2.y, p2.z);
            
            return t;
        }
        else
        {
            return 0.0f;
        }
    }
    
    static IntersectionData get_intersection_point(SceneManager *manager, InputController *input_controller, Camera &camera, math::Vec3 position)
    {
        math::Vec3 camera_position = camera.position;
        math::Vec3 axis;
        math::Vec3 entity_position = position;
        math::Vec3 point;
        math::Vec3 normal;
            
        math::Ray ray = cast_ray(get_scene(manager->loaded_scene), (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);
        
        switch(manager->gizmos.selected_gizmo)
        {
        case Gizmos::X_ARROW:
        {
            normal = math::Vec3(1.0f, 0.0f, 0.0f);
            axis = math::Vec3(1, 0, 0);
            point = position + math::Vec3(100, 0, 0);
        }
        break;
        case Gizmos::Y_ARROW:
        {
            normal = math::Vec3(0.0f, 1.0f, 0.0f);
            axis = math::Vec3(0, 1, 0);
            point = position + math::Vec3(0, 100, 0);
        }
        break;
        case Gizmos::Z_ARROW:
        {
            normal = math::Vec3(0.0f, 0.0f, 1.0f);
            axis = math::Vec3(0, 0, 1);
            point = position + math::Vec3(0, 0, 100);
        }
        break;
        case Gizmos::NONE:
        return { false, math::Vec3(0.0f) };
        }
        
        math::Ray ray2;
        ray2.direction = normal;
        ray2.origin = entity_position;

        r32 t = closest_distance_between_lines_new(ray, ray2, *manager->renderer);
        
        // Ray intersection
        rendering::Transform xf = {};
        xf.scale = math::Vec3(0.1f);

        rendering::set_uniform_value(*manager->renderer, manager->debug_material_instance, "color", math::Rgba(1.0f, 1.0f, 0.0f, 1.0f));
        xf.position = ray2.origin + ray2.direction * t;
        rendering::push_buffer_to_render_pass(*manager->renderer, manager->debug_cube, manager->debug_material_instance, xf, manager->debug_shader_handle, rendering::get_render_pass_handle_for_name(STANDARD_PASS, *manager->renderer), rendering::CommandType::NO_DEPTH);
        
        
        rendering::push_line_to_render_pass(*manager->renderer, ray2.origin, ray2.origin + ray2.direction * 80, rendering::get_render_pass_handle_for_name(STANDARD_PASS, *manager->renderer), rendering::CommandType::NO_DEPTH);
        
        return { true, ray2.origin + ray2.direction * t };

        // math::Plane p = get_plane(entity_position, camera_position, point);

        // math::Vec3 intersection_point = ray_vs_plane(ray.origin, ray.direction, p.normal, p.d);

        // return {true, intersection_point };
    }

    static math::Vec3 closest_line_point(math::Vec3 origin, math::Vec3 dir, math::Vec3 p)
    {
        r32 t = math::dot((p - origin), dir);
        return origin + dir * t;
    }
    
    static void update_transform(TransformComponent &transform, Camera &camera, SceneManager *manager, InputController *input_controller, r64 delta_time)
    {
        if(manager->dragging)
        {

            IntersectionData data = get_intersection_point(manager, input_controller, camera, transform.position);

            debug("intersection point: %f %f %f\n", data.intersection_point.x, data.intersection_point.y, data.intersection_point.z);
            if(data.intersected)
            {
                math::Vec3 diff = data.intersection_point - manager->gizmos.first_intersection_point;
                
                manager->gizmos.first_intersection_point = data.intersection_point;
                
                switch(manager->gizmos.selected_gizmo)
                {
                case Gizmos::X_ARROW:
                {
                    transform.position.x += diff.x;
                }
                break;
                case Gizmos::Y_ARROW:
                {
                    transform.position.y += diff.y;
                }
                break;
                case Gizmos::Z_ARROW:
                {
                    transform.position.z += diff.z;
                }
                break;
                case Gizmos::NONE:
                return;
                }
            }
        }
    }

    static void _deselect_gizmos(SceneHandle handle)
    {
        RenderComponent &render_x = get_render_comp(handle.manager->gizmos.x_arrow, handle);
        rendering::set_uniform_value(*handle.manager->renderer, render_x.v2.material_handle, "color", math::Rgba(1.0f, 0.0f, 0.0f, 1.0f));
        RenderComponent &render_y = get_render_comp(handle.manager->gizmos.y_arrow, handle);
        rendering::set_uniform_value(*handle.manager->renderer, render_y.v2.material_handle, "color", math::Rgba(0.0f, 1.0f, 0.0f, 1.0f));
        RenderComponent &render_z = get_render_comp(handle.manager->gizmos.z_arrow, handle);
        rendering::set_uniform_value(*handle.manager->renderer, render_z.v2.material_handle, "color", math::Rgba(0.0f, 0.0f, 1.0f, 1.0f));
    }

    static void _select_gizmo(SceneHandle handle, EntityHandle gizmo)
    {
        _deselect_gizmos(handle);
        RenderComponent &render_selected = get_render_comp(gizmo, handle);
        rendering::set_uniform_value(*handle.manager->renderer, render_selected.v2.material_handle, "color", math::Rgba(1.0f, 1.0f, 0.0f, 1.0f));
    }

    static void update_scene_editor(SceneHandle handle, InputController *input_controller, r64 delta_time)
    {
        Scene &scene = get_scene(handle);
        
        SceneManager *manager = handle.manager;
        
        if(KEY_DOWN(Key_E) && KEY(Key_LeftCtrl))
        {
            if(manager->mode == SceneMode::RUNNING)
            {
                manager->play_camera = scene.camera;
                manager->mode = SceneMode::EDITING;

                _register_gizmos(handle);
                
            }
            else
            {
                scene.camera = manager->play_camera;
                manager->mode = SceneMode::RUNNING;
            }
        }

        if(manager->mode == SceneMode::EDITING)
        {
            manager->dragging = MOUSE(Mouse_Left);

            if(!manager->dragging)
                _deselect_gizmos(handle);

            Camera &camera = scene.camera;
            
            if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
            {
                TransformComponent &t = get_transform_comp(manager->selected_entity, handle);
                
                update_transform(t, camera, manager, input_controller, delta_time);
                
                TransformComponent &x_t = scene::get_transform_comp(manager->gizmos.x_arrow, handle);
                TransformComponent &y_t = scene::get_transform_comp(manager->gizmos.y_arrow, handle);
                TransformComponent &z_t = scene::get_transform_comp(manager->gizmos.z_arrow, handle);

                manager->gizmos.x_scale = math::Vec3(1.0f, 0.1f, 0.1f);
                manager->gizmos.y_scale = math::Vec3(0.1f, 1.0f, 0.1f);
                manager->gizmos.z_scale = math::Vec3(0.1f, 0.1f, 1.0f);

                r32 distance = math::distance(camera.position, t.position) * 0.1f;

                x_t.scale = manager->gizmos.x_scale * distance;
                y_t.scale = manager->gizmos.y_scale * distance;
                z_t.scale = manager->gizmos.z_scale * distance;
                x_t.position = t.position + math::Vec3(0.5f * distance, 0, 0);
                y_t.position = t.position + math::Vec3(0, 0.5f * distance, 0);
                z_t.position = t.position + math::Vec3(0, 0, 0.5f * distance);

                if(KEY_DOWN(Key_F))
                {
                    set_target(camera, t.position);
                }
            }

            if(MOUSE_DOWN(Mouse_Left))
            {        
                manager->dragging = true;
                
                scene::EntityHandle entity = scene::pick_entity(handle, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);

                if(IS_ENTITY_HANDLE_VALID(entity))
                {
                    if(_is_gizmo(entity, manager))
                    {
                        TransformComponent &transform = get_transform_comp(entity, manager->loaded_scene);
                        IntersectionData data = get_intersection_point(manager, input_controller, camera, transform.position);                       
                        _select_gizmo(handle, entity);
                        
                        manager->gizmos.first_intersection_point = data.intersection_point;
                        manager->gizmos.starting_transform_position = transform.position;
                    }
                    else
                    {
                        // Deselect the previously selected entity
                        if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                            scene::set_wireframe_enabled(false, manager->selected_entity, handle);                        
                        
                        manager->selected_entity = entity;
                        scene::set_wireframe_enabled(true, entity, handle);
                        TransformComponent &t = get_transform_comp(entity, handle);
                        
                        _set_gizmo_arrow(t.position, handle);
                        
                        manager->callbacks.on_entity_selected(entity, handle);
                    }
                    
                    //scene::TransformComponent t = scene::get_transform_comp(entity, scene);
                    //set_target(camera, t.position);
                }
            }
                
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
    }
    
    static void load_scene(SceneHandle handle, u64 load_flags = 0)
    {
        SceneManager *scene_manager = handle.manager;
        if(scene_manager->scene_loaded)
        {
            scene::deactivate_particle_systems(handle);
            
            if(load_flags & SceneLoadFlags::FREE_CURRENT_SCENE)
            {
                free_instance_buffers(get_scene(scene_manager->loaded_scene));
                get_scene(scene_manager->loaded_scene).loaded = false;
                free_scene(scene_manager->loaded_scene);
                scene_manager->loaded_scene = { -1 };
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
        comp.material_handle = { entity.render_handle.handle };
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

    static void add_to_render_pass(const char *pass_name, rendering::ShaderHandle shader_handle, RenderComponent &comp, Renderer &renderer)
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
        
        return(comp);
    }

    static TransformComponent& add_transform_component(SceneHandle handle, EntityHandle entity_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_transform_component(scene, entity_handle);
    }

    
    static ParticleSystemComponent & _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material, rendering::MaterialInstanceArrayHandle array_handle)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_PARTICLES;
        
        entity.particle_system_handle = {scene.particle_system_component_count++};
        scene::ParticleSystemComponent &comp = scene.particle_system_components[entity.particle_system_handle.handle];
        
        comp.handle = create_particle_system(*scene.renderer, max_particles, material, array_handle);
        ParticleSystemInfo* info = get_particle_system_info(comp.handle, *scene.renderer);
        assert(info);
        
        info->attributes = attributes;
        
        return(comp);
    }

    static ParticleSystemComponent & add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material, rendering::MaterialInstanceArrayHandle array_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_particle_system_component(scene, entity_handle, attributes, max_particles, material, array_handle);
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
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene)
    {
        i32 new_handle = _unused_entity_handle(scene) + 1;
        
        EntityHandle handle = { new_handle };
        scene._internal_handles[new_handle - 1] = scene.entity_count++;
        
        Entity &entity = scene.entities[scene._internal_handles[new_handle - 1]];
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

    static EntityHandle register_entity(u64 comp_flags, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return _register_entity(comp_flags, scene);
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
                                char pass_name[128];
                                char shader_file[256];
                                sscanf(buffer, "shd_pass: %s - %s", pass_name, shader_file);
                                rendering::ShaderHandle shader_handle = rendering::load_shader(*scene.renderer, shader_file);

                                // Add the pass information
                                strncpy(templ.render.v2.render_pass_names[templ.render.v2.render_pass_count], pass_name, strlen(pass_name) + 1);
                                templ.render.v2.shader_handles[templ.render.v2.render_pass_count++] = shader_handle;
                            }
                            else
                            {
                                char shader_file[256];
                                sscanf(buffer, "shd: %s", shader_file);
                                rendering::ShaderHandle shader_handle = rendering::load_shader(*scene.renderer, shader_file);
                                templ.render.v2.material_handle = rendering::create_material(*scene.renderer, shader_handle);

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
                            templ.render.v2.buffer_handle = rendering::load_obj(*scene.renderer, obj_file, &templ.render.v2.material_handle, &templ.render.mesh_scale);
                        }
                        else if(starts_with(buffer, "prim"))
                        {
                            char *prim_type = buffer + sizeof(char) * 6;

                            if(starts_with(prim_type, "cube"))
                            {
                                templ.render.is_new_version = true;
                                templ.render.v2.buffer_handle = rendering::create_cube(*scene.renderer, &templ.render.mesh_scale);
                            }
                            else if(starts_with(prim_type, "plane"))
                            {
                                templ.render.is_new_version = true;
                                templ.render.v2.buffer_handle = rendering::create_plane(*scene.renderer, &templ.render.mesh_scale);
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
                            rendering::load_material_from_mtl(*scene.renderer, templ.render.v2.material_handle, mtl_file);
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
                            rendering::ShaderHandle shader_handle = rendering::load_shader(*scene.renderer, shader_file);
                            templ.particles.material_handle = rendering::create_material(*scene.renderer, shader_handle);

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
                            load_texture(texture_path, *scene.renderer, LINEAR, REPEAT, TextureFormat::RGBA, attributes.texture_handle);
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

    static void _set_entity_name(EntityHandle handle, const char *name, Scene& scene)
    {
        Entity &entity = scene.entities[scene._internal_handles[handle.handle - 1]];
        strcpy(entity.name, name);
    }
    
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene)
    {
        EntityHandle handle = _register_entity(templ.comp_flags, scene);
        _set_entity_name(handle, templ.name, scene);
        
        if(templ.comp_flags & COMP_TRANSFORM)
        {
            TransformComponent &transform = _get_transform_comp(handle, scene);
            transform.position = templ.transform.position;
            transform.scale = templ.transform.scale;
            transform.rotation = templ.transform.rotation;
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
                render.v2.material_handle = rendering::create_material_instance(*scene.renderer, templ.render.v2.material_handle, scene.material_array_handle);
                render.casts_shadows = templ.render.casts_shadows;
                render.mesh_scale = templ.render.mesh_scale;
                
                for(i32 i = 0; i < templ.render.v2.render_pass_count; i++)
                {
                    add_to_render_pass(templ.render.v2.render_pass_names[i], templ.render.v2.shader_handles[i], render, *scene.renderer);
                }
            }
            else
            {
                assert(false);
            }
        }
        
        if(templ.comp_flags & COMP_PARTICLES)
        {
            scene::ParticleSystemComponent &ps_comp = scene::_add_particle_system_component(scene, handle, templ.particles.attributes, templ.particles.max_particles, templ.particles.material_handle, scene.material_array_handle);

            ParticleSystemInfo *ps = get_particle_system_info(ps_comp.handle, *scene.renderer);
            
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
                ps->transform.position = ps->attributes.base_position + templ.transform.position;
                ps->transform.scale = templ.transform.position;
                ps->transform.rotation = templ.transform.rotation;
            }

            if(templ.particles.attributes.texture_handle.handle != 0)
            {
                rendering::set_uniform_value(*scene.renderer, ps->material_handle, "tex0", templ.particles.attributes.texture_handle);
            }

            ps_comp.render_pass = rendering::get_render_pass_handle_for_name(STANDARD_PASS, *scene.renderer);
            
            if(templ.particles.started)
                start_particle_system(ps_comp.handle, *scene.renderer);
        }
        
        return(handle);
    }
    
    static EntityHandle _register_entity_from_template_file(const char *path, Scene &scene)
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
        
        return(_register_entity_with_template(*templ, scene));
    }

    static EntityHandle register_entity_from_template_file(const char *path, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return _register_entity_from_template_file(path, scene);
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
            remove_particle_system(*scene.renderer, ps_handle);
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
        if(handle.handle == 0 || handle.handle - 1 >= scene.entity_count || scene._internal_handles[handle.handle - 1] == -1)
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
                
                if(scene.entities[index].transform_handle.handle > transform_handle)
                {
                    scene.entities[index].transform_handle.handle--;
                }
                
                if(scene.entities[index].render_handle.handle > render_handle)
                {
                    scene.entities[index].render_handle.handle--;
                }
                
                if(scene.entities[index].particle_system_handle.handle > particle_system_handle)
                {
                    scene.entities[index].particle_system_handle.handle--;
                }

                if(scene.entities[index].light_handle.handle > light_component_handle)
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
    
	static void set_uniform_value(EntityHandle handle, const char* name, r32 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec2 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec3 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec4 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, i32 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Mat4 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

	static void set_uniform_value(EntityHandle handle, const char* name, rendering::TextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        assert(value.handle != 0);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

    static void set_uniform_value(EntityHandle handle, const char* name, rendering::MSTextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        assert(value.handle != 0);
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

#define SET_MAT_ARRAY_VALUE(type) static void set_uniform_array_value(EntityHandle handle, const char *array_name, i32 index, const char *variable_name, type value, SceneHandle &scene) \
    { \
       RenderComponent &render = get_render_comp(handle, scene); \
    rendering::set_uniform_array_value(*scene.manager->renderer, render.v2.material_handle, array_name, index, variable_name, value);\
    } \

SET_MAT_ARRAY_VALUE(r32)
SET_MAT_ARRAY_VALUE(math::Vec2)
SET_MAT_ARRAY_VALUE(math::Vec3)
SET_MAT_ARRAY_VALUE(math::Vec4)
SET_MAT_ARRAY_VALUE(i32)
SET_MAT_ARRAY_VALUE(math::Mat4)
SET_MAT_ARRAY_VALUE(rendering::TextureHandle)
    
}
