#ifndef MEMORY_H
#define MEMORY_H

struct memory_arena
{
    platform_memory_block* CurrentBlock;
    u64 MinimumBlockSize;
    
    u64 AllocationFlags;
};

enum Arena_Flags
{
    AFlag_Zero = (1 << 0)
};

struct push_params
{
    u32 Flags;
    u32 Alignment;
};

struct arena_bootstrap_params
{
    u64 AllocationFlags;
    umm MinimumBlockSize;
};

inline arena_bootstrap_params DefaultBootstrapParams()
{
    arena_bootstrap_params Params = {};
    return Params;
}

inline push_params DefaultPushParams()
{
    push_params Params;
    Params.Flags = AFlag_Zero;
    Params.Alignment = 4;
    return Params;
}

inline push_params NoClear()
{
    push_params Params;
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

inline sz GetAlignmentOffset(memory_arena* Arena, sz Alignment)
{
    sz AlignmentOffset = 0;
    
    sz ResultPointer = (sz)Arena->CurrentBlock->Base + Arena->CurrentBlock->Used;
    sz AlignmentMask = Alignment - 1;
    if(ResultPointer & AlignmentMask)
    {
        AlignmentOffset = Alignment - (ResultPointer & AlignmentMask);
    }
    return AlignmentOffset;
}


inline sz GetEffectiveSizeFor(memory_arena* Arena, sz SizeInit, push_params Params = DefaultPushParams())
{
    sz Size = SizeInit;
    
    sz AlignmentOffset = GetAlignmentOffset(Arena, Params.Alignment);
    
    Size += AlignmentOffset;
    return Size;
}

#define PushStruct(Arena, type, ...) (type *)PushSize_(Arena, sizeof(type), __VA_ARGS__)
#define PushArray(Arena, Count, type, ...) (type*)PushSize_(Arena, (Count)*sizeof(type), __VA_ARGS__)
#define PushSize(Arena, Size, type, ...) (type*)PushSize_(Arena, Size, __VA_ARGS__)
void* PushSize_(memory_arena* Arena, sz SizeInit, push_params Params = DefaultPushParams())
{
    void* Result = 0;
    
    umm Size = 0;
    
    if(Arena->CurrentBlock)
    {
        Size = GetEffectiveSizeFor(Arena, SizeInit, Params);
    }
    
    if(!Arena->CurrentBlock || (Arena->CurrentBlock->Used + Size) > Arena->CurrentBlock->Size)
    {
        Size = SizeInit;
        if(Arena->AllocationFlags & (PM_OverflowCheck | PM_UnderflowCheck))
        {
            Arena->MinimumBlockSize = 0;
            Size = AlignPow2((u32)Size, Params.Alignment);
        }
        else if(!Arena->MinimumBlockSize)
        {
            Arena->MinimumBlockSize = 1024 * 1024;
        }
        
        sz BlockSize = Max(Size, Arena->MinimumBlockSize);
        
        //@Incomplete: Send some sort of allocation flags here
        platform_memory_block* NewBlock = Platform.AllocateMemory(BlockSize, Arena->AllocationFlags);
        
        NewBlock->Prev = Arena->CurrentBlock;
        Arena->CurrentBlock = NewBlock;
    }
    
    Assert((Arena->CurrentBlock->Used + Size) <= Arena->CurrentBlock->Size);
    sz AlignmentOffset = GetAlignmentOffset(Arena, Params.Alignment);
    Result = Arena->CurrentBlock->Base + Arena->CurrentBlock->Used + AlignmentOffset;
    Arena->CurrentBlock->Used += Size;
    
    Assert(Size >= SizeInit);
    
    if(Params.Flags & AFlag_Zero)
    {
        ZeroSize(SizeInit, Result);
    }
    
    return Result;
}

inline u64 DefaultFlags()
{
    return PM_OverflowCheck | PM_UnderflowCheck;
}

#define PushTempStruct(type, ...) (type *)PushTempSize_(sizeof(type), __VA_ARGS__)
#define PushTempArray(Count, type, ...) (type*)PushTempSize_((Count)*sizeof(type), __VA_ARGS__)
#define PushTempSize(Size, type, ...) (type*)PushTempSize_( Size, __VA_ARGS__)
inline void* PushTempSize_(sz Size, push_params = DefaultPushParams(), u64 Flags = DefaultFlags())
{
    platform_memory_block* Block = Platform.AllocateMemory(Size, Flags | PM_Temporary);
    void* Result = Block->Base;
    return Result;
}

inline char* PushTempString(u32 Length)
{
    platform_memory_block* Block = Platform.AllocateMemory(Length, PM_Temporary);
    char* Result = (char*)Block->Base;
    Result[Length + 1] = 0;
    return Result;
}

inline char* PushTempString(char* Source)
{
    auto Length = strlen(Source);
    char* Dest = PushTempString((u32)Length);
    for(u32 CharIndex = 0; CharIndex < Length; CharIndex++)
    {
        Dest[CharIndex] = Source[CharIndex];
    }
    Dest[Length + 1] = 0;
    return Dest;
}

inline char* PushTempString(const char* Source)
{
    auto Length = strlen(Source);
    char* Dest = PushTempString((u32)Length);
    for(u32 CharIndex = 0; CharIndex < Length; CharIndex++)
    {
        Dest[CharIndex] = Source[CharIndex];
    }
    Dest[Length + 1] = 0;
    return Dest;
}

#define Copy(Arena, Dest, Src, Size, type) Dest = PushSize(Arena, Size, type); memcpy(Dest, Src, Size);

#define CopyTemp(Dest, Src, Size, type) Dest = PushTempSize(Size, type); memcpy(Dest, Src, Size);

inline void FreeLastBlock(memory_arena* Arena)
{
    platform_memory_block* Free = Arena->CurrentBlock;
    Arena->CurrentBlock = Free->Prev;
    Platform.DeallocateMemory(Free);
}


static void Clear(memory_arena *Arena)
{
    while(Arena->CurrentBlock)
    {
        FreeLastBlock(Arena);
    }
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

char* PushString(memory_arena* Arena, sz Length, char* Source)
{
    return PushString(Arena, (u32)Length, Source);
}

#define BootstrapPushStruct(type, Member, ...) (type*)BootstrapPushSize_(sizeof(type), OffsetOf(type, Member), __VA_ARGS__)
inline void* BootstrapPushSize_(umm StructSize, umm OffsetToArena,
                                arena_bootstrap_params BootstrapParams = DefaultBootstrapParams(),
                                push_params Params = DefaultPushParams())
{
    memory_arena Bootstrap = {};
    Bootstrap.AllocationFlags = BootstrapParams.AllocationFlags;
    Bootstrap.MinimumBlockSize = BootstrapParams.MinimumBlockSize;
    void* Struct = PushSize(&Bootstrap, StructSize, void*, Params);
    *(memory_arena *)((u8 *)Struct + OffsetToArena) = Bootstrap;
    
    return Struct;
}

#endif