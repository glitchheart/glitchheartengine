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
    uint32 ViewportWidth;
    uint32 ViewportHeight;
    real32 Zoom; //NOTE(Daniel) The higher the number the closer you are zoomed in. 1.0f is NORMAL
    glm::vec2 Center;
    glm::vec2 CenterTarget;
    real32 FollowSpeed;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
    timer* ScreenShakeTimer;
    
    Fading_Mode FadingMode = Fading_None;
    glm::vec3 FadingTint;
    
    bool32 FadingIn;
    real32 EndAlpha;
    real32 FadingAlpha = 0.0f;
    real32 FadingSpeed;
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
    bool32 IsInitialized;
    bool32 Paused;
    bool32 ShouldReload;
    
    Player_State PlayerState = Player_Alive;
    timer* DeathScreenTimer;
    
    Game_Mode GameMode;
    main_menu MainMenu;
    render_state RenderState;
    
    camera Camera;
    camera GameCamera;
    camera EditorCamera;
    
    uint32 PlayerIndex;
    char * LevelPath;
    level CurrentLevel;
    input_controller InputController;
    sound_manager SoundManager;
    
    uint16 EntityCount;
    entity Entities[NUM_ENTITIES];
    
    health_bar HealthBar;
    
    uint32 TimerCount;
    real64 Timers[NUM_TIMERS];
    
    // Editor
    console Console;
    editor_ui EditorUI;
    editor_state EditorState;
    
    std::map<char*, animation, CompareCStrings> Animations;
    
    entity_file_reload_data* ReloadData;
};

#define UPDATE(name)void name(real64 DeltaTime, game_state* GameState)
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

void StartTimer(game_state* GameState, timer* Timer)
{
    Timer->TimerHandle = GameState->TimerCount;
    GameState->Timers[Timer->TimerHandle] = Timer->TimerMax;
    
    GameState->TimerCount++;
    if(GameState->TimerCount == NUM_TIMERS)
        GameState->TimerCount = 0;
}

bool32 TimerDone(game_state* GameState, timer* Timer)
{
    if(Timer->TimerHandle != -1 && 
       GameState->Timers[Timer->TimerHandle] <= 0)
    {
        Timer->TimerHandle = -1;
    }
    
    return Timer->TimerHandle == -1;
}


static void StartFade(camera& Camera, Fading_Mode Mode, real32 FadingSpeed, glm::vec3 FadingTint, real32 StartAlpha = 0, real32 EndAlpha = 0)
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
