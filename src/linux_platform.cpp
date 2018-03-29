#include <time.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include "unistd.h"
#include "dlfcn.h"

using PlatformHandle = i32;

struct PlatformFile
{
    PlatformHandle handle;
};


inline PLATFORM_FILE_EXISTS(linux_file_exists)
{
    struct stat buffer;
    return (stat(file_path,&buffer) == 0);
}

static b32 copy_file(const char* src, const char* dst, b32 dont_overwrite, MemoryArena* arena = nullptr, b32 binary = true)
{
    FILE* in;
    FILE* out;
    
    if(linux_file_exists(dst) && dont_overwrite)
    {
        return false;
    }
    
    if(binary)
    {
        in = fopen(src, "rb");
    }
    else
    {
        in = fopen(src, "r");
    }
    
    if(in == NULL)
    {
        printf("Failed in\n");
        printf("Src: %s\n", src);
        printf("Dst: %s\n", dst);
        return false;
    }
    
    if(binary)
    {
        out = fopen(dst, "wb");
    }
    else
    {
        out = fopen(dst, "w");
    }
    
    if(out == NULL)
    {
        fclose(in);
        printf("Failed out\n");
        return false;
    }
    
    size_t n,m;
    unsigned char buff[8192];
    do
    {
        n = fread(buff, 1, sizeof(buff), in);
        if(n)
        {
            m = fwrite(buff, 1, n, out);
        }
        else
        {
            m = 0;
        }
    } while ((n > 0) && (n == m));
    if(m)
    {
        printf("COPY\n");
    }
    
    fclose(out);
    fclose(in);
    
    if(binary)
    {
        auto temp_mem = begin_temporary_memory(arena);
        //        system(concat("chmod +xr ", dst, arena));
        end_temporary_memory(temp_mem);
    }
    return true;
}

static time_t get_last_write_time(const char* file_path)
{
    struct stat result;
    if(stat(file_path, &result) == 0)
    {
        auto mod_time = result.st_mtime;
        return mod_time;
    }
    return 0;
}

// @Incomplete:(Niels): Flags?
PLATFORM_LOAD_LIBRARY(linux_load_library)
{
    return dlopen(path, RTLD_LAZY);
}

PLATFORM_FREE_LIBRARY(linux_free_library)
{
    dlclose(library);
}

PLATFORM_LOAD_SYMBOL(linux_load_symbol)
{
    return dlsym(library, symbol);
}

PLATFORM_ALLOCATE_MEMORY(linux_allocate_memory)
{
    Assert(sizeof(MemoryBlock) == 64);
    
    umm page_size = 4096; //TODO: Not really always correct?
    umm total_size = size + sizeof(MemoryBlock);
    umm base_offset = sizeof(MemoryBlock);
    umm protect_offset = 0;
    
    if(flags & PM_UNDERFLOW_CHECK)
    {
        total_size = size + 2 * page_size;
        base_offset = 2 * page_size;
        protect_offset = page_size;
    }
    
    if(flags & PM_OVERFLOW_CHECK)
    {
        umm size_rounded_up = align_pow2(size, page_size);
        total_size = size_rounded_up + 2 * page_size;
        base_offset = page_size + size_rounded_up - size;
        protect_offset = page_size + size_rounded_up;
    }
    
    MemoryBlock* block = (MemoryBlock*)mmap(0, total_size,
                                            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,
                                            -1, 0);
    memset(block, 0, total_size);
    
    Assert(block);
    block->block.base = (u8*)block + base_offset;
    Assert(block->block.used == 0);
    Assert(block->block.prev == 0);
    
    // if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    // {
    //     i32 Protected = mprotect((u8*)Block + ProtectOffset, PageSize, PROT_NONE);
    //     Assert(Protected);
    // }
    
    block->block.size = size;
    block->block.flags = flags;
    
    PlatformMemoryBlock* plat_block = &block->block;
    
    memory_state.blocks++;
    memory_state.size_allocated += total_size;
    
    
    return plat_block;
}

PLATFORM_DEALLOCATE_MEMORY(linux_deallocate_memory)
{
    if(block)
    {
        memory_state.blocks--;
        memory_state.size_allocated -= (block->size + sizeof(MemoryBlock));
        
        
        MemoryBlock *new_block =  ((MemoryBlock*)block);
        munmap(new_block, block->size + sizeof(MemoryBlock));
    }
}


static PLATFORM_OPEN_FILE(linux_open_file)
{
    PlatformFile result = {};
    result.handle = -1;
    
    auto flags = 0;
    
    if((open_flags & (POF_READ | POF_WRITE)) == (POF_READ | POF_WRITE))
    {
        flags = O_RDWR;
    }
    else if(open_flags & POF_WRITE)
    {
        flags = O_WRONLY;
    }
    else
    {
        flags = O_RDONLY;
    }
    
    result.handle = open(path, flags);
    
    return(result);
}

static PLATFORM_CLOSE_FILE(linux_close_file)
{
    close(file.handle);
}

static PLATFORM_WRITE_FILE(linux_write_file)
{
    write(file.handle, src, size_bytes * size);
}

static PLATFORM_READ_FILE(linux_read_file)
{
    read(file.handle, dst, size_bytes * size);
}

static PLATFORM_SEEK_FILE(linux_seek_file)
{
    switch(seek_options)
    {
        case SO_END:
        {
            lseek(file.handle, offset, SEEK_END);
        }
        break;
        case SO_CUR:
        {
            lseek(file.handle, offset, SEEK_CUR);
        }
        break;
        case SO_SET:
        {
            lseek(file.handle, offset, SEEK_SET);
        }
        break;
    }
}

static PLATFORM_TELL_FILE(linux_tell_file)
{
    return (i32)lseek(file.handle, 0, SEEK_CUR);
}


static void init_platform(PlatformApi& platform_api)
{
    platform_api.file_exists = linux_file_exists;
    platform_api.allocate_memory = linux_allocate_memory;
    platform_api.deallocate_memory = linux_deallocate_memory;
    platform_api.load_dynamic_library = linux_load_library;
    platform_api.free_dynamic_library = linux_free_library;
    platform_api.load_symbol = linux_load_symbol;
    platform_api.open_file = linux_open_file;
    platform_api.read_file = linux_read_file;
    platform_api.write_file = linux_write_file;
    platform_api.close_file = linux_close_file;
    platform_api.seek_file = linux_seek_file;
    platform_api.tell_file = linux_tell_file;
}
