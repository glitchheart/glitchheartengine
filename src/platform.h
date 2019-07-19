#ifndef PLATFORM_H
#define PLATFORM_H

#define array_count(array) (sizeof(array) / sizeof(array[0])) 

#define COMMA_IF_PARENS(...) ,

#if DEBUG
#define debug(format, ...) printf(format , ## __VA_ARGS__)
#else
#define debug(format, ...)
#endif

#if DEBUG
#ifdef _WIN32
#define assert_(expression, ...) if(!(expression)) {debug("Assertion failed\nFile: %s \n Line: %d \n Expression: %s \n Message: %s \n", __FILE__, __LINE__, #expression, ## __VA_ARGS__); __debugbreak();}
#elif __linux
#define assert(expression, ...) if(!(expression)) {debug("Assertion failed \n File: %s \n Line: %d \n Expression: %s \n Message: %s \n", __FILE__, __LINE__, #expression, ## __VA_ARGS__); abort();}
#elif __APPLE__
#define assert(expression, ...) if(!(expression)) {debug("Assertion failed \n File: %s \n Line: %d \n Expression: %s \n Message: %s \n", __FILE__, __LINE__, #expression, ## __VA_ARGS__); abort();}
#endif
#else
#define assert(expression, ...)
#endif

#define not_implemented() assert(false, "This feature has not yet been implemented")

#define UNUSED(var) (void)var

#if DEBUG
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

#define offset_of(type, member) (umm)&(((type *)nullptr)->member)

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <stdint.h>
#ifdef __linux
#include <ctype.h>
#else
#include <cctype>
#endif

#define u16max 65535
#define i32min ((i32)0x80000000)
#define i32max ((i32)0x7fffffff)
#define u32min 0
#define u32max ((u32)-1)
#define u64max ((u64)-1)
#define r32max FLT_MAX
#define r32min -FLT_MAX

#include <limits.h>
#ifndef SIZE_MAX
 #ifdef __SIZE_MAX__
  #define SIZE_MAX __SIZE_MAX__
 #else
  #define SIZE_MAX std::numeric_limits<size_t>::max()
 #endif
#endif

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
#include "modelformat.h"

enum WindowMode
{
    FM_WINDOWED = 0,
    FM_BORDERLESS = 1
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
    char title[64];
    char version[64];
    i32 screen_width;
    i32 screen_height;
    WindowMode window_mode;
    r32 contrast;
    r32 brightness;
    b32 muted;
    r32 master_volume;
    r32 sfx_volume;
    r32 music_volume;
    b32 vsync;
};

#define MAX_FILE_PATHS 128
#define MAX_FILE_NAMES 128
struct DirectoryData
{
    char file_paths[128][64];
    char file_names[128][32];
    i32 file_count = 0;
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
    PlatformMemoryBlock* next;
};

enum PlatformFileFlags
{
    PM_APPEND = (1 << 0)
};

struct PlatformFile
{
    i32 handle;
};

enum FileOpenFlags
{
    POF_READ = (1 << 0),
    POF_WRITE = (1 << 1),
    POF_CREATE_ALWAYS = (1 << 2),
    POF_OPEN_EXISTING = (1 << 3),
    POF_OPEN_ALWAYS = (1 << 4),
    POF_IGNORE_ERROR = (1 << 5)
};

enum SeekOptions
{
    SO_SET,
    SO_CUR,
    SO_END
};

enum class FileType
{
    DIRECTORY,
    FILE
};

struct File
{
    FileType type;
    char name[32];
};

struct FileList
{
    char path[64];
    
    File *files;
    i32 file_count;
    
    File *dirs;
    i32 dir_count;

    i32 allocated_size;
    b32 allocated;
};

#define PLATFORM_LIST_ALL_FILES_AND_DIRECTORIES(name) void name(const char *path, FileList *list)
typedef PLATFORM_LIST_ALL_FILES_AND_DIRECTORIES(PlatformListAllFilesAndDirectories);

#define PLATFORM_GET_ALL_FILES_WITH_EXTENSION(name) void name(const char* directory_path, const char* extension, DirectoryData *directory_data, b32 with_sub_directories)
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

#define PLATFORM_SLEEP(name) void name(u32 ms)
typedef PLATFORM_SLEEP(PlatformSleep);

