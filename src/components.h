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

#define TransformComponent() CP_TRANSFORM
#define AnimationComponent() CP_ANIMATION
#define SpriteRendererComponent() CP_SPRITE_RENDERER
#define MeshRendererComponent() CP_MESH_RENDERER
#define BoxColliderComponent() CP_BOX_COLLIDER
#define BoxCollider2DComponent() CP_BOX_COLLIDER_2D
#define LightingComponent() CP_LIGHTING

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

#define get_component(controller, entity, type) (type*)get__component(controller, entity, type(), sizeof(type))
void* get__component(ComponentController &controller, i32 entity, ComponentType type, size_t size_bytes)
{
    i32 handle = controller.entity_mappings[entity].component_handles[type];
    return (&controller.components[type].components) + (handle * size_bytes);
}

#define add_component(controller, entity, type) (type*)add__component(controller, entity, type(), sizeof(type))
void* add__component(ComponentController &controller, i32 entity, ComponentType type, size_t size_bytes)
{
    controller.entity_components[entity] |= type_to_flag[type];
    controller.entity_mappings[entity].component_handles[type] = controller.components[type].count++;
    i32 handle = controller.entity_mappings[entity].component_handles[type];
    return (&controller.components[type].components) + (handle * size_bytes);
}

void component_test()
{
    ComponentController controller = {0};
    
    controller.entity_components = (u64*)malloc(sizeof(u64) * CP_MAX);
    controller.entity_mappings = (EntityComponentMapping*)malloc(sizeof(EntityComponentMapping));
    
    controller.transform_components.components = malloc(sizeof(TransformComponent));
    
    TransformComponent *added_t_comp = add_component(controller, 0, TransformComponent);
    added_t_comp->position = math::Vec3(1.0f, 5.0f, 300.0f);
    added_t_comp->rotation = math::Vec3(45.0f, 0.0f, 0.0f);
    
    TransformComponent *t_comp = get_component(controller, 0, TransformComponent);
    debug("Position: %f %f %f\n", t_comp->position.x, t_comp->position.y, t_comp->position.z);
    debug("Rotation: %f %f %f\n", t_comp->rotation.x, t_comp->rotation.y, t_comp->rotation.z);
    debug("Position: %f %f %f\n", added_t_comp->position.x, added_t_comp->position.y, added_t_comp->position.z);
    debug("Rotation: %f %f %f\n", added_t_comp->rotation.x, added_t_comp->rotation.y, added_t_comp->rotation.z);
}

ComponentList *get_all_components(ComponentController &controller, ComponentType type)
{
    add_component(controller, 0, TransformComponent);
    return &controller.components[type];
}

void __sort_components(ComponentController &controller)
{}

#endif