#ifndef DEBUG_H
#define DEBUG_H

#if GLITCH_DEBUG

struct DebugRect
{
    math::Vec2 rect_origin;
    math::Vec2 rect_size;
    
    b32 selected;
};

enum DebugType
{
    DB_FLOAT,
    DB_INT,
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
        } d_float;
        struct
        {
            i32 value;
        } d_int;
        struct
        {
            u64 value;
        } d_u64;
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
    debug_value.type = DB_INT;
    char* format = concat("\t", concat(description, ": %d", debug_arena), debug_arena);
    debug_value.format = push_string(debug_arena, (u32)strlen(format));
    strcpy(debug_value.format, format);
    debug_value.d_int.value = value;
    debug_info->debug_values[debug_info->debug_value_count++] = debug_value;
}

inline void add_debug_value(MemoryArena* debug_arena, DebugInfo* debug_info, char* description, u64 value)
{
    DebugValue debug_value = {};
    debug_value.type = DB_U64;
    char* format = concat("\t", concat(description, ": %llu", debug_arena), debug_arena);
    debug_value.format = push_string(debug_arena, (u32)strlen(format));
    strcpy(debug_value.format, format);
    debug_value.d_u64.value = value;
    debug_info->debug_values[debug_info->debug_value_count++] = debug_value;
}

inline void add_debug_value(MemoryArena* debug_arena, DebugInfo* debug_info, char* description, r32 value)
{
    DebugValue debug_value = {};
    debug_value.type = DB_FLOAT;
    char* format = concat("\t", concat(description, ": %f", debug_arena), debug_arena);
    debug_value.format = push_string(debug_arena, (u32)strlen(format));
    strcpy(debug_value.format, format);
    debug_value.d_float.value = value;
    debug_info->debug_values[debug_info->debug_value_count++] = debug_value;
}

#endif

#endif

