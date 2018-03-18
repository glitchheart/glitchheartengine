#ifndef LOG_H
#define LOG_H

#include "time.h"
#include "stdio.h"


#define Log(Msg) _Log(__LINE__, __FILE__, Msg)
static void log(i32 line_num, const char* file, const char* message)
{
    Assert(LogState.LogCount < MAX_LOG_MESSAGES);
    time_t timer;
    char buffer[26];
    struct tm* tm_info;
    
    time(&timer);
    tm_info = localtime(&timer);
    
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    LogState.LogBuffer[LogState.LogCount++] = push_temp_string(2048);
    
    sprintf(LogState.LogBuffer[LogState.LogCount - 1], "[INFO] - %s in file %s on line %d - %s\n", buffer, file, line_num, message);
}

static void update_log()
{
    if(LogState.Flags & LFlag_File)
    {
        for(i32 log = 0; log < LogState.LogCount; log++)
        {
            fwrite(LogState.LogBuffer[log], sizeof(char), strlen(LogState.LogBuffer[log]), LogState.File.FileHandle);
        }
    }
    
    if(LogState.Flags & LFlag_Debug)
    {
        for(i32 log = 0; Log < LogState.LogCount; log++)
        {
            Debug("%s", LogState.LogBuffer[Log]);
        }
    }
    LogState.LogCount = 0;
}

static void init_log(u32 flags, const char* file_path = "")
{
    if((flags & LFlag_File) && strlen(file_path) > 0)
    {
        LogState.File.FileHandle = fopen(file_path, "w");
        LogState.Flags = flags;
    }
}

static void close_log()
{
    if(LogState.Flags & LFlag_File)
    {
        update_log();
        fclose(LogState.File.FileHandle);
    }
}


#endif
