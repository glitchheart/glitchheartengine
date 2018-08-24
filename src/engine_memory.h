#ifndef MEMORY_H
#define MEMORY_H

struct MemoryArena
{
    PlatformMemoryBlock* current_block;
    u64 minimum_block_size = 0;
    
    u64 allocation_flags;
    i32 temp_count;
};

struct TemporaryMemory
{
    MemoryArena* arena;
    PlatformMemoryBlock* block;
    umm used;
};

enum ArenaFlags
{
    A_FLAG_ZERO = (1 << 0)
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
    params.flags = A_FLAG_ZERO;
    params.alignment = 4;
    return params;
}

inline PushParams no_clear()
{
    PushParams params;
    params.flags &= ~A_FLAG_ZERO;
    return params;
}

inline PushParams default_with_alignment(u32 alignment)
{
    PushParams params;
    params.flags = A_FLAG_ZERO;
    params.alignment = alignment;
    return params;
}

#define zero_struct(instance) zero_size(sizeof(instance), &(instance)
#define zero_array(count, pointer) zero_size(count * sizeof(pointer[0]), pointer)
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
    
    umm result_pointer = (umm)arena->current_block->base + arena->current_block->used;
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
    
    umm alignment_offset = get_alignment_offset(arena, params.alignment);
    
    size += alignment_offset;
    return size;
}

#define push_struct(arena, type, ...) (type *)push_size_(arena, sizeof(type), ## __VA_ARGS__)
#define push_array(arena, count, type, ...) (type *)push_size_(arena, (count)*sizeof(type), ## __VA_ARGS__)
#define push_size(arena, size, type, ...) (type *)push_size_(arena, (umm)size, ## __VA_ARGS__)
void* push_size_(MemoryArena* arena, umm size_init, PushParams params = default_push_params())
{
    void* result = 0;
    
    umm size = 0;
    
    if(arena->current_block)
    {
        size = get_effective_size_for(arena, size_init, params);
    }
    
    if(!arena->current_block || (arena->current_block->used + size) > arena->current_block->size)
    {
        size = size_init;
        
        if(arena->allocation_flags & (PM_OVERFLOW_CHECK | PM_UNDERFLOW_CHECK))
        {
            arena->minimum_block_size = 0;
            size = align_pow2((u32)size, params.alignment);
        }
        else if(!arena->minimum_block_size)
        {
            arena->minimum_block_size = 1024 * 1024;
        }
        
        umm block_size = MAX(size, arena->minimum_block_size);
        
        PlatformMemoryBlock* new_block = platform.allocate_memory(block_size, arena->allocation_flags);
        
        new_block->prev = arena->current_block;
        arena->current_block = new_block;
    }
    
    assert((arena->current_block->used + size_init) <= arena->current_block->size);
    umm alignment_offset = get_alignment_offset(arena, params.alignment);
    result = arena->current_block->base + arena->current_block->used + alignment_offset;
    arena->current_block->used += size;
    
    assert(size >= size_init);
    
    if(params.flags & A_FLAG_ZERO)
    {
        zero_size(size_init, result);
    }
    
    return result;
}

inline u64 default_flags()
{
    return PM_OVERFLOW_CHECK | PM_UNDERFLOW_CHECK;
}

inline TemporaryMemory begin_temporary_memory(MemoryArena* arena)
{
    TemporaryMemory result = {};
    
    result.arena = arena;
    result.block = arena->current_block;
    result.used = arena->current_block ? arena->current_block->used : 0;
    
    ++arena->temp_count;
    
    return result;
}

inline void free_last_block(MemoryArena* arena)
{
    PlatformMemoryBlock* free = arena->current_block;
    arena->current_block = free->prev;
    platform.deallocate_memory(free);
}

inline void end_temporary_memory(TemporaryMemory temp_mem)
{
    MemoryArena* arena = temp_mem.arena;
    while(arena->current_block != temp_mem.block)
    {
        free_last_block(arena);
    }
    
    if(arena->current_block)
    {
        assert(arena->current_block->used >= temp_mem.used);
        arena->current_block->used = temp_mem.used;
        assert(arena->temp_count > 0);
    }
    
    --arena->temp_count;
}

#define copy(arena, dest, src, size, type) dest = push_size(arena, size, type); memcpy(dest, src, size);

#define copy_temp(dest, src, size, type) dest = push_temp_size(size, type); memcpy(dest, src, size);

static void clear(MemoryArena *arena)
{
    while(arena->current_block)
    {
        free_last_block(arena);
    }
}

char* push_string(MemoryArena* arena, size_t length)
{
    //@Incomplete: Fix NoClear() bug here. 
    // We don't care about zeroing
    auto result = (char*)push_size_(arena, (length + 1));
    result[length] = 0;
    return result;
}


inline void push_string(MemoryArena* arena, char** dst_buffer, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int len = vsnprintf(0, 0, format, args);
    va_end(args);
    
    *dst_buffer = push_string(arena, (u32)len);
    va_start(args, format);
    vsprintf(*dst_buffer, format, args);
    va_end(args);
    
    (*dst_buffer)[len] = '\0';
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
    char* dest = (char*)push_size_(arena, length + 1);
    for(u32 char_index = 0; char_index < length; char_index++)
    {
        dest[char_index] = source[char_index];
    }
    dest[length] = 0;
    
    return dest;
}

char* push_string(MemoryArena* arena, u32 length, const char* source)
{
    char* dest = (char*)push_size_(arena, length + 1);
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

#define bootstrap_push_struct(type, member, ...) (type*)bootstrap_push_size_(sizeof(type), offset_of(type, member), ## __VA_ARGS__)
inline void* bootstrap_push_size_(umm struct_size, umm offset_to_arena,
                                  ArenaBootstrapParams bootstrap_params = default_bootstrap_params(),
                                  PushParams params = default_push_params())
{
    MemoryArena bootstrap = {};
    bootstrap.allocation_flags = bootstrap_params.allocation_flags;
    bootstrap.minimum_block_size = bootstrap_params.minimum_block_size;
    void* new_struct = push_size(&bootstrap, struct_size, void*, params);
    *(MemoryArena *)((u8 *)new_struct + offset_to_arena) = bootstrap;
    
    return new_struct;
}

// Stretchy buffer
struct BufHdr
{
    size_t len;
    size_t cap;
    char buf[1];
};

#define buf__hdr(b) ((BufHdr *)((char *)(b) - offsetof(BufHdr, buf)))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_end(b) ((b) + buf_len(b))
#define buf_sizeof(b) ((b) ? buf_len(b)*sizeof(*b) : 0)

#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)
#define buf_fit(b, n) ((n) <= buf_cap(b) ? 0 : ((b) = (decltype(b))buf__grow((b), (n), sizeof(*(b)))))
#define buf_push(b, ...) (buf_fit(b, 1 + buf_len(b)), (b)[buf__hdr(b)->len++] = (__VA_ARGS__))


void *buf__grow(const void *buf, size_t new_len, size_t elem_size)
{
    assert(buf_cap(buf) <= (SIZE_MAX - 1)/2);
    size_t new_cap = MAX(16, MAX(1 + 2*buf_cap(buf), new_len));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(BufHdr, buf)) / elem_size);
    size_t new_size = offsetof(BufHdr, buf) + new_cap * elem_size;
    BufHdr *new_hdr;
    
    if (buf)
    {
        new_hdr = (BufHdr *)realloc(buf__hdr(buf), new_size);
    }
    else
    {
        new_hdr = (BufHdr *)malloc(new_size);
        new_hdr->len = 0;
    }   
    new_hdr->cap = new_cap;
    return new_hdr->buf;
}

#endif
