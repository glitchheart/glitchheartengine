#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "glm/gtc/matrix_transform.hpp"

#include <GLFW/glfw3.h>

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

struct camera
{
    uint32 ViewportWidth;
    uint32 ViewportHeight;
    real32 Zoom; //NOTE(Daniel) The higher the number the closer you are zoomed in. 1.0f is NORMAL
    glm::vec2 Center;
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

enum Game_Mode
{
    Mode_MainMenu,
    Mode_InGame,
    Mode_Paused,
    Mode_Editor,
    Mode_Exit
};

#define NUM_TIMERS 128

struct game_state
{
    bool32 IsInitialized;
    bool32 Paused;
    bool32 ShouldReload;
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
    
    union
    {
        animation Animation[27];
        struct
        {
            animation PlayerIdleUpAnimation;
            animation PlayerIdleDownAnimation;
            animation PlayerIdleLeftAnimation;
            animation PlayerIdleRightAnimation;
            
            animation PlayerRunUpAnimation;
            animation PlayerRunDownAnimation;
            animation PlayerRunLeftAnimation;
            animation PlayerRunRightAnimation;
            animation PlayerAttackUpAnimation;
            animation PlayerAttackDownAnimation;
            animation PlayerAttackLeftAnimation;
            animation PlayerAttackRightAnimation;
            
            animation EnemyIdleAnimation;
            animation EnemyWalkAnimation;
            animation EnemyWalkUpAnimation;
            animation EnemyWalkDownAnimation;
            animation EnemyAttackAnimation;
            
            animation EnemyHitAnimation;
            animation SwordAttackAnimation;
            
            animation SkeletonIdleAnimation;
            animation SkeletonAttackAnimation;
            animation SkeletonWalkAnimation;
            animation SkeletonHitAnimation;
            animation SkeletonDeathAnimation;
            animation SkeletonReactAnimation;
            
            animation BlobAnimation;
            animation ExplosionAnimation;
        };
    };
};

#define UPDATE(name)void name(real64 DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

#endif
