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
    time_t vertex_shader_times[SHADER_COUNT];
    time_t fragment_shader_times[SHADER_COUNT];
    
    //textures
    char* tileset_texture_path;
    b32 dirty_tileset;
    time_t tileset_time;
    
    //libs
    char* lib_paths[1];
    u32 dirty_game_lib;
};

static GLchar* load_shader_from_file(const char* path, MemoryArena* arena)
{
    GLchar *source = {};
    
    FILE *file;
    file = fopen(path, "rb");
    
    if(file)
    {
        fseek(file, 0, SEEK_END);
        u32 size = (u32)ftell(file);
        fseek(file, 0, SEEK_SET);
        
        source = push_size(arena, size + 1, GLchar);
        
        fread(source, size, 1, file); 
        source[size] = '\0';
        
        fclose(file);
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
    if(!asset_manager->is_initialized)
    {
        asset_manager->tileset_texture_path = "../assets/textures/tiles.png";
        asset_manager->lib_paths[0] = push_string(&asset_manager->arena, lib_path);
        asset_manager->is_initialized = true;
    }
    
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
            
            check_dirty(vertex_path, asset_manager->vertex_shader_times[i], &asset_manager->dirty_vertex_shader_indices[i], &asset_manager->vertex_shader_times[i]);
            check_dirty(fragment_path, asset_manager->fragment_shader_times[i], &asset_manager->dirty_fragment_shader_indices[i], &asset_manager->fragment_shader_times[i]);
            
        }
    }
    end_temporary_memory(temp_mem);
}

#endif
