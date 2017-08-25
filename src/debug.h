#ifndef DEBUG_H
#define DEBUG_H

#if GLITCH_DEBUG

struct debug_rect
{
    math::v2 RectOrigin;
    math::v2 RectSize;
    
    b32 Selected;
};

struct debug_memory_info
{
    i32 TempBlockCount;
    u64 TempSizeAllocated;
    
    i32 BlocksAllocated;
    u64 SizeAllocated;
    
    i32 PermanentBlocks;
    u64 PermanentSizeAllocated;
    
    debug_rect DebugRect;
};

struct debug_state
{
    debug_memory_info DebugMemoryInfo;
    b32 DebugMemory;
};

#endif

#endif

