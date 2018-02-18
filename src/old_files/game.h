#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <time.h>

#include "platform.h"
#include "shared.h"
#include "math.h"
#if GLITCH_DEBUG
#include "debug.h"
#endif

#include "sound.h"
#include "animation.h"

#include "collision.h"
#include "ai.h"
#include "entity.h"
#include "level.h"
#include "keycontroller.h"

#include "console.h"
#include "editor.h"
#include "menu.h"
#include "ui.h"

GENERIC_MAP(animation,animation*, char*, StrCmp, NULL, "%s", STR_ASSIGN, PTR_COPY)

enum Game_Mode
{
    Mode_MainMenu,
    Mode_InGame,
    Mode_Paused,
    Mode_Editor,
    Mode_Exit
};

enum Player_State
{
    Player_Alive,
    Player_Dead
};

enum Player_Gain_Type
{
    Gain_Health,
    Gain_Stamina,
    Gain_Strength
};

struct character_level
{
    i32 WillForLevel = 0;
};

struct character_data
{
    b32 IsInitialized = 0;
    i32 Level = 0;
    i32 Health = 0;
    i32 Stamina = 0;
    i32 Strength = 0;
    i32 HealthPotionCount = 3;
    math::v3 CurrentCheckpoint;
    b32 HasCheckpoint;
    i32 CheckpointHandle = -1;
};

#define NUM_TIMERS 1024
#define MAX_MODELS 300

struct ray
{
    math::v3 Origin;
    math::v3 Direction;
};

struct game_state
{
    memory_arena TotalArena;
    memory_arena WorldArena;
    
    b32 IsInitialized;
    b32 Paused;
    b32 ShouldReload;
    b32 RenderGame = true;
    b32 RenderLight = true;
    b32 ClearTilePositionFrame = false;
    
    character_data CharacterData;
    b32 StatGainModeOn = false;
    
    i32 SelectedGainIndex = 0;
    
    b32 AIDebugModeOn = false;
    b32 GodModeOn = false;
    r32 GodModePanSpeed = 10.0f;
    r32 GodModeZoomSpeed = 45.0f;
    r32 ZoomBeforeGodMode;
    r32 GodModeMinZoom = 5.0f;
    r32 GodModeMaxZoom = 100.0f;
    
    r32 PrevMouseX;
    r32 PrevMouseY;
    
    r32 InitialZoom;
    
    math::v3 EntityPositions[NUM_ENTITIES];
    
    Player_State PlayerState = Player_Alive;
    timer DeathScreenTimer;
    
    character_level StatData[10];
    character_data LastCharacterData;
    
    Game_Mode GameMode;
    main_menu MainMenu;
    
    i32 PlayerIndex;
    char * LevelPath;
    level CurrentLevel;
    
    
    i32 PlayerModel;
    i32 SelectedModel;
    
    model Models[MAX_MODELS];
    i32 ModelCount;
    
    collision_volume CollisionVolumes[16];
    i32 CollisionVolumeCount;
    
    u16 EntityCount;
    entity Entities[NUM_ENTITIES];
    
    i32 ObjectCount;
    object_entity Objects[NUM_ENTITIES];
    
    render_entity RenderEntities[NUM_ENTITIES];
    i32 RenderEntityCount;
    
    i32** EntityTilePositions;
    
    player_ui PlayerUI;
    health_bar HealthBar;
    
    u32 TimerCount;
    r64 Timers[NUM_TIMERS];
    
    // Editor
    console Console;
    editor_state EditorState;
    
    animation AnimationArray[100];
    i32 AnimationIndex;
    
    animation_map AnimationMap;
    
    entity_file_reload_data* ReloadData;
    
    i32 GameCameraHandle;
    i32 EditorCameraHandle;
};

void StartTimer(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle == -1)
    {
        Timer.TimerHandle = GameState->TimerCount;
        GameState->TimerCount++;
        Assert(GameState->TimerCount < NUM_TIMERS);
    }
    
    GameState->Timers[Timer.TimerHandle] = Timer.TimerMax;
}

b32 TimerDone(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle == -1)
        return true;
    
    return GameState->Timers[Timer.TimerHandle] <= 0;;
}

r64 ElapsedTimer(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle == -1)
        return 1.0;
    return GameState->Timers[Timer.TimerHandle];
}

r64 ElapsedTimerFrac(game_state* GameState, timer& Timer)
{
    return ElapsedTimer(GameState,Timer) / Timer.TimerMax;
}

void StartFade(camera& Camera, Fading_Mode Mode, r32 FadingSpeed, math::v3 FadingTint, r32 StartAlpha = 0, r32 EndAlpha = 0)
{
    Camera.FadingMode = Mode;
    Camera.FadingTint = FadingTint;
    Camera.EndAlpha = EndAlpha;
    Camera.FadingSpeed = FadingSpeed;
    
    switch(Mode)
    {
        case Fading_In:
        {
            Camera.FadingAlpha = 1.0f;
        }
        break;
        case Fading_Out:
        {
            Camera.FadingAlpha = 0.0f;
        }
        break;
        case Fading_OutIn:
        {
            Camera.FadingAlpha = StartAlpha;
        }
        break;
    }
}

void SaveGame(game_state* GameState)
{
    FILE* File;
    File = fopen("../savefile1.gs", "wb");
    
    if(File)
    {
        auto& Entity = GameState->Entities[0];
        
        fwrite(&GameState->CharacterData,sizeof(character_data), 1, File);
        fwrite(&GameState->LastCharacterData,sizeof(character_data), 1, File);
        fwrite(&Entity.Position,sizeof(math::v2), 1, File);
        
        fclose(File);
        DEBUG_PRINT("Saved game!\n");
    }
}

void LoadGame(game_state* GameState)
{
    FILE* File;
    char* FilePath = "../savefile1.gs";
    if(Platform.FileExists(FilePath))
    {
        File = fopen(FilePath, "rb");
        if(File)
        {
            fread(&GameState->CharacterData, sizeof(character_data), 1, File);
            fread(&GameState->LastCharacterData, sizeof(character_data), 1, File);
            fread(&GameState->Entities[0].Position, sizeof(math::v2),1,File);
            
            fclose(File);
            DEBUG_PRINT("Loaded game!\n");
            GameState->CharacterData.IsInitialized = true;
        }
    }
    else
    {
        SaveGame(GameState);
    }
}


#endif