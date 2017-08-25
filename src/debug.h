#ifndef DEBUG_H
#define DEBUG_H

#if GLITCH_DEBUG

struct debug_rect
{
    math::v2 RectOrigin;
    math::v2 RectSize;
    
    b32 Selected;
};

enum DebugType
{
    DB_Float,
    DB_Int,
    DB_U64
};

struct debug_value
{
    DebugType Type;
    char* Format;
    
    union
    {
        struct 
        {
            r32 Value;
        } Float;
        struct
        {
            i32 Value;
        } Int;
        struct
        {
            u64 Value;
        } U64;
    };
};

struct debug_info
{
    char* Header;
    debug_value DebugValues[16];
    i32 DebugValueCount;
};

struct debug_memory_info
{
    debug_rect DebugRect;
    debug_info DebugInfo[16];
    i32 DebugInfoCount;
};

struct debug_state
{
    debug_memory_info DebugMemoryInfo;
    b32 DebugMemory;
};

struct memory_arena;

inline void AddDebugValue(memory_arena* DebugArena, debug_info* DebugInfo, char* Description, i32 Value)
{
    debug_value DebugValue = {};
    DebugValue.Type = DB_Int;
    char* Format = Concat("\t", Concat(Description, ": %d", DebugArena), DebugArena);
    DebugValue.Format = PushString(DebugArena, (u32)strlen(Format));
    strcpy(DebugValue.Format, Format);
    DebugValue.Int.Value = Value;
    DebugInfo->DebugValues[DebugInfo->DebugValueCount++] = DebugValue;
}

inline void AddDebugValue(memory_arena* DebugArena, debug_info* DebugInfo, char* Description, u64 Value)
{
    debug_value DebugValue = {};
    DebugValue.Type = DB_U64;
    char* Format = Concat("\t", Concat(Description, ": %llu", DebugArena), DebugArena);
    DebugValue.Format = PushString(DebugArena, (u32)strlen(Format));
    strcpy(DebugValue.Format, Format);
    DebugValue.U64.Value = Value;
    DebugInfo->DebugValues[DebugInfo->DebugValueCount++] = DebugValue;
}

inline void AddDebugValue(memory_arena* DebugArena, debug_info* DebugInfo, char* Description, r32 Value)
{
    debug_value DebugValue = {};
    DebugValue.Type = DB_Float;
    char* Format = Concat("\t", Concat(Description, ": %f", DebugArena), DebugArena);
    DebugValue.Format = PushString(DebugArena, (u32)strlen(Format));
    strcpy(DebugValue.Format, Format);
    DebugValue.Float.Value = Value;
    DebugInfo->DebugValues[DebugInfo->DebugValueCount++] = DebugValue;
}

#endif

#endif

