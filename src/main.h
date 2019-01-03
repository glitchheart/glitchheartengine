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
    
    b32 is_valid;
};

struct MemoryState
{
    i32 blocks;
    u64 size_allocated;
};

#endif


