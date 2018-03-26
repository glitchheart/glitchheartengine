#ifndef PLATFORM_H
#define PLATFORM_H

#define NUM_ENTITIES 100

#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0])) 

#define COMMA_IF_PARENS(...) ,

#if GLITCH_DEBUG
#define Debug(format, ...) printf(format , ## __VA_ARGS__)
#else
#define Debug(format, ...)
#endif

#if GLITCH_DEBUG
#ifdef _WIN32
#define Assert(expression) if(!(expression)) {Debug("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); __debugbreak();}
#elif __linux
#define Assert(expression) if(!(expression)) {Debug("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); abort();}
#elif __APPLE__
#define Assert(expression) if(!(expression)) {Debug("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); abort();}
#endif
#else
#define Assert(expression)
#endif


#if GLITCH_DEBUG
#define Static_Assert(expression)  int i = 1/(i32)expression
#else
#define Static_Assert(expression)
#endif

#define Min(A,B) ((A < B) ? (A) : (B))
#define Max(A,B) ((A > B) ? (A) : (B))
#define Abs(x) ((x) < 0 ? -(x) : (x))

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define align_pow2(value, alignment) ((value + ((alignment) - 1)) & ~((alignment) - 1))

#define PI 3.141592653589793f
#define DEGREE_IN_RADIANS 0.0174532925f

#define offset_of(type, member) (umm)&(((type *)0)->member)

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

struct TextureData;

inline char* to_string(i32 i);
inline char* to_string(r64 r);
inline char* to_string(r32 r);
char* to_string(TextureData* data);

//#include "log_state.h"
#include "engine_math.h"
#include "modelformat.h"



enum WindowMode
{
    FM_WINDOWED = 0,
    FM_FULL = 1,
    FM_BORDERLESS = 2
};

enum GraphicsApi
{
    GRAPHICS_OPEN_GL,
    GRAPHICS_VULKAN
};

struct ConfigData
{
    char* title;
    char* version;
    char* starting_level_file_path;
    GraphicsApi graphics_api;
    i32 screen_width;
    i32 screen_height;
    i32 scale_from_width;
    i32 scale_from_height;
    WindowMode fullscreen;
    r32 contrast;
    r32 brightness;
    b32 muted;
    r32 sfx_volume;
    r32 music_volume;
    r32 zoom;
    b32 skip_splash_screen;
};

struct DirectoryData
{
    char* file_paths[512];
    char* file_names[512];
    i32 files_length = 0;
};

enum PlatformMemoryBlockFlags
{
    PM_OVERFLOW_CHECK =  (1 << 0),
    PM_UNDERFLOW_CHECK = (1 << 1),
    PM_TEMPORARY =      (1 << 2)
};

struct PlatformMemoryBlock
{
    u64 flags;
    u64 size;
    u8* base;
    umm used;
    PlatformMemoryBlock* prev;
};

enum PlatformFileFlags
{
    PM_APPEND = (1 << 0)
};

struct PlatformFile
{
    FILE* file;
    char path[260];
    char extension[16];
};

#define PLATFORM_GET_ALL_FILES_WITH_EXTENSION(name) void name(MemoryArena* arena, const char* directory_path, const char* extension, DirectoryData* directory_data, b32 with_sub_directories)
typedef PLATFORM_GET_ALL_FILES_WITH_EXTENSION(platform_get_all_files_with_extension);

#define PLATFORM_FILE_EXISTS(name) b32 name(const char* file_path)
typedef PLATFORM_FILE_EXISTS(platform_file_exists);

#define PLATFORM_ALLOCATE_MEMORY(name) PlatformMemoryBlock* name(umm size, u64 flags)
typedef PLATFORM_ALLOCATE_MEMORY(platform_allocate_memory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(PlatformMemoryBlock* block)
typedef PLATFORM_DEALLOCATE_MEMORY(platform_deallocate_memory);

#define PLATFORM_OPEN_FILE_WITH_DIALOG(name) PlatformFile name(MemoryArena* arena, char* extension)
typedef PLATFORM_OPEN_FILE_WITH_DIALOG(platform_open_file_with_dialog);

#define PLATFORM_SAVE_FILE_WITH_DIALOG(name) PlatformFile name(MemoryArena* arena, char* extension, u64 flags)
typedef PLATFORM_SAVE_FILE_WITH_DIALOG(platform_save_file_with_dialog);

#define PLATFORM_GET_TIME_OF_DAY(name) u32 name()
typedef PLATFORM_GET_TIME_OF_DAY(platform_get_time_of_day);

#define PLATFORM_LOAD_LIBRARY(name) void* name(const char* path)
typedef PLATFORM_LOAD_LIBRARY(platform_load_library);

#define PLATFORM_FREE_LIBRARY(name) void name(void* library)
typedef PLATFORM_FREE_LIBRARY(platform_free_library);

#define PLATFORM_LOAD_SYMBOL(name) void* name(void* library, const char* symbol)
typedef PLATFORM_LOAD_SYMBOL(platform_load_symbol);

struct PlatformApi
{
    platform_get_all_files_with_extension *get_all_files_with_extension;
    platform_file_exists *file_exists;
    platform_allocate_memory *allocate_memory;
    platform_deallocate_memory *deallocate_memory;
    platform_open_file_with_dialog* open_file_with_dialog;
    platform_save_file_with_dialog* save_file_with_dialog;
    platform_get_time_of_day* get_time_of_day;
    platform_load_library* load_dynamic_library;
    platform_free_library* free_dynamic_library;
    platform_load_symbol* load_symbol;
};
extern PlatformApi platform;

struct MemoryArena;
struct DebugState;

struct GameMemory
{
    b32 is_initialized;
    b32 should_reload;
    b32 exit_game;
    ConfigData config_data;
    PlatformApi platform_api;
    struct LogState* log_state;
    struct MemoryArena* temp_arena;
    
    struct GameState* game_state;
    
#if GLITCH_DEBUG
    DebugState* debug_state;
#endif
    
};

struct InputController;
struct SoundCommands;
struct RenderState;

struct Renderer;
struct TimerController;

#define UPDATE(name)void name(r64 delta_time, GameMemory* game_memory, Renderer& renderer, InputController* input_controller , SoundCommands* sound_commands , TimerController& timer_controller)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

#define ERR(msg) HandleError(__FILE__,__LINE__,msg)

#endif
