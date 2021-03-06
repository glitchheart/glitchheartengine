#include "io.h"
#include "Commdlg.h"
#include <windows.h>
#include <timeapi.h>
#include <shellapi.h>
#include <shlwapi.h>

#include "win32_threading.cpp"

struct PlatformHandle
{
    HANDLE handle;
};

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
    assert(sizeof(MemoryBlock) == 72);
    
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
    
    assert(block);
    block->block.base = (u8*)block + base_offset;
    assert(block->block.used == 0);
    assert(block->block.prev == 0);
    
    if(flags & (PM_UNDERFLOW_CHECK | PM_OVERFLOW_CHECK))
    {
        DWORD old_protect = 0;
        BOOL is_protected = VirtualProtect((u8*)block + protect_offset, page_size, PAGE_NOACCESS, &old_protect);
        assert(is_protected);
    }
    
    block->block.size = size;
    block->block.flags = flags;
    
    PlatformMemoryBlock* plat_block = &block->block;
    
    memory_state.blocks++;
    memory_state.size_allocated += total_size;
    
    
    return plat_block;
}

PLATFORM_SLEEP_IS_GRANULAR(win32_sleep_is_granular)
{
    b32 result = timeBeginPeriod(ms) == TIMERR_NOERROR; 
    return(result);
}

PLATFORM_SLEEP(win32_sleep)
{
    Sleep(ms);
}

PLATFORM_DEALLOCATE_MEMORY(win32_deallocate_memory)
{
    if(block)
    {
        memory_state.blocks--;
        memory_state.size_allocated -= (block->size + sizeof(MemoryBlock));
        
        
        MemoryBlock *new_block =  ((MemoryBlock*)block);
        VirtualFree(new_block, 0, MEM_RELEASE);
    }
}

PLATFORM_GET_ALL_DIRECTORIES(win32_get_all_directories)
{
    char **dir_buf = nullptr;
    
    WIN32_FIND_DATA find_file;
    HANDLE h_find = NULL;
    
    // Workaround on Windows, since Windows needs the * to search for all files and OSX + Linux don't
    char combined_path[256];
    
    h_find = FindFirstFile(combined_path, &find_file);
    
    if(h_find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(strcmp(find_file.cFileName, ".") != 0
               && strcmp(find_file.cFileName, "..") != 0 &&
               find_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                char *dir_name = (char *)malloc(strlen(find_file.cFileName) + 1);
                strcpy(dir_name, find_file.cFileName);
                buf_push(dir_buf, dir_name);
            }
        }
        
        while(FindNextFile(h_find, &find_file));
        FindClose(h_find);
    }
    else
    {
        debug("No directories found at %s\n", path);
    }
    return dir_buf;
}

inline PLATFORM_GET_ALL_FILES_WITH_EXTENSION(win32_find_files_with_extensions)
{
    WIN32_FIND_DATA find_file;
    HANDLE h_find = NULL;
    char path[2048];
    
    //Process directories
    sprintf(path, "%s*", directory_path);
    h_find = FindFirstFile(path, &find_file);
    
    if(h_find != INVALID_HANDLE_VALUE && with_sub_directories)
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
    
    //Process files
    sprintf(path, "%s*.%s", directory_path, extension);
    h_find = FindFirstFile(path, &find_file);
    if(h_find != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(find_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                if(strcmp(find_file.cFileName, ".") != 0
                   && strcmp(find_file.cFileName, "..") != 0)
                {
                    char* file_name = strtok(find_file.cFileName, ".");
                    
                    sprintf(directory_data->file_paths[directory_data->file_count], "%s%s.%s", directory_path, find_file.cFileName, extension);
                    sprintf(directory_data->file_names[directory_data->file_count], "%s", file_name);
                    directory_data->file_count++;
                }
            }
        } while (FindNextFile(h_find, &find_file));
        FindClose(h_find);
    }
    else
    {
        debug("No files with extension %s found in %s\n", extension, directory_path);
        return;
    }
}

#include <tchar.h>
#include <strsafe.h>

static b32 _has_extension(const char *str)
{
    size_t len = strlen(str);
    
    for(i32 i = 0; i < len; i++)
    {
        if(str[i] == '.' && i != len - 1 && str[i + 1] != '.')
        {
            printf("Has extension: %s", str);
            return true;
        }
    }

    return false;
}

