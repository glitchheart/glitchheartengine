#ifndef SCENE_H
#define SCENE_H

namespace scene
{
    struct EntityHandle
    {
        i32 handle;
    };
    
    struct ModelHandle
    {
        i32 handle;
    };
    
    struct MaterialHandle
    {
        i32 handle;
    };
    
    struct TransformComponentHandle
    {
        i32 handle;
    };
    
    struct RenderComponentHandle
    {
        i32 handle;
    };
    
    enum ComponentTypeFlags
    {
        COMP_TRANSFORM = 1 << 0,
        COMP_RENDER = 1 << 1
    };
    
    struct Entity
    {
        u64 comp_flags;
        TransformComponentHandle transform_handle;
        RenderComponentHandle render_handle;
    };
    
    struct TransformComponent
    {
        math::Vec3 position;
        math::Vec3 scale;
        math::Vec3 rotation;
    };
    
    struct RenderComponent
    {
        b32 active;
        ModelHandle model_handle;
        MaterialHandle material_handle;
    };
    
    struct Scene
    {
        Entity *entities;
        i32 entity_count;
        
        TransformComponent *transform_components;
        i32 transform_component_count;
        
        RenderComponent *render_components;
        i32 render_component_count;
        
        size_t byte_size;
    };
}

#endif