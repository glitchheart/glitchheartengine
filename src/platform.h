#ifndef PLATFORM_H
#define PLATFORM_H

#define array_count(array) (sizeof(array) / sizeof(array[0])) 

#define COMMA_IF_PARENS(...) ,

#if GLITCH_DEBUG
#define debug(format, ...) printf(format , ## __VA_ARGS__)
#else
#define debug(format, ...)
#endif

#if GLITCH_DEBUG
#ifdef _WIN32
#define assert(expression, ...) if(!(expression)) {debug("Assertion failed in: %s on line %d\n with message: %s\n",__FILE__,__LINE__ , ## __VA_ARGS__); __debugbreak();}
#elif __linux
#define assert(expression, ...) if(!(expression)) {debug("Assertion failed in: %s on line %d\n with message: %s\n",__FILE__,__LINE__ , ## __VA_ARGS__); abort();}
#elif __APPLE__
#define assert(expression, ...) if(!(expression)) {debug("Assertion failed in: %s on line %d\n with message: %s\n",__FILE__,__LINE__ , ## __VA_ARGS__); abort();}
#endif
#else
#define assert(expression, ...)
#endif

#define not_implemented() assert(false, "This feature has not yet been implemented")

#define UNUSED(var) (void)var

#if GLITCH_DEBUG
#define assert_static(expression)  int i = 1/(i32)expression
#else
#define assert_static(expression)
#endif

#define MIN(A,B) ((A <= B) ? (A) : (B))
#define MAX(A,B) ((A >= B) ? (A) : (B))
#define ABS(x) ((x) < 0 ? -(x) : (x))

#define kilo_bytes(value) ((value)*1024LL)
#define mega_bytes(value) (kily_bytes(value)*1024LL)
#define giga_bytes(value) (mega_bytes(value)*1024LL)
#define tera_bytes(value) (giga_bytes(value)*1024LL)

#define align_pow2(value, alignment) ((value + ((alignment) - 1)) & ~((alignment) - 1))

#define PI 3.141592653589793f
#define DEGREE_IN_RADIANS 0.0174532925f

#define offset_of(type, member) (umm)&(((type *)0)->member)

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

#include "engine_math.h"
#include "simd.h"
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

enum SoundSpace
{
    SOUND_2D,
    SOUND_3D
};

struct ConfigData
{
    char* starting_level_file_path;
    i32 screen_width;
    i32 screen_height;
    i32 scale_from_width;
    i32 scale_from_height;
    WindowMode window_mode;
    r32 contrast;
    r32 brightness;
    b32 muted;
    r32 sfx_volume;
    r32 music_volume;
    b32 skip_splash_screen;
};