inline PLATFORM_LIST_ALL_FILES_AND_DIRECTORIES(win32_list_all_files_and_directories)
{
    list->file_count = 0;
    list->dir_count = 0;
    
    strcpy(list->path, path);
    WIN32_FIND_DATA ffd;
    LARGE_INTEGER filesize;
    TCHAR szDir[MAX_PATH];
    size_t length_of_arg;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError=0;
   
    StringCchCopy(szDir, MAX_PATH, path);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind) 
    {
        //DisplayErrorBox(TEXT("FindFirstFile"));
        //return dwError;
    } 
   
    do
    {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            list->dirs[list->dir_count].type = FileType::DIRECTORY;
            strcpy(list->dirs[list->dir_count++].name, ffd.cFileName);
            
        }
        else
        {
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;

			char last_char = ffd.cFileName[strlen(ffd.cFileName) - 1];

            if(last_char == '~')
                continue;
            
            list->files[list->file_count].type = FileType::FILE;
            strcpy(list->files[list->file_count++].name, ffd.cFileName);
        }
    }
    while (FindNextFile(hFind, &ffd) != 0);
 
    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) 
    {
        //DisplayErrorBox(TEXT("FindFirstFile"));
    }

    FindClose(hFind);
}


inline PLATFORM_FILE_EXISTS(win32_file_exists)
{
    struct stat buffer;
    return (stat(file_path, &buffer) == 0);
}

inline PLATFORM_CREATE_DIRECTORY(win32_create_directory)
{
    BOOL result = CreateDirectoryA(path, 0);
    return result;
}

/*
inline PLATFORM_OPEN_FILE_WITH_DIALOG(win32_open_file_with_dialog)
{
    auto temp_mem = begin_temporary_memory(arena);
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
        ofn.lpstrFilter = concat(extension, "\0*.*\0", arena);
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
            char* p = push_string(arena, result.path);
            auto tok = str_sep(&p, ".");
            tok = str_sep(&p, ".");
            strcpy(result.extension, tok);
        }
    }
    end_temporary_memory(temp_mem);
    return result;
}
*/

/*
inline PLATFORM_SAVE_FILE_WITH_DIALOG(win32_save_file_with_dialog)
{
    auto temp_mem = begin_temporary_memory(arena);
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
        ofn.lpstrFilter = concat(extension, "\0*.*\0", arena);
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
            hf = CreateFile(concat(concat(ofn.lpstrFile, ".", arena), extension, arena), GENERIC_READ | GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
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
            debug("Flags: %d\n", o_flags);
            result.file = _fdopen(_open_osfhandle((imm)hf, o_flags), fd_flags);
            strcpy(result.path, ofn.lpstrFile);
            if(extension)
            {
                strcpy(result.extension, extension);
            }
        }
        else
        {
            debug("Open file for saving failed with error: %ld\n", err);
        }
    }
    end_temporary_memory(temp_mem);
    return result;
}
*/

inline i32 Win32ToFileDescriptor(HANDLE h, u32 open_flags)
{
    auto read = (open_flags & POF_READ) ? _O_RDONLY : 0;
    read = (open_flags & POF_WRITE) ? _O_APPEND | read : read;
    return MAX(0, _open_osfhandle((intptr_t)h, read));
}

inline HANDLE FileDescriptorToWin32(i32 i)
{
    return (HANDLE)_get_osfhandle(i);
}

inline PLATFORM_GET_TIME_OF_DAY(win32_get_time_of_day)
{
    
}

static PLATFORM_OPEN_FILE(win32_open_file)
{
    PlatformFile result = {};
    result.handle = 0;
    
    auto read = (open_flags & POF_READ) ? GENERIC_READ : 0;
    auto write = (open_flags & POF_WRITE) ? GENERIC_WRITE : 0;
    auto open = (open_flags & POF_CREATE_ALWAYS) ? CREATE_ALWAYS : (open_flags & POF_OPEN_EXISTING ? OPEN_EXISTING : (open_flags & POF_OPEN_ALWAYS ? OPEN_ALWAYS : 0));
    auto flags = read | write;
    
    auto file = CreateFile(path, (DWORD)flags, 0, 0, (DWORD)open, FILE_ATTRIBUTE_NORMAL, 0);
    
    if(file == INVALID_HANDLE_VALUE && (open_flags & POF_IGNORE_ERROR) == 0)
    {
        auto err = GetLastError();
        log_error("Could not open file: %s failed with error: %d\n", path, err);
    }
    
    result.handle = Win32ToFileDescriptor(file, open_flags);
    
    return(result);
}

static PLATFORM_REMOVE_FILE(win32_remove_file)
{
    WIN32_FIND_DATA find_file;
    HANDLE h_find = NULL;

    h_find = FindFirstFile(path, &find_file);

    if(h_find != INVALID_HANDLE_VALUE)
    {
        if(find_file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            log_error("Could not remove directory\n");
            return false;
        }
    }

    i32 result = DeleteFile(path);
    if(!result)
    {
        DWORD err = GetLastError();
        log_error("Coult not remove file %s: %d\n", path, err);
        return false;
    }
    return true;
}

