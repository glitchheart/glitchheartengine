#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/mman.h>
#include "unistd.h"
#include "dlfcn.h"
#include <mach/error.h>
#include <dirent.h>
#include <sys/stat.h>

using PlatformHandle = i32;

struct PlatformFile
{
    PlatformHandle handle;
};

inline PLATFORM_FILE_EXISTS(osx_file_exists)
{
    struct stat buffer;
    return (stat(file_path,&buffer) == 0);
}

static b32 copy_file(const char* src, const char* dst, b32 dont_overwrite, MemoryArena* arena = nullptr, b32 binary = false)
{
    FILE* in;
    FILE* out;
    
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
    
    size_t n, m;
    unsigned char buff[8192];
    do
    {
        n= fread(buff, 1, sizeof(buff), in);
        if(n)
        {
            m = fwrite(buff, 1, n, out);
        }
        else
        {
            n = 0;
        }
    }
    while ((n > 0) && (n == m));
    if(m)
    {
        printf("COPY\n");
    }
    
    fclose(out);
    fclose(in);
    
    if(binary)
    {
        system(concat("chmod +xr ", dst, arena));
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
PLATFORM_LOAD_LIBRARY(osx_load_library)
{
    return dlopen(path, RTLD_LAZY);
}

PLATFORM_FREE_LIBRARY(osx_free_library)
{
    dlclose(library);
}

PLATFORM_LOAD_SYMBOL(osx_load_symbol)
{
    return dlsym(library, symbol);
}


PLATFORM_ALLOCATE_MEMORY(osx_allocate_memory)
{
    assert(sizeof(MemoryBlock) == 64);
    
    umm page_size = sysconf(_SC_PAGE_SIZE);
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
    
    MemoryBlock* block = (MemoryBlock*)malloc(total_size);
    memset(block, 0, total_size);
    
    assert(block);
    block->block.base = (u8*)block + base_offset;
    assert(block->block.used == 0);
    assert(block->block.prev == 0);
    
    //if(flags & (PM_UNDERFLOW_CHECK | PM_OVERFLOW_CHECK))
    //{
    //i32 Protected = mprotect((u8*)Block + protect_offset, page_size, PROT_NONE);
    //printf("Last error: %d\n", Protected);
    //printf("Error %d\n", errno);
    //assert(Protected == 0);
    //}
    
    block->block.size = size;
    block->block.flags = flags;
    
    PlatformMemoryBlock* plat_block = &block->block;
    
    memory_state.blocks++;
    memory_state.size_allocated += total_size;
    
    return plat_block;
}

PLATFORM_DEALLOCATE_MEMORY(osx_deallocate_memory)
{
    if(block)
    {
        if((block->flags & PM_TEMPORARY) == 0)
        {
            memory_state.blocks--;
            memory_state.size_allocated -= (block->size + sizeof(MemoryBlock));
        }
        
        MemoryBlock *new_block =  ((MemoryBlock*)block);
        free(new_block);
    }
}

static PLATFORM_OPEN_FILE(osx_open_file)
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

static PLATFORM_CLOSE_FILE(osx_close_file)
{
    close(file.handle);
}

static PLATFORM_WRITE_FILE(osx_write_file)
{
    write(file.handle, src, size_bytes * size);
}

static PLATFORM_READ_FILE(osx_read_file)
{
    read(file.handle, dst, size_bytes * size);
}

static PLATFORM_SEEK_FILE(osx_seek_file)
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

static PLATFORM_TELL_FILE(osx_tell_file)
{
    return (i32)lseek(file.handle, 0, SEEK_CUR);
}

PLATFORM_GET_ALL_DIRECTORIES(osx_get_all_directories)
{
    char ** dir_buf = nullptr;
    struct dirent *de;  // Pointer for directory entry
 
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir(path);
 
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        printf("Could not open current directory" );
        return nullptr;
    }
 
    // Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html
    // for readdir()
    while ((de = readdir(dr)) != NULL)
    {
        if(de->d_type & DT_DIR)
        {
            char *dir_name = (char *)malloc(strlen(de->d_name) + 1);
            strcpy(dir_name, de->d_name);
            buf_push(dir_buf, dir_name);
        }
    }
 
    closedir(dr);    
    
    return dir_buf;
}

static void init_platform(PlatformApi& platform_api)
{
    platform_api.get_all_directories = osx_get_all_directories;
    platform_api.allocate_memory = osx_allocate_memory;
    platform_api.deallocate_memory = osx_deallocate_memory;
    platform_api.load_dynamic_library = osx_load_library;
    platform_api.free_dynamic_library = osx_free_library;
    platform_api.load_symbol = osx_load_symbol;
    platform_api.open_file = osx_open_file;
    platform_api.read_file = osx_read_file;
    platform_api.write_file = osx_write_file;
    platform_api.close_file = osx_close_file;
    platform_api.seek_file = osx_seek_file;
    platform_api.tell_file = osx_tell_file;
}


