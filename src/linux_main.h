#ifndef MAIN_H
#define MAIN_H

struct linux_memory_block
{
    platform_memory_block Block;
    u64 Pad[3];
};

struct linux_state
{
    memory_arena PermArena;
};

using HMODULE = void*;
using FILETIME = time_t;

struct game_code
{
    HMODULE GameCodeLib;
    FILETIME LastLibWriteTime;
    update *Update;
    
    b32 IsValid;
    const char *LibPath = "libgame.so";
    const char *TempLibPath = "libgame_temp.so";
    //TODO: Replace above with linux library stuff
};

#define MAX_TEMP_BLOCKS 2048

struct linux_memory_state
{
    // Will this work thoooo (apparently yes!)
    platform_memory_block* Blocks[MAX_TEMP_BLOCKS];
    i32 TempCount;
    u64 TempSizeAllocated;
    
    i32 PermanentBlocks;
    u64 PermanentSizeAllocated;
};

#endif