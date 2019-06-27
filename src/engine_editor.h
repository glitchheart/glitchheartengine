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
    
    struct EditorState
    {
        EditorMode mode;
        
        struct
        {
            b32 show_hierarchy;
            b32 show_inspector;
            b32 show_resources;
            b32 show_scene_settings;
        } windows;
    };


    // Internal functions
    static void _recursive_entity_item(scene::Entity& entity, scene::SceneManager *scene_manager);
    static void _render_hierarchy();
    static void _render_inspector();
    static void _render_resources(project::ProjectState *project_state, scene::SceneManager *scene_manager);

    static void set_editor_mode(EditorMode mode, EditorState *state);
}

#endif
