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
        scene.entities = (Entity*)malloc(sizeof(EntityHandle) * initial_entity_array_size);
        scene._internal_handles = (i32*)malloc(sizeof(i32) * initial_entity_array_size);
        scene.active_entities = (b32*)malloc(sizeof(b32) * initial_entity_array_size);
        scene.transform_components = (TransformComponent*)malloc(sizeof(TransformComponent) * initial_entity_array_size);
        scene.render_components = (RenderComponent*)malloc(sizeof(RenderComponent) * initial_entity_array_size);
        scene.particle_system_components = (ParticleSystemComponent*)malloc(sizeof(ParticleSystemComponent) * initial_entity_array_size);
        scene.material_instances = (Material*)malloc(sizeof(Material) * initial_entity_array_size);
        
        for(i32 index = 0; index < initial_entity_array_size; index++)
        {
            scene._internal_handles[index] = -1;
            scene.active_entities[index] = true;
            scene.transform_components[index].position = math::Vec3(0, 0, 0);
            scene.transform_components[index].scale = math::Vec3(1, 1, 1);
            scene.transform_components[index].rotation = math::Vec3(0, 0, 0);
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
            
            free(scene.entities);
            free(scene._internal_handles);
            free(scene.active_entities);
            free(scene.transform_components);
            free(scene.render_components);
            free(scene.particle_system_components);
            free(scene.material_instances);
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
                                debug("A material file was already loaded for this template file: %s\n", templ.file_path);

                            char mtl_file[256];
                            sscanf(buffer, "mtl: %s", mtl_file);
                            load_material(*scene.renderer, mtl_file, &templ.render.material_handle);
                        }
                    }
                }
                else if(starts_with(buffer, "-particle"))
                {
                    templ.comp_flags |= COMP_PARTICLES;
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                    }
                }
            }
            
            fclose(file);
        }
        
        return(templ);
    }
    
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene)
    {
        EntityHandle handle = register_entity(templ.comp_flags, scene);
        
        if(templ.comp_flags & COMP_TRANSFORM)
        {
            TransformComponent &transform = get_transform_comp(handle, scene);
            transform.position = templ.transform.position;
            transform.scale = templ.transform.scale;
            transform.rotation = templ.transform.rotation;
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
            
            // Pack the components in scene by removing the unregistered entities components and moving the rest to pack the arrays. If the returned handles are -1 the entity didn't have that component set.
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
