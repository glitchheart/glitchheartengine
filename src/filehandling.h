#ifndef FILEHANDLING_H
#define FILEHANDLING_H

struct asset_manager
{
    b32 IsInitialized;
    b32 ListenForChanges;
    
    memory_arena Arena;
    
    //shaders
    b32 DirtyVertexShaderIndices[Shader_Count]; //set to 1 if they should be reloaded
    b32 DirtyFragmentShaderIndices[Shader_Count];
    time_t VertexShaderTimes[Shader_Count];
    time_t FragmentShaderTimes[Shader_Count];
    
    //textures
    char* TilesetTexturePath;
    b32 DirtyTileset;
    time_t TilesetTime;
    
    //libs
    char* LibPaths[1];
    u32 DirtyGameLib;
};

static GLchar* LoadShaderFromFile(const char* Path, memory_arena* Arena)
{
    GLchar *Source = {};
    
    FILE *File;
    File = fopen(Path, "rb");
    
    if(File)
    {
        fseek(File, 0, SEEK_END);
        u32 Size = (u32)ftell(File);
        fseek(File, 0, SEEK_SET);
        
        Source = PushSize(Arena, Size + 1, GLchar);
        
        fread(Source, Size, 1, File); 
        Source[Size] = '\0';
        
        fclose(File);
    }
    else
    {
        fprintf(stderr, "Could not read file %s. File does not exist.\n",Path);
    }	
    
    return Source;
}



static void CheckDirty(const char* FilePath, time_t LastTime, b32* DirtyId, time_t* Time)
{
    struct stat sb;
    stat(FilePath, &sb);
    
    time_t time = sb.st_mtime;
    
    if (LastTime != 0 && LastTime < time)
    {
        *DirtyId = 1;
    }
    *Time = time;
}

static void StartupFileTimeChecks(asset_manager* AssetManager, char* LibPath)
{
    if(!AssetManager->IsInitialized)
    {
        AssetManager->TilesetTexturePath = "../assets/textures/tiles.png";
        AssetManager->LibPaths[0] = PushString(&AssetManager->Arena, LibPath);
        AssetManager->IsInitialized = true;
    }
    
    for (int i = 0; i < Shader_Count; i++) 
    {
        struct stat sb1;
        auto ConcatedVertexShaderString = Concat(ShaderPaths[i], ".vert");
        stat(ConcatedVertexShaderString, &sb1);
        AssetManager->VertexShaderTimes[i] =  sb1.st_mtime;
        
        struct stat sb2;
        auto ConcatedFragmentShaderString = Concat(ShaderPaths[i], ".frag");
        stat(ConcatedFragmentShaderString, &sb2);
        AssetManager->FragmentShaderTimes[i] =  sb2.st_mtime;
    }
}

static void ListenToFileChanges(asset_manager* AssetManager)
{
    AssetManager->ListenForChanges = true;
    
    if(AssetManager->ListenForChanges) 
    {
        for (int i = 0; i < Shader_Count; i++)
        {
            char* VertexPath = Concat(ShaderPaths[i], ".vert");
            char* FragmentPath = Concat(ShaderPaths[i], ".frag");
            
            CheckDirty(VertexPath, AssetManager->VertexShaderTimes[i], &AssetManager->DirtyVertexShaderIndices[i], &AssetManager->VertexShaderTimes[i]);
            CheckDirty(FragmentPath, AssetManager->FragmentShaderTimes[i], &AssetManager->DirtyFragmentShaderIndices[i], &AssetManager->FragmentShaderTimes[i]);
            
        }
    }
}



#endif
