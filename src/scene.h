#ifndef SCENE_H
#define SCENE_H

#define ENTITY_IS_VALID(handle) handle.handle > 0

#define SCALE_SENSITIVITY 0.01f

#define EMPTY_ENTITY_HANDLE {0}
#define EMPTY_COMP_HANDLE {-1}
#define EMPTY_TEMPLATE_HANDLE {-1}
#define IS_ENTITY_HANDLE_VALID(ent_handle) (ent_handle.handle > 0)
#define IS_COMP_HANDLE_VALID(comp_handle) (comp_handle.handle > -1)
#define IS_TEMPLATE_HANDLE_VALID(comp_handle) comp_handle.handle != -1
#define IS_SCENE_HANDLE_VALID(scene_handle) (scene_handle.handle > 0)
#define HANDLES_EQUAL(h1, h2) (h1.handle == h2.handle)
#define SCENES_EQUAL(s1, s2) HANDLES_EQUAL(s1, s2)

namespace scene
{
    struct SceneHandle
    {
        i32 handle;
        SceneManager *manager;
    };
    
    // @Note(Daniel): Invalid EntityHandles have the value 0.
    // All handles besides EntityHandles are 0-indexed
    struct EntityHandle
    {
        i32 handle;
        SceneHandle scene_handle;
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

    struct AnimatorHandle
    {
        i32 handle;
    };

    struct CameraComponentHandle
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
        COMP_LIGHT = 1 << 3,
        COMP_ANIMATOR = 1 << 4,
        COMP_CAMERA = 1 << 5
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

    #define ENTITY_LIST_SIZE 512
    struct EntityList
    {
        EntityHandle handles[ENTITY_LIST_SIZE];
        i32 entity_count;
    };

    #define MAX_CHILDREN 256
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
        AnimatorHandle animator_handle;
        ParticleSystemComponentHandle particle_system_handle;
        LightComponentHandle light_handle;
        CameraComponentHandle camera_handle;

        EntityHandle parent;
        
        EntityHandle children[MAX_CHILDREN];
        i32 child_count;
    };
    
    struct TransformComponent
    {
        rendering::Transform transform;
        EntityHandle entity;
	};
    
    enum RenderMode
    {
        RENDER_OPAQUE,
        RENDER_TRANSPARENT
    };
    
    struct RenderComponent
    {
        RenderMode render_mode;
        
        rendering::BufferHandle buffer_handle;
        
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

    typedef void (*AnimationCallback)();
    
    enum class AnimationType
    {
        FLOAT,
        VEC3
    };
    
    // HANDLES
    struct AnimationTransitionHandle
    {
        i32 handle;
    };

    struct AnimationTransitionConditionHandle
    {
        i32 handle;
    };

    struct AnimatorParameterHandle
    {
        i32 handle;
    };
    
    struct RootAnimationHandle
    {
        i32 handle;
        EntityHandle entity;
    };

    struct AnimationHandle
    {
        i32 handle;
        AnimationType type;
    };
    
    // END HANDLES

    #define MAX_ANIMATIONS 16
    #define MAX_KEY_FRAMES 16
    #define MAX_ANIMATION_TRANSITIONS 4
    #define NAX_TRANSITION_CONDITIONS 4
    #define MAX_PARAMETERS 4

    enum class AnimationEasingMode
    {
        LERP,
        EASE_IN,
        EASE_OUT,
        EASE_IN_OUT,
        CONSTANT
    };

    struct FloatAnimation
    {
        AnimationEasingMode easing_mode;

        scene::EntityHandle entity;
        
        b32 running;
        i32 current_key_frame;
        r64 current_time;

        char value_name[32];

        r32 key_frame_times[MAX_KEY_FRAMES];
        r32 key_frame_values[MAX_KEY_FRAMES];
        
        i32 count;
    };

    enum class Vec3Type
    {
        UNIFORM,
        TRANSFORM_POSITION,
        TRANSFORM_ROTATION,
        TRANSFORM_SCALE
    };

    struct Vec3Animation
    {
        AnimationEasingMode easing_mode;

        scene::EntityHandle entity;
        
        b32 running;
        i32 current_key_frame;
        r64 current_time;

        char value_name[16];

        Vec3Type type;

        r32 key_frame_times[MAX_KEY_FRAMES];
        math::Vec3 key_frame_values[MAX_KEY_FRAMES];
        
        i32 count;
    };

    struct AnimationTransitionCondition
    {
        AnimatorParameterHandle handle;
        
        union
        {
            i32 expected_int_val;
            r32 expected_float_val;
            b32 expected_bool_val;
        };
    };
    
    struct AnimationTransition
    {
        RootAnimationHandle from_handle;
        RootAnimationHandle to_handle;

        AnimationTransitionCondition conditions[NAX_TRANSITION_CONDITIONS];
        i32 condition_count;
    };
    
    struct Animation
    {
        b32 loop;

        FloatAnimation float_animations[MAX_ANIMATIONS];
        i32 float_anim_count;

        Vec3Animation vec3_animations[MAX_ANIMATIONS];
        i32 vec3_anim_count;

