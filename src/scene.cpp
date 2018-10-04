#define ENTITY_IS_VALID(handle) handle.handle > 0

namespace scene
{
    // Creates and returns a new "Scene". The "inital_entity_array_size" specifies the max entity count of the "Scene".
    // @Incomplete: We need to make sure that we can grow in size if we need more than what we allocated at any point in time.
    static Scene create_scene(Renderer renderer, i32 initial_entity_array_size = 1024)
    {
        Scene scene = {};
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
        scene.material_instances = (RenderMaterial*)malloc(sizeof(RenderMaterial) * initial_entity_array_size);
        
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
            entity.transform_handle = { scene.transform_component_count++ };
        }
        
        if(comp_flags & COMP_RENDER)
        {
            entity.render_handle = { scene.render_component_count++ };
            scene::RenderComponent &comp = scene.render_components[entity.render_handle.handle];
            comp.material_handle = { entity.render_handle.handle };
            comp.mesh_handle = { -1 };
            comp.receives_shadows = true;
            comp.cast_shadows = true;
        }
        
        if(comp_flags & COMP_PARTICLES)
        {
            entity.particle_system_handle = {scene.particle_system_component_count};
            scene::ParticleSystemComponent &comp = scene.particle_system_components[entity.particle_system_handle.handle];
            comp.handle.handle = -1;
            
        }
        
        return(handle);
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
    
    static void unregister_entity(EntityHandle &handle, Scene &scene)
    {
        if(handle.handle == 0)
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
                
                scene.active_entities[index] = scene.active_entities[index + 1];
            }
            
            scene.entity_count--;
            
            // We also have to update the internal handles if they were placed after the unregistered entity
            for(i32 internal_index = 0; internal_index < scene.max_entity_count; internal_index++)
            {
                i32 current_handle = scene._internal_handles[internal_index];
                if(current_handle > -1 && current_handle > real_handle)
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
    
    static MaterialHandle create_material(MaterialHandle material_to_copy, Scene &scene, Renderer &renderer)
    {
        scene.material_instances[scene.material_count] = renderer.materials[material_to_copy.handle];
        return { scene.material_count++ };
    }
    
    static RenderMaterial& get_material(EntityHandle handle, Scene &scene)
    {
        i32 internal_handle = scene._internal_handles[handle.handle - 1];
        assert(internal_handle != -1);
        
        Entity &entity = scene.entities[internal_handle];
        return scene.material_instances[entity.render_handle.handle];
    }
}
