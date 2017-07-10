#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "windows.h"
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>
#include <time.h>
#include <stdlib.h>
#include <map>
#include "platform.h"
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

#define NUM_TIMERS 128

struct game_state
{
    b32 IsInitialized;
    b32 Paused;
    b32 ShouldReload;
    b32 RenderGame = true;
    
    b32 GodModeOn = false;
    r32 GodModePanSpeed = 12.0f;
    r32 GodModeZoomSpeed = 25.0f;
    r32 ZoomBeforeGodMode;
    
    r32 InitialZoom;
    
    Player_State PlayerState = Player_Alive;
    timer DeathScreenTimer;
    
    Game_Mode GameMode;
    main_menu MainMenu;
    render_state RenderState;
    
    camera Camera;
    camera GameCamera;
    camera EditorCamera;
    
    u32 PlayerIndex;
    char * LevelPath;
    level CurrentLevel;
    input_controller InputController;
    sound_manager SoundManager;
    
    u16 EntityCount;
    entity Entities[NUM_ENTITIES];
    
    player_ui PlayerUI;
    health_bar HealthBar;
    
    u32 TimerCount;
    r64 Timers[NUM_TIMERS];
    
    light_source LightSources[32];
    u32 LightSourceCount;
    
    // Editor
    console Console;
    editor_state EditorState;
    
    std::map<char*, animation, CompareCStrings> Animations;
    
    entity_file_reload_data* ReloadData;
};

#define UPDATE(name)void name(r64 DeltaTime, game_state* GameState)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

void StartTimer(game_state* GameState, timer& Timer)
{
    Timer.TimerHandle = GameState->TimerCount;
    GameState->Timers[Timer.TimerHandle] = Timer.TimerMax;
    
    GameState->TimerCount++;
    if(GameState->TimerCount == NUM_TIMERS)
        GameState->TimerCount = 0;
}

b32 TimerDone(game_state* GameState, timer& Timer)
{
    if(Timer.TimerHandle != -1 && 
       GameState->Timers[Timer.TimerHandle] <= 0)
    {
        Timer.TimerHandle = -1;
    }
    
    return Timer.TimerHandle == -1;
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


#endif
