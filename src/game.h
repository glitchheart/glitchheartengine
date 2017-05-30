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

struct game_state
{
    bool32 IsInitialized;
    bool32 ShouldReload;
    Game_Mode GameMode;
    main_menu MainMenu;
    render_state RenderState;
    camera Camera;
    uint32 PlayerIndex;
    char * LevelPath;
    level CurrentLevel;
    input_controller InputController;
    sound_manager SoundManager;
    
    uint16 EntityCount;
    entity Entities[NUM_ENTITIES];
    
    health_bar HealthBar;
    
    // Editor
    console Console;
    editor_ui EditorUI;
    camera EditorCamera;
    editor_state EditorState;
    
    union
    {
        animation Animation[16];
        struct
        {
            animation PlayerIdleAnimation;
            animation PlayerWalkAnimation;
            animation PlayerWalkDownAnimation;
            animation PlayerWalkUpAnimation;
            animation PlayerAttackAnimation;
            
            animation PlayerHitAnimation;
            animation EnemyIdleAnimation;
            animation EnemyWalkAnimation;
            animation EnemyWalkUpAnimation;
            animation EnemyWalkDownAnimation;
            animation EnemyAttackAnimation;
            
            animation EnemyHitAnimation;
            animation SwordTopRightAnimation;
            animation SwordUpAnimation;
            animation SwordDownAnimation;
            animation SwordAttackAnimation;
        };
    };
};

#define UPDATE(name)void name(real64 DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

#endif
