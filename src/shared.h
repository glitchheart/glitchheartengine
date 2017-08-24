#ifndef SHARED_H
#define SHARED_H

#include "memory.h"
#include "rendering.h"

inline char* Concat(const char *s1, const char *s2, memory_arena* Arena = 0)
{
    char* result;
    if(Arena)
    {
        result = PushString(Arena, (u32)(strlen(s1) + strlen(s2)));
    }
    else
    {
        result = PushTempString((u32)(strlen(s1) + strlen(s2)));
    }
    
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

inline b32 StartsWith(const char *A, const char *B)
{
    if(strncmp(A, B, strlen(B)) == 0) return 1;
    return 0;
}


inline void HandleError(char const *File, i32 LineNum, char const *msg)
{
    /*if(File)
    {
        fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
        fprintf(stderr, "%s\n", msg);
    }*/
}

#endif