

struct CompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return std::strcmp(lhs, rhs) < 0;
    }
};

struct config_data
{
    char* Title;
    char* Version;
    uint32 ScreenWidth;
    uint32 ScreenHeight;
    bool32 Fullscreen;
};

static void LoadConfig(const char* FilePath, config_data* ConfigData)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    ConfigData->Title = (char*)malloc(sizeof(char) * 30);
    ConfigData->Version = (char*)malloc(sizeof(char) * 30);
    
    if(File)
    {
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "title %s", ConfigData->Title);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "version %s", ConfigData->Version);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "screen_width %d", &ConfigData->ScreenWidth);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "fullscreen %d", &ConfigData->Fullscreen);
        }
        fclose(File);
    }
    
}



