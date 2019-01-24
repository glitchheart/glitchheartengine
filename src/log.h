#ifndef LOG_H
#define LOG_H

#include "time.h"
#include <stdarg.h>

#if DEBUG
#define debug_log(Msg, ...) log(Msg, ##__VA_ARGS__)
#define debug_log_error(Msg, ...) log_error(Msg, ##__VA_ARGS__); update_log()
#else
#define debug_log(Msg, ...)
#define debug_log_error(Msg, ...)
#endif

#define log(Msg, ...) _log(LOG_INFO, __LINE__, __FILE__, Msg, ##__VA_ARGS__)
#define log_error(Msg, ...) _log(LOG_ERROR, __LINE__, __FILE__, Msg, ##__VA_ARGS__); update_log()

enum LogType
{
    LOG_INFO,
    LOG_ERROR
};

static void _log(LogType log_type, i32 line_num, const char* file, const char* message, ...)
{
    char message_buffer[4096];
    va_list args;
    va_start(args, message);
    vsnprintf(message_buffer, 4096, message, args); // @Robustness: This could lead to buffer overflow. vsnprintf should be safer?
    va_end(args);
    
    assert(log_state->log_count < MAX_LOG_MESSAGES);
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    log_state->log_buffer[log_state->log_count++] = push_string(&log_state->arena, 2048);
    
    char *type_name = nullptr;
    if(log_type == LOG_INFO)
    {
        type_name = "INFO";
    }
    else if(log_type == LOG_ERROR)
    {
        type_name = "ERROR";
    }
    sprintf(log_state->log_buffer[log_state->log_count - 1], "[%s] - %s file: %s line: %d - %s\n", type_name, buffer, file, line_num, message_buffer);
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
            printf("%s", log_state->log_buffer[log]);
        }
    }
    clear(&log_state->arena);
    log_state->log_count = 0;
}

static void init_log(u32 flags, const char* file_path = "")
{
    if((flags & L_FLAG_FILE) && strlen(file_path) > 0)
    {
        log_state->file.file_handle = fopen(file_path, "a+");
        fwrite("\n[NEW SESSION]\n", sizeof(char), strlen("\n[NEW SESSION]\n"), log_state->file.file_handle);
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