#define PLATFORM_SLEEP_IS_GRANULAR(name) b32 name(u32 ms)
typedef PLATFORM_SLEEP_IS_GRANULAR(PlatformSleepIsGranular);

#define PLATFORM_LOAD_LIBRARY(name) void* name(const char *path)
typedef PLATFORM_LOAD_LIBRARY(PlatformLoadLibrary);

#define PLATFORM_FREE_LIBRARY(name) void name(void* library)
typedef PLATFORM_FREE_LIBRARY(PlatformFreeLibrary);

#define PLATFORM_LOAD_SYMBOL(name) void* name(void* library, const char* symbol)
typedef PLATFORM_LOAD_SYMBOL(PlatformLoadSymbol);

#define PLATFORM_OPEN_FILE(name) PlatformFile name(const char* path, u32 open_flags)
typedef PLATFORM_OPEN_FILE(PlatformOpenFile);

#define PLATFORM_REMOVE_FILE(name) b32 name(const char* path)
typedef PLATFORM_REMOVE_FILE(PlatformRemoveFile);

#define PLATFORM_REMOVE_DIRECTORY(name) b32 name(const char* path)
typedef PLATFORM_REMOVE_DIRECTORY(PlatformRemoveDirectory);

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

#define PLATFORM_CREATE_DIRECTORY(name) b32 name(const char* path)
typedef PLATFORM_CREATE_DIRECTORY(PlatformCreateDirectory);

struct WorkQueue;
struct WorkQueueEntry;
struct ThreadInfo;
typedef void (*WorkCallback)(WorkQueue *queue, void *data);

#define PLATFORM_REQUEST_QUEUE(name) WorkQueue * name()
typedef PLATFORM_REQUEST_QUEUE(PlatformRequestQueue);

#define PLATFORM_REQUEST_THREAD_INFO(name) ThreadInfo * name()
typedef PLATFORM_REQUEST_THREAD_INFO(PlatformRequestThreadInfo);

#define PLATFORM_ADD_ENTRY(name) void name(WorkQueue *queue, WorkCallback work_ptr, void *data)
typedef PLATFORM_ADD_ENTRY(PlatformAddEntry);

#define PLATFORM_MAKE_QUEUE(name) void name(WorkQueue *queue, u32 thread_count, ThreadInfo *thread_infos)
typedef PLATFORM_MAKE_QUEUE(PlatformMakeQueue);

#define PLATFORM_COMPLETE_ALL_WORK(name) void name(WorkQueue *queue)
typedef PLATFORM_COMPLETE_ALL_WORK(PlatformCompleteAllWork);

#define PLATFORM_IS_EOL(name) b32 name(char c)
typedef PLATFORM_IS_EOL(PlatformIsEOL);

struct PlatformApi
{
    PlatformGetAllFilesWithExtension *get_all_files_with_extension;
    PlatformListAllFilesAndDirectories *list_all_files_and_directories;
    PlatformGetAllDirectories *get_all_directories;
    PlatformFileExists *file_exists;
    PlatformAllocateMemory *allocate_memory;
    PlatformDeallocateMemory *deallocate_memory;
    PlatformOpenFileWithDialog *open_file_with_dialog;
    PlatformSaveFileWithDialog *save_file_with_dialog;
    PlatformGetTimeOfDay *get_time_of_day;
    PlatformSleep *sleep;
    PlatformSleepIsGranular *sleep_is_granular;
    PlatformLoadLibrary *load_dynamic_library;
    PlatformFreeLibrary *free_dynamic_library;
    PlatformLoadSymbol *load_symbol;
    PlatformOpenFile *open_file;
    PlatformRemoveFile *remove_file;
    PlatformRemoveDirectory *remove_directory;
    PlatformCloseFile *close_file;
    PlatformWriteFile *write_file;
    PlatformReadFile *read_file;
    PlatformSeekFile *seek_file;
    PlatformTellFile *tell_file;
    PlatformReadLineFile *read_line_file;
    PlatformPrintFile *print_file;
    PlatformCreateDirectory *create_directory;

    PlatformRequestQueue* request_queue;
    PlatformRequestThreadInfo* request_thread_info;
    PlatformAddEntry *add_entry;
    PlatformCompleteAllWork *complete_all_work;
    PlatformMakeQueue *make_queue;
    PlatformIsEOL *is_eol;

    WorkQueue *asset_queue;
};
   
