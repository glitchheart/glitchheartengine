static void AddCommand(char* Name, char* (*FunctionPointer)(game_state*, char*))
{
    command_info Info = { Name, FunctionPointer };
    Commands[CommandCount++] = Info;
}

static char* Zoom(game_state* GameState, char* Arguments)
{
    real32 ZoomAmount = (real32) strtod(&Arguments[0], NULL);
    GameState->Camera.Zoom = ZoomAmount;
    return CombineStrings("Zoom set to ", &Arguments[0]);
}

static void InitCommands()
{
    AddCommand("zoom", &Zoom);
}

//CONSOLE STUFF TODO(Daniel) MOOOOOOOOOOOOOOOOOOVE
void ExecuteCommand(game_state *GameState)
{
    char* Result = &GameState->Console.Buffer[0];
    char ResultCopy[40];
    strcpy(&ResultCopy[0], Result);
    
    char* Pointer;
    char* StrZoomAmount = 0;
    
    //Pointer = strtok(&ResultCopy[0], " "); //skip only spaces
    
    int Count = -1;
    
    char* ArgumentBuffer[10];
    char* CommandName;
    
    printf("We got pretty far\n");
    
    while(Pointer != NULL)
    {
        if(Count == -1)
        {
            CommandName = strtok(NULL, " ");
        }
        else
            ArgumentBuffer[Count] = strtok(NULL, " ");
        Count++;
    }
    
    
    for(int i = 0; i < CommandCount; i++)
    {
        if(strcmp(CommandName, Commands[i].Name) == 0)
        {
            Commands[i].FunctionPointer(GameState, ArgumentBuffer[0]);
            break;
        }
    }
    
    /*
    if(strcmp(" ",  GameState->Console.Buffer) != 0
    && strcmp("",  GameState->Console.Buffer) != 0) //NOTE(Daniel) if the command isn't an empty string
    {
    char* Result = &GameState->Console.Buffer[0];
    
    if (strcmp(GameState->Console.Buffer, "exit") == 0)
    {
    //TODO(niels): Need to find a way to call this from here
    //             This should probably be in platform code anyway?
    //             Doesn't really make sense to have it in game code
    //CleanupSound(GameState);
    
    alcMakeContextCurrent(0);
    alcDestroyContext(GameState->SoundManager.Context);
    alcCloseDevice(GameState->SoundManager.Device);
    
    glfwDestroyWindow(GameState->RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    }
    else if(strcmp(GameState->Console.Buffer, "build") == 0)
    {
    Result = "Building...";
    system("..\\build.bat"); //TODO(Daniel) separate thread
    }
    else if(strstr(GameState->Console.Buffer, "zoom") != NULL)
    {
    //NOTE(Daniel) copy the string before splitting it. The call to strtok manipulates it.
    char ResultCopy[40];
    strcpy(&ResultCopy[0], Result);
    
    char* Pointer;
    char* StrZoomAmount = 0;
    
    Pointer = strtok(&ResultCopy[0], " "); //skip only spaces
    
    int Count = 0;
    
    while(Pointer != NULL && Count < 1)
    {
    if(Count == 0)
    StrZoomAmount  = strtok(NULL, " ");
    else
    strtok(NULL, " ");
    Count++;
    }
    
    real32 ZoomAmount = (real32) strtod(StrZoomAmount, NULL);
    GameState->Camera.Zoom = ZoomAmount;
    
    Result = CombineStrings("Zoom set to ", StrZoomAmount);
    }
    else if(strstr(GameState->Console.Buffer, "jump"))
    {
    //NOTE(Daniel) copy the string before splitting it. The call to strtok manipulates it.
    char ResultCopy[40];
    strcpy(&ResultCopy[0], Result);
    
    char* Pointer;
    char* StrX = 0;
    char* StrY = 0;
    
    Pointer = strtok(&ResultCopy[0], " "); //skip only spaces
    
    int Count = 0;
    
    while(Pointer != NULL && Count < 2)
    {
    if(Count == 0)
    StrX  = strtok(NULL, " ");
    else if(Count == 1)
    StrY = strtok(NULL, " ");
    else
    strtok(NULL, " ");
    Count++;
    }
    
    real32 X = (real32) strtod(StrX, NULL);
    
    real32 Y = (real32) strtod(StrY, NULL);
    
    GameState->Player.Position = glm::vec2(X, Y);
    
    sprintf(Result, "Jumped to position %.2f %.2f", X, Y);
    }
    else
    {
    Result = CombineStrings(Result, ": Command not found");
    }
    
    //NOTE(Daniel) Copy the command into the history buffer
    for(int i = HISTORY_BUFFER_LINES - 1; i > 0; i--)
    {
    sprintf(GameState->Console.HistoryBuffer[i], GameState->Console.HistoryBuffer[i - 1]);
    }
    
    sprintf(GameState->Console.HistoryBuffer[0], Result);
    
    for(int i = 0; i < CONSOLE_BUFFER_SIZE; i++)
    GameState->Console.Buffer[i] = '\0';
    
    GameState->Console.BufferIndex = 0;
    }*/    
}

//console stuff
static void CheckConsoleInput(game_state* GameState, real32 DeltaTime)
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
    
    if (GetKeyDown(Key_Backspace, GameState) && GameState->Console.Open)
    {
        if (GameState->Console.BufferIndex > 0)
            GameState->Console.Buffer[--GameState->Console.BufferIndex] = '\0';
    }
    
    if (GetKeyDown(Key_Enter, GameState) && GameState->Console.Open)
    {
        ExecuteCommand(GameState);
    }
}

