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
        
        b32 casts_shadows;

        b32 is_new_version;
        struct
        {
            rendering::BufferHandle buffer_handle;
            rendering::MaterialInstanceHandle material_handle;
            
            // @Note: All the render passes this component should be rendered in
            rendering::RenderPassHandle render_passes[8];
            rendering::ShaderHandle shader_handles[8];
            i32 render_pass_count;
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
            
            b32 casts_shadows;
            
            b32 is_new_version;
            struct
            {
                rendering::BufferHandle buffer_handle;
                rendering::MaterialHandle material_handle;

                char render_pass_names[8][32];
                rendering::ShaderHandle shader_handles[8];
                i32 render_pass_count;
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

    struct SceneManager
    {
        Renderer *renderer;
        EntityTemplateState template_state;
        MemoryArena arena;

        b32 scene_loaded;
        Scene *loaded_scene;
        Scene *scenes;
        i32 scene_count;
        i32 *_internal_scene_handles;
        i32 current_internal_index;
    };

    struct SceneHandle
    {
        i32 handle;
        SceneManager *manager;
    };

    static SceneManager create_scene_manager(Renderer &renderer)
    {
        SceneManager scene_manager = {};
        scene_manager.current_internal_index = 0;
        scene_manager.loaded_scene = nullptr;
        scene_manager.scenes = push_array(&scene_manager.arena, global_max_scenes, scene::Scene);
        scene_manager._internal_scene_handles = push_array(&scene_manager.arena, global_max_scenes, i32);

        for(i32 i = 0; i < global_max_scenes; i++)
        {
            scene_manager._internal_scene_handles[i] = -1;
        }
        
        scene_manager.scene_count = 0;
        scene_manager.template_state.templates = push_array(&scene_manager.arena, global_max_entity_templates, EntityTemplate);
        scene_manager.template_state.template_count = 0;
       
        scene_manager.renderer = &renderer;
        return scene_manager;
    }
}



#endif
