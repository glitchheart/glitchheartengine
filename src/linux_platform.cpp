#include <time.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include "unistd.h"
#include <dirent.h>
#include "dlfcn.h"
#include "linux_threading.cpp"

struct PlatformHandle
{
    i32 handle;
};

inline PLATFORM_FILE_EXISTS(linux_file_exists)
{
    struct stat buffer;
    return (stat(file_path,&buffer) == 0);
}

inline PLATFORM_CREATE_DIRECTORY(linux_create_directory)
{
    i32 result = mkdir(path, S_IRWXU);
    
    if(!result)
    {
        return false;
    }
    
    return true;
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
        // auto temp_mem = begin_temporary_memory(arena);
        //        system(concat("chmod +xr ", dst, arena));
        // end_temporary_memory(temp_mem);
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

// @Incomplete(Niels): Flags?
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
    
    MemoryBlock* block = (MemoryBlock*)mmap(0, total_size,
                                            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON,
                                            -1, 0);
    //memset(block, 0, total_size);
    
    assert(block);
    block->block.base = (u8*)block + base_offset;
    assert(block->block.used == 0);
    assert(block->block.prev == 0);
    
    // if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    // {
    //     i32 Protected = mprotect((u8*)Block + ProtectOffset, PageSize, PROT_NONE);
    //     assert(Protected);
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
    if(result.handle == -1)
    {
        log_error("ERROR in open file: %s", strerror(errno));
    }
    
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

static PLATFORM_READ_LINE_FILE(linux_read_line_file)
{
    assert(false);
    return nullptr;
}

static PLATFORM_PRINT_FILE(linux_print_file)
{
    FILE* f = fdopen(file.handle, "wr");
    if(f)
    {
	va_list args;
	va_start(args, format);
	fprintf(f, format, args);
	va_end(args);
    }
    return 0;
}

PLATFORM_GET_ALL_FILES_WITH_EXTENSION(linux_get_all_files_with_extension)
{
    struct dirent *de;

    DIR *dr = opendir(directory_path);

    if(dr == nullptr)
    {
        log_error("Could not open current directory");
        return;
    }

    while((de = readdir(dr)) != nullptr)
    {
        if(with_sub_directories && de->d_type == DT_DIR)
        {
            if(strcmp(de->d_name, ".") != 0 &&
               strcmp(de->d_name, "..") != 0)
            {
                char sub_path[2048];
                sprintf(sub_path, "%s%s/", directory_path, de->d_name);
                linux_get_all_files_with_extension(sub_path, extension, directory_data, true);
            }
        }
        else if(de->d_type == DT_REG)
        {
            const char *ext = strrchr(de->d_name, '.');

            if((ext) && (ext != de->d_name))
            {
                if(strcmp((++ext), extension) == 0)
                {
                    char sub_path[2048];
                    sprintf(sub_path, "%s%s", directory_path, de->d_name);

                    char *file_name = strtok(de->d_name, ".");

                    strcpy(directory_data->file_paths[directory_data->file_count], sub_path);
                    strcpy(directory_data->file_names[directory_data->file_count], file_name);
                    directory_data->file_count++;
                }
            }
        }        
    }
    closedir(dr);    
}

PLATFORM_GET_ALL_DIRECTORIES(linux_get_all_directories)
{
    char ** dir_buf = nullptr;
    struct dirent *de;
    
    DIR *dr = opendir(path);
    
    if (dr == NULL)
    {
        printf("Could not open current directory" );
        return nullptr;
    }
    
    while ((de = readdir(dr)) != NULL)
    {
        if(de->d_type & DT_DIR)
        {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;
            
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
    platform_api.get_all_files_with_extension = linux_get_all_files_with_extension;
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
    platform_api.print_file = linux_print_file;
    platform_api.get_all_directories = linux_get_all_directories;
    platform_api.create_directory = linux_create_directory;

    // Threading
    platform_api.add_entry = add_entry;
    platform_api.complete_all_work = complete_all_work;
    platform_api.make_queue = make_queue;
}
