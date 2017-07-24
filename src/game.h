#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "windows.h"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include <time.h>
#include <cstdio>

#include "platform.h"
#include "gmap.h"
#include "opengl_rendering.h"
#include "animation.h"

#include "collision.h"
#include "ai.h"
#include "entity.h"
#include "level.h"
#include "keycontroller.h"
#include "sound.h"
#include "console.h"
#include "editor.h"
#include "menu.h"
#include "ui.h"

GENERIC_MAP(animation,animation*)

enum Fading_Mode
{
    Fading_None,
    Fading_In,
    Fading_Out,
    Fading_OutIn
};

struct camera
{
    u32 ViewportWidth;
    u32 ViewportHeight;
    r32 Zoom; //NOTE(Daniel) The higher the number the closer you are zoomed in. 1.0f is NORMAL
    glm::vec2 Center;
    glm::vec2 CenterTarget;
    r32 FollowSpeed;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    timer ScreenShakeTimer;
    
    Fading_Mode FadingMode = Fading_None;
    glm::vec3 FadingTint;
    
    b32 FadingIn;
    r32 EndAlpha;
    r32 FadingAlpha = 0.0f;
    r32 FadingSpeed;
};

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
    i32 Level = 0;
    i32 Health = 0;
    i32 Stamina = 0;
    i32 Strength = 0;
    i32 HealthPotionCount = 3;
    glm::vec2 CurrentCheckpoint;
    b32 HasCheckpoint;
    i32 CheckpointHandle = -1;
    b32 HasLostWill;
    i32 LostWill;
    i32 LostWillObjectHandle = -1;
    glm::vec2 LostWillPosition;
    b32 RenderWillButtonHint = false;
};

#define NUM_TIMERS 1024

struct tile_position
{
    i32 Entities[20];
    i32 Count;
};

struct game_state
{
    b32 IsInitialized;
    b32 Paused;
    b32 ShouldReload;
    b32 RenderGame = true;
    b32 RenderLight = true;
    
    character_data CharacterData;
    b32 StatGainModeOn = false;
    
    i32 SelectedGainIndex = 0;
    
    loot CurrentLoot[64];
    i32 CurrentLootCount = 0;
    
    b32 AIDebugModeOn = false;
    b32 GodModeOn = false;
    r32 GodModePanSpeed = 10.0f;
    r32 GodModeZoomSpeed = 45.0f;
    r32 ZoomBeforeGodMode;
    r32 GodModeMinZoom = 5.0f;
    r32 GodModeMaxZoom = 100.0f;
    
    r32 InitialZoom;
    
    Player_State PlayerState = Player_Alive;
    timer DeathScreenTimer;
    
    character_level StatData[10];
    character_data LastCharacterData;
    
    Game_Mode GameMode;
    main_menu MainMenu;
    render_state RenderState;
    
    camera Camera;
    camera GameCamera;
    camera EditorCamera;
    
    i32 PlayerIndex;
    char * LevelPath;
    level CurrentLevel;
    input_controller InputController;
    sound_manager SoundManager;
    
    u16 EntityCount;
    entity Entities[NUM_ENTITIES];
    
    i32 ObjectCount;
    object_entity Objects[NUM_ENTITIES];
    
    tile_position** EntityTilePositions;
    
    player_ui PlayerUI;
    health_bar HealthBar;
    
    u32 TimerCount;
    r64 Timers[NUM_TIMERS];
    
    light_source LightSources[32];
    u32 LightSourceCount;
    
    // Editor
    console Console;
    editor_state EditorState;
    
    animation AnimationArray[50];
    i32 AnimationIndex;
    
    animation_map AnimationMap;
    
    integer_map KeyMappings;
    integer_map MouseButtonMappings;
    integer_map ControllerMappings;
    
    entity_file_reload_data* ReloadData;
};

#define UPDATE(name)void name(r64 DeltaTime, game_state* GameState)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

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

void StartFade(camera& Camera, Fading_Mode Mode, r32 FadingSpeed, glm::vec3 FadingTint, r32 StartAlpha = 0, r32 EndAlpha = 0)
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
        auto Entity = GameState->Entities[0];
        
        fwrite(&GameState->CharacterData,sizeof(character_data), 1, File);
        fwrite(&GameState->LastCharacterData,sizeof(character_data), 1, File);
        fwrite(&Entity.Player.Will,sizeof(i32), 1, File);
        fwrite(&Entity.Position,sizeof(glm::vec2), 1, File);
        fwrite(&Entity.Player.Inventory,sizeof(player_inventory), 1, File);
        
        fclose(File);
        printf("Saved game!\n");
    }
}

void LoadGame(game_state* GameState)
{
    FILE* File;
    char* FilePath = "../savefile1.gs";
    if(FileExists(FilePath))
    {
        File = fopen(FilePath, "rb");
        if(File)
        {
            fread(&GameState->CharacterData, sizeof(character_data), 1, File);
            fread(&GameState->LastCharacterData, sizeof(character_data), 1, File);
            fread(&GameState->Entities[0].Player.Will, sizeof(i32), 1, File);
            fread(&GameState->Entities[0].Position, sizeof(glm::vec2),1,File);
            fread(&GameState->Entities[0].Player.Inventory, sizeof(player_inventory), 1 , File);
            
            fclose(File);
            printf("Loaded game!\n");
        }
    }
    else
    {
        SaveGame(GameState);
    }
}


#endif
