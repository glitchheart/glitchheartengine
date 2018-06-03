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

const size_t type_to_size[] =
{
    sizeof(TransformComponent),
    sizeof(AnimationComponent),
    sizeof(SpriteRendererComponent),
    sizeof(MeshRendererComponent),
    sizeof(BoxColliderComponent),
    sizeof(BoxCollider2DComponent),
    sizeof(LightingComponent)
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
    union
    {
        void *components;
    };
    i32 count;
};

struct ComponentGroup
{
    ComponentList *component_lists;
    i32 buffer_count;
    i32 num_indices;
    TemporaryMemory temp_memory;
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
            ComponentList sprite_renderer_components;
            ComponentList mesh_renderer_components;
            ComponentList box_collider_components;
            ComponentList box_collider_2D_components;
            ComponentList lighting_components;
        };
    };
    
    MemoryArena buffer_arena;
};

struct Entity
{
    u32 handle;
};

b32 has_queried_components(ComponentController &controller, i32 entity, u64 component_flags)
{
    return (b32)((controller.entity_components[entity] & component_flags) == component_flags);
}

#define IS_MSVC _MSC_VER && !__INTEL_COMPILER

#if IS_MSVC
#define MSVC_HACK(FUNC, ARGS) FUNC ARGS
#define APPLY(FUNC, ...) MSVC_HACK(FUNC, (__VA_ARGS__))
#define VA_LENGTH(...) APPLY(VA_LENGTH_, 0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#else
#define VA_LENGTH(...) VA_LENGTH_(0, ## __VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#endif

#define VA_LENGTH_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, N, ...) N

#define ExecVF(Func, controller, ...) Func(controller, VA_LENGTH(__VA_ARGS__), __VA_ARGS__)

