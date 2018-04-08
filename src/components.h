#ifndef COMPONENTS_H
#define COMPONENTS_H

struct TransformComponent
{
    math::Vec3 position;
    math::Vec3 rotation;
    math::Vec3 scale;
};

struct AnimationComponent
{
    i32 animation_controller;
};

struct SpriteRendererComponent
{
    RenderMaterial material;
    b32 flip_x;
    b32 flip_y;
};

struct MeshRendererComponent
{
    RenderMaterial material;
    b32 receive_shadows;
};

struct BoxColliderComponent
{
    
};

struct BoxCollider2DComponent
{
    
};

struct LightingComponent
{
    
};

enum ComponentTypeFlags
{
    CPF_TRANSFORM = 1 << 0,
    CPF_ANIMATION = 1 << 1,
    CPF_SPRITE_RENDERER = 1 << 2,
    CPF_MESH_RENDERER = 1 << 3,
    CPF_BOX_COLLIDER = 1 << 4,
    CPF_BOX_COLLIDER_2D = 1 << 5,
    CPF_LIGHTING = 1 << 6
};

enum ComponentType
{
    CP_TRANSFORM,
    CP_ANIMATION,
    CP_SPRITE_RENDERER,
    CP_MESH_RENDERER,
    CP_BOX_COLLIDER,
    CP_BOX_COLLIDER_2D,
    CP_LIGHTING,
    CP_MAX
};

const ComponentTypeFlags type_to_flag[] =
{
    CPF_TRANSFORM,
    CPF_ANIMATION,
    CPF_SPRITE_RENDERER,
    CPF_MESH_RENDERER,
    CPF_BOX_COLLIDER,
    CPF_BOX_COLLIDER_2D,
    CPF_LIGHTING
};

struct EntityComponentMapping
{
    i32 component_handles[CP_MAX];
};

struct ComponentList
{
    void *components;
    i32 count;
};

struct ComponentController
{
    u64 *entity_components;
    EntityComponentMapping *entity_mappings;
    i32 entity_count;
    
    union
    {
        ComponentList components[CP_MAX];
        struct
        {
            ComponentList transform_components;
            ComponentList animation_components;
            ComponentList rendering_components;
            ComponentList lighting_components;
        };
    };
};

struct Entity
{
    u32 handle;
};

b32 has_queried_components(ComponentController &controller, i32 entity, u64 component_flags)
{
    return (b32)(controller.entity_components[entity] & component_flags);
}

#define add_component(entity_handle, enum_type, type, ptr)\
controller.entity_components[entity_handle] |= type_to_flag[enum_type];\
controller.entity_mappings[entity_handle].component_handles[enum_type] = controller.components[enum_type].count;\
ptr = &((type(*))\
(controller.components[enum_type].components))\
[controller.entity_mappings[entity_handle].component_handles[enum_type]];\

#define get_component(entity, enum_type, type)\
&((type(*))(controller.components[type].components)[controller.entity_mappings[entity].component_handles[type]]);\

ComponentList *get_all_components(ComponentController &controller, ComponentType type)
{
    TransformComponent *transform = 0;
    add_component(0, CP_TRANSFORM, TransformComponent, transform);
    return &controller.components[type];
}

void __sort_components(ComponentController &controller)
{}

#endif