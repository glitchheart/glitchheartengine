#ifndef SCENE_H
#define SCENE_H

#define EMPTY_ENTITY_HANDLE {0}
#define EMPTY_COMP_HANDLE {-1}
#define EMPTY_TEMPLATE_HANDLE {-1}
#define IS_ENTITY_HANDLE_VALID(ent_handle) (ent_handle.handle > 0)
#define IS_COMP_HANDLE_VALID(comp_handle) comp_handle.handle != -1
#define IS_TEMPLATE_HANDLE_VALID(comp_handle) comp_handle.handle != -1
#define IS_SCENE_HANDLE_VALID(scene_handle) (scene_handle.handle > 0)

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
        char name[256];
        b32 savable; // Should this be saved in the scene file?
        
        char template_path[256];
        
        EntityHandle handle;
        
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
        rendering::MaterialHandle material_handle;
        rendering::MaterialHandle original_material_handle;
        
        b32 casts_shadows;
        math::Vec3 mesh_scale;

        b32 wireframe_enabled;
        b32 ignore_depth;
        
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

        rendering::RenderPassHandle render_pass;
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
        char name[256];
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
            rendering::MaterialHandle material_handle;

            b32 ignore_depth;
            b32 casts_shadows;
            math::Vec3 mesh_scale;
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
            rendering::MaterialHandle material_handle;
            rendering::ShaderHandle shader_handle;

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

    enum SceneLoadFlags
    {
        FREE_CURRENT_INSTANCE_BUFFERS = 1 << 0,
        FREE_CURRENT_SCENE = 1 << 1
    };

    #define MAX_INSTANCE_BUFFER_HANDLES 128
    struct InstanceBufferData
    {
        rendering::BufferHandle buffer_handle;
        rendering::MaterialHandle source_material_handle;

        rendering::InstanceBufferHandle instance_buffer_handles[MAX_INSTANCE_BUFFER_HANDLES];
        i32 instance_buffer_count;

        i32 max_count;
    };

    #define GIZMO_TOLERANCE 0.07f
    struct Scene
    {
        b32 valid;
        b32 loaded;
        
        MemoryArena memory_arena;

        Camera camera;

        InstanceBufferData instance_buffer_data[MAX_INSTANCE_BUFFER_HANDLES];
        i32 instance_buffer_data_count;

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

        i32 max_entity_count;
        
        EntityTemplateState *template_state;
        SceneManager *scene_manager;
        Renderer* renderer;
    };

    struct SceneHandle
    {
        i32 handle;
        SceneManager *manager;
    };

    typedef void (*OnStartedEditMode)(SceneHandle scene);
    typedef void (*OnLoad)(SceneHandle scene);
    typedef void (*OnSave)(SceneHandle scene);
    typedef void (*OnEntityUpdated)(EntityHandle entity, SceneHandle scene);
    typedef void (*OnEntitySelected)(EntityHandle entity, SceneHandle scene);

    enum class SceneMode
    {
        RUNNING,
        EDITING
    };

    enum class Gizmos
    {
        NONE,
        X_ARROW,
        Y_ARROW,
        Z_ARROW
    };

    enum class TranslationConstraint
    {
        NONE,
        X,
        Y,
        Z
    };
    
    struct Line
    {
        math::Vec3 start;
        math::Vec3 end;
    };

    struct SceneManager
    {
        Renderer *renderer;
        EntityTemplateState template_state;
        MemoryArena arena;

        b32 scene_loaded;
        SceneHandle loaded_scene;
        Scene *scenes;
        i32 scene_count;
        i32 *_internal_scene_handles;
        i32 current_internal_index;

        SceneMode mode;

        Camera play_camera;
        EntityHandle selected_entity;

        b32 dragging;

        rendering::BufferHandle debug_cube;
        rendering::Material debug_material_instance;
        rendering::Material debug_material_instance_2;
        rendering::MaterialHandle debug_material;
        rendering::ShaderHandle debug_shader_handle;
        
        struct
        {
            b32 active;
            Gizmos selected_gizmo;
            TranslationConstraint constraint;

            rendering::BufferHandle x_buffer;
            rendering::BufferHandle y_buffer;
            rendering::BufferHandle z_buffer;
            rendering::ShaderHandle line_shader;
            rendering::MaterialHandle line_material;
            rendering::MaterialInstanceHandle x_material;
            rendering::MaterialInstanceHandle y_material;
            rendering::MaterialInstanceHandle z_material;
            
            math::Vec3 x_scale;
            math::Vec3 y_scale;
            math::Vec3 z_scale;
            r32 current_distance_to_camera;
            
            math::Vec3 initial_offset;
            Line current_line;
        } gizmos;
        
        struct
        {
            OnStartedEditMode on_started_edit_mode;
            OnLoad on_load;
            OnSave on_save;
            OnEntityUpdated on_entity_updated;
            OnEntitySelected on_entity_selected;
        } callbacks;
    };

    static SceneManager* create_scene_manager(MemoryArena *arena, Renderer &renderer)
    {
        SceneManager *scene_manager = push_struct(arena, SceneManager);
        scene_manager->current_internal_index = 0;
        scene_manager->scene_loaded = false;
        scene_manager->loaded_scene = { -1 };
        scene_manager->scenes = push_array(arena, global_max_scenes, scene::Scene);
        scene_manager->_internal_scene_handles = push_array(arena, global_max_scenes, i32);

        for(i32 i = 0; i < global_max_scenes; i++)
        {
            scene_manager->_internal_scene_handles[i] = -1;
        }
        
        scene_manager->scene_count = 0;
        scene_manager->template_state.templates = push_array(&scene_manager->arena, global_max_entity_templates, EntityTemplate);
        scene_manager->template_state.template_count = 0;
       
        scene_manager->renderer = &renderer;

        scene_manager->debug_cube.handle = 0;

        scene_manager->gizmos.x_buffer = rendering::create_line_buffer(renderer);
        scene_manager->gizmos.y_buffer = rendering::create_line_buffer(renderer);
        scene_manager->gizmos.z_buffer = rendering::create_line_buffer(renderer);

        scene_manager->gizmos.line_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/line.shd");
        scene_manager->gizmos.line_material = rendering::create_material(renderer, scene_manager->gizmos.line_shader);
        
        scene_manager->gizmos.x_material = rendering::create_material_instance(renderer, scene_manager->gizmos.line_material);
        scene_manager->gizmos.y_material = rendering::create_material_instance(renderer, scene_manager->gizmos.line_material);
        scene_manager->gizmos.z_material = rendering::create_material_instance(renderer, scene_manager->gizmos.line_material);
        
        return scene_manager;
    }

    static Scene &get_scene(SceneHandle handle)
    {
        assert(handle.handle > 0);
        SceneManager *manager = handle.manager;
        return manager->scenes[manager->_internal_scene_handles[handle.handle - 1]];
    }
}



#endif
