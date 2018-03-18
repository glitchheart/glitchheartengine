#include "io.h"
#include "Commdlg.h"
#include <windows.h>

#define copy_file(game_library_path, temp_game_library_path, overwrite) CopyFile(game_library_path, temp_game_library_path, overwrite)

time_t file_time_to_time_t(const FILETIME& ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;
    
    return (time_t)(ull.QuadPart / 10000000ULL - 11644473600ULL);
}

static time_t get_last_write_time(const char* file_path)
{
    FILETIME last_write_time = {};
    
    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFileA(file_path, &find_data);
    
    if(find_handle != INVALID_HANDLE_VALUE)
    {
        last_write_time = find_data.ftLastWriteTime;
        FindClose(find_handle);
    }
    
    return file_time_to_time_t(last_write_time);
}

PLATFORM_LOAD_LIBRARY(win32_load_library)
{
    return LoadLibraryA(path);
}

PLATFORM_FREE_LIBRARY(win32_free_library)
{
    FreeLibrary((HMODULE)library);
}

PLATFORM_LOAD_SYMBOL(win32_load_symbol)
{
    return GetProcAddress((HMODULE)library, symbol);
}

PLATFORM_ALLOCATE_MEMORY(win32_allocate_memory)
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
    
    MemoryBlock* block  = (MemoryBlock*)VirtualAlloc(0, total_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    Assert(block);
    block->block.base = (u8*)block + base_offset;
    Assert(block->block.used == 0);
    Assert(block->block.prev == 0);
    
    if(flags & (PM_UNDERFLOW_CHECK | PM_OVERFLOW_CHECK))
    {
        DWORD old_protect = 0;
        BOOL is_protected = VirtualProtect((u8*)block + protect_offset, page_size, PAGE_NOACCESS, &old_protect);
        Assert(is_protected);
    }
    
    block->block.size = size;
    block->block.flags = flags;
    
    PlatformMemoryBlock* plat_block = &block->block;
    
    if(flags & PM_TEMPORARY)
    {
        Assert((memory_state.temp_count + 1) < MAX_TEMP_BLOCKS);
        memory_state.temp_size_allocated += total_size;
        memory_state.blocks[memory_state.temp_count++] = plat_block;
    }
    else
    {
        memory_state.permanent_blocks++;
        memory_state.permanent_size_allocated += total_size;
    }
    
    return plat_block;
}

PLATFORM_DEALLOCATE_MEMORY(win32_deallocate_memory)
{
    if(block)
    {
        if((block->flags & PM_TEMPORARY) == 0)
        {
            memory_state.permanent_blocks--;
            memory_state.permanent_size_allocated -= (block->size + sizeof(MemoryBlock));
        }
        
        MemoryBlock *new_block =  ((MemoryBlock*)block);
        VirtualFree(new_block, 0, MEM_RELEASE);
    }
}

static void clear_temp_memory()
{
    for(i32 temp = 0; temp < memory_state.temp_count; temp++)
    {
        win32_deallocate_memory(memory_state.blocks[temp]);
    }
    
    memory_state.temp_count = 0;
    memory_state.temp_size_allocated = 0;
}

inline PLATFORM_GET_ALL_FILES_WITH_EXTENSION(win32_find_files_with_extensions)
{
    if(directory_data->files_length == 0)
    {
        directory_data->file_names = push_temp_array(512, char*);
        directory_data->file_paths = push_temp_array(512, char*);
    }
    
    WIN32_FIND_DATA find_file;
    HANDLE h_find = NULL;
    
    char path[2048];
    
    //Process directories
    sprintf(path, "%s*", directory_path);
    h_find = FindFirstFile(path, &find_file);
    if(h_find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(find_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(strcmp(find_file.cFileName, ".") != 0
                   && strcmp(find_file.cFileName, "..") != 0)
                {
                    char sub_path[2048];
                    sprintf(sub_path, "%s%s/", directory_path, find_file.cFileName);
                    win32_find_files_with_extensions(sub_path, extension, directory_data, with_sub_directories);
                }
                
            }
        }
        
        while(FindNextFile(h_find, &find_file));
        FindClose(h_find);
    }
    else
    {
        Debug("No files with extension %s found in %s\n", extension, directory_path);
        return;
    }
    
    //Process files
    sprintf(path, "%s*.%s", directory_path, extension);
    h_find = FindFirstFile(path, &find_file);
    if(h_find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(find_file.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY))
            {
                if(strcmp(find_file.cFileName, ".") != 0
                   && strcmp(find_file.cFileName, "..") != 0)
                {
                    char* concat_str = concat(directory_path, find_file.cFileName);
                    char* file_name = strtok(find_file.cFileName, ".");
                    
                    directory_data->file_paths[directory_data->files_length] = push_temp_string(concat_str);
                    directory_data->file_names[directory_data->files_length] = push_temp_string(file_name);
                    directory_data->files_length++;
                }
            }
        } while (FindNextFile(h_find, &find_file));
        FindClose(h_find);
    }
    else
    {
        Debug("No files with extension %s found in %s\n", extension, directory_path);
        return;
    }
}


