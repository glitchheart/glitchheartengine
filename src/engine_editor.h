#ifndef EDITOR_H
#define EDITOR_H

namespace editor
{
    enum class WindowType
    {
        HIERARCHY,
        INSPECTOR,
        RESOURCES,
        SCENE_SETTINGS
    };

    enum class EditorMode
    {
        BUILT_IN,
        CUSTOM
    };

    typedef void (*OnCustomMainMenu)();
    
    struct EditorState
    {
        EditorMode mode;
        
        struct
        {
            b32 show_hierarchy;
            b32 show_inspector;
            b32 show_resources;
            b32 show_scene_settings;
            b32 show_stats;

            struct
            {
                b32 show_shadow_map;
            } scene_settings;
            
        } windows;

        struct
        {
            OnCustomMainMenu on_custom_main_menu_bar;
        } callbacks;
    };


    // Internal functions
    static void _init(EditorState *editor_state);
    static void _update_engine_editor(InputController *input_controller, scene::SceneManager *scene_manager);
    static void _recursive_entity_item(scene::Entity& entity, scene::SceneManager *scene_manager, r64 delta_time);
    static void _render_hierarchy(scene::Scene &scene, EditorState *editor_state, InputController *input_controller, r64 delta_time);
    static void _render_inspector(scene::Scene &scene, EditorState *editor_state, InputController *input_controller, r64 delta_time);
    static void _render_resources(project::ProjectState *project_state, EditorState *editor_state, scene::SceneManager *scene_manager, r64 delta_time);
    static void _render_stats(EditorState *editor_state, scene::SceneManager *scene_manager, InputController *input_controller, sound::SoundSystem *sound_system, r64 delta_time);

    static void set_editor_mode(EditorMode mode, EditorState *state);
}

#endif
