#ifndef PROJECT_H
#define PROJECT_H

namespace project
{
    struct ProjectSettings
    {
        char *name;
        char *lib_file_name;
        char *project_root_path;
        char *resources_folder_path;
        char *startup_scene_path;
    
        MemoryArena arena;
    };

    struct ProjectState
    {
        ProjectSettings project_settings;

        struct
        {
            FileList *resource_file_structures;
            MemoryArena *arenas;
            i32 structure_count;

            MemoryArena file_structure_arena;
        } resources;

        b32 startup_scene_loaded;
    };

// Loads a project file (.gpj format)
    static b32 load_project(ProjectState *project_state);
}

#endif
