#ifndef MAIN_H
#define MAIN_H

struct win32_memory_block
{
    platform_memory_block Block;
};

struct win32_state
{
    memory_arena PermArena;
    memory_arena TempArena;
};


#endif
