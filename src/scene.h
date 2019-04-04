#ifndef SCENE_H
#define SCENE_H

#define SCALE_SENSITIVITY 0.01f

#define EMPTY_ENTITY_HANDLE {0}
#define EMPTY_COMP_HANDLE {-1}
#define EMPTY_TEMPLATE_HANDLE {-1}
#define HANDLES_EQUAL(h1, h2) ( h1.handle == h2.handle )
#define IS_ENTITY_HANDLE_VALID(ent_handle) (ent_handle.handle > 0)
#define IS_COMP_HANDLE_VALID(comp_handle) (comp_handle.handle > -1)
#define IS_TEMPLATE_HANDLE_VALID(comp_handle) comp_handle.handle != -1
#define IS_SCENE_HANDLE_VALID(scene_handle) (scene_handle.handle > 0)
#define SCENES_EQUAL(s1, s2) (s1.handle == s2.handle)

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

    struct EntityData
    {
        EntityHandle handle;
    };
    
    enum FieldType
    {
        INT,
        UINT,
        BOOL,
        FLOAT,
        VEC2,
        VEC3,
        VEC4,
        STRING,
        COLOR
    };
    
    struct Field
    {
        FieldType type;
        char name[32];
        size_t offset;
    };

    struct RegisteredEntityType
    {
        u32 type_id;
        Field fields[32];
        i32 field_count;
    };

