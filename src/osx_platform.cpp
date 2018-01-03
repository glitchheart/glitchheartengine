#include <time.h>
#include <errno.h>
//#include <sys/sendfile.h>

// @Incomplete: TEMPORARY FOR INCLUDING THE SAME AS LINUX with sendfile.h. Check later if all are necessary
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <sys/mman.h>
#include "unistd.h"
#include "dlfcn.h"

inline PLATFORM_FILE_EXISTS(OSXFileExists)
{
    struct stat Buffer;
    return (stat(FilePath,&Buffer) == 0);
}


static b32 CopyFile(const char* Src, const char* Dst, b32 Overwrite, b32 Binary = false)
{
    FILE* In;
    FILE* Out;
    
    if(OSXFileExists(Dst) && !Overwrite)
    {
        return false;
    }
    
    if(Binary)
    {
        In = fopen(Src, "rb");
    }
    else
    {
        In = fopen(Src, "r");
    }
    
    if(In == NULL)
    {
        printf("Failed in\n");
        printf("Src: %s\n", Src);
        printf("Dst: %s\n", Dst);
        return false;
    }
    
    if(Binary)
    {
        Out = fopen(Dst, "wb");
    }
    else
    {
        Out = fopen(Dst, "w");
    }
    
    if(Out == NULL)
    {
        fclose(In);
        printf("Failed out\n");
        return false;
    }
    
    size_t N,M;
    unsigned char Buff[8192];
    do
    {
        N = fread(Buff, 1, sizeof(Buff), In);
        if(N)
        {
            M = fwrite(Buff, 1, N, Out);
        }
        else
        {
            M = 0;
        }
    } while ((N > 0) && (N == M));
    if(M)
    {
        printf("COPY\n");
    }
    
    fclose(Out);
    fclose(In);
    
    if(Binary)
    {
        system(Concat("chmod +xr ", Dst));
    }
    return true;
}

static time_t GetLastWriteTime(const char* FilePath)
{
    struct stat Result;
    if(stat(FilePath, &Result) == 0)
    {
        auto ModTime = Result.st_mtime;
        return ModTime;
    }
    return 0;
}

// @Incomplete:(Niels): Flags?
PLATFORM_LOAD_LIBRARY(OSXLoadLibrary)
{
    return dlopen(Path, RTLD_LAZY);
}

PLATFORM_FREE_LIBRARY(OSXFreeLibrary)
{
    dlclose(Library);
}

PLATFORM_LOAD_SYMBOL(OSXLoadSymbol)
{
    return dlsym(Library, Symbol);
}


PLATFORM_ALLOCATE_MEMORY(OSXAllocateMemory)
{
    Assert(sizeof(memory_block) == 64);
    
    umm PageSize = 4096; //TODO: Not really always correct?
    umm TotalSize = Size + sizeof(memory_block);
    umm BaseOffset = sizeof(memory_block);
    umm ProtectOffset = 0;
    
    if(Flags & PM_UnderflowCheck)
    {
        TotalSize = Size + 2 * PageSize;
        BaseOffset = 2 * PageSize;
        ProtectOffset = PageSize;
    }
    
    if(Flags & PM_OverflowCheck)
    {
        umm SizeRoundedUp = AlignPow2(Size, PageSize);
        TotalSize = SizeRoundedUp + 2 * PageSize;
        BaseOffset = PageSize + SizeRoundedUp - Size;
        ProtectOffset = PageSize + SizeRoundedUp;
    }
    
    memory_block* Block = (memory_block*)malloc(TotalSize);
    memset(Block, 0, TotalSize);
    
    Assert(Block);
    Block->Block.Base = (u8*)Block + BaseOffset;
    Assert(Block->Block.Used == 0);
    Assert(Block->Block.Prev == 0);
    
    if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    {
        i32 Protected = mprotect((u8*)Block + ProtectOffset, PageSize, PROT_NONE);
        Assert(Protected);
    }
    
    Block->Block.Size = Size;
    Block->Block.Flags = Flags;
    
    platform_memory_block* PlatBlock = &Block->Block;
    
    if(Flags & PM_Temporary)
    {
        Assert((MemoryState.TempCount + 1) < MAX_TEMP_BLOCKS);
        MemoryState.TempSizeAllocated += TotalSize;
        MemoryState.Blocks[MemoryState.TempCount++] = PlatBlock;
    }
    else
    {
        MemoryState.PermanentBlocks++;
        MemoryState.PermanentSizeAllocated += TotalSize;
    }
    
    return PlatBlock;
}

PLATFORM_DEALLOCATE_MEMORY(OSXDeallocateMemory)
{
    if(Block)
    {
        if((Block->Flags & PM_Temporary) == 0)
        {
            MemoryState.PermanentBlocks--;
            MemoryState.PermanentSizeAllocated -= (Block->Size + sizeof(memory_block));
        }
        
        memory_block *NewBlock =  ((memory_block*)Block);
        free(NewBlock);
    }
}

static void ClearTempMemory()
{
    for(i32 Temp = 0; Temp < MemoryState.TempCount; Temp++)
    {
        OSXDeallocateMemory(MemoryState.Blocks[Temp]);
    }
    
    MemoryState.TempCount = 0;
    MemoryState.TempSizeAllocated = 0;
}

static void InitPlatform(platform_api& PlatformAPI)
{
    PlatformAPI.AllocateMemory = OSXAllocateMemory;
    PlatformAPI.DeallocateMemory = OSXDeallocateMemory;
    PlatformAPI.LoadDynamicLibrary = OSXLoadLibrary;
    PlatformAPI.FreeDynamicLibrary = OSXFreeLibrary;
    PlatformAPI.LoadSymbol = OSXLoadSymbol;
}


