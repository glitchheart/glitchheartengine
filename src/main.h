#ifndef MAIN_H
#define MAIN_H

#include "platform.h"

struct win32_memory
{
    u64 PermanentStorageSize;
    void* PermanentStorage;
    
    u64 TemporaryStorageSize;
    void* TemporaryStorage;
};

struct win32_state
{
    memory_arena PermArena;
    memory_arena TempArena;
};



#endif
