#ifndef MAIN_H
#define MAIN_H

struct memory_block
{
    platform_memory_block Block;
    u64 Pad[3];
};

struct platform_state
{
    memory_arena PermArena;
};

struct game_code
{
    void* GameCodeLibrary;
    time_t LastLibraryWriteTime;
    
    update *Update;
    
    b32 IsValid;
};

#define MAX_TEMP_BLOCKS 4096

struct memory_state
{
    // Will this work thoooo (apparently yes!)
    platform_memory_block* Blocks[MAX_TEMP_BLOCKS];
    i32 TempCount;
    u64 TempSizeAllocated;
    
    i32 PermanentBlocks;
    u64 PermanentSizeAllocated;
};

#endif


