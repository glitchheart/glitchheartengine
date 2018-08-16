namespace scene
{
    // Creates and returns a new "Scene". The "inital_entity_array_size" specifies the max entity count of the "Scene".
    // @Incomplete: We need to make sure that we can grow in size if we need more than what we allocated at any point in time.
    static Scene create_scene(Renderer renderer, i32 initial_entity_array_size = 1024)
    {
        Scene scene = {};
        scene.entity_count = 0;
        scene.transform_component_count = 0;
        scene.render_component_count = 0;
        scene.entities = (Entity*)malloc(sizeof(EntityHandle) * initial_entity_array_size);
        scene.active_entities = (b32*)malloc(sizeof(b32) * initial_entity_array_size);
        scene.transform_components = (TransformComponent*)malloc(sizeof(TransformComponent) * initial_entity_array_size);
        scene.render_components = (RenderComponent*)malloc(sizeof(RenderComponent) * initial_entity_array_size);
        scene.material_instances = (RenderMaterial*)malloc(sizeof(RenderMaterial) * initial_entity_array_size);
        scene.byte_size = (sizeof(EntityHandle) + sizeof(TransformComponent) + sizeof(RenderComponent)) * initial_entity_array_size;
        
        for(i32 index = 0; index < initial_entity_array_size; index++)
        {
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
            
            free(scene.entities);
            free(scene.transform_components);
            free(scene.render_components);
        }
    }
    
    // Returns a new valid "EntityHandle". "comp_flags" Specifies the components that the entity should contain.
    static EntityHandle register_entity(u64 comp_flags, Scene &scene)
    {
        EntityHandle handle = { scene.entity_count++ };
        Entity &entity = scene.entities[handle.handle];
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
            comp.receives_shadows = true;
            comp.cast_shadows = false;
        }
        
        return(handle);
    }
    
    static void set_active(EntityHandle handle, b32 active, Scene &scene)
    {
        scene.active_entities[handle.handle] = active;
    }
    
    // Returns a direct pointer to the TransformComponent of the specified entity
    static TransformComponent* get_transform_comp(EntityHandle handle, Scene &scene)
    {
        Entity entity = scene.entities[handle.handle];
        
        assert(entity.comp_flags & COMP_TRANSFORM);
        
        TransformComponent* comp = &scene.transform_components[entity.transform_handle.handle];
        return(comp);
    }
    
    // @Note(Daniel): Should we really return a pointer here? A reference might suffice, since we don't ever use the null-value for anything....
    // Returns a direct pointer to the RenderComponent of the specified entity
    static RenderComponent* get_render_comp(EntityHandle handle, Scene &scene)
    {
        Entity entity = scene.entities[handle.handle];
        
        assert(entity.comp_flags & COMP_RENDER);
        
        RenderComponent* comp = &scene.render_components[entity.render_handle.handle];
        return(comp);
    }

    // TODO RenderMaterial should know it's original handle...
    static MaterialHandle create_material(MaterialHandle material_to_copy, Scene &scene, Renderer &renderer)
    {
        scene.material_instances[scene.material_count] = renderer.materials[material_to_copy.handle];
        return { scene.material_count++ };
    }

    static RenderMaterial & get_material(EntityHandle handle, Scene &scene)
    {
        Entity &entity = scene.entities[handle.handle];
        return scene.material_instances[entity.render_handle.handle];
    }
}
