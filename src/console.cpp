#include "console_commands.h"

static void AddCommand(char* Name, char* (*FunctionPointer)(game_state*, char*))
{
    command_info Info = { Name, FunctionPointer };
    Commands[CommandCount++] = Info;
}

//call this before using the console
static void InitCommands()
{
    AddCommand("zoom", &Zoom);
    AddCommand("jump", &Jump);
    AddCommand("exit", &Exit);
    AddCommand("view", &View);
    AddCommand("editor", &Editor);
}

void ExecuteCommand(game_state *GameState)
{
    if(strcmp(" ",  GameState->Console.Buffer) != 0
       && strcmp("",  GameState->Console.Buffer) != 0) //NOTE(Daniel) if the command isn't an empty string
    {
        char* Result = &GameState->Console.Buffer[0];
        
        char ResultCopy[40];
        strcpy(&ResultCopy[0], Result);
        
        char* Pointer;
        char* CommandName;
        
        Pointer = strtok(&ResultCopy[0], " ");
        CommandName = Pointer;
        
        int Count = 0;
        
        char* ArgumentBuffer[10];
        
        while(Pointer != NULL)
        {
            Pointer = strtok(NULL, " ");
            ArgumentBuffer[Count] = Pointer;
            printf("%d\n", Count);
            Count++;
        }
        
        bool32 Found;
        
        for(int i = 0; i < CommandCount; i++)
        {
            if(strcmp(CommandName, Commands[i].Name) == 0)
            {
                Found = true;
                Result = Commands[i].FunctionPointer(GameState, ArgumentBuffer[0]);
                break;
            }
        }
        
        if(!Found)
        {
            Result = CombineStrings(Result, ": Command not found");
        }
        
        //Copy the command into the history buffer
        for(int i = HISTORY_BUFFER_LINES - 1; i > 0; i--)
        {
            sprintf(GameState->Console.HistoryBuffer[i], GameState->Console.HistoryBuffer[i - 2]);
        }
        
        sprintf(GameState->Console.HistoryBuffer[0], Result);
        sprintf(GameState->Console.HistoryBuffer[1], &GameState->Console.Buffer[0]);
        
        for(int i = 0; i < CONSOLE_BUFFER_SIZE; i++)
            GameState->Console.Buffer[i] = '\0';
        
        GameState->Console.BufferIndex = 0;
    }
}

static void CheckConsoleInput(game_state* GameState, real64 DeltaTime)
{
    if(GameState->Console.Open && GameState->Console.CurrentTime < GameState->Console.TimeToAnimate)
    {
        GameState->Console.CurrentTime += DeltaTime;
    }
    else if(!GameState->Console.Open && GameState->Console.CurrentTime > 0)
    {
        GameState->Console.CurrentTime -= DeltaTime;
    }
    
    if (GetKeyDown(Key_Tab, GameState))
    {
        GameState->Console.Open = !GameState->Console.Open;
        
        if(GameState->Console.Open)
            GameState->Console.CurrentTime = 0.0f;
        else
            GameState->Console.CurrentTime = GameState->Console.TimeToAnimate;
    }
    
    //@Fix there could still be some issues with deleting too many characters at once.
    if (GetKey(Key_Backspace, GameState) && GameState->Console.Open)
    {
        if(GameState->Console.DeleteTime >= 0.1 || GameState->Console.DeleteTime == 0) //character delete delay
        {
            GameState->Console.DeleteTime = 0;
            if (GameState->Console.BufferIndex > 0)
                GameState->Console.Buffer[--GameState->Console.BufferIndex] = '\0';
        }
        GameState->Console.DeleteTime += DeltaTime;
    }
    
    if(GetKeyUp(Key_Backspace, GameState))
        GameState->Console.DeleteTime = 0;
    
    if (GetKeyDown(Key_Enter, GameState) && GameState->Console.Open)
    {
        ExecuteCommand(GameState);
    }
}