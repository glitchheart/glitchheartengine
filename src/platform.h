#ifndef PLATFORM_H
#define PLATFORM_H

#define NUM_ENTITIES 100

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0])) 

#define COMMA_IF_PARENS(...) ,

#if GLITCH_DEBUG
#define DEBUG_PRINT(format, ...) printf(format , ## __VA_ARGS__)
#else
#define DEBUG_PRINT(format, ...)
#endif

#if GLITCH_DEBUG
#define DEBUG_PRINT_V3(vec, ...) printf(## __VA_ARGS__ "" "(%f, %f, %f)\n", vec.x, vec.y, vec.z)
#else
#define DEBUG_PRINT_V3(vec, ...)
#endif

#if GLITCH_DEBUG
#ifdef _WIN32
#define Assert(Expression) if(!(Expression)) {DEBUG_PRINT("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); __debugbreak();}
#elif __linux
#define Assert(Expression) if(!(Expression)) {DEBUG_PRINT("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); abort();}
#endif
#else
#define Assert(Expression)
#endif


#if GLITCH_DEBUG
#define Static_Assert(Expression)  int i = 1/(i32)Expression
#else
#define Static_Assert(Expression)
#endif


#define Min(A,B) ((A < B) ? (A) : (B))
#define Max(A,B) ((A > B) ? (A) : (B))
#define Abs(x) ((x) < 0 ? -(x) : (x))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define AlignPow2(Value, Alignment) ((Value + ((Alignment) - 1)) & ~((Alignment) - 1))

#define PI 3.141592653589793f
#define DEGREE_IN_RADIANS 0.0174532925f

#define OffsetOf(type, Member) (umm)&(((type *)0)->Member)


#include <cstdio>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <stdint.h>


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

using umm = uintptr_t; // Casey uses this for sizes (why?)
using imm = intptr_t;


struct texture_data;

inline char* ToString(i32 I);
inline char* ToString(r64 R);
inline char* ToString(r32 R);
char* ToString(texture_data* Data);

#include "log_state.h"
#include "engine_math.h"
#include "modelformat.h"

struct timer
{
    i32 TimerHandle = -1;
    r64 TimerMax = 0.0;
    char* Name;
};


enum Window_Mode
{
    FM_Windowed = 0,
    FM_Full = 1,
    FM_Borderless = 2
    
};


struct config_data
{
    char* Title;
    char* Version;
    char* StartingLevelFilePath;
    i32 ScreenWidth;
    i32 ScreenHeight;
    i32 ScaleFromWidth;
    i32 ScaleFromHeight;
    Window_Mode Fullscreen;
    r32 Contrast;
    r32 Brightness;
    b32 Muted;
    r32 SFXVolume;
    r32 MusicVolume;
    r32 Zoom;
    b32 SkipSplashScreen;
};

struct directory_data
{
    char** FilePaths;
    char** FileNames;
    i32 FilesLength = 0;
};

enum platform_memory_block_flags
{
    PM_OverflowCheck =  (1 << 0),
    PM_UnderflowCheck = (1 << 1),
    PM_Temporary =      (1 << 2)
};

struct platform_memory_block
{
    u64 Flags;
    u64 Size;
    u8* Base;
    umm Used;
    platform_memory_block* Prev;
};

enum platform_file_flags
{
    PM_Append = (1 << 0)
};

struct platform_file
{
    FILE* File;
    char Path[260];
    char Extension[16];
};

#define PLATFORM_GET_ALL_FILES_WITH_EXTENSION(name) void name(const char* DirectoryPath, const char* Extension, directory_data* DirectoryData, b32 WithSubDirectories)
typedef PLATFORM_GET_ALL_FILES_WITH_EXTENSION(platform_get_all_files_with_extension);

#define PLATFORM_FILE_EXISTS(name) b32 name(const char* FilePath)
typedef PLATFORM_FILE_EXISTS(platform_file_exists);

#define PLATFORM_ALLOCATE_MEMORY(name) platform_memory_block* name(umm Size, u64 Flags)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(platform_memory_block* Block)
typedef PLATFORM_DEALLOCATE_MEMORY(platform_deallocate_memory);

#define PLATFORM_OPEN_FILE_WITH_DIALOG(name) platform_file name(char* Extension)
typedef PLATFORM_OPEN_FILE_WITH_DIALOG(platform_open_file_with_dialog);

#define PLATFORM_SAVE_FILE_WITH_DIALOG(name) platform_file name(char* Extension, u64 Flags)
typedef PLATFORM_SAVE_FILE_WITH_DIALOG(platform_save_file_with_dialog);

#define PLATFORM_GET_TIME_OF_DAY(name) u32 name()
typedef PLATFORM_GET_TIME_OF_DAY(platform_get_time_of_day);

#define PLATFORM_LOAD_LIBRARY(name) void* name(const char* Path)
typedef PLATFORM_LOAD_LIBRARY(platform_load_library);

#define PLATFORM_FREE_LIBRARY(name) void name(void* Library)
typedef PLATFORM_FREE_LIBRARY(platform_free_library);

#define PLATFORM_LOAD_SYMBOL(name) void* name(void* Library, const char* Symbol)
typedef PLATFORM_LOAD_SYMBOL(platform_load_symbol);

struct platform_api
{
    platform_get_all_files_with_extension *GetAllFilesWithExtension;
    platform_file_exists *FileExists;
    platform_allocate_memory *AllocateMemory;
    platform_deallocate_memory *DeallocateMemory;
    platform_open_file_with_dialog* OpenFileWithDialog;
    platform_save_file_with_dialog* SaveFileWithDialog;
    platform_get_time_of_day* GetTimeOfDay;
    platform_load_library* LoadDynamicLibrary;
    platform_free_library* FreeDynamicLibrary;
    platform_load_symbol* LoadSymbol;
};
extern platform_api Platform;

struct debug_state;

struct game_memory
{
    b32 IsInitialized;
    b32 ShouldReload;
    b32 ExitGame;
    config_data ConfigData;
    platform_api PlatformAPI;
    log_state LogState;
    
    struct game_state* GameState;
    
#if GLITCH_DEBUG
    debug_state* DebugState;
#endif
    
};

struct input_controller;
struct sound_commands;
struct render_state;

struct renderer;

#define UPDATE(name)void name(r64 DeltaTime, game_memory* GameMemory, renderer& Renderer, input_controller* InputController, sound_commands* SoundCommands)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

#define ERR(msg) HandleError(__FILE__,__LINE__,msg)



#endif
