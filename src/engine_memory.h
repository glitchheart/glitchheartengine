#ifndef MEMORY_H
#define MEMORY_H

struct MemoryArena
{
    platform_memory_block* current_block;
    u64 minimum_block_size = 0;
    
    u64 allocation_flags;
};

enum ArenaFlags
{
    AFlag_Zero = (1 << 0)
};

struct PushParams
{
    u32 flags;
    u32 alignment;
};

struct ArenaBootstrapParams
{
    u64 allocation_flags;
    umm minimum_block_size;
};

inline ArenaBootstrapParams default_bootstrap_params()
{
    ArenaBootstrapParams params = {};
    return params;
}

inline PushParams default_push_params()
{
    PushParams params;
    params.Flags = AFlag_Zero;
    params.Alignment = 4;
    return params;
}

inline PushParams no_clear()
{
    PushParams params;
    params.Flags &= ~AFlag_Zero;
    return params;
}

#define ZeroStruct(Instance) ZeroSize(sizeof(Instance), &(Instance)
#define ZeroArray(Count, Pointer) ZeroSize(Count * sizeof((Pointer[0]), Pointer)
inline void zero_size(umm size, void *ptr)
{
    u8* byte = (u8*)ptr;
    while(size--)
    {
        *byte++ = 0;
    }
}

inline umm get_alignment_offset(MemoryArena* arena, umm alignment)
{
    umm alignment_offset = 0;
    
    umm result_pointer = (umm)arena->CurrentBlock->Base + arena->CurrentBlock->Used;
    umm alignment_mask = alignment - 1;
    if(result_pointer & alignment_mask)
    {
        alignment_offset = alignment - (result_pointer & alignment_mask);
    }
    return alignment_offset;
}

inline umm get_effective_size_for(MemoryArena* arena, umm size_init, PushParams params = default_push_params())
{
    umm size = size_init;
    
    umm alignment_offset = get_alignment_offset(arena, params.Alignment);
    
    size += alignment_offset;
    return size;
}

#define PushStruct(Arena, type, ...) (type *)PushSize_(Arena, sizeof(type), ## __VA_ARGS__)
#define PushArray(Arena, Count, type, ...) (type*)PushSize_(Arena, (Count)*sizeof(type), ## __VA_ARGS__)
#define PushSize(Arena, Size, type, ...) (type*)PushSize_(Arena, Size, ## __VA_ARGS__)
void* PushSize_(MemoryArena* arena, umm SizeInit, PushParams params = default_push_params())
{
    void* result = 0;
    
    umm Size = 0;
    
    if(Arena->CurrentBlock)
    {
        Size = get_effective_size_for(arena, SizeInit, params);
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
        
        umm block_size = Max(Size, Arena->MinimumBlockSize);
        
        platform_memory_block* new_block = Platform.AllocateMemory(block_size, Arena->AllocationFlags);
        
        new_block->Prev = Arena->CurrentBlock;
        Arena->CurrentBlock = new_block;
    }
    
    Assert((Arena->CurrentBlock->Used + SizeInit) <= Arena->CurrentBlock->Size);
    umm alignment_offset = get_alignment_offset(Arena, Params.Alignment);
    result = Arena->CurrentBlock->Base + Arena->CurrentBlock->Used + alignment_offset;
    Arena->CurrentBlock->Used += Size;
    
    Assert(Size >= SizeInit);
    
    if(Params.Flags & AFlag_Zero)
    {
        zero_size(SizeInit, result);
    }
    
    return result;
}

inline u64 default_flags()
{
    return PM_OverflowCheck | PM_UnderflowCheck;
}

#define PushTempStruct(type, ...) (type *)PushTempSize_(sizeof(type), ## __VA_ARGS__)
#define PushTempArray(Count, type, ...) (type*)PushTempSize_((Count)*sizeof(type), ## __VA_ARGS__)
#define PushTempSize(Size, type, ...) (type*)PushTempSize_((umm)Size, ## __VA_ARGS__)
inline void* PushTempSize_(umm size, PushParams = default_push_params(), u64 flags = default_flags())
{
    platform_memory_block* block = Platform.AllocateMemory(size, flags | PM_Temporary);
    void* result = block->Base;
    return result;
}

inline char* push_temp_string(u32 length)
{
    platform_memory_block* block = Platform.AllocateMemory(length + 1, PM_Temporary);
    char* result = (char*)block->Base;
    result[length] = 0;
    return result;
}

inline char* push_temp_string(char* source)
{
    auto length = strlen(source);
    char* dest = push_temp_string((u32)length);
    for(u32 char_index = 0; char_index < length; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    return dest;
}

inline char* push_temp_string(const char* source)
{
    auto length = strlen(source);
    char* dest = push_temp_string((u32)length);
    for(u32 char_index = 0; char_index < length; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    return dest;
}

#define Copy(Arena, Dest, Src, Size, type) Dest = PushSize(Arena, Size, type); memcpy(Dest, Src, Size);

#define CopyTemp(Dest, Src, Size, type) Dest = PushTempSize(Size, type); memcpy(Dest, Src, Size);

inline void free_last_block(MemoryArena* arena)
{
    platform_memory_block* free = arena->CurrentBlock;
    arena->CurrentBlock = free->Prev;
    Platform.DeallocateMemory(free);
}

static void clear(MemoryArena *arena)
{
    while(arena->CurrentBlock)
    {
        free_last_block(arena);
    }
}

char* push_string(MemoryArena* arena, size_t length)
{
    //@Incomplete: Fix NoClear() bug here. 
    // We don't care about zeroing
    auto result = (char*)PushSize_(arena, (length + 1));
    result[length] = 0;
    return result;
}

char* push_string(MemoryArena* arena, char* source)
{
    auto length = strlen(source);
    char* dest = push_string(arena, (u32)length);
    for(u32 char_index = 0; char_index < length; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    
    return dest;
}

char* push_string(MemoryArena* arena, const char* source)
{
    auto length = strlen(source);
    char* dest = push_string(arena, (u32)length);
    for(u32 char_index = 0; char_index < length + 1; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    
    return dest;
}

char* push_string(MemoryArena* arena, u32 length, char* source)
{
    char* dest = (char*)PushSize_(arena, length + 1);
    for(u32 char_index = 0; char_index < length; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    
    return dest;
}

char* push_string(MemoryArena* arena, u32 length, const char* source)
{
    char* dest = (char*)PushSize_(arena, length + 1);
    for(u32 char_index = 0; char_index < length; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    
    return dest;
}

char* push_string(MemoryArena* arena, umm length, char* source)
{
    return push_string(arena, (u32)length, source);
}

#define BootstrapPushStruct(type, Member, ...) (type*)BootstrapPushSize_(sizeof(type), OffsetOf(type, Member), ## __VA_ARGS__)
inline void* bootstrap_push_size(umm struct_size, umm offset_to_arena,
                                ArenaBootstrapParams bootstrap_params = default_bootstrap_params(),
                                PushParams params = default_push_params())
{
    MemoryArena bootstrap = {};
    bootstrap.AllocationFlags = bootstrap_params.AllocationFlags;
    bootstrap.MinimumBlockSize = bootstrap_params.MinimumBlockSize;
    void* struct = PushSize(&Bootstrap, StructSize, void*, Params);
    *(MemoryArena *)((u8 *)struct + OffsetToArena) = Bootstrap;
    
    return struct;
}

#endif
