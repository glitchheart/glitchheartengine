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

#define TransformComponentType CP_TRANSFORM
#define AnimationComponentType CP_ANIMATION
#define SpriteRendererComponentType CP_SPRITE_RENDERER
#define MeshRendererComponentType CP_MESH_RENDERER
#define BoxColliderComponentType CP_BOX_COLLIDER
#define BoxCollider2DComponentType CP_BOX_COLLIDER_2D
#define LightingComponentType CP_LIGHTING

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

struct ComponentGroup
{
    ComponentList **componentLists;
};

struct ComponentController
{
    u64 *entity_components;
    EntityComponentMapping *entity_mappings;
    i32 entity_count;
    
    union
    {
        ComponentList components[4];
        struct
        {
            ComponentList transform_components;
            ComponentList animation_components;
            ComponentList rendering_components;
            ComponentList lighting_components;
        };
    };
    
    MemoryArena buffer_arena;
};

struct Entity
{
    u32 handle;
};
#define _CRT_NO_VA_START_VALIDATION
ComponentGroup begin_component_group_block(ComponentController &controller, i32 count, ...)
{
    ComponentGroup group = {};
    va_list args;
    va_start(args, count);
    return(group);
}

b32 has_queried_components(ComponentController &controller, i32 entity, u64 component_flags)
{
    return (b32)(controller.entity_components[entity] & component_flags);
}

#define get_component(controller, entity, type) (type*)get__component(controller, entity, type##Type, sizeof(type))
void* get__component(ComponentController &controller, i32 entity, ComponentType type, size_t size_bytes)
{
    i32 handle = controller.entity_mappings[entity].component_handles[type];
    return (&controller.components[type].components) + (handle * size_bytes);
}

#define add_component(controller, entity, type) (type*)add__component(controller, entity, type##Type, sizeof(type))
void* add__component(ComponentController &controller, i32 entity, ComponentType type, size_t size_bytes)
{
    controller.entity_components[entity] |= type_to_flag[type];
    controller.entity_mappings[entity].component_handles[type] = controller.components[type].count++;
    i32 handle = controller.entity_mappings[entity].component_handles[type];
    return ((char *)controller.components[type].components) + (handle * size_bytes);
}

ComponentList *get_all_components(ComponentController &controller, ComponentType type)
{
    return &controller.components[type];
}

void __sort_components(ComponentController &controller)
{}

void component_test()
{
    i32 num_entities = 3;
    ComponentController controller = {0};
    
    controller.entity_components = (u64*)malloc(sizeof(u64) * num_entities);
    controller.entity_mappings = (EntityComponentMapping*)malloc(sizeof(EntityComponentMapping) * num_entities);
    
    controller.transform_components.components = malloc(sizeof(TransformComponent) * num_entities);
    controller.transform_components.count = 0;
    
    TransformComponent *added_t_comp = add_component(controller, 0, TransformComponent);
    added_t_comp->position = math::Vec3(1.0f, 5.0f, 300.0f);
    added_t_comp->rotation = math::Vec3(45.0f, 0.0f, 0.0f);
    
    added_t_comp = add_component(controller, 1, TransformComponent);
    added_t_comp->position = math::Vec3(1.0f, 5.0f, 300.0f);
    added_t_comp->rotation = math::Vec3(45.0f, 0.0f, 0.0f);
    
    // added_t_comp = add_component(controller, 2, TransformComponent);
    // added_t_comp->position = math::Vec3(1.0f, 5.0f, 300.0f);
    // added_t_comp->rotation = math::Vec3(45.0f, 0.0f, 0.0f);
    
    //ComponentList *transforms = get_all_components(controller, CP_TRANSFORM);
    
    // printf("Count %d/n", transforms->count);
    // for(i32 i = 0; i < transforms->count; i++)
    // {
    //     TransformComponent *transform = ((TransformComponent**)(transforms->components))[i];
    //     debug("Position: %f %f %f\n", transform->position.x, transform->position.y, transform->position.z);
    // }
    
    TransformComponent *t_comp = get_component(controller, 0, TransformComponent);
    debug("Position: %f %f %f\n", t_comp->position.x, t_comp->position.y, t_comp->position.z);
    debug("Rotation: %f %f %f\n", t_comp->rotation.x, t_comp->rotation.y, t_comp->rotation.z);
    debug("Position: %f %f %f\n", added_t_comp->position.x, added_t_comp->position.y, added_t_comp->position.z);
    debug("Rotation: %f %f %f\n", added_t_comp->rotation.x, added_t_comp->rotation.y, added_t_comp->rotation.z);
}

#endif