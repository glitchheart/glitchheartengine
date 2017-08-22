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
void* PushSize_(memory_arena* Arena, sz Size)
{
    Assert((Arena->Used + Size) <= Arena->Size);
    void* Result = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return Result;
}

static void Clear(memory_arena *Arena)
{
    Arena->Used = 0;
}



