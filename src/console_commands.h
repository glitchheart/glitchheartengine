#ifndef CONSOLE_COMMANDS_H
#define CONSOLE_COMMANDS_H

static char* Zoom(game_state* GameState, char* Arguments)
{
    real32 ZoomAmount = (real32) strtod(&Arguments[0], NULL);
    GameState->Camera.Zoom = ZoomAmount;
    return CombineStrings("Zoom set to ", &Arguments[0]);
}

static char* Jump(game_state* GameState, char* Arguments)
{
    real32 X = (real32) strtod(&Arguments[0], NULL);
    real32 Y = (real32) strtod(&Arguments[1], NULL);
    
    //@Cleanup this should be changed to access the entities array instead
    GameState->Entities[GameState->PlayerIndex].Position = glm::vec2(X, Y);
    
    char* Result;
    sprintf(Result, "Jumped to position %.2f %.2f", X, Y);
    return Result;
}

static char* Exit(game_state* GameState, char* Arguments)
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

static char* TriggerEditor(game_state* GameState, char* Arguments)
{
    if(strcmp(&Arguments[0], "entity_list") == 0)
    {
        GameState->EditorUI.State = State_ShowEntityList;
        return "Toggled entity list";
    }
    else if(strcmp(&Arguments[0], "off") == 0)
    {
        GameState->EditorUI.State = State_Off;
        return "Toggled editor off";
    }
}

#endif