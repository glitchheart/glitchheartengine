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
        scene.transform_components = (TransformComponent*)malloc(sizeof(TransformComponent) * initial_entity_array_size);
        scene.render_components = (RenderComponent*)malloc(sizeof(RenderComponent) * initial_entity_array_size);
        scene.byte_size = (sizeof(EntityHandle) + sizeof(TransformComponent) + sizeof(RenderComponent)) * initial_entity_array_size;
        return(scene);
    }
    
    // Zeroes all counts and frees all memory allocated for the "Scene"
    static void free_scene(Scene& scene)
    {
        scene.entity_count = 0;
        scene.transform_component_count = 0;
        scene.render_component_count = 0;
        
        free(scene.entities);
        free(scene.transform_components);
        free(scene.render_components);
    }
    
    // Returns a new valid "EntityHandle". "comp_flags" Specifies the components that the entity should contain.
    static EntityHandle register_entity(u64 comp_flags, Scene &scene)
    {
        EntityHandle handle = { scene.entity_count++ };
        Entity &entity = scene.entities[handle.handle];
        
        if(comp_flags & COMP_TRANSFORM)
        {
            entity.transform_handle = { scene.transform_component_count++ };
        }
        
        if(comp_flags & COMP_RENDER)
        {
            entity.render_handle = { scene.render_component_count++ };
        }
        
        return(handle);
    }
    
    // Returns a direct pointer to the TransformComponent of the specified entity
    static TransformComponent* get_transform_comp(EntityHandle handle, Scene &scene)
    {
        Entity entity = scene.entities[handle.handle];
        
        assert(entity.comp_flags & COMP_TRANSFORM);
        
        TransformComponent* comp = &scene.transform_components[entity.transform_handle.handle];
        return(comp);
    }

    // Returns a direct pointer to the RenderComponent of the specified entity
    static RenderComponent* get_render_comp(EntityHandle handle, Scene &scene)
    {
        Entity entity = scene.entities[handle.handle];
        
        assert(entity.comp_flags & COMP_RENDER);
        
        RenderComponent* comp = &scene.render_components[entity.render_handle.handle];
        return(comp);
    }
}
