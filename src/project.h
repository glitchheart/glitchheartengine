#ifndef PROJECT_H
#define PROJECT_H

struct ProjectSettings
{
    char *name;
    char *lib_file_name;
    char *project_root_path;
    char *resources_folder_path;
    
    MemoryArena arena;
};

struct ProjectState
{
    ProjectSettings project_settings;

    struct
    {
        FileList *resource_file_structures;
        i32 structure_count;
        MemoryArena file_structure_arena;
    } resources;
};

// Loads a project file (.gpj format)
static b32 load_project(char *project_file_path, GameState *game_state, ProjectSettings *settings_ptr);

#endif
