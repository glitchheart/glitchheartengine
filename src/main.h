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
    
    update *update;
    
    b32 is_valid;
};

#define MAX_TEMP_BLOCKS 4096

struct MemoryState
{
    // Will this work thoooo (apparently yes!)
    PlatformMemoryBlock* blocks[MAX_TEMP_BLOCKS];
    i32 temp_count;
    u64 temp_size_allocated;
    
    i32 permanent_blocks;
    u64 permanent_size_allocated;
};

#endif


