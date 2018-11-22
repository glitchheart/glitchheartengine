#define ENTITY_IS_VALID(handle) handle.handle > 0

namespace scene
{
    static Scene create_scene(Renderer &renderer, EntityTemplateState &template_state, i32 initial_entity_array_size);
    static void free_scene(Scene& scene);
    i32 _unused_entity_handle(Scene &scene);
    static RenderComponent& add_render_component(Scene &scene, EntityHandle entity_handle, b32 receives_shadows, b32 cast_shadows);
    static TransformComponent& add_transform_component(Scene &scene, EntityHandle entity_handle);
    static ParticleSystemComponent& add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles);
    static EntityHandle register_entity(u64 comp_flags, Scene &scene);
    static EntityTemplate _load_template(const char *path, Scene &scene);
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene);
    static EntityHandle register_entity_from_template_file(const char *path, Scene &scene);
    i32 _pack_transform_components(Entity &entity, Scene &scene);
    i32 _pack_render_components(Entity &entity, Scene &scene);
    i32 _pack_particle_system_components(Entity &entity, Scene &scene);
    static void unregister_entity(EntityHandle &handle, Scene &scene);
    static void set_active(EntityHandle handle, b32 active, Scene &scene);
    static TransformComponent& get_transform_comp(EntityHandle handle, Scene &scene);
    static RenderComponent& get_render_comp(EntityHandle handle, Scene &scene);
    static ParticleSystemComponent& get_particle_system_comp(EntityHandle handle, Scene &scene);
    static MaterialHandle create_material(MaterialHandle material_to_copy, Scene &scene);
    static Material& get_material(EntityHandle handle, Scene &scene);
    
    // Creates and returns a new "Scene". The "inital_entity_array_size" specifies the max entity count of the "Scene".
    // @Incomplete: We need to make sure that we can grow in size if we need more than what we allocated at any point in time.
    static Scene create_scene(Renderer &renderer, EntityTemplateState &template_state, i32 initial_entity_array_size = 1024)
    {
        Scene scene = {};
        scene.template_state = &template_state;
        scene.renderer = &renderer;
        scene.max_entity_count = initial_entity_array_size;
        scene.entity_count = 0;
        scene.current_internal_handle = 0;
        scene.transform_component_count = 0;
        scene.render_component_count = 0;
        scene.material_count = 0;
		
        auto &memory_arena = scene.memory_arena;
        scene.entities = push_array(&memory_arena, initial_entity_array_size, Entity);
        scene._internal_handles = push_array(&memory_arena, initial_entity_array_size, i32);
        scene.active_entities = push_array(&memory_arena, initial_entity_array_size, b32);
        scene.transform_components = push_array(&memory_arena, initial_entity_array_size, TransformComponent);
        scene.render_components = push_array(&memory_arena, initial_entity_array_size, RenderComponent);
        scene.particle_system_components = push_array(&memory_arena, initial_entity_array_size, ParticleSystemComponent);
        scene.material_instances = push_array(&memory_arena, initial_entity_array_size, Material);
        
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
        
        return(scene);
    }
    
    // Zeroes all counts and frees all memory allocated for the "Scene"
    static void free_scene(Scene& scene)
    {
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
            
            scene.particle_system_component_count = 0;
            scene.material_count = 0;
            scene.current_internal_handle = 0;
            clear(&scene.memory_arena);
        }
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
    
    static RenderComponent& add_render_component(Scene &scene, EntityHandle entity_handle, b32 receives_shadows = true, b32 cast_shadows = true)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_RENDER;
        
        entity.render_handle = { scene.render_component_count++ };
        scene::RenderComponent &comp = scene.render_components[entity.render_handle.handle];
        comp.material_handle = { entity.render_handle.handle };
        comp.mesh_handle = { -1 };
        comp.receives_shadows = receives_shadows;
        comp.cast_shadows = cast_shadows;
        
        return(comp);
    }
    
    static TransformComponent& add_transform_component(Scene &scene, EntityHandle entity_handle)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_TRANSFORM;
        entity.transform_handle = { scene.transform_component_count++ };
        scene::TransformComponent &comp = scene.transform_components[entity.transform_handle.handle];
        
        return(comp);
    }
    
    static ParticleSystemComponent& add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles = 0)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_PARTICLES;
        
        entity.particle_system_handle = {scene.particle_system_component_count++};
        scene::ParticleSystemComponent &comp = scene.particle_system_components[entity.particle_system_handle.handle];
        
        comp.handle = create_particle_system(*scene.renderer, max_particles);
        ParticleSystemInfo* info = get_particle_system_info(comp.handle, *scene.renderer);
        assert(info);
        
        info->attributes = attributes;
        
        return(comp);
    }
    
    // Returns a new valid "EntityHandle". "comp_flags" Specifies the components that the entity should contain.
    static EntityHandle register_entity(u64 comp_flags, Scene &scene)
    {
        i32 new_handle = _unused_entity_handle(scene) + 1;
        
        EntityHandle handle = { new_handle };
        scene._internal_handles[new_handle - 1] = scene.entity_count++;
        
        Entity &entity = scene.entities[scene._internal_handles[new_handle - 1]];
        entity.comp_flags = comp_flags;
        
        if(comp_flags & COMP_TRANSFORM)
        {
            add_transform_component(scene, handle);
        }
        
        if(comp_flags & COMP_RENDER)
        {
            add_render_component(scene, handle, true, true);
        }
        
        if(comp_flags & COMP_PARTICLES)
        {
            add_particle_system_component(scene, handle, get_default_particle_system_attributes(), 0);
        }
        
        return(handle);
    }
    
    static EntityTemplate _load_template(const char *path, Scene &scene)
    {
        EntityTemplate templ = {};
        
        FILE *file = fopen(path, "r");
        if(file)
        {
            sprintf(templ.file_path, "%s", path);
            
            char buffer[256];
            
            while(fgets(buffer, 256, file))
            {
                if(starts_with(buffer, "-transform"))
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
                        if(starts_with(buffer, "receives shadows"))
                        {
                            sscanf(buffer, "receives shadows: %d\n", &templ.render.receives_shadows);
                        }
                        else if(starts_with(buffer, "cast shadows"))
                        {
                            sscanf(buffer, "cast shadows: %d\n", &templ.render.cast_shadows);
                        }
                        else if(starts_with(buffer, "obj"))
                        {
                            char obj_file[256];
                            sscanf(buffer, "obj: %s", obj_file);
                            
                            load_obj(*scene.renderer, obj_file, &templ.render.mesh_handle, &templ.render.material_handle);
                        }
                        else if(starts_with(buffer, "prim"))
                        {
                            char *prim_type = buffer + sizeof(char) * 6;
                            if(starts_with(prim_type, "cube"))
                            {
                                create_cube(*scene.renderer, &templ.render.mesh_handle);
                            }
                            else if(starts_with(prim_type, "plane"))
                            {
                                create_plane(*scene.renderer, &templ.render.mesh_handle.handle);
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
                            load_material(*scene.renderer, mtl_file, &templ.render.material_handle);
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
		    
                    ParticleSystemAttributes attributes = get_default_particle_system_attributes();
		    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "max_particles"))
                        {
                            sscanf(buffer, "max_particles: %d", &templ.particles.max_particles);
                        }
                        else if(starts_with(buffer, "started"))
                        {
                            sscanf(buffer, "started: %d", &templ.particles.started);
                        }
                        else if(starts_with(buffer, "start_size"))
                        {
                            sscanf(buffer, "start_size: %f %f", &attributes.start_size.x, &attributes.start_size.y);
                        }
                        else if(starts_with(buffer, "start_color"))
                        {
                            sscanf(buffer, "start_color: %f %f %f %f", &attributes.start_color.r, &attributes.start_color.g, &attributes.start_color.b, &attributes.start_color.a);
                        }
                        else if(starts_with(buffer, "start_speed"))
                        {
                            sscanf(buffer, "start_speed: %f", &attributes.start_speed);
                        }
                        else if(starts_with(buffer, "life_time"))
                        {
                            sscanf(buffer, "life_time: %lf", &attributes.life_time);
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
                            load_texture(texture_path, *scene.renderer, LINEAR, &attributes.texture_handle);
                        }
                        else if(starts_with(buffer, "gravity"))
                        {
                            sscanf(buffer, "gravity: %f", &attributes.gravity);
                        }
                        else if(starts_with(buffer, "emitter_func"))
                        {
                            char func_name[32];
                            sscanf(buffer, "emitter_func: %s", func_name);

                            attributes.emission_module.emitter_func = emit_from_2D_square_random;
                            if(starts_with(func_name, "2d_square_random"))
                            {
                                attributes.emission_module.emitter_func = emit_from_2D_square_random;
                            }
                            else
                            {
                                // @Incomplete: Add missing functions!!!
                                // @Incomplete: Add missing functions!!!
                                // @Incomplete: Add missing functions!!!
                                // @Incomplete: Add missing functions!!!
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
                        else if(starts_with(buffer, "one_shot"))
                        {
                            sscanf(buffer, "one_shot: %d", &attributes.one_shot);
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
                            math::Rgba &value = templ.particles.color_over_lifetime.values[templ.particles.color_over_lifetime.value_count];
                            r64 &key = templ.particles.color_over_lifetime.keys[templ.particles.color_over_lifetime.value_count];
                            sscanf(buffer, "color_key: %lf %f %f %f %f", &key, &value.r, &value.g, &value.b, &value.a);
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
        
        return(templ);
    }

#define EMPTY_TRANSFORM { math::Vec3(), math::Vec3(1, 1, 1), math::Vec3(), EMPTY_COMP_HANDLE, EMPTY_COMP_HANDLE };
    
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene)
    {
        EntityHandle handle = register_entity(templ.comp_flags, scene);
        
        if(templ.comp_flags & COMP_TRANSFORM)
        {
            TransformComponent &transform = get_transform_comp(handle, scene);
            transform.position = templ.transform.position;
            transform.scale = templ.transform.scale;
            transform.rotation = templ.transform.rotation;
            // @Incomplete: Parent and child handles
        }
        
        if(templ.comp_flags & COMP_RENDER)
        {
            RenderComponent &render = get_render_comp(handle, scene);
            render.mesh_handle = templ.render.mesh_handle;
            render.material_handle = create_material(templ.render.material_handle, scene);
            render.receives_shadows = templ.render.receives_shadows;
            render.cast_shadows = templ.render.cast_shadows;
        }
        
        if(templ.comp_flags & COMP_PARTICLES)
        {
            scene::ParticleSystemComponent &ps_comp = scene::add_particle_system_component(scene, handle, templ.particles.attributes, templ.particles.max_particles);

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

            if(templ.particles.started)
                start_particle_system(ps_comp.handle, *scene.renderer);
        }
        
        return(handle);
    }
    
    static EntityHandle register_entity_from_template_file(const char *path, Scene &scene)
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
                scene.material_instances[index] = scene.material_instances[index + 1];
                scene.render_components[index].material_handle.handle--;
            }
            scene.render_component_count--;
            scene.material_count--;
            return entity.render_handle.handle;
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
    
    static void unregister_entity(EntityHandle &handle, Scene &scene)
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
                
                if(scene.entities[index].particle_system_handle.handle > render_handle)
                {
                    scene.entities[index].particle_system_handle.handle--;
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
    
    static void set_active(EntityHandle handle, b32 active, Scene &scene)
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
    
    // Returns a direct pointer to the TransformComponent of the specified entity
    static TransformComponent& get_transform_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle > -1);
        
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_TRANSFORM);
        
        TransformComponent& comp = scene.transform_components[entity.transform_handle.handle];
        return(comp);
    }
    
    // @Note(Daniel): Should we really return a pointer here? A reference might suffice, since we don't ever use the null-value for anything....
    // Returns a direct pointer to the RenderComponent of the specified entity
    static RenderComponent& get_render_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_RENDER);
        
        RenderComponent& comp = scene.render_components[entity.render_handle.handle];
        return(comp);
    }
    
    static ParticleSystemComponent& get_particle_system_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        Entity entity = scene.entities[internal_handle];
        
        assert(entity.comp_flags & COMP_PARTICLES);
        
        ParticleSystemComponent& comp = scene.particle_system_components[entity.particle_system_handle.handle];
        return(comp);
    }
    
    static MaterialHandle create_material(MaterialHandle material_to_copy, Scene &scene)
    {
        scene.material_instances[scene.material_count] = scene.renderer->materials[material_to_copy.handle];
        return { scene.material_count++ };
    }
    
    static Material& get_material(EntityHandle handle, Scene &scene)
    {
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        
        Entity &entity = scene.entities[internal_handle];
        return scene.material_instances[entity.render_handle.handle];
    }
}
