#ifndef MAIN_H
#define MAIN_H

struct win32_memory_block
{
    platform_memory_block Block;
    u64 Pad[3];
};

struct win32_state
{
    memory_arena PermArena;
};


struct game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDllWriteTime;
    update *Update;
    
    b32 IsValid;
    const char *DllPath = "game.dll";
    const char *TempDllPath = "game_temp.dll";
};

#define MAX_TEMP_BLOCKS 2048

struct win32_memory_state
{
    // Will this work thoooo
    platform_memory_block* Blocks[MAX_TEMP_BLOCKS];
    i32 TempCount;
    u64 TempSizeAllocated;
    
    i32 PermanentBlocks;
    u64 PermanentSizeAllocated;
};



#endif
