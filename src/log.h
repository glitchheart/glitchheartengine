#ifndef LOG_H
#define LOG_H

#include "time.h"
#include "stdio.h"


#define Log(Msg) _Log(__LINE__, __FILE__, Msg)
static void _Log(i32 LineNum, const char* File, const char* Message)
{
    Assert(LogState.LogCount < MAX_LOG_MESSAGES);
    time_t Timer;
    char Buffer[26];
    struct tm* TmInfo;
    
    time(&Timer);
    TmInfo = localtime(&Timer);
    
    strftime(Buffer, 26, "%Y-%m-%d %H:%M:%S", TmInfo);
    
    LogState.LogBuffer[LogState.LogCount++] = PushTempString(2048);
    
    sprintf(LogState.LogBuffer[LogState.LogCount - 1], "[INFO] - %s in file %s on line %d - %s\n", Buffer, File, LineNum, Message);
}

static void UpdateLog()
{
    if(LogState.Flags & LFlag_File)
    {
        for(i32 Log = 0; Log < LogState.LogCount; Log++)
        {
            fwrite(LogState.LogBuffer[Log], sizeof(char), strlen(LogState.LogBuffer[Log]), LogState.File.FileHandle);
        }
    }
    
    if(LogState.Flags & LFlag_Debug)
    {
        for(i32 Log = 0; Log < LogState.LogCount; Log++)
        {
            Debug("%s", LogState.LogBuffer[Log]);
        }
    }
    LogState.LogCount = 0;
}

static void InitLog(u32 Flags, const char* FilePath = "")
{
    if((Flags & LFlag_File) && strlen(FilePath) > 0)
    {
        LogState.File.FileHandle = fopen(FilePath, "w");
        LogState.Flags = Flags;
    }
}

static void CloseLog()
{
    if(LogState.Flags & LFlag_File)
    {
        UpdateLog();
        fclose(LogState.File.FileHandle);
    }
}


#endif
