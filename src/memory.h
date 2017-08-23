#ifndef MEMORY_H
#define MEMORY_H

struct memory_block
{
    u64 Size;
    u8* Base;
    memory_block* Prev;
};

struct memory_arena
{
    u64 Flags;
    u64 Size;
    u8 *Base;
    umm Used;
    memory_block Prev;
};

enum Arena_Flags
{
    AFlag_Zero = 0x1
};

struct arena_params
{
    u32 Flags;
};

static void InitializeArena(memory_arena *Arena, sz Size, u8* Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

inline arena_params DefaultParams()
{
    arena_params Params;
    Params.Flags = AFlag_Zero;
    return Params;
}

inline arena_params NoClear()
{
    arena_params Params;
    Params.Flags &= ~AFlag_Zero;
}

#define ZeroStruct(Instance) ZeroSize(sizeof(Instance), &(Instance)
#define ZeroArray(Count, Pointer) ZeroSize(Count * sizeof((Pointer[0]), Pointer)
inline void ZeroSize(sz Size, void *Ptr)
{
    u8* Byte = (u8*)Ptr;
    while(Size--)
    {
        *Byte++ = 0;
    }
}


#define PushStruct(Arena, type, ...) (type *)PushSize_(Arena, sizeof(type), __VA_ARGS__)
#define PushArray(Arena, Count, type, ...) (type*)PushSize_(Arena, (Count)*sizeof(type), __VA_ARGS__)
#define PushSize(Arena, Size, type, ...) (type*)PushSize_(Arena, Size)
void* PushSize_(memory_arena* Arena, sz Size, arena_params Params = DefaultParams())
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void* Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    memory_block Block;
    Block.Size = Size;
    Block.Base = (u8*)Result;
    
    if(Arena->Prev.Size > 0)
    {
        Block.Prev = &Arena->Prev;
    }
    Arena->Prev = Block;
    
    if(Params.Flags & AFlag_Zero)
    {
        ZeroSize(Size, Result);
    }
    
    return Result;
}

char* PushString(memory_arena* Arena, u32 Length)
{
    auto Result = (char*)PushSize_(Arena, (Length + 1));
    Result[Length + 1] = 0;
    return Result;
}

char* PushString(memory_arena* Arena, char* Source)
{
    auto Length = strlen(Source);
    char* Dest = PushString(Arena, (u32)Length);
    for(u32 CharIndex = 0; CharIndex < Length + 1; CharIndex++)
    {
        Dest[CharIndex] = Source[CharIndex];
    }
    Dest[Length + 1] = 0;
    
    return Dest;
}

char* PushString(memory_arena* Arena, const char* Source)
{
    auto Length = strlen(Source);
    char* Dest = PushString(Arena, (u32)Length);
    for(u32 CharIndex = 0; CharIndex < Length + 1; CharIndex++)
    {
        Dest[CharIndex] = Source[CharIndex];
    }
    Dest[Length + 1] = 0;
    
    return Dest;
}

char* PushString(memory_arena* Arena, u32 Length, char* Source)
{
    char* Dest = (char*)PushSize_(Arena, Length + 1);
    for(u32 CharIndex = 0; CharIndex < Length; CharIndex++)
    {
        Dest[CharIndex] = Source[CharIndex];
    }
    Dest[Length] = 0;
    
    return Dest;
}

char* PushString(memory_arena* Arena, u32 Length, const char* Source)
{
    char* Dest = (char*)PushSize_(Arena, Length + 1);
    for(u32 CharIndex = 0; CharIndex < Length; CharIndex++)
    {
        Dest[CharIndex] = Source[CharIndex];
    }
    Dest[Length] = 0;
    
    return Dest;
}

#define Copy(Dest, Src, Size, Arena, type) Dest = PushSize(Arena, Size, type);\
memcpy(Dest, Src, Size);

static void Reset(memory_arena *Arena, sz Size = 0)
{
    Arena->Used = 0;
}

void FreeLastBlock(memory_arena* Arena)
{
    Arena->Used -= Arena->Prev.Size;
    if(Arena->Prev.Prev)
    {
        Arena->Prev = *Arena->Prev.Prev;
    }
}

#endif