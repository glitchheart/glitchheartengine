#ifndef SCENE_H
#define SCENE_H

#define EMPTY_ENTITY_HANDLE {0}
#define EMPTY_COMP_HANDLE {-1}
#define EMPTY_TEMPLATE_HANDLE {-1}
#define IS_ENTITY_HANDLE_VALID(ent_handle) ent_handle.handle != 0
#define IS_COMP_HANDLE_VALID(comp_handle) comp_handle.handle != -1
#define IS_TEMPLATE_HANDLE_VALID(comp_handle) comp_handle.handle != -1

namespace scene
{
    // @Note(Daniel): Invalid EntityHandles have the value 0.
    // All handles besides EntityHandles are 0-indexed
    struct EntityHandle
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
    
    struct ParticleSystemComponentHandle
    {
        i32 handle;
    };

    struct LightComponentHandle
    {
        i32 handle;
    };

    enum LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT
    };
    
    enum ComponentTypeFlags
    {
        COMP_TRANSFORM = 1 << 0,
        COMP_RENDER = 1 << 1,
        COMP_PARTICLES = 1 << 2,
        COMP_LIGHT = 1 << 3
    };
    
    struct Entity
    {
        u64 comp_flags;
        TransformComponentHandle transform_handle;
        RenderComponentHandle render_handle;
        ParticleSystemComponentHandle particle_system_handle;
        LightComponentHandle light_handle;
    };
    
    struct TransformComponent
    {
        math::Vec3 position;
        math::Vec3 scale;
        math::Vec3 rotation;

		TransformComponentHandle parent_handle;
		TransformComponentHandle child_handle; // Remember the child's handle to be able to quickly remove the childs parent handle if the parent is removed
	};
    
    struct RenderComponent
    {
        MeshHandle mesh_handle;
        MaterialHandle material_handle;
        MaterialHandle original_material_handle;
        
        b32 cast_shadows;
        b32 receives_shadows;

        b32 is_new_version;
        struct
        {
            rendering::BufferHandle buffer_handle;
            rendering::MaterialInstanceHandle material_handle;
        } v2;
    };
    
    struct ParticleSystemComponent
    {
        ParticleSystemHandle handle;
    };

    struct LightComponent
    {
        LightType type;
        
        union
        {
            DirectionalLight dir_light;
            PointLight point_light;
        };
    };

    struct TemplateHandle
    {
        i32 handle;
    };
	
    struct EntityTemplate
    {
        char file_path[256];
        u64 comp_flags;
        
        struct
        {
            math::Vec3 position;
            math::Vec3 scale;
            math::Vec3 rotation;
            TemplateHandle child_handle;
        } transform;
        struct
        {
            MeshHandle mesh_handle;
            MaterialHandle material_handle;
            
            b32 cast_shadows;
            b32 receives_shadows;

            
            b32 is_new_version;
            struct
            {
                rendering::BufferHandle buffer_handle;
                rendering::MaterialHandle material_handle;
            } v2;
        } render;
        struct
        {
            ParticleSystemAttributes attributes;
            i32 max_particles;
            b32 started;
            
            struct
            {
                math::Rgba values[32];
                r64 keys[32];
                i32 value_count;
            } color_over_lifetime;
            struct 
            {
                math::Vec2 values[32];
                r64 keys[32];
                i32 value_count;
            } size_over_lifetime;
            struct
            {
                r32 values[32];
                r64 keys[32];
                i32 value_count;
            } speed_over_lifetime;
        } particles;
        struct
        {
        } light;
    };
    
    // Holds all the currently loaded templates
    struct EntityTemplateState
    {
        EntityTemplate *templates;
        i32 template_count;
    };

    enum CameraFlags
    {
        C_FLAG_ORTHOGRAPHIC = (1 << 0),
        C_FLAG_PERSPECTIVE  = (1 << 1),
        C_FLAG_NO_LOOK_AT     = (1 << 2)
    };

    struct CameraParams
    {
        u32 view_flags;
    };

    static CameraParams default_camera_params()
    {
        CameraParams params;
        params.view_flags = C_FLAG_ORTHOGRAPHIC | C_FLAG_NO_LOOK_AT;
        return params;
    }

    static CameraParams orthographic_camera_params()
    {
        CameraParams params;
        params.view_flags = C_FLAG_ORTHOGRAPHIC;
        return params;
    }

    static CameraParams perspective_camera_params()
    {
        CameraParams params;
        params.view_flags = C_FLAG_PERSPECTIVE;
        return params;
    }
    
    struct Scene
    {
        b32 valid;
        
        MemoryArena memory_arena;

        Camera camera;

        Entity *entities;
        i32 *_internal_handles;
        i32 current_internal_handle;
        b32 *active_entities;
        i32 entity_count;
        
        TransformComponent *transform_components;
        i32 transform_component_count;
        
        RenderComponent *render_components;
        i32 render_component_count;
        
        ParticleSystemComponent *particle_system_components;
        i32 particle_system_component_count;
        
        LightComponent *light_components;
        i32 light_component_count;
        
        Material *material_instances;
        i32 material_count;
        
        i32 max_entity_count;
        
        EntityTemplateState *template_state;
        Renderer* renderer;
    };
}



#endif
