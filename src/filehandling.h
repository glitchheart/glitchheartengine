#ifndef FILEHANDLING_H
#define FILEHANDLING_H

struct AssetManager
{
    b32 is_initialized;
    b32 listen_for_changes;
    
    MemoryArena arena;
    
    //shaders
    b32 dirty_vertex_shader_indices[Shader_Count]; //set to 1 if they should be reloaded
    b32 dirty_fragment_shader_indices[Shader_Count];
    time_t vertex_shader_times[Shader_Count];
    time_t fragment_shader_times[Shader_Count];
    
    //textures
    char* tileset_texture_path;
    b32 dirty_tileset;
    time_t tileset_time;
    
    //libs
    char* lib_paths[1];
    u32 dirty_game_lib;
};

static GLchar* load_shader_from_file(const char* path, MemoryArena* Arena)
{
    GLchar *source = {};
    
    FILE *file;
    file = fopen(path, "rb");
    
    if(file)
    {
        fseek(file, 0, SEEK_END);
        u32 Size = (u32)ftell(file);
        fseek(file, 0, SEEK_SET);
        
        source = PushSize(Arena, Size + 1, GLchar);
        
        fread(source, Size, 1, file); 
        source[Size] = '\0';
        
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
    
    time_t time = sb.st_mtime;
    
    if (last_time != 0 && last_time < time)
    {
        *dirty_id = 1;
    }
    *time = time;
}

static void startup_file_time_checks(AssetManager* asset_manager, char* lib_path)
{
    if(!asset_manager->IsInitialized)
    {
        asset_manager->TilesetTexturePath = "../assets/textures/tiles.png";
        asset_manager->LibPaths[0] = push_string(&asset_manager->Arena, lib_path);
        asset_manager->IsInitialized = true;
    }
    
    for (int i = 0; i < Shader_Count; i++) 
    {
        struct stat sb1;
        auto concated_vertex_shader_string = concat(ShaderPaths[i], ".vert");
        stat(concated_vertex_shader_string, &sb1);
        asset_manager->VertexShaderTimes[i] =  sb1.st_mtime;
        
        struct stat sb2;
        auto concated_fragment_shader_string = concat(ShaderPaths[i], ".frag");
        stat(concated_fragment_shader_string, &sb2);
        asset_manager->FragmentShaderTimes[i] =  sb2.st_mtime;
    }
}

static void listen_to_file_changes(AssetManager* asset_manager)
{
    asset_manager->ListenForChanges = true;
    
    if(asset_manager->ListenForChanges) 
    {
        for (int i = 0; i < Shader_Count; i++)
        {
            char* vertex_path = concat(ShaderPaths[i], ".vert");
            char* fragment_path = concat(ShaderPaths[i], ".frag");
            
            check_dirty(vertex_path, asset_manager->VertexShaderTimes[i], &asset_manager->DirtyVertexShaderIndices[i], &asset_manager->VertexShaderTimes[i]);
            check_dirty(fragment_path, asset_manager->FragmentShaderTimes[i], &asset_manager->DirtyFragmentShaderIndices[i], &asset_manager->FragmentShaderTimes[i]);
            
        }
    }
}

#endif
