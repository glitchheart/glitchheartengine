#include <time.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include "unistd.h"
#include "dlfcn.h"

inline PLATFORM_FILE_EXISTS(LinuxFileExists)
{
    struct stat buffer;
    return (stat(FilePath,&buffer) == 0);
}

static b32 copy_file(const char* src, const char* dst, b32 dont_overwrite, b32 binary = true)
{
    FILE* in;
    FILE* out;
    
    if(LinuxFileExists(dst) && dont_overwrite)
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
        system(concat("chmod +xr ", dst));
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
PLATFORM_LOAD_LIBRARY(LinuxLoadLibrary)
{
    return dlopen(Path, RTLD_LAZY);
}

PLATFORM_FREE_LIBRARY(LinuxFreeLibrary)
{
    dlclose(Library);
}

PLATFORM_LOAD_SYMBOL(LinuxLoadSymbol)
{
    return dlsym(Library, Symbol);
}


PLATFORM_ALLOCATE_MEMORY(LinuxAllocateMemory)
{
    Assert(sizeof(memory_block) == 64);
    
    umm PageSize = 4096; //TODO: Not really always correct?
    umm total_size = Size + sizeof(memory_block);
    umm base_offset = sizeof(memory_block);
    umm protect_offset = 0;
    
    if(Flags & PM_UnderflowCheck)
    {
        total_size = Size + 2 * PageSize;
        base_offset = 2 * PageSize;
        protect_offset = PageSize;
    }
    
    if(Flags & PM_OverflowCheck)
    {
        umm size_rounded_up = AlignPow2(Size, PageSize);
        total_size = size_rounded_up + 2 * PageSize;
        base_offset = PageSize + size_rounded_up - Size;
        protect_offset = PageSize + size_rounded_up;
    }
    
    memory_block* Block = (memory_block*)malloc(total_size);
    memset(Block, 0, total_size);
    
    Assert(Block);
    Block->Block.Base = (u8*)Block + base_offset;
    Assert(Block->Block.Used == 0);
    Assert(Block->Block.Prev == 0);
    
    // if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    // {
    //     i32 Protected = mprotect((u8*)Block + ProtectOffset, PageSize, PROT_NONE);
    //     Assert(Protected);
    // }
    
    Block->Block.Size = Size;
    Block->Block.Flags = Flags;
    
    platform_memory_block* plat_block = &Block->Block;
    
    if(Flags & PM_Temporary)
    {
        Assert((MemoryState.TempCount + 1) < MAX_TEMP_BLOCKS);
        MemoryState.TempSizeAllocated += total_size;
        MemoryState.Blocks[MemoryState.TempCount++] = plat_block;
    }
    else
    {
        MemoryState.PermanentBlocks++;
        MemoryState.PermanentSizeAllocated += total_size;
    }
    
    return plat_block;
}

PLATFORM_DEALLOCATE_MEMORY(LinuxDeallocateMemory)
{
    if(Block)
    {
        if((Block->Flags & PM_Temporary) == 0)
        {
            MemoryState.PermanentBlocks--;
            MemoryState.PermanentSizeAllocated -= (Block->Size + sizeof(memory_block));
        }
        
        memory_block *new_block =  ((memory_block*)Block);
        free(new_block);
    }
}

static void clear_temp_memory()
{
    for(i32 temp = 0; temp < MemoryState.TempCount; temp++)
    {
        LinuxDeallocateMemory(MemoryState.Blocks[temp]);
    }
    
    MemoryState.TempCount = 0;
    MemoryState.TempSizeAllocated = 0;
}

static void init_platform(PlatformApi& platform_api)
{
    platform_api.FileExists = LinuxFileExists;
    platform_api.AllocateMemory = LinuxAllocateMemory;
    platform_api.DeallocateMemory = LinuxDeallocateMemory;
    platform_api.LoadDynamicLibrary = LinuxLoadLibrary;
    platform_api.FreeDynamicLibrary = LinuxFreeLibrary;
    platform_api.LoadSymbol = LinuxLoadSymbol;
}
