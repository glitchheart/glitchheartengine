struct memory_arena
{
    u64 Flags;
    u64 Size;
    u8 *Base;
    umm Used;
};

static void InitializeArena(memory_arena *Arena, sz Size, u8* Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

#define PushStruct(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type*)PushSize_(Arena, (Count)*sizeof(type))
#define PushSize(Arena, Size, type) (type*)PushSize_(Arena, Size)
void* PushSize_(memory_arena* Arena, sz Size)
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void* Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return Result;
}

char* PushString(memory_arena* Arena, u32 Length)
{
    auto Result = (char*)PushSize_(Arena, (Length + 1) * sizeof(char));
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

static void Reset(memory_arena *Arena)
{
    Arena->Used = 0;
}