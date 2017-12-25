#include "io.h"
#include "Commdlg.h"
#include <windows.h>

static FILETIME GetLastWriteTime(const char* FilePath)
{
    FILETIME LastWriteTime = {};
    
    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(FilePath, &FindData);
    
    if(FindHandle != INVALID_HANDLE_VALUE)
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }
    return LastWriteTime;
}

PLATFORM_LOAD_LIBRARY(Win32LoadLibrary)
{
    return LoadLibrary(Path);
}

PLATFORM_FREE_LIBRARY(Win32FreeLibrary)
{
    if(Library)
    {
        FreeLibrary((HMODULE)Library);
    }
}

PLATFORM_LOAD_SYMBOL(Win32LoadSymbol)
{
    return GetProcAddress((HMODULE)Library, Symbol);
}

PLATFORM_ALLOCATE_MEMORY(Win32AllocateMemory)
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
    
    memory_block* Block  = (memory_block*)VirtualAlloc(0, TotalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    Assert(Block);
    Block->Block.Base = (u8*)Block + BaseOffset;
    Assert(Block->Block.Used == 0);
    Assert(Block->Block.Prev == 0);
    
    if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    {
        DWORD OldProtect = 0;
        BOOL Protected = VirtualProtect((u8*)Block + ProtectOffset, PageSize, PAGE_NOACCESS, &OldProtect);
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

PLATFORM_DEALLOCATE_MEMORY(Win32DeallocateMemory)
{
    if(Block)
    {
        if((Block->Flags & PM_Temporary) == 0)
        {
            MemoryState.PermanentBlocks--;
            MemoryState.PermanentSizeAllocated -= (Block->Size + sizeof(memory_block));
        }
        
        memory_block *NewBlock =  ((memory_block*)Block);
        VirtualFree(NewBlock, 0, MEM_RELEASE);
    }
}

static void ClearTempMemory()
{
    for(i32 Temp = 0; Temp < MemoryState.TempCount; Temp++)
    {
        Win32DeallocateMemory(MemoryState.Blocks[Temp]);
    }
    
    MemoryState.TempCount = 0;
    MemoryState.TempSizeAllocated = 0;
}


inline PLATFORM_GET_ALL_FILES_WITH_EXTENSION(Win32FindFilesWithExtensions)
{
    if(DirectoryData->FilesLength == 0)
    {
        DirectoryData->FileNames = PushTempArray(512, char*);
        DirectoryData->FilePaths = PushTempArray(512, char*);
    }
    
    WIN32_FIND_DATA FindFile;
    HANDLE hFind = NULL;
    
    char Path[2048];
    
    //Process directories
    sprintf(Path, "%s*", DirectoryPath);
    hFind = FindFirstFile(Path, &FindFile);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(FindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(strcmp(FindFile.cFileName, ".") != 0
                   && strcmp(FindFile.cFileName, "..") != 0)
                {
                    char SubPath[2048];
                    sprintf(SubPath, "%s%s/", DirectoryPath, FindFile.cFileName);
                    Win32FindFilesWithExtensions(SubPath, Extension, DirectoryData, WithSubDirectories);
                }
                
            }
        }
        
        while(FindNextFile(hFind, &FindFile));
        FindClose(hFind);
    }
    else
    {
        DEBUG_PRINT("No files with extension %s found in %s\n", Extension, DirectoryPath);
        return;
    }
    
    //Process files
    sprintf(Path, "%s*.%s", DirectoryPath, Extension);
    hFind = FindFirstFile(Path, &FindFile);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(FindFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY))
            {
                if(strcmp(FindFile.cFileName, ".") != 0
                   && strcmp(FindFile.cFileName, "..") != 0)
                {
                    char* ConcatStr = Concat(DirectoryPath, FindFile.cFileName);
                    char* FileName = strtok(FindFile.cFileName, ".");
                    
                    DirectoryData->FilePaths[DirectoryData->FilesLength] = PushTempString(ConcatStr);
                    DirectoryData->FileNames[DirectoryData->FilesLength] = PushTempString(FileName);
                    DirectoryData->FilesLength++;
                }
            }
        } while (FindNextFile(hFind, &FindFile));
        FindClose(hFind);
    }
    else
    {
        DEBUG_PRINT("No files with extension %s found in %s\n", Extension, DirectoryPath);
        return;
    }
}


inline PLATFORM_FILE_EXISTS(Win32FileExists)
{
    struct stat Buffer;
    return (stat(FilePath,&Buffer) == 0);
}

