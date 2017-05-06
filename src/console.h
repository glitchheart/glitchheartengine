#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_BUFFER_SIZE 100
#define HISTORY_BUFFER_LINES 7

struct game_state;

struct command_info
{
    char* Name;
    char* (*FunctionPointer)(game_state*, char*);
};

#define COMMAND_ARRAY_SIZE 10

static uint32 CommandCount;
static command_info Commands[COMMAND_ARRAY_SIZE];

struct console
{
    bool Open;
    glm::vec3 EndPosition = glm::vec3(-1, 0.5, 0);
    real64 TimeToAnimate = 0.3f;
    real64 CurrentTime;
    real64 DeleteTime;
    glm::vec4 Color;
    glm::vec3 CursorColor;
    uint32 MaxHeight;
    uint32 BufferIndex;
    char Buffer[CONSOLE_BUFFER_SIZE];
    char HistoryBuffer[HISTORY_BUFFER_LINES][CONSOLE_BUFFER_SIZE + 20]; //NOTE(Daniel) + 20 to make room for : command not found
};

#endif