// @Note(Daniel): The count parameter is necessary since the va_args macro won't work with a ref parameter before the arguments
#define begin_component_group_block(controller, ...) ExecVF(_begin_component_group_block, controller, __VA_ARGS__)
ComponentGroup _begin_component_group_block(ComponentController &controller, i32 count, ...)
{
    ComponentGroup group = {};
    
    group.temp_memory = begin_temporary_memory(&controller.buffer_arena);
    
    va_list args;
    va_start(args, count);
    
    u64 flags = 0;
    void * buffers[2];
    buffers[0] = nullptr;
    buffers[1] = nullptr;
    
    ComponentType *types = nullptr;
    
    for (int i = 0; i < count; ++i)
    {
        auto component_type = va_arg(args, ComponentType);
        auto flag = type_to_flag[component_type];
        flags |= flag;
        buf_push(types, component_type);
    }
    
    va_end(args);
    
    group.buffer_count = count;
    group.num_indices = 0;
    group.component_lists = push_array(group.temp_memory.arena, group.buffer_count, ComponentList);
    
    for(i32 entity_handle = 0; entity_handle < controller.entity_count; entity_handle++)
    {
        if(has_queried_components(controller, entity_handle, flags))
        {
            group.num_indices++;
            
            for(i32 type_index = 0; type_index < (i32)buf_len(types); type_index++)
            {
                auto type = types[type_index];
                
                switch(type)
                {
                    case CP_TRANSFORM:
                    {
                        auto type_buffer = (TransformComponent*)buffers[type_index];
                        buf_push(type_buffer, ((TransformComponent*)controller.transform_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                        printf("Damn\n");
                    }
                    break;
                    case CP_ANIMATION:
                    {
                        auto type_buffer = (AnimationComponent*)buffers[type_index];
                        buf_push(type_buffer, ((AnimationComponent*)controller.animation_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                    }
                    break;
                    case CP_SPRITE_RENDERER:
                    {
                        auto type_buffer = (SpriteRendererComponent*)buffers[type_index];
                        buf_push(type_buffer, ((SpriteRendererComponent*)controller.sprite_renderer_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                    }
                    break;
                    case CP_MESH_RENDERER:
                    {
                        auto type_buffer = (MeshRendererComponent*)buffers[type_index];
                        buf_push(type_buffer, ((MeshRendererComponent*)controller.mesh_renderer_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                    }
                    break;
                    case CP_BOX_COLLIDER:
                    {
                        auto type_buffer = (BoxColliderComponent*)buffers[type_index];
                        buf_push(type_buffer, ((BoxColliderComponent*)controller.box_collider_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                    }
                    break;
                    case CP_BOX_COLLIDER_2D:
                    {
                        auto type_buffer = (BoxCollider2DComponent*)buffers[type_index];
                        buf_push(type_buffer, ((BoxCollider2DComponent*)controller.box_collider_2D_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                    }
                    break;
                    case CP_LIGHTING:
                    {
                        auto type_buffer = (LightingComponent*)buffers[type_index];
                        buf_push(type_buffer, ((LightingComponent*)controller.lighting_components.components)[controller.entity_mappings[entity_handle].component_handles[type]]);
                    }
                    break;
                    default:
                    break;
                }
            }
        }
    }
    
    for(i32 type_index = 0; type_index < group.buffer_count; type_index++)
    {
        size_t size =  buf_len(buffers[type_index]) * type_to_size[types[type_index]];
        group.component_lists[type_index].components = push_size(group.temp_memory.arena, size, void *);
        memcpy(group.component_lists[type_index].components, buffers[type_index], size);
        buf_free(buffers[type_index]);
    }
    
    buf_free(types);
    
    return(group);
}

void end_component_group_block(ComponentGroup& group)
{
    end_temporary_memory(group.temp_memory);
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
    
    controller.entity_count = num_entities;
    controller.entity_components = (u64*)malloc(sizeof(u64) * num_entities);
    controller.entity_components[0] = 0;
    controller.entity_components[1] = 0;
    controller.entity_components[2] = 0;
    
    controller.entity_mappings = (EntityComponentMapping*)malloc(sizeof(EntityComponentMapping) * num_entities);
    
    controller.transform_components.components = malloc(sizeof(TransformComponent) * num_entities);
    controller.mesh_renderer_components.components = malloc(sizeof(MeshRendererComponent) * num_entities);
    controller.transform_components.count = 0;
    
    TransformComponent *added_t_comp = add_component(controller, 0, TransformComponent);
    added_t_comp->position = math::Vec3(1.0f, 5.0f, 300.0f);
    added_t_comp->rotation = math::Vec3(45.0f, 0.0f, 0.0f);
    
    added_t_comp = add_component(controller, 1, TransformComponent);
    added_t_comp->position = math::Vec3(1.0f, 5.0f, 300.0f);
    added_t_comp->rotation = math::Vec3(45.0f, 0.0f, 0.0f);
    //MeshRendererComponent *added_mesh_comp = add_component(controller, 0, MeshRendererComponent);
    
    auto group_block = begin_component_group_block(controller, CP_TRANSFORM, CP_MESH_RENDERER);
    
    printf("Indices %d\n", group_block.num_indices);
    
    for(i32 index = 0; index < group_block.num_indices; index++)
    {
        auto transform = ((TransformComponent *)group_block.component_lists[0].components)[index];
        auto mesh_renderer = ((MeshRendererComponent *)group_block.component_lists[1].components)[index];
        
        debug("Position: %f %f %f\n", transform.position.x, transform.position.y, transform.position.z);
    }
    
    end_component_group_block(group_block);
    
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
    
    // TransformComponent *t_comp = get_component(controller, 0, TransformComponent);
    // debug("Rotation: %f %f %f\n", t_comp->rotation.x, t_comp->rotation.y, t_comp->rotation.z);
    // debug("Position: %f %f %f\n", added_t_comp->position.x, added_t_comp->position.y, added_t_comp->position.z);
    // debug("Rotation: %f %f %f\n", added_t_comp->rotation.x, added_t_comp->rotation.y, added_t_comp->rotation.z);
}

#endif