#ifndef PLATFORM_H
#define PLATFORM_H

#define NUM_ENTITIES 100

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0])) 

#if GLITCH_DEBUG
#define DEBUG_PRINT(format, ...) printf(format, __VA_ARGS__)
#else
#define DEBUG_PRINT(format, ...)
#endif

#if GLITCH_DEBUG
#define Assert(Expression) if(!(Expression)) {DEBUG_PRINT("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
#else
#define Assert(Expression)
#endif

#define Min(A,B) ((A < B) ? (A) : (B))
#define Max(A,B) ((A > B) ? (A) : (B))
#define Abs(x) ((x) < 0 ? -(x) : (x))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define PI 3.141592653589793f

#include <stdint.h>
#include <malloc.h>
#include <sys/types.h>  
#include <sys/stat.h>  

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using b32 = i32;

using r32 = float;
using r64 = double;

#include <GLFW/glfw3.h>
#include "math.h"

#include "gmap.h"
#include "gmap.cpp"
#include "keycontroller.h"
#include "rendering.h"

struct timer
{
    i32 TimerHandle = -1;
    r64 TimerMax = 0.0;
    char* Name;
};

struct entity_file_reload_data
{
    b32 ReloadPlayerFile;
    b32 ReloadSkeletonFile;
    b32 ReloadMinotaurFile;
    b32 ReloadWraithFile;
    b32 ReloadBonfireFile;
};

struct config_data
{
    char* Title;
    char* Version;
    char* StartingLevelFilePath;
    u32 ScreenWidth;
    u32 ScreenHeight;
    b32 Fullscreen;
    r32 Contrast;
    r32 Brightness;
    b32 Muted;
    r32 SFXVolume;
    r32 MusicVolume;
    r32 Zoom;
};

struct game_memory
{
    b32 IsInitialized;
    b32 ShouldReload;
    config_data ConfigData;
    
    u64 PermanentStorageSize;
    void* PermanentStorage;
};

struct input_controller;
struct sound_queue;
struct sound_effects;
struct render_state;

struct game_update_return
{
    math::v2 EntityPositions[NUM_ENTITIES];
    i32 EntityCount;
};

#define UPDATE(name)void name(r64 DeltaTime, game_memory* GameMemory, render_state& RenderState, renderer& Renderer, input_controller* InputController, sound_queue* SoundQueue, game_update_return* GameUpdateStruct, sound_effects* SoundEffects)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

inline void HandleError(char const *File, i32 LineNum, char const *msg)
{
    fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
    fprintf(stderr, "%s\n", msg);
}

#define ERR(msg) HandleError(__FILE__,__LINE__,msg)

struct CompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return strcmp(lhs, rhs) < 0;
    }
};

//Remember to free string after usage
inline char* Concat(const char *s1, const char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

inline b32 StartsWith(const char *A, const char *B)
{
    if(strncmp(A, B, strlen(B)) == 0) return 1;
    return 0;
}

inline void LoadConfig(const char* FilePath, config_data* ConfigData)
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
            else if(StartsWith(LineBuffer, "contrast"))
            {
                sscanf(LineBuffer, "contrast %f", &ConfigData->Contrast);
            }
            else if(StartsWith(LineBuffer, "brightness"))
            {
                sscanf(LineBuffer, "brightness %f", &ConfigData->Brightness);
            }
            else if(StartsWith(LineBuffer, "fullscreen"))
            {
                sscanf(LineBuffer, "fullscreen %d", &ConfigData->Fullscreen);
            } 
            else if(StartsWith(LineBuffer, "muted"))
            {
                sscanf(LineBuffer, "muted %d", &ConfigData->Muted);
            }
            else if(StartsWith(LineBuffer, "sfx_volume"))
            {
                sscanf(LineBuffer, "sfx_volume %f", &ConfigData->SFXVolume);
            }
            else if(StartsWith(LineBuffer, "music_volume"))
            {
                sscanf(LineBuffer, "music_volume %f", &ConfigData->MusicVolume);
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
    i32 FilesLength = 0;
};

inline void FindFilesWithExtensions(const char* DirectoryPath, const char* Extension, directory_data* DirectoryData, b32 WithSubDirectories = false)
{
    if(DirectoryData->FilesLength == 0)
    {
        DirectoryData->FileNames = (char**)malloc(128 * sizeof(char*));
        DirectoryData->FilePaths = (char**)malloc(128 * sizeof(char*));
    }
    
    WIN32_FIND_DATA FindFile;
    HANDLE hFind = NULL;
    
    char Path[2048];
    
    sprintf(Path, "%s\\*.%s", DirectoryPath, Extension);
    
    if((hFind = FindFirstFile(Path, &FindFile)) == INVALID_HANDLE_VALUE)
    {
        DEBUG_PRINT("Path not found: %s\n", DirectoryPath);
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

inline void DebugPrintVec2(math::v2 Vec2, const char* Msg = "")
{
    DEBUG_PRINT(Concat(Msg, " (%f,%f)\n"),Vec2.x,Vec2.y);
}

inline b32 FileExists(char* FilePath)
{
    struct stat Buffer;
    return (stat(FilePath,&Buffer) == 0);
}


#endif