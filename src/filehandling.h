#include <Windows.h>

static GLchar* LoadShaderFromFile(const std::string Path)
{
    GLchar *Source = {};
    
    FILE *File;
    File = fopen(Path.c_str(), "rb");
    if(File)
    {
        fseek(File, 0, SEEK_END);
        uint32 Size = ftell(File);
        fseek(File, 0, SEEK_SET);
        
        Source = (GLchar *)malloc(Size+1);
        fread(Source, Size, 1, File); 
        Source[Size] = '\0';
        
        fclose(File);
    }
    else
    {
        std::cerr << "Could not read file " << Path << ". File does not exist." << std::endl;
    }	
    
    return Source;
}

struct asset_manager
{
    bool ListenForChanges;
    
    //shaders
    uint32 DirtyVertexShaderIndices[Shader_Count]; //2 is the number of shaders, set to 1 if they should be reloaded
    uint32 DirtyFragmentShaderIndices[Shader_Count];
    time_t VertexShaderTimes[Shader_Count];
    time_t FragmentShaderTimes[Shader_Count];
    
    //textures
    const char* TilesetTexturePath = "'../assets/textures/tiles.png";
    uint32 DirtyTileset;
    time_t TilesetTime;
    
    //dll's
    const char* DllPaths[1] = { "game.dll" };
    uint32 DirtyGameDll;
    FILETIME GameDllTime;
};

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

static void CheckDirty(const char* FilePath, time_t LastTime, uint32* DirtyId, time_t* Time)
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

static char* CombineStrings(const char * str1, const char* str2)
{
    char * str3 = (char *) malloc(1 + strlen(str1) + strlen(str2));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

static void StartupFileTimeChecks(asset_manager* AssetManager)
{
    struct stat sb;
    stat(AssetManager->TilesetTexturePath, &sb);
    AssetManager->TilesetTime =  sb.st_mtime;
    
    for (int i = 0; i < Shader_Count; i++) 
    {
        struct stat sb1;
        stat(CombineStrings(ShaderPaths[i], ".vert"), &sb1);
        AssetManager->VertexShaderTimes[i] =  sb1.st_mtime;
        
        struct stat sb2;
        stat(CombineStrings(ShaderPaths[i], ".frag"), &sb2);
        AssetManager->FragmentShaderTimes[i] =  sb2.st_mtime;
    }
}

static void ListenToFileChanges(asset_manager* AssetManager)
{
    AssetManager->ListenForChanges = true;
    
    using namespace std::chrono_literals;
    
    while (AssetManager->ListenForChanges) 
    {
        for (int i = 0; i < Shader_Count; i++)
        {
            CheckDirty(CombineStrings(ShaderPaths[i], ".vert"), AssetManager->VertexShaderTimes[i], &AssetManager->DirtyVertexShaderIndices[i], &AssetManager->VertexShaderTimes[i]);
            CheckDirty(CombineStrings(ShaderPaths[i], ".frag"), AssetManager->FragmentShaderTimes[i], &AssetManager->DirtyFragmentShaderIndices[i], &AssetManager->FragmentShaderTimes[i]);
        }
        
        CheckDirty(AssetManager->TilesetTexturePath, AssetManager->TilesetTime, &AssetManager->DirtyTileset, &AssetManager->TilesetTime);	
        
        std::this_thread::sleep_for(0.5s);
    }
}