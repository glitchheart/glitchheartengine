#ifndef SHARED_H
#define SHARED_H

struct MemoryArena;
inline char* concat(const char *s1, const char *s2, MemoryArena* arena = 0);

#include "types.h"

#include "platform.h"
#include "engine_memory.h"
#include "log_state.h"
#include "log.h"

#include "gmap.h"

#include "animation.h"
#include "rendering.h"

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
    char* result;
    if(arena)
    {
        result = push_string(arena, (u32)(strlen(s1) + strlen(s2) + 1));
    }
    else
    {
        result = push_temp_string((u32)(strlen(s1) + strlen(s2) + 1));
    }
    
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

inline char* to_string(i32 i)
{
    char* v = push_temp_string(64);
    sprintf(v, "%d", i);
    return v;
}

inline char* to_string(r64 r)
{
    char* result = push_temp_string(64);
    sprintf(result, "%lf", r);
    return result;
}

inline char* to_string(r32 r)
{
    char* result = push_temp_string(64);
    sprintf(result, "%f", r);
    return result;
}

struct TextureData;

char* to_string(texture_data* data)
{
    char* result = push_temp_string(64);
    sprintf(result, "{Handle: %d, \n Name: %s, \n Width: %d, \n Height: %d,}", data->handle, data->name, data->width, data->height);
    return result;
}

inline b32 starts_with(const char *a, const char *b)
{
    if(strncmp(a, b, strlen(b)) == 0) return 1;
    return 0;
}

inline char* get_file_name_from_path(char* path, char* extension = 0)
{
    const char* compare_string = ".";
    if(extension)
    {
        compare_string = concat(".", extension);
    }
    
    char* p = push_temp_string(path);
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
