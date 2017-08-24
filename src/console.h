#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_BUFFER_SIZE 100
#define HISTORY_BUFFER_LINES 7

struct game_state;
struct transient_state;

struct command_info
{
    char* Name;
    char* (*FunctionPointer)(game_state*, char**);
};

#define COMMAND_ARRAY_SIZE 10

static u32 CommandCount;
static command_info Commands[COMMAND_ARRAY_SIZE];

struct console
{
    bool Open;
    math::v3 EndPosition = math::v3(-1, 0.5f, 0);
    r32 TimeToAnimate = 0.3f;
    r64 CurrentTime;
    r64 DeleteTime;
    math::v4 Color;
    math::v3 CursorColor;
    u32 MaxHeight;
    u32 BufferIndex;
    char Buffer[CONSOLE_BUFFER_SIZE];
    char HistoryBuffer[HISTORY_BUFFER_LINES][CONSOLE_BUFFER_SIZE + 20]; //NOTE(Daniel) + 20 to make room for : command not found
};

#endif