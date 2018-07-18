#ifndef FILEHANDLING_H
#define FILEHANDLING_H

struct AssetManager
{
    b32 is_initialized;
    b32 listen_for_changes;
    
    MemoryArena arena;
    
    //shaders
    b32 dirty_vertex_shader_indices[SHADER_COUNT]; //set to 1 if they should be reloaded
    b32 dirty_fragment_shader_indices[SHADER_COUNT];
    b32 dirty_geometry_shader_indices[SHADER_COUNT];
    time_t vertex_shader_times[SHADER_COUNT];
    time_t fragment_shader_times[SHADER_COUNT];
    time_t geometry_shader_times[SHADER_COUNT];
    
    //libs
    char* lib_paths[1];
    u32 dirty_game_lib;
};

static GLchar* load_shader_from_file(const char* path, MemoryArena* arena)
{
    GLchar *source = {};
    
    PlatformFile f = platform.open_file(path, POF_READ | POF_OPEN_EXISTING);
    
    if(f.handle)
    {
        platform.seek_file(f, 0, SO_END);
        i32 size = platform.tell_file(f);
        platform.seek_file(f, 0, SO_SET);
        
        if(size == -1)
            return source;
        
        source = push_size(arena, size + 1, GLchar);
        
        platform.read_file(source, size, 1, f); 
        source[size] = '\0';
        
        platform.close_file(f);
    }
    else
    {
        fprintf(stderr, "Could not read file %s. File does not exist.\n",path);
    }
    
    return source;
}

static void check_dirty(const char* file_path, time_t last_time, b32* dirty_id, time_t* time)
{
    struct stat sb;
    stat(file_path, &sb);
    
    time_t time_new = sb.st_mtime;
    
    if (last_time != 0 && last_time < time_new)
    {
        *dirty_id = 1;
    }
    
    *time = time_new;
}

static void startup_file_time_checks(MemoryArena* arena, AssetManager* asset_manager, char* lib_path)
{
    auto temp_mem = begin_temporary_memory(arena);
    
    for (int i = 0; i < SHADER_COUNT; i++) 
    {
        struct stat sb1;
        auto concated_vertex_shader_string = concat(shader_paths[i], ".vert", arena);
        stat(concated_vertex_shader_string, &sb1);
        asset_manager->vertex_shader_times[i] =  sb1.st_mtime;
        
        struct stat sb2;
        auto concated_fragment_shader_string = concat(shader_paths[i], ".frag", arena);
        stat(concated_fragment_shader_string, &sb2);
        asset_manager->fragment_shader_times[i] =  sb2.st_mtime;
        
        struct stat sb3;
        auto concated_geometry_shader_string = concat(shader_paths[i], ".geom", arena);
        stat(concated_geometry_shader_string, &sb3);
        asset_manager->geometry_shader_times[i] =  sb3.st_mtime;
    }
    end_temporary_memory(temp_mem);
}

static void listen_to_file_changes(MemoryArena* arena, AssetManager* asset_manager)
{
    asset_manager->listen_for_changes = true;
    
    auto temp_mem = begin_temporary_memory(arena);
    if(asset_manager->listen_for_changes) 
    {
        for (int i = 0; i < SHADER_COUNT; i++)
        {
            
            char* vertex_path = concat(shader_paths[i], ".vert", arena);
            char* fragment_path = concat(shader_paths[i], ".frag", arena);
            char* geometry_path = concat(shader_paths[i], ".geom", arena);
            
            check_dirty(vertex_path, asset_manager->vertex_shader_times[i], &asset_manager->dirty_vertex_shader_indices[i], &asset_manager->vertex_shader_times[i]);
            check_dirty(fragment_path, asset_manager->fragment_shader_times[i], &asset_manager->dirty_fragment_shader_indices[i], &asset_manager->fragment_shader_times[i]);
            check_dirty(geometry_path, asset_manager->geometry_shader_times[i], &asset_manager->dirty_geometry_shader_indices[i], &asset_manager->geometry_shader_times[i]);
            
        }
    }
    end_temporary_memory(temp_mem);
}

#endif
