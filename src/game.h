#ifndef GAMESTATE_H
#define GAMESTATE_H


#include "glm/gtc/matrix_transform.hpp"

#include <GLFW/glfw3.h>

#include <map>
#include "platform.h"
#include "opengl_rendering.h"
#include "animation.h"
#include "collision.h"
#include "entity.h"
#include "level.h"
#include "keycontroller.h"

#include "sound.h"
#include "console.h"
#include "editor_ui.h"
#include "menu.h"

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
    Mode_Exit
};

struct game_state
{
    bool32 IsInitialized;
    Game_Mode GameMode;
    main_menu MainMenu;
    render_state RenderState;
    camera Camera;
    uint32 PlayerIndex;
    level CurrentLevel;
    input_controller InputController;
    sound_manager SoundManager;
    uint16 EntityCount;
    entity Entities[NUM_ENTITIES]; // entity.h NUM_ENTITIES
    
    console Console;
    editor_ui EditorUI;
    
    union
    {
        animation Animation[16];
        struct
        {
            animation PlayerIdleAnimation;
            animation PlayerWalkAnimation;
            animation PlayerAttackAnimation;
            animation EnemyIdleAnimation;
            animation EnemyWalkAnimation;
            animation EnemyAttackAnimation;
        };
    };
};

#define UPDATE(name)void name(real64 DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

#endif