static PLATFORM_REMOVE_DIRECTORY(win32_remove_directory)
{
    WIN32_FIND_DATA find_dir;
    HANDLE h_find = NULL;

    h_find = FindFirstFile(path, &find_dir);

    if(h_find != INVALID_HANDLE_VALUE)
    {
        if(find_dir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            char file_buf[256];
            sprintf(file_buf, "%s\\*", path);

            if(PathIsDirectoryEmpty(path))
            {
                RemoveDirectory(path);
            }
            else
            {
                log_error("Directory not empty: %s\n", path);
                return false;
            }
            return true;
        }
    }
    return false;
}

static PLATFORM_CLOSE_FILE(win32_close_file)
{
    auto result = CloseHandle(FileDescriptorToWin32(file.handle));
    if(!result)
    {
        auto err = GetLastError();
        log_error("Couldn't close file: %d\n", err);
    }
}

static PLATFORM_WRITE_FILE(win32_write_file)
{
    DWORD bytes_read;
    auto result = WriteFile(FileDescriptorToWin32(file.handle), src, (DWORD)(size_bytes * size), &bytes_read, 0);
    if(!result)
    {
        auto err = GetLastError();
        log_error("Couldn't write file: %d\n", err);
    }
}

static PLATFORM_READ_FILE(win32_read_file)
{
    DWORD bytes_read;
    auto result = ReadFile(FileDescriptorToWin32(file.handle), dst, DWORD(size_bytes * size), &bytes_read, 0);
    if(!result)
    {
        auto err = GetLastError();
        log_error("Couldn't read file: %d\n", err);
    }
}

static PLATFORM_SEEK_FILE(win32_seek_file)
{
    DWORD result = 0;
    switch(seek_options)
    {
        case SO_END:
        {
            result = SetFilePointer(FileDescriptorToWin32(file.handle), offset, 0, FILE_END);
        }
        break;
        case SO_CUR:
        {
            result = SetFilePointer(FileDescriptorToWin32(file.handle), offset, 0, FILE_CURRENT);
        }
        break;
        case SO_SET:
        {
            result = SetFilePointer(FileDescriptorToWin32(file.handle), offset, 0, FILE_BEGIN);
        }
        break;
    }
}

static PLATFORM_TELL_FILE(win32_tell_file)
{
    auto result = SetFilePointer(FileDescriptorToWin32(file.handle), 0, 0, FILE_CURRENT);
    return (i32)result;
}

static PLATFORM_READ_LINE_FILE(win32_read_line_file)
{
    
}

static PLATFORM_PRINT_FILE(win32_print_file)
{
    va_list args;
    va_start(args, format);
    size_t len = (size_t)vsnprintf(nullptr, 0, format, args);
    
    char* buf = (char*)malloc(len + 1);
    vsnprintf(buf, len + 1, format, args);
    buf[len] = 0;
    auto result = WriteFile(FileDescriptorToWin32(file.handle), buf, (DWORD)(len + 1), 0, 0);
    
    va_end(args);
    
    if(!result)
    {
        auto err = GetLastError();
        log_error("Couldn't write '%s' to file with error %d", format, err);
    }
    
    return result;
}

static PLATFORM_IS_EOL(win32_is_eol)
{
    return c == '\n' || c == '\r';
}

static void init_platform(PlatformApi& platform_api)
{
    platform_api.get_all_files_with_extension = win32_find_files_with_extensions;
    platform_api.file_exists = win32_file_exists;
    platform_api.list_all_files_and_directories = win32_list_all_files_and_directories;
    platform_api.allocate_memory = win32_allocate_memory;
    platform_api.deallocate_memory = win32_deallocate_memory;
    platform_api.free_dynamic_library = win32_free_library;
    platform_api.load_dynamic_library = win32_load_library;
    platform_api.load_symbol = win32_load_symbol;
    platform_api.open_file = win32_open_file;
    platform_api.remove_file = win32_remove_file;
    platform_api.remove_directory = win32_remove_directory;
    platform_api.read_file = win32_read_file;
    platform_api.write_file = win32_write_file;
    platform_api.close_file = win32_close_file;
    platform_api.seek_file = win32_seek_file;
    platform_api.tell_file = win32_tell_file;
    platform_api.sleep = win32_sleep;
    platform_api.sleep_is_granular = win32_sleep_is_granular;
    platform_api.print_file = win32_print_file;
    platform_api.get_all_directories = win32_get_all_directories;
    platform_api.create_directory =  win32_create_directory;
    platform_api.is_eol = win32_is_eol;

    // Threading
    platform_api.request_queue = request_queue;
    platform_api.request_thread_info = request_thread_info;
    platform_api.add_entry = add_entry;
    platform_api.complete_all_work = complete_all_work;
    platform_api.make_queue = make_queue;
}
