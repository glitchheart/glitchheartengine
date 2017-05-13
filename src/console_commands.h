#ifndef CONSOLE_COMMANDS_H
#define CONSOLE_COMMANDS_H

static char* Zoom(game_state* GameState, char** Arguments)
{
    real32 ZoomAmount = (real32) strtod(Arguments[0], NULL);
    GameState->Camera.Zoom = ZoomAmount;
    return CombineStrings("Zoom set to ", Arguments[0]);
}

static char* Jump(game_state* GameState, char** Arguments)
{
    real32 X = (real32) strtod(Arguments[0], NULL);
    real32 Y = (real32) strtod(Arguments[1], NULL);
    
    //@Cleanup this should be changed to access the entities array instead
    GameState->Entities[GameState->PlayerIndex].Position = glm::vec2(X, Y);
    char* Result = (char*)malloc(40 * sizeof(char));
    sprintf(Result, "Jumped to position %.2f %.2f", X, Y);
    return Result;
}

static char* Reset(game_state* GameState, char** Arguments)
{
    GameState->Entities[GameState->PlayerIndex].Position = glm::vec2(0, 0); //TODO(Daniel) still needs to reset a lot more
    GameState->Camera.Center = glm::vec2(GameState->Entities[GameState->PlayerIndex].Position.x, GameState->Entities[GameState->PlayerIndex].Position.y);
    printf("reset\n");
    char* Result = (char*)malloc(12 * sizeof(char));
    sprintf(Result, "Reset level");
    return Result;
}

static char* LoadLevel(game_state* GameState, char** Arguments)
{
    level Level;
    
    char* PathPrefix = "../assets/levels/";
    char* PathSuffix = ".plv";
    char * Path = (char *) malloc(1 + strlen(PathPrefix) + strlen(Arguments[0] + strlen(PathSuffix)) );
    strcpy(Path, PathPrefix);
    strcat(Path, Arguments[0]);
    strcat(Path, PathSuffix);
    
    char* Result = (char*)malloc(25 * sizeof(char));
    
    if(LoadLevelFromFile(Path, &Level))
    {
        GameState->CurrentLevel = Level;
        sprintf(Result, "Loaded level");
    }
    else
    {
        sprintf(Result, "Level not found: %s.plv", Arguments[0]);
    }
    
    return Result;
}

static char* Exit(game_state* GameState, char** Arguments)
{
    //TODO(niels): Need to find a way to call this from here
    //             This should probably be in platform code anyway?
    //             Doesn't really make sense to have it in game code
    //CleanupSound(GameState);
    /*
    alcMakeContextCurrent(0);
    alcDestroyContext(GameState->SoundManager.Context);
    alcCloseDevice(GameState->SoundManager.Device);
    */
    glfwDestroyWindow(GameState->RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static char* View(game_state* GameState, char** Arguments)
{
    if(Arguments) 
    {
        if(strcmp(Arguments[0], "entity_list") == 0)
        {
            GameState->EditorUI.On = true;
            GameState->EditorUI.State = State_EntityList;
            return "Toggled entity list";
        }
        else if(strcmp(Arguments[0], "normal") == 0)
        {
            GameState->EditorUI.On = false;
            return "Toggled view to normal mode";
        }
    }
    return "Error: No arguments provided";
}

static char* Editor(game_state* GameState, char** Arguments)
{
    if(Arguments)
    {
        if(strcmp(Arguments[0], "off") == 0)
        {
            GameState->EditorUI.On = false;
            return "Toggled editor off";
        }
        else if(strcmp(Arguments[0], "on") == 0)
        {
            GameState->EditorUI.On = true;
            return "Toggled editor on";
            
        }
    }
    return "Error: No arguments provided";
}

#endif