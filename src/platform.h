#ifndef PLATFORM_H
#define PLATFORM_H

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0])) 

#ifdef POWDER_DEBUG
#define Assert(Expression) if(!(Expression)) {printf("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
#else
#define Assert(Expression)
#endif

#define NUM_JOYSTICK_KEYS Joystick_Count
#define NUM_KEYS Key_Count
#define NUM_MOUSE_BUTTONS Mouse_Count

#define Min(A,B) ((A < B) ? (A) : (B))
#define Max(A,B) ((A > B) ? (A) : (B))
#define Abs(x) ((x) < 0 ? -(x) : (x))

#include "stdint.h"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef float real32;
typedef double real64;

struct timer
{
    int32 TimerHandle = -1;
    real64 TimerMax;
};

struct v2i
{
    int32 X;
    int32 Y;
};

struct entity_file_reload_data
{
    bool32 ReloadPlayerFile;
    bool32 ReloadSkeletonFile;
    bool32 ReloadMinotaurFile;
    bool32 ReloadWraithFile;
};

void HandleError(char const *File, int32 LineNum, char const *msg)
{
    fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
    fprintf(stderr, "%s\n", msg);
}

struct CompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return strcmp(lhs, rhs) < 0;
    }
};

char* Concat(const char *s1, const char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

struct config_data
{
    char* Title;
    char* Version;
    char* StartingLevelFilePath;
    uint32 ScreenWidth;
    uint32 ScreenHeight;
    bool32 Fullscreen;
    bool32 Muted;
    real32 Zoom;
};


bool32 StartsWith(const char *A, const char *B)
{
    if(strncmp(A, B, strlen(B)) == 0) return 1;
    return 0;
}

void LoadConfig(const char* FilePath, config_data* ConfigData)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    ConfigData->Title = (char*)malloc(sizeof(char) * 40);
    ConfigData->Version = (char*)malloc(sizeof(char) * 40);
    
    if(File)
    {
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "title"))
            {
                sscanf(LineBuffer, "title %s", ConfigData->Title);
            }
            else if(StartsWith(LineBuffer, "version"))
            {
                sscanf(LineBuffer, "version %s", ConfigData->Version);
            }
            else if(StartsWith(LineBuffer, "screen_width"))
            {
                sscanf(LineBuffer, "screen_width %d", &ConfigData->ScreenWidth);
            }
            else if(StartsWith(LineBuffer, "screen_height"))
            {
                sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
            }
            else if(StartsWith(LineBuffer, "screen_height"))
            {
                sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
            }
            else if(StartsWith(LineBuffer, "fullscreen"))
            {
                sscanf(LineBuffer, "fullscreen %d", &ConfigData->Fullscreen);
                ConfigData->Fullscreen = false;
            } 
            else if(StartsWith(LineBuffer, "muted"))
            {
                sscanf(LineBuffer, "muted %d", &ConfigData->Muted);
            }
            else if(StartsWith(LineBuffer, "muted"))
            {
                sscanf(LineBuffer, "muted %d", &ConfigData->Muted);
            }
            else if(StartsWith(LineBuffer, "starting_level_path"))
            {
                ConfigData->StartingLevelFilePath = (char*)malloc(40 * sizeof(char));
                sscanf(LineBuffer, "starting_level_path %s", ConfigData->StartingLevelFilePath);
            }
            else if(StartsWith(LineBuffer, "zoom"))
            {
                sscanf(LineBuffer, "zoom %f", &ConfigData->Zoom);
            }
        }
        
        fclose(File);
    }
}

struct directory_data
{
    char** FilePaths;
    char** FileNames;
    int32 FilesLength = 0;
};

void FindFilesWithExtensions(const char* DirectoryPath, const char* Extension, directory_data* DirectoryData, bool32 WithSubDirectories = false)
{
    if(DirectoryData->FilesLength == 0)
    {
        DirectoryData->FileNames = (char**)malloc(70 * sizeof(char*));
        DirectoryData->FilePaths = (char**)malloc(70 * sizeof(char*));
    }
    
    WIN32_FIND_DATA FindFile;
    HANDLE hFind = NULL;
    
    char Path[2048];
    
    sprintf(Path, "%s\\*.%s", DirectoryPath, Extension);
    
    if((hFind = FindFirstFile(Path, &FindFile)) == INVALID_HANDLE_VALUE)
    {
        printf("Path not found: %s\n", DirectoryPath);
        return;
    }
    
    if(FindFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
    {
        if(WithSubDirectories)
            FindFilesWithExtensions(Concat(DirectoryPath, FindFile.cFileName), Extension, DirectoryData, true);
    }
    else
    {
        char* ConcatStr = Concat(DirectoryPath, FindFile.cFileName);
        char* FileName = strtok(FindFile.cFileName, ".");
        
        DirectoryData->FilePaths[DirectoryData->FilesLength] = (char*)malloc((strlen(ConcatStr) + 1) * sizeof(char));
        DirectoryData->FileNames[DirectoryData->FilesLength] = (char*)malloc((strlen(FileName) + 1) * sizeof(char));
        strcpy(DirectoryData->FilePaths[DirectoryData->FilesLength], ConcatStr);
        strcpy(DirectoryData->FileNames[DirectoryData->FilesLength], FileName);
        DirectoryData->FilesLength++;
    }
    
    while(FindNextFile(hFind, &FindFile))
    {
        if(strcmp(FindFile.cFileName, ".") != 0
           && strcmp(FindFile.cFileName, "..") != 0)
        {
            if(FindFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY)
            {
                if(WithSubDirectories)
                    FindFilesWithExtensions(Path, Extension, DirectoryData, true);
            }
            else
            {
                char* ConcatStr = Concat(DirectoryPath, FindFile.cFileName);
                char* FileName = strtok(FindFile.cFileName, ".");
                
                DirectoryData->FilePaths[DirectoryData->FilesLength] = (char*)malloc((strlen(ConcatStr) + 1) * sizeof(char));
                DirectoryData->FileNames[DirectoryData->FilesLength] = (char*)malloc((strlen(FileName)  + 1) * sizeof(char));
                strcpy(DirectoryData->FilePaths[DirectoryData->FilesLength], ConcatStr);
                strcpy(DirectoryData->FileNames[DirectoryData->FilesLength], FileName);
                DirectoryData->FilesLength++;
            }
        }
    }
}

#endif