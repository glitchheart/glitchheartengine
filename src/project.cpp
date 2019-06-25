static b32 load_project(char *project_file_path, ProjectState *project_state, ProjectSettings *settings_ptr)
{
    if(!settings_ptr)
    {
        debug("ProjectSettings ptr is null");
        return false;
    }
    
    settings_ptr->name = push_string(&settings_ptr->arena, 32);
    settings_ptr->lib_file_name = push_string(&settings_ptr->arena, 32);
    settings_ptr->project_root_path = push_string(&settings_ptr->arena, 64);
    settings_ptr->resources_folder_path = push_string(&settings_ptr->arena, 32);
    
    // Start by getting the project's root folder from the project file path
    // This can later be used for loading any 
    strcpy(settings_ptr->project_root_path, project_file_path);

    for(i32 i = (i32)strlen(settings_ptr->project_root_path); i >= 0; i--)
    {
        if(settings_ptr->project_root_path[i] == '/')
        {
            settings_ptr->project_root_path[i] = '\0';
            break;
        }
    }
    
    FILE *file = fopen(project_file_path, "r");

    if(file)
    {
        char buffer[256];
        while(fgets(buffer, 256, file))
        {
            if(starts_with(buffer, "project_name"))
            {
                sscanf(buffer, "project_name: %[^\n]", settings_ptr->name);
            }
            else if(starts_with(buffer, "lib_name"))
            {
                sscanf(buffer, "lib_name: %[^\n]", settings_ptr->lib_file_name);
            }
            else if(starts_with(buffer, "resources_folder"))
            {
                char folder[32];
                sscanf(buffer, "resources_folder: %[^\n]", folder);

                sprintf(settings_ptr->resources_folder_path, "%s/%s/", settings_ptr->project_root_path, folder);

                printf("Resources path: %s\n", settings_ptr->resources_folder_path);

                game_state->resources.resource_file_structures = push_array(&game_state->resources.file_structure_arena, 8, FileList);

                FileList list = {};
                strcpy(list.path, settings_ptr->resources_folder_path);

                list.files = push_array(&game_state->resources.file_structure_arena, 48, File);
                list.dirs = push_array(&game_state->resources.file_structure_arena, 48, File);
                platform.list_all_files_and_directories(settings_ptr->resources_folder_path, &list);
                game_state->resources.resource_file_structures[game_state->resources.structure_count++] = list;
            }
        }
        
        fclose(file);
    }

    return true;
}
