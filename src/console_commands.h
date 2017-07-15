#ifndef CONSOLE_COMMANDS_H
#define CONSOLE_COMMANDS_H

static char* Zoom(game_state* GameState, char** Arguments)
{
    r32 ZoomAmount = (r32) strtod(Arguments[0], NULL);
    GameState->GameCamera.Zoom = ZoomAmount;
    return Concat("Zoom set to ", Arguments[0]);
}

static char* Jump(game_state* GameState, char** Arguments)
{
    r32 X = (r32) strtod(Arguments[0], NULL);
    r32 Y = (r32) strtod(Arguments[1], NULL);
    
    GameState->Entities[GameState->PlayerIndex].Position = glm::vec2(X, Y);
    char* Result = (char*)malloc(40 * sizeof(char));
    sprintf(Result, "Jumped to position %.2f %.2f", X, Y);
    return Result;
}

static char* LoadLevel(game_state* GameState, char** Arguments)
{
    char* Result = (char*)malloc(25 * sizeof(char));
    
    if(Arguments)
    {
        level Level;
        
        char* PathPrefix = "../assets/levels/";
        char* PathSuffix = ".plv";
        
        char* Path = Concat(Concat(PathPrefix, Arguments[0]), PathSuffix);
        
        printf("String allocation\n");
        
        FILE* File;
        File = fopen(Path, "r");
        
        if(File)
        {
            fclose(File);
            GameState->LevelPath = Path;
            GameState->IsInitialized = false;
            GameState->ShouldReload = true;
            
            for(u32 X = 0; X < GameState->CurrentLevel.Tilemap.Width; X++)
            {
                free(GameState->CurrentLevel.Tilemap.Data[X]);
            }
            free(GameState->CurrentLevel.Tilemap.Data);
            
            GameState->CurrentLevel = {};
            
            for(i32 Index = 0; Index < GameState->EntityCount; Index++)
                GameState->Entities[Index] = {};
            
            for(i32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++)
                GameState->RenderState.RenderEntities[Index] = {};
            
            GameState->EntityCount = 0;
            GameState->RenderState.RenderEntityCount = 0;
            
            sprintf(Result, "Loaded level");
            
            printf("End of LoadLevel\n");
        }
        else
        {
            sprintf(Result, "'%s' could not be loaded", Path);
        }
    }
    else
    {
        sprintf(Result, "Level name is expected as argument.");
    }
    return Result;
}


static void ReloadCurrentLevel(game_state* GameState)
{
    level Level;
    
    //@Incomplete still needs to respawn the player
    GameState->IsInitialized = false;
    GameState->PlayerState = Player_Alive;
    
    for(u32 X = 0; X < GameState->CurrentLevel.Tilemap.Width; X++)
    {
        free(GameState->CurrentLevel.Tilemap.Data[0][X]);
        free(GameState->CurrentLevel.Tilemap.Data[1][X]);
    }
    
    free(GameState->CurrentLevel.Tilemap.Data[0]);
    free(GameState->CurrentLevel.Tilemap.Data[1]);
    
    //memset(GameState->Entities, 0, sizeof(entity) * GameState->EntityCount);
    for(i32 Index = 0; Index < GameState->EntityCount; Index++)
        GameState->Entities[Index] = {};
    
    //memset(GameState->LightSources, 0, sizeof(light_source) * GameState->LightSourceCount);
    
    //for(i32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++)
    //GameState->RenderState.RenderEntities[Index] = {};
    
    GameState->LightSourceCount = 0;
    GameState->EntityCount = 0;
    GameState->ObjectCount = 0;
    GameState->RenderState.RenderEntityCount = 0;
}


static char* Reset(game_state* GameState, char** Arguments)
{
    ReloadCurrentLevel(GameState);
    char* Result = (char*)malloc(12 * sizeof(char));
    sprintf(Result, "Reset level");
    return Result;
}

static char* Exit(game_state* GameState, char** Arguments)
{
    GameState->RenderState.ShouldClose = true;
    return "Exited";
}

static char* View(game_state* GameState, char** Arguments)
{
    if(Arguments) 
    {
    }
    return "Error: No arguments provided";
}

static char* Editor(game_state* GameState, char** Arguments)
{
    if(Arguments)
    {
        
    }
    return "Error: No arguments provided";
}

#endif