        AnimationTransition transitions[MAX_ANIMATION_TRANSITIONS];
        i32 transition_count;

        AnimationCallback callback;
    };

    enum class AnimatorParameterType
    {
        INT,
        FLOAT,
        BOOL       
    };
    
    struct AnimatorParameter
    {
        char name[16];

        AnimatorParameterType type;
        
        union
        {
            i32 int_val;
            r32 float_val;
            b32 bool_val;
        };
    };
    
    struct AnimatorComponent
    {
        EntityHandle entity;

        b32 running;
        RootAnimationHandle current_handle;

        Animation animations[4];
        i32 anim_count;

        AnimatorParameter params[MAX_PARAMETERS];
        i32 param_count;

        AnimationTransition transitions[MAX_ANIMATION_TRANSITIONS];
        i32 transition_count;
    };

    struct CameraComponent
    {
        EntityHandle entity;
        b32 is_main_camera;
        Camera camera;
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
        Tags tags;

        i32 type_id;
        
        struct
        {
            math::Vec3 position;
            math::Vec3 scale;
            math::Vec3 rotation;
            TemplateHandle child_handle;
        } transform;

        Camera camera;
        
        struct
        {
            RenderMode render_mode;
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
                r32 keys[32];
                i32 value_count;
            } color_over_lifetime;
            struct 
            {
                math::Vec2 values[32];
                r32 keys[32];
                i32 value_count;
            } size_over_lifetime;
            struct
            {
                r32 values[32];
                r32 keys[32];
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

    struct Settings
    {
        struct
        {
            r32 near_plane;
            r32 far_plane;
            r32 fov;
            i32 map_width;
            i32 map_height;
        } shadows;
    };

    #define GIZMO_TOLERANCE 0.07f
    struct Scene
    {
        SceneHandle handle;

        char file_path[256];
        
        b32 valid;
        b32 loaded;
        b32 persistent; // When true the scene is not freed, when a new scene is loaded

        scene::EntityHandle editor_camera;

        MemoryArena memory_arena;

        InstanceBufferData instance_buffer_data[MAX_INSTANCE_BUFFER_HANDLES];
        i32 instance_buffer_data_count;

        list::SwapList<Entity> entities;
        
        /* Entity *entities; */
        /* i32 *_internal_handles; */
        /* i32 current_internal_handle; */
        b32 *active_entities;
        /* i32 entity_count; */

        Settings settings;
        
        EntityHandle render_pass_cameras[32];
        
        TransformComponent *transform_components;
        i32 transform_component_count;
        
        RenderComponent *render_components;
        i32 render_component_count;

        AnimatorComponent **animator_components;
        i32 animator_component_count;
        
        ParticleSystemComponent *particle_system_components;
        i32 particle_system_component_count;
        
        LightComponent *light_components;
        i32 light_component_count;

        CameraComponent *camera_components;
        i32 camera_component_count;

        EntityHandle main_camera_handle;
        Camera main_camera;

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
    typedef void (*OnSceneLoaded)(SceneHandle scene, b32 is_first_load);
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

        EntityHandle editor_camera;

        struct
        {
            b32 show_camera_preview;
            EntityHandle handle;
        } camera_preview;
        
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

    static Camera get_standard_camera(SceneManager& manager);

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

    static Scene create_scene(Renderer *renderer, EntityTemplateState &template_state, i32 initial_entity_array_size);
    static SceneHandle create_scene_from_file(const char *scene_file_path, SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size);
    static SceneHandle create_scene(SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size);
    
