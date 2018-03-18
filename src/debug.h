#ifndef DEBUG_H
#define DEBUG_H

#if GLITCH_DEBUG

struct DebugRect
{
    math::v2 rect_origin;
    math::v2 rect_size;
    
    b32 selected;
};

enum DebugType
{
    DB_Float,
    DB_Int,
    DB_U64
};

struct DebugValue
{
    DebugType type;
    char* format;
    
    union
    {
        struct
        {
            r32 value;
        } float;
        struct
        {
            i32 value;
        } int;
        struct
        {
            u64 value;
        } u64;
    };
};

struct DebugInfo
{
    char* header;
    DebugValue debug_values[16];
    i32 debug_value_count;
};

struct DebugMemoryInfo
{
    DebugRect debug_rect;
    DebugInfo debug_info[16];
    i32 debug_info_count;
};

struct DebugState
{
    DebugMemoryInfo debug_memory_info;
    b32 debug_memory;
};

struct MemoryArena;

inline void add_debug_value(MemoryArena* debug_arena, DebugInfo* debug_info, const char* description, i32 value)
{
    DebugValue debug_value = {};
    debug_value.Type = DB_Int;
    char* format = concat("\t", concat(description, ": %d", debug_arena), debug_arena);
    debug_value.Format = push_string(debug_arena, (u32)strlen(format));
    strcpy(debug_value.Format, format);
    debug_value.Int.Value = value;
    debug_info->DebugValues[debug_info->DebugValueCount++] = debug_value;
}

inline void add_debug_value(MemoryArena* debug_arena, DebugInfo* debug_info, char* description, u64 value)
{
    DebugValue debug_value = {};
    debug_value.Type = DB_U64;
    char* format = concat("\t", concat(description, ": %llu", debug_arena), debug_arena);
    debug_value.Format = push_string(debug_arena, (u32)strlen(format));
    strcpy(debug_value.Format, format);
    debug_value.U64.Value = value;
    debug_info->DebugValues[debug_info->DebugValueCount++] = debug_value;
}

inline void add_debug_value(MemoryArena* debug_arena, DebugInfo* debug_info, char* description, r32 value)
{
    DebugValue debug_value = {};
    debug_value.Type = DB_Float;
    char* format = concat("\t", concat(description, ": %f", debug_arena), debug_arena);
    debug_value.Format = push_string(debug_arena, (u32)strlen(format));
    strcpy(debug_value.Format, format);
    debug_value.Float.Value = value;
    debug_info->DebugValues[debug_info->DebugValueCount++] = debug_value;
}

#endif

#endif

