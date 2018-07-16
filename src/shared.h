#ifndef SHARED_H
#define SHARED_H

struct MemoryArena;

#include "types.h"

#include "platform.h"
#include "engine_memory.h"
#include "simd.h"
#include "init_globals.h"

#include "log_state.h"
#include "log.h"

#include "animation.h"
#include "rendering.h"
#include "particles.h"

#include "map.h"

inline char* str_sep(char** s, const char* delim)
{
    char* start = *s;
    char* p;
    
    p = (start != NULL) ? strpbrk(start, delim) : NULL;
    
    if(p == NULL)
    {
        *s = NULL;
    }
    else
    {
        *p = '\0';
        *s = p + 1;
    }
    return start;
}

inline char* concat(const char *s1, const char *s2, MemoryArena* arena)
{
    char* result = nullptr;
    if(arena)
    {
        result = push_string(arena, (u32)(strlen(s1) + strlen(s2) + 1));
    }
    
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

struct TextureData;

inline b32 starts_with(const char *a, const char *b)
{
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

inline char* get_file_name_from_path(char* path, MemoryArena* arena, char* extension = 0)
{
    auto temp_mem = begin_temporary_memory(arena);
    const char* compare_string = ".";
    if(extension)
    {
        compare_string = concat(".", extension, arena);
    }
    
    char* p = push_string(arena, path);
    auto tok = str_sep(&p, ".");
    tok = str_sep(&tok, "/");
    while(tok)
    {
        
        if(strstr(tok, compare_string))
        {
            tok = str_sep(&tok, ".");
            return tok;
        }
        else 
        {
            tok = str_sep(&path, "/");
        }
    }
    end_temporary_memory(temp_mem);
    return 0;
}

inline void handle_error(char const *file, i32 line_num, char const *msg)
{
    if(file)
    {
        fprintf(stderr, "Error on in file %s on line %d\n", file, line_num);
        fprintf(stderr, "%s\n", msg);
    }
}

inline void get_current_min(math::Vec3 v1, math::Vec3 v2, r32 val1, r32 val2, math::Vec3* out_vec, r32* out_val)
{
    if(val1 < val2)
    {
        *out_vec = v1;
        *out_val = val1;
        return;
    }
    *out_vec = v2;
    *out_val = val2;
}

#endif