inline PLATFORM_FILE_EXISTS(win32_file_exists)
{
    struct stat buffer;
    return (stat(file_path,&buffer) == 0);
}

inline PLATFORM_OPEN_FILE_WITH_DIALOG(win32_open_file_with_dialog)
{
    OPENFILENAME ofn;
    char sz_file[260];
    PlatformFile result = {};
    
    HANDLE hf;
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = 0;
    ofn.lpstrFile = sz_file;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(sz_file);
    if(extension)
    {
        ofn.lpstrFilter = concat(extension, "\0*.*\0");
    }
    else
    {
        ofn.lpstrFilter = "All\0*.*\0";
    }
    
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_NOCHANGEDIR;
    
    if(GetOpenFileName(&ofn) == TRUE)
    {
        hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        
        if(hf != INVALID_HANDLE_VALUE)
        {
            result.file = _fdopen(_open_osfhandle((imm)hf, 0), "r");
            strcpy(result.path, ofn.lpstrFile);
            char* p = push_temp_string(result.path);
            auto tok = str_sep(&p, ".");
            tok = str_sep(&p, ".");
            strcpy(result.extension, tok);
        }
    }
    return result;
}


inline PLATFORM_SAVE_FILE_WITH_DIALOG(win32_save_file_with_dialog)
{
    OPENFILENAME ofn;
    char sz_file[260];
    PlatformFile result = {};
    
    HANDLE hf;
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = 0;
    ofn.lpstrFile = sz_file;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(sz_file);
    if(extension)
    {
        ofn.lpstrFilter = concat(extension, "\0*.*\0");
    }
    else
    {
        ofn.lpstrFilter = "All\0*.*\0";
    }
    
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT |OFN_NOCHANGEDIR ;
    
    if(GetSaveFileName(&ofn) == TRUE)
    {
        if(extension && !strstr(ofn.lpstrFile, extension))
        {
            hf = CreateFile(concat(concat(ofn.lpstrFile, "."), extension), GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        }
        else
        {
            hf = CreateFile(ofn.lpstrFile, GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        }
        
        auto err = GetLastError();
        if(hf != INVALID_HANDLE_VALUE)
        {
            auto o_flags = flags & PM_APPEND ? _O_APPEND : 0;
            auto fd_flags = flags & PM_APPEND ? "a" : "w";
            Debug("Flags: %d\n", o_flags);
            result.file = _fdopen(_open_osfhandle((imm)hf, o_flags), fd_flags);
            strcpy(result.path, ofn.lpstrFile);
            if(extension)
            {
                strcpy(result.extension, extension);
            }
        }
        else
        {
            Debug("Open file for saving failed with error: %ld\n", err);
        }
    }
    return result;
}


inline PLATFORM_GET_TIME_OF_DAY(win32_get_time_of_day)
{
    
}

static void init_platform(PlatformApi& platform_api)
{
    platform_api.get_all_files_with_extension = win32_find_files_with_extensions;
    platform_api.file_exists = win32_file_exists;
    platform_api.allocate_memory = win32_allocate_memory;
    platform_api.deallocate_memory = win32_deallocate_memory;
    platform_api.open_file_with_dialog = win32_open_file_with_dialog;
    platform_api.save_file_with_dialog = win32_save_file_with_dialog;
    platform_api.load_symbol = win32_load_symbol;
    platform_api.free_dynamic_library = win32_free_library;
    platform_api.load_dynamic_library = win32_load_library;
}