    static void free_scene(SceneHandle scene, b32 invalidate_handle = true);
    static void load_scene(SceneHandle handle, u64 scene_load_flags = 0);
    static void unload_scene(SceneManager *scene_manager);
    
// Scene handle
    static RenderComponent& add_render_component(SceneHandle scene, EntityHandle entity_handle, b32 cast_shadows);
    static TransformComponent& add_transform_component(SceneHandle scene, EntityHandle entity_handle);
    static AnimatorComponent& add_animator_component(SceneHandle scene, EntityHandle entity_handle);
    static ParticleSystemComponent& add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material);
    static LightComponent & add_light_component(SceneHandle &handle, EntityHandle entity_handle);
    static EntityHandle register_entity(u64 comp_flags, SceneHandle scene, b32 savable);
    static void unregister_entity(EntityHandle handle, SceneHandle scene);
    static EntityTemplate _load_template(const char *path, EntityTemplateState template_state, SceneHandle scene);
    static EntityHandle register_entity_from_template_file(const char *path, SceneHandle scene, b32 savable = false, Tags* tags = nullptr);
    static void set_active(EntityHandle handle, b32 active, SceneHandle scene, b32 recursive = true);
    static void set_hide_in_ui(EntityHandle handle, b32 hide, SceneHandle scene_handle);
    static TransformComponent& get_transform_comp(EntityHandle handle, Scene &scene);
    static TransformComponent& get_transform_comp(EntityHandle handle, SceneHandle scene);
    static TransformComponent& get_transform_comp(TransformComponentHandle handle, SceneHandle scene);
    static RenderComponent& get_render_comp(EntityHandle handle, SceneHandle scene);
    static CameraComponent& get_camera_comp(EntityHandle handle, SceneHandle scene);
    static AnimatorComponent& get_animator_comp(EntityHandle handle, SceneHandle scene);
    static ParticleSystemComponent& get_particle_system_comp(EntityHandle handle, SceneHandle scene);
    static LightComponent &get_light_comp(EntityHandle handle, SceneHandle scene);
    static Camera & get_scene_camera(SceneHandle handle);
    static TransformComponent &get_main_camera_transform(SceneHandle handle);
    static CameraComponent &get_main_camera_comp(SceneHandle handle);
    static EntityHandle get_main_camera_handle(SceneHandle handle);
    static void set_camera_for_render_pass(EntityHandle camera_entity, rendering::RenderPassHandle handle, SceneHandle scene_handle);
    static EntityHandle pick_entity(SceneHandle handle, i32 mouse_x, i32 mouse_y);
    static Entity& get_entity(EntityHandle handle, Scene &scene);
    static Entity& get_entity(EntityHandle handle, SceneHandle& scene_handle);
    static RegisteredEntityType * get_registered_type(u32 type_id, SceneManager *manager);
    static b32 has_light_component(EntityHandle entity_handle, SceneHandle& scene);
    static b32 has_particle_component(EntityHandle entity_handle, SceneHandle& scene);
    static b32 has_render_component(EntityHandle entity_handle, SceneHandle& scene);
    static b32 has_transform_component(EntityHandle entity_handle, SceneHandle& scene);
    static b32 has_camera_component(EntityHandle entity_handle, SceneHandle scene);
    static void add_child(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene);
    static b32 has_tag(const char* tag_name, EntityHandle entity_handle, SceneHandle scene_handle);
    static void set_entity_tag(const char *tag, EntityHandle entity_handle, SceneHandle scene_handle);
	static void deactivate_particle_systems(SceneHandle handle);

// @Deprecated
    static RenderComponent& _add_render_component(Scene &scene, EntityHandle entity_handle, b32 cast_shadows);
    static TransformComponent& _add_transform_component(Scene &scene, EntityHandle entity_handle);
    static ParticleSystemComponent& _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material);
    static LightComponent& _add_light_component(Scene &scene, EntityHandle entity_handle);
    static CameraComponent& _add_camera_component(EntityHandle entity_handle, Scene &scene);
    static CameraComponent & add_camera_component(EntityHandle entity_handle, SceneHandle handle);
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene, b32 savable);
    static void _unregister_entity(EntityHandle handle, Scene &scene);
    static EntityTemplate _load_template(const char *path, EntityTemplateState &template_state, Scene &scene);
    static EntityHandle _register_entity_from_template_file(const char *path, Scene &scene, b32 savable, Tags* tags);
    static b32 is_active(EntityHandle handle, SceneHandle scene_handle);
    static void _set_active(EntityHandle handle, b32 active, Scene &scene, b32 recursive);
    static TransformComponent& _get_transform_comp(EntityHandle handle, Scene &scene);
    static RenderComponent& _get_render_comp(EntityHandle handle, Scene &scene);
    static CameraComponent& _get_camera_comp(EntityHandle handle, Scene &scene);
    static ParticleSystemComponent& _get_particle_system_comp(EntityHandle handle, Scene &scene);
    static LightComponent &_get_light_comp(EntityHandle handle, Scene &scene);

    static i32 get_child_count(EntityHandle handle, SceneHandle scene);
    static EntityHandle get_child_handle(EntityHandle handle, i32 index, SceneHandle scene);
    
    i32 _unused_entity_handle(Scene &scene);
    i32 _pack_transform_components(Entity &entity, Scene &scene);
    i32 _pack_render_components(Entity &entity, Scene &scene);
    i32 _pack_particle_system_components(Entity &entity, Scene &scene);
    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene);
    static void select_entity(EntityHandle entity, SceneManager *manager);

    static void set_bounding_box_enabled(b32 enabled, EntityHandle entity_handle, SceneHandle &handle);
    static b32 get_bounding_box_enabled(EntityHandle entity_handle, SceneHandle &handle);

    // Transform
    static void set_position(TransformComponent& comp, math::Vec3 position);
    static void set_rotation(TransformComponent& comp, math::Vec3 rotation);
    static void set_scale(TransformComponent& comp, math::Vec3 scale);

    static void set_entity_selection_enabled(EntityHandle entity_handle, b32 enabled, SceneHandle scene_handle);
    static inline void set_entity_name(EntityHandle handle, const char *name, SceneHandle scene_handle);
    static inline void _set_entity_name(EntityHandle handle, const char *name, Scene& scene);
    static inline void _set_entity_template_path(EntityHandle handle, const char *template_path, Scene& scene);
    static inline void _set_entity_type(EntityHandle handle, u32 type, Scene &scene);

    // CAMERA
    static void update_cameras(Scene &scene, SceneManager *scene_manager);
    static void set_main_camera(EntityHandle entity_handle, SceneHandle scene_handle);
    static void set_camera_preview(EntityHandle entity_handle, SceneManager *scene_manager);
    static void stop_camera_preview(SceneManager *scene_manager);
}

#endif
