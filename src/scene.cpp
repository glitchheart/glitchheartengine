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
    }

    static void free_scene(SceneHandle handle)
    {
        SceneManager *scene_manager = handle.manager;
        i32 internal_handle = scene_manager->_internal_scene_handles[handle.handle - 1];
        Scene &scene = scene_manager->scenes[internal_handle];
        if(scene.valid)
        {
			scene.valid = false;

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

//            free_instance_buffers(scene);
            
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

    static void load_scene(SceneHandle handle, u64 load_flags = SceneLoadFlags::FREE_CURRENT_INSTANCE_BUFFERS)
    {
        SceneManager *scene_manager = handle.manager;
        if(scene_manager->scene_loaded)
        {
            scene::deactivate_particle_systems(handle);
            
            free_instance_buffers(get_scene(scene_manager->loaded_scene));

            if(load_flags & SceneLoadFlags::FREE_CURRENT_SCENE)
            {
                free_scene(scene_manager->loaded_scene);
                scene_manager->loaded_scene = { -1 };
            }
        }
        
        Scene &scene = get_scene(handle);
        assert(scene.valid);
        
        allocate_instance_buffers(scene);
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

    
    static ParticleSystemComponent& _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material, rendering::MaterialInstanceArrayHandle array_handle)
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

    static ParticleSystemComponent& add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material, rendering::MaterialInstanceArrayHandle array_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_particle_system_component(scene, entity_handle, attributes, max_particles, material, array_handle);
    }

    static LightComponent &add_light_component(Scene &scene, EntityHandle entity_handle)
    {
        Entity &entity = scene.entities[scene._internal_handles[entity_handle.handle - 1]];
        entity.comp_flags |= COMP_LIGHT;
        entity.light_handle = { scene.light_component_count++ };
        scene::LightComponent &comp = scene.light_components[entity.light_handle.handle];
        
        return(comp); 
    }
    
    // Returns a new valid "EntityHandle". "comp_flags" Specifies the components that the entity should contain.
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene)
    {
        i32 new_handle = _unused_entity_handle(scene) + 1;
        
        EntityHandle handle = { new_handle };
        scene._internal_handles[new_handle - 1] = scene.entity_count++;
        
        Entity &entity = scene.entities[scene._internal_handles[new_handle - 1]];
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
            add_light_component(scene, handle);
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
            
            char buffer[256];

            b32 is_new_version = false;
            
            while(fgets(buffer, 256, file))
            {
                if(starts_with(buffer, "v2"))
                {
                    is_new_version = true;
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

                            if(is_new_version)
                            {
                                templ.render.is_new_version = true;
                                templ.render.v2.buffer_handle = rendering::load_obj(*scene.renderer, obj_file, &templ.render.v2.material_handle);
                            }
                            // else
                            //     load_obj(*scene.renderer, obj_file, &templ.render.mesh_handle, &templ.render.material_handle);
                        }
                        else if(starts_with(buffer, "prim"))
                        {
                            char *prim_type = buffer + sizeof(char) * 6;

                            if(starts_with(prim_type, "cube"))
                            {
                                if(is_new_version)
                                {
                                    templ.render.is_new_version = true;
                                    templ.render.v2.buffer_handle = rendering::create_plane(*scene.renderer);
                                }
                            }
                            else if(starts_with(prim_type, "plane"))
                            {
                                if(is_new_version)
                                {
                                    templ.render.is_new_version = true;
                                    templ.render.v2.buffer_handle = rendering::create_plane(*scene.renderer);
                                }
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

                            if(is_new_version)
                            {
                                rendering::load_material_from_mtl(*scene.renderer, templ.render.v2.material_handle, mtl_file);
                            }
                            else
                            {
                                // load_material(*scene.renderer, mtl_file, &templ.render.material_handle);
                            }
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
    
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene)
    {
        EntityHandle handle = _register_entity(templ.comp_flags, scene);
        
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
                render.v2.buffer_handle = templ.render.v2.buffer_handle;
                render.v2.material_handle = rendering::create_material_instance(*scene.renderer, templ.render.v2.material_handle, scene.material_array_handle);
                render.casts_shadows = templ.render.casts_shadows;
                
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
                
                if(scene.entities[index].particle_system_handle.handle > render_handle)
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
        rendering::set_uniform_value(*scene.manager->renderer, render.v2.material_handle, name, value);
	}

    static void set_uniform_value(EntityHandle handle, const char* name, rendering::MSTextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
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
