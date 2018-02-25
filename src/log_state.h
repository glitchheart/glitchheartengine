#ifndef LOG_STATE_H
#define LOG_STATE_H


#define MAX_LOG_MESSAGES 2048


enum Log_Flags
{
    LFlag_File = (1 << 0),
    LFlag_Debug = (2 << 0)
};

struct log_state
{
    char* LogBuffer[MAX_LOG_MESSAGES];
    i32 LogCount;
    
    u32 Flags;
    
    union
    {
        struct
        {
            char* FilePath;
            FILE* FileHandle;
        } File;
        struct
        {
            
        } PrintLogging;
    };
    
};

extern log_state LogState;

#endif