inline PLATFORM_OPEN_FILE_WITH_DIALOG(Win32OpenFileWithDialog)
{
    OPENFILENAME Ofn;
    char SzFile[260];
    platform_file Result = {};
    
    HANDLE Hf;
    
    ZeroMemory(&Ofn, sizeof(Ofn));
    Ofn.lStructSize = sizeof(Ofn);
    Ofn.hwndOwner = 0;
    Ofn.lpstrFile = SzFile;
    Ofn.lpstrFile[0] = '\0';
    Ofn.nMaxFile = sizeof(SzFile);
    if(Extension)
    {
        Ofn.lpstrFilter = Concat(Extension, "\0*.*\0");
    }
    else
    {
        Ofn.lpstrFilter = "All\0*.*\0";
    }
    
    Ofn.nFilterIndex = 1;
    Ofn.lpstrFileTitle = NULL;
    Ofn.nMaxFileTitle = 0;
    Ofn.lpstrInitialDir = NULL;
    Ofn.Flags = OFN_NOCHANGEDIR;
    
    if(GetOpenFileName(&Ofn) == TRUE)
    {
        Hf = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        
        if(Hf != INVALID_HANDLE_VALUE)
        {
            Result.File = _fdopen(_open_osfhandle((imm)Hf, 0), "r");
            strcpy(Result.Path, Ofn.lpstrFile);
            char* P = PushTempString(Result.Path);
            auto Tok = StrSep(&P, ".");
            Tok = StrSep(&P, ".");
            strcpy(Result.Extension, Tok);
        }
    }
    return Result;
}


inline PLATFORM_SAVE_FILE_WITH_DIALOG(Win32SaveFileWithDialog)
{
    OPENFILENAME Ofn;
    char SzFile[260];
    platform_file Result = {};
    
    HANDLE Hf;
    
    ZeroMemory(&Ofn, sizeof(Ofn));
    Ofn.lStructSize = sizeof(Ofn);
    Ofn.hwndOwner = 0;
    Ofn.lpstrFile = SzFile;
    Ofn.lpstrFile[0] = '\0';
    Ofn.nMaxFile = sizeof(SzFile);
    if(Extension)
    {
        Ofn.lpstrFilter = Concat(Extension, "\0*.*\0");
    }
    else
    {
        Ofn.lpstrFilter = "All\0*.*\0";
    }
    
    Ofn.nFilterIndex = 1;
    Ofn.lpstrFileTitle = NULL;
    Ofn.nMaxFileTitle = 0;
    Ofn.lpstrInitialDir = NULL;
    Ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT |OFN_NOCHANGEDIR ;
    
    if(GetSaveFileName(&Ofn) == TRUE)
    {
        if(Extension && !strstr(Ofn.lpstrFile, Extension))
        {
            Hf = CreateFile(Concat(Concat(Ofn.lpstrFile, "."), Extension), GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        }
        else
        {
            Hf = CreateFile(Ofn.lpstrFile, GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        }
        auto Err = GetLastError();
        if(Hf != INVALID_HANDLE_VALUE)
        {
            auto OFlags = Flags & PM_Append ? _O_APPEND : 0;
            auto FDFlags = Flags & PM_Append ? "a" : "w";
            DEBUG_PRINT("Flags: %d\n", OFlags);
            Result.File = _fdopen(_open_osfhandle((imm)Hf, OFlags), FDFlags);
            strcpy(Result.Path, Ofn.lpstrFile);
            if(Extension)
            {
                strcpy(Result.Extension, Extension);
            }
        }
        else
        {
            DEBUG_PRINT("Open file for saving failed with error: %ld\n", Err);
        }
    }
    return Result;
}


inline PLATFORM_GET_TIME_OF_DAY(Win32GetTimeOfDay)
{
    
}

static void InitPlatform(platform_api& PlatformAPI)
{
    PlatformAPI.GetAllFilesWithExtension = Win32FindFilesWithExtensions;
    PlatformAPI.FileExists = Win32FileExists;
    PlatformAPI.AllocateMemory = Win32AllocateMemory;
    PlatformAPI.DeallocateMemory = Win32DeallocateMemory;
    PlatformAPI.OpenFileWithDialog = Win32OpenFileWithDialog;
    PlatformAPI.SaveFileWithDialog = Win32SaveFileWithDialog;
    PlatformAPI.LoadSymbol = Win32LoadSymbol;
}