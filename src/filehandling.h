#ifndef FILEHANDLING_H
#define FILEHANDLING_H

struct asset_manager
{
    bool ListenForChanges;
    
    //shaders
    b32 DirtyVertexShaderIndices[Shader_Count]; //set to 1 if they should be reloaded
    b32 DirtyFragmentShaderIndices[Shader_Count];
    time_t VertexShaderTimes[Shader_Count];
    time_t FragmentShaderTimes[Shader_Count];
    
    //textures
    const char* TilesetTexturePath = "'../assets/textures/tiles.png";
    b32 DirtyTileset;
    time_t TilesetTime;
    
    //dll's
    const char* DllPaths[1] = { "game.dll" };
    u32 DirtyGameDll;
    FILETIME GameDllTime;
    
    entity_file_reload_data ReloadData;
    time_t PlayerFileTime;
    time_t SkeletonFileTime;
    time_t MinotaurFileTime;
};

static GLchar* LoadShaderFromFile(const char* Path)
{
    GLchar *Source = {};
    
    FILE *File;
    File = fopen(Path, "rb");
    if(File)
    {
        fseek(File, 0, SEEK_END);
        u32 Size = ftell(File);
        fseek(File, 0, SEEK_SET);
        
        Source = (GLchar *)malloc(Size+1);
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

static FILETIME GetLastWriteTime(const char* FilePath)
{
    FILETIME LastWriteTime = {};
    
    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(FilePath, &FindData);
    
    if(FindHandle != INVALID_HANDLE_VALUE)
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }
    return LastWriteTime;
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

static void StartupFileTimeChecks(asset_manager* AssetManager)
{
    struct stat sb;
    stat(AssetManager->TilesetTexturePath, &sb);
    AssetManager->TilesetTime =  sb.st_mtime;
    
    stat("../assets/entities/player.dat", &sb);
    AssetManager->PlayerFileTime = sb.st_mtime;
    
    stat("../assets/entities/skeleton.dat", &sb);
    AssetManager->SkeletonFileTime = sb.st_mtime;
    
    stat("../assets/entities/minotaur.dat", &sb);
    AssetManager->MinotaurFileTime = sb.st_mtime;
    
    for (int i = 0; i < Shader_Count; i++) 
    {
        struct stat sb1;
        stat(Concat(ShaderPaths[i], ".vert"), &sb1);
        AssetManager->VertexShaderTimes[i] =  sb1.st_mtime;
        
        struct stat sb2;
        stat(Concat(ShaderPaths[i], ".frag"), &sb2);
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
            CheckDirty(Concat(ShaderPaths[i], ".vert"), AssetManager->VertexShaderTimes[i], &AssetManager->DirtyVertexShaderIndices[i], &AssetManager->VertexShaderTimes[i]);
            CheckDirty(Concat(ShaderPaths[i], ".frag"), AssetManager->FragmentShaderTimes[i], &AssetManager->DirtyFragmentShaderIndices[i], &AssetManager->FragmentShaderTimes[i]);
        }
        
        CheckDirty(AssetManager->TilesetTexturePath, AssetManager->TilesetTime, &AssetManager->DirtyTileset, &AssetManager->TilesetTime);
        
        CheckDirty("../assets/entities/player.dat", AssetManager->PlayerFileTime, &AssetManager->ReloadData.ReloadPlayerFile, &AssetManager->PlayerFileTime);
        
        CheckDirty("../assets/entities/skeleton.dat", AssetManager->SkeletonFileTime, &AssetManager->ReloadData.ReloadSkeletonFile, &AssetManager->SkeletonFileTime);
        
        CheckDirty("../assets/entities/minotaur.dat", AssetManager->MinotaurFileTime, &AssetManager->ReloadData.ReloadMinotaurFile, &AssetManager->MinotaurFileTime);
        
    }
}

#endif