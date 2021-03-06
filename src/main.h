#ifndef MAIN_H
#define MAIN_H

struct MemoryBlock
{
    PlatformMemoryBlock block;
    u64 pad[3];
};

struct PlatformState
{
    MemoryArena perm_arena;
};

struct GameCode
{
    void* game_code_library;
    time_t last_library_write_time;
    
    Update *update;
    UpdateEditor *update_editor;
    BuildRenderPipeline *build_render_pipeline;
    OnLoadAssets *on_load_assets;
    OnAssetsLoaded *on_assets_loaded;
    InitializeGame *initialize_game;
    ReinitializeGame *reinitialize_game;
    
    b32 is_valid;
};

struct MemoryState
{
    i32 blocks;
    u64 size_allocated;
};

struct SoundDevice;
struct Renderer;

void save_config(const char *file_path, Renderer* renderer = nullptr, sound::SoundSystem *sound_system = nullptr);

#endif


