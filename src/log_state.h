#ifndef LOG_STATE_H
#define LOG_STATE_H


#define MAX_LOG_MESSAGES 2048


enum LogFlags
{
    LFlag_File = (1 << 0),
    LFlag_Debug = (2 << 0)
};

struct LogState
{
    char* log_buffer[MAX_LOG_MESSAGES];
    i32 log_count;
    
    u32 flags;
    
    union
    {
        struct
        {
            char* file_path;
            FILE* file_handle;
        } file;
        struct
        {
            
        } print_logging;
    };
    
};

extern LogState LogState;

#endif