extern PlatformApi platform;

namespace os
{
    using File = PlatformFile;
    static b32 is_eol(char c)
    {
        return platform.is_eol(c);
    }
    
    static PlatformFile open_file(const char* path, u32 open_flags)
    {
        return platform.open_file(path, open_flags);
    }

    static void close_file(PlatformFile file)
    {
        platform.close_file(file);
    }

    static void read_file(void* dst, i32 size, i32 size_bytes, PlatformFile& file)
    {
        platform.read_file(dst, size, size_bytes, file);
    }

    static void remove_file(const char* path)
    {
        platform.remove_file(path);
    }

    static void remove_directory(const char* path)
    {
        platform.remove_directory(path);
    }    

    static void create_directory(const char* path)
    {
        platform.create_directory(path);
    }    

    static void write_file(const void* src, i32 size, i32 size_bytes, PlatformFile& file)
    {
        platform.write_file(src, size, size_bytes, file);
    }

    static void seek_file(PlatformFile& file, i32 offset, SeekOptions seek_options)
    {
        platform.seek_file(file, offset, seek_options);
    }

    static b32 file_exists(const char *file_path)
    {
        return platform.file_exists(file_path);
    }

    static i32 tell_file(PlatformFile& file)
    {
        return platform.tell_file(file);
    }

    static void get_all_files_with_extension(const char* path, const char* extension, DirectoryData* data, b32 recursive)
    {
        platform.get_all_files_with_extension(path, extension, data, recursive);
    }
}

struct MemoryArena;
struct InputController;

namespace sound
{
    struct SoundSystem;
}

struct Renderer;
struct RenderState;
namespace editor
{
    struct EditorState;
}


namespace scene
{
    struct SceneManager;
}

namespace assets
{
    struct AssetState;
}

namespace fade
{
    struct FadeState;
}

namespace project
{
    struct ProjectState;
}

struct TimerController;

struct Core
{
    Renderer* renderer;
    InputController* input_controller;
    TimerController* timer_controller;
    sound::SoundSystem* sound_system;
    scene::SceneManager *scene_manager;
    project::ProjectState *project_state;
    
    editor::EditorState *editor_state;
    
    ImGuiContext *imgui_context;

    r64 delta_time;
    r64 current_time;

    ConfigData config_data;
    assets::AssetState* asset_state;
    fade::FadeState* fade_state;
};

extern Core core;

struct GameMemory
{
    b32 is_initialized;
    b32 should_reload;
    b32 exit_game;
    PlatformApi platform_api;
    Core core;
    struct LogState* log_state;
    struct MemoryArena* temp_arena;
#if ENABLE_ANALYTICS
    struct AnalyticsEventState *analytics_state;
#endif
    struct GameState* game_state;
};

#define UPDATE(name) void name(GameMemory* game_memory)

typedef UPDATE(Update);
UPDATE(update_stub)
{
}

#define UPDATE_EDITOR(name) void name(GameMemory* game_memory)

typedef UPDATE_EDITOR(UpdateEditor);
UPDATE_EDITOR(update_editor_stub)
{
}

#define BUILD_RENDER_PIPELINE(name) void name(PlatformApi &platform_api, Renderer *renderer)
typedef BUILD_RENDER_PIPELINE(BuildRenderPipeline);
BUILD_RENDER_PIPELINE(build_render_pipeline_stub)
{}

#define ON_LOAD_ASSETS(name) void name()
typedef ON_LOAD_ASSETS(OnLoadAssets);
ON_LOAD_ASSETS(on_load_ssets_stub)
{}

#define ON_ASSETS_LOADED(name) void name()
typedef ON_ASSETS_LOADED(OnAssetsLoaded);
ON_ASSETS_LOADED(on_assets_loaded_stub)
{}

#define INITIALIZE_GAME(name) void name(GameMemory* game_memory)
typedef INITIALIZE_GAME(InitializeGame);
INITIALIZE_GAME(initialize_game_stub)
{
}

#define REINITIALIZE_GAME(name) struct GameState* name(GameMemory* game_memory)
typedef REINITIALIZE_GAME(ReinitializeGame);
REINITIALIZE_GAME(reinitialize_game_stub)
{
    return nullptr;
}

#define ERR(msg) HandleError(__FILE__,__LINE__,msg)

#endif
