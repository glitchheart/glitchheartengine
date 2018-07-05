#ifndef LOG_H
#define LOG_H

#include "time.h"

#define Log(Msg) _Log(__LINE__, __FILE__, Msg)
static void log(MemoryArena* memory_arena, i32 line_num, const char* file, const char* message)
{
    assert(log_state->log_count < MAX_LOG_MESSAGES);
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    log_state->log_buffer[log_state->log_count++] = push_string(&log_state->arena, 2048);
    
    sprintf(log_state->log_buffer[log_state->log_count - 1], "[INFO] - %s in file %s on line %d - %s\n", buffer, file, line_num, message);
}

static void update_log()
{
    if(log_state->flags & L_FLAG_FILE)
    {
        for(i32 log = 0; log < log_state->log_count; log++)
        {
            fwrite(log_state->log_buffer[log], sizeof(char), strlen(log_state->log_buffer[log]), log_state->file.file_handle);
        }
    }
    
    if(log_state->flags & L_FLAG_DEBUG)
    {
        for(i32 log = 0; log < log_state->log_count; log++)
        {
            debug("%s", log_state->log_buffer[log]);
        }
    }
    clear(&log_state->arena);
    log_state->log_count= 0;
}

static void init_log(u32 flags, const char* file_path = "")
{
    if((flags & L_FLAG_FILE) && strlen(file_path) > 0)
    {
        log_state->file.file_handle = fopen(file_path, "w");
        log_state->flags = flags;
    }
}

static void close_log()
{
    if(log_state->flags & L_FLAG_FILE)
    {
        update_log();
        fclose(log_state->file.file_handle);
    }
}


#endif