#define MAX_FILE_PATHS 128
#define MAX_FILE_NAMES 128
struct DirectoryData
{
    char file_paths[256][MAX_FILE_PATHS];
    char file_names[256][MAX_FILE_NAMES];
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

struct PlatformFile;

enum FileOpenFlags
{
    POF_READ = (1 << 0),
    POF_WRITE = (1 << 1)
};

enum SeekOptions
{
    SO_SET,
    SO_CUR,
    SO_END
};

#define PLATFORM_GET_ALL_FILES_WITH_EXTENSION(name) void name(MemoryArena* arena, const char* directory_path, const char* extension, DirectoryData* directory_data, b32 with_sub_directories)
typedef PLATFORM_GET_ALL_FILES_WITH_EXTENSION(PlatformGetAllFilesWithExtension);

#define PLATFORM_GET_ALL_DIRECTORIES(name) char ** name(const char* path)
typedef PLATFORM_GET_ALL_DIRECTORIES(PlatformGetAllDirectories);

#define PLATFORM_FILE_EXISTS(name) b32 name(const char *file_path)
typedef PLATFORM_FILE_EXISTS(PlatformFileExists);

#define PLATFORM_ALLOCATE_MEMORY(name) PlatformMemoryBlock* name(umm size, u64 flags)
typedef PLATFORM_ALLOCATE_MEMORY(PlatformAllocateMemory);

#define PLATFORM_DEALLOCATE_MEMORY(name) void name(PlatformMemoryBlock* block)
typedef PLATFORM_DEALLOCATE_MEMORY(PlatformDeallocateMemory);

#define PLATFORM_OPEN_FILE_WITH_DIALOG(name) PlatformFile name(MemoryArena* arena, char* extension)
typedef PLATFORM_OPEN_FILE_WITH_DIALOG(PlatformOpenFileWithDialog);

#define PLATFORM_SAVE_FILE_WITH_DIALOG(name) PlatformFile name(MemoryArena* arena, char* extension, u64 flags)
typedef PLATFORM_SAVE_FILE_WITH_DIALOG(PlatformSaveFileWithDialog);

#define PLATFORM_GET_TIME_OF_DAY(name) u32 name()
typedef PLATFORM_GET_TIME_OF_DAY(PlatformGetTimeOfDay);

#define PLATFORM_LOAD_LIBRARY(name) void* name(const char *path)
typedef PLATFORM_LOAD_LIBRARY(PlatformLoadLibrary);

#define PLATFORM_FREE_LIBRARY(name) void name(void* library)
typedef PLATFORM_FREE_LIBRARY(PlatformFreeLibrary);

#define PLATFORM_LOAD_SYMBOL(name) void* name(void* library, const char* symbol)
typedef PLATFORM_LOAD_SYMBOL(PlatformLoadSymbol);

#define PLATFORM_OPEN_FILE(name) PlatformFile name(const char* path, u32 open_flags)
typedef PLATFORM_OPEN_FILE(PlatformOpenFile);

#define PLATFORM_CLOSE_FILE(name) void name(PlatformFile& file)
typedef PLATFORM_CLOSE_FILE(PlatformCloseFile);

#define PLATFORM_WRITE_FILE(name) void name(const void* src, i32 size, i32 size_bytes, PlatformFile& file)
typedef PLATFORM_WRITE_FILE(PlatformWriteFile);

#define PLATFORM_READ_FILE(name) void name(void* dst, i32 size, i32 size_bytes, PlatformFile& file)
typedef PLATFORM_READ_FILE(PlatformReadFile);

#define PLATFORM_SEEK_FILE(name) void name(PlatformFile& file, i32 offset, SeekOptions seek_options)
typedef PLATFORM_SEEK_FILE(PlatformSeekFile);

#define PLATFORM_TELL_FILE(name) i32 name(PlatformFile& file)
typedef PLATFORM_TELL_FILE(PlatformTellFile);

#define PLATFORM_READ_LINE_FILE(name) char *name(char* dst, i32 buf_size, PlatformFile& file)
typedef PLATFORM_READ_LINE_FILE(PlatformReadLineFile);

#define PLATFORM_PRINT_FILE(name) i32 name(PlatformFile& file, const char* format, ...)
typedef PLATFORM_PRINT_FILE(PlatformPrintFile);

struct PlatformApi
{
    PlatformGetAllFilesWithExtension *get_all_files_with_extension;
    PlatformGetAllDirectories *get_all_directories;
    PlatformFileExists *file_exists;
    PlatformAllocateMemory *allocate_memory;
    PlatformDeallocateMemory *deallocate_memory;
    PlatformOpenFileWithDialog *open_file_with_dialog;
    PlatformSaveFileWithDialog *save_file_with_dialog;
    PlatformGetTimeOfDay *get_time_of_day;
    PlatformLoadLibrary *load_dynamic_library;
    PlatformFreeLibrary *free_dynamic_library;
    PlatformLoadSymbol *load_symbol;
    PlatformOpenFile *open_file;
    PlatformCloseFile *close_file;
    PlatformWriteFile *write_file;
    PlatformReadFile *read_file;
    PlatformSeekFile *seek_file;
    PlatformTellFile *tell_file;
    PlatformReadLineFile *read_line_file;
    PlatformPrintFile *print_file;
};

extern PlatformApi platform;

struct MemoryArena;
struct DebugState;
struct InputController;
struct SoundSystem;
struct RenderState;
struct Renderer;
struct TimerController;

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

#define UPDATE(name)void name(r64 delta_time, GameMemory* game_memory, Renderer& renderer, InputController* input_controller, SoundSystem* sound_system, TimerController& timer_controller)
typedef UPDATE(Update);
UPDATE(update_stub)
{
}

#define ERR(msg) HandleError(__FILE__,__LINE__,msg)

#endif