#define MAX_ENTITY_TAGS 8
    struct Tags
    {
        char tags[MAX_ENTITY_TAGS][32];
        i32 tag_count;
    };

    struct EntityList
    {
        EntityHandle handles[64];
        i32 entity_count;
    };

    #define MAX_CHILDREN 64
    struct Entity
    {
        char name[256];
        Tags tags;
        
        u32 type; // The entity type can be used to map the saved entities to game-specific logic
        RegisteredEntityType type_info;
        EntityData* entity_data;

        b32 selection_enabled;
        b32 savable; // Should this be saved in the scene file?
        b32 hide_in_ui;
        
        char template_path[256];
        
        EntityHandle handle;
        
        u64 comp_flags;
        TransformComponentHandle transform_handle;
        RenderComponentHandle render_handle;
        ParticleSystemComponentHandle particle_system_handle;
        LightComponentHandle light_handle;

        EntityHandle parent;
        
        EntityHandle children[MAX_CHILDREN];
        i32 child_count;
    };
    
    struct TransformComponent
    {
        rendering::Transform transform;
        EntityHandle entity;
	};
    
    struct RenderComponent
    {
        rendering::BufferHandle buffer_handle;
        rendering::BufferHandle bounding_box_buffer;
        
        math::Vec3 mesh_scale;
        math::BoundingBox bounding_box;

        b32 is_static;
        b32 casts_shadows;
        b32 wireframe_enabled;
        b32 bounding_box_enabled;
        b32 ignore_depth;
            
        // @Note: All the render passes this component should be rendered in
        rendering::RenderPassHandle render_passes[8];
        rendering::MaterialInstanceHandle material_handles[8];
        rendering::MaterialHandle original_materials[8];
        b32 is_pushed[8];
        
        i32 render_pass_count;
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
        b32 hide_in_ui;
        
        struct
        {
            math::Vec3 position;
            math::Vec3 scale;
            math::Vec3 rotation;
            TemplateHandle child_handle;
        } transform;
        struct
        {
            b32 is_static;
            b32 ignore_depth;
            b32 casts_shadows;
            math::Vec3 mesh_scale;
            math::BoundingBox bounding_box;
            b32 is_new_version;
            rendering::BufferHandle buffer_handle;
            rendering::BufferHandle bounding_box_buffer;
            rendering::MaterialHandle material_handle;

            char render_pass_names[8][32];
            rendering::MaterialHandle material_handles[8];
            i32 render_pass_count;
        } render;
        struct
        {
            ParticleSystemAttributes attributes;
            rendering::MaterialHandle material_handle;
            rendering::ShaderHandle shader_handle;

            i32 max_particles;
            
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
            LightType type;

            math::Vec3 ambient;
            math::Vec3 diffuse;
            math::Vec3 specular;
            
            union
            {
                struct
                {                    
                    math::Vec3 direction;
                } directional;
                struct
                {
                    math::Vec3 position;
                    r32 constant;
                    r32 linear;
                    r32 quadratic;
                } point;
            };
        } light;
        
#define MAX_CHILD_HANDLES 64
        TemplateHandle child_handles[MAX_CHILD_HANDLES];
        i32 child_count;
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

    #define MAX_INSTANCE_BUFFER_HANDLES 256
    struct InstanceBufferData
    {
        rendering::BufferHandle buffer_handle;
        rendering::MaterialHandle source_material_handle;
        rendering::RenderPassHandle pass_handle;
        
        rendering::InstanceBufferHandle instance_buffer_handles[MAX_INSTANCE_BUFFER_HANDLES];
        i32 instance_buffer_count;

        i32 max_count;
    };

    struct SceneHandle
    {
        i32 handle;
        SceneManager *manager;
    };

    #define GIZMO_TOLERANCE 0.07f
    struct Scene
    {
        SceneHandle handle;

        char file_path[256];
        
        b32 valid;
        b32 loaded;
        b32 persistent; // When true the scene is not freed, when a new scene is loaded

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

    typedef void (*OnStartedEditMode)(SceneHandle scene);
    typedef void (*OnExitedEditMode)(SceneHandle scene);
    typedef void (*OnLoad)(SceneHandle scene);
    typedef void (*OnSave)(SceneHandle scene);
    typedef void (*OnSceneWillBeFreed)(SceneHandle scene);
    typedef void (*OnSceneWillLoad)(SceneHandle scene);
    typedef void (*OnSceneLoaded)(SceneHandle scene);
    typedef void (*OnEntityUpdated)(EntityHandle entity, SceneHandle scene);
    typedef void (*OnEntitySelected)(EntityHandle entity, SceneHandle scene);
    typedef void (*OnEntityWillBeDeleted)(EntityHandle entity, SceneHandle scene);
    typedef void (*OnEntityRegisteredWithType)(EntityHandle entity, u32 type, SceneHandle scene);
    typedef EntityData* (*OnLoadEntityOfType)(EntityHandle entity, u32 type, SceneHandle scene);
    typedef void (*OnLoadedEntityOfType)(EntityHandle entity, u32 type, SceneHandle scene);

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

    enum class TransformationType
    {
        POSITION,
        SCALE,
        ROTATION
    };

    enum class ScalingMode
    {
        SINGLE_AXIS,
        ALL_AXIS
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

        RegisteredEntityType registered_types[64];
        i32 registered_type_count;
        
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

            rendering::ShaderHandle line_shader;
            rendering::MaterialHandle line_material;
            rendering::MaterialInstanceHandle x_material;
            rendering::MaterialInstanceHandle y_material;
            rendering::MaterialInstanceHandle z_material;

            scene::EntityHandle scale_cubes[4];
            
            math::Vec3 x_scale;
            math::Vec3 y_scale;
            math::Vec3 z_scale;
            r32 current_distance_to_camera;

            TransformationType transformation_type;
            ScalingMode scaling_mode;
            math::Vec2 scale_mouse_offset;
            
            math::Vec3 initial_offset;
            math::Vec3 initial_scale;
            // @Incomplete: initial_rotation
            
            Line current_line;
        } gizmos;
        
        struct
        {
            b32 selection_enabled;
            b32 lock_camera;
            DirectoryData template_files;
        } editor;
        
        struct
        {
            OnStartedEditMode on_started_edit_mode;
            OnExitedEditMode on_exited_edit_mode;
            OnLoad on_load;
            OnSave on_save;
            OnSceneWillBeFreed on_scene_will_be_freed;
            OnSceneWillLoad on_scene_will_load;
            OnSceneLoaded on_scene_loaded;
            OnEntityUpdated on_entity_updated;
            OnEntitySelected on_entity_selected;
            OnEntityWillBeDeleted on_entity_will_be_deleted;
            OnEntityRegisteredWithType on_entity_registered_with_type;
            OnLoadEntityOfType on_load_entity_of_type;
            OnLoadedEntityOfType on_loaded_entity_of_type;
        } callbacks;
    };

    static SceneManager* create_scene_manager(MemoryArena *arena, Renderer *renderer)
    {
        SceneManager *scene_manager = push_struct(arena, SceneManager);
        scene_manager->current_internal_index = 0;
        scene_manager->scene_loaded = false;
        scene_manager->editor.selection_enabled = true;
        scene_manager->editor.lock_camera = false;
        scene_manager->loaded_scene = { -1 , nullptr};
        scene_manager->scenes = push_array(arena, global_max_scenes, scene::Scene);
        scene_manager->_internal_scene_handles = push_array(arena, global_max_scenes, i32);

        for(i32 i = 0; i < global_max_scenes; i++)
        {
            scene_manager->_internal_scene_handles[i] = -1;
        }
        
        scene_manager->scene_count = 0;
        scene_manager->template_state.templates = push_array(&scene_manager->arena, global_max_entity_templates, EntityTemplate);
        scene_manager->template_state.template_count = 0;
       
        scene_manager->renderer = renderer;

        scene_manager->debug_cube.handle = 0;
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
