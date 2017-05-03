#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "types.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/noise.hpp>
#include <GLFW/glfw3.h>
#include "al.h"
#include "alc.h"
#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include "util.h"
#include "error.h"
#include "opengl_rendering.h"
#include "animation.h"
#include "collision.h"
#include "entity.h"
#include "level.h"
#include "keycontroller.h"
#include "platform_sound.h"
#include "sound.h"
#include "console.h"

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

struct camera
{
    uint32 ViewportWidth;
    uint32 ViewportHeight;
    real32 Zoom; //NOTE(Daniel) The higher the number the closer you are zoomed in. 1.0f is NORMAL
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

struct game_state
{
    render_state RenderState;
    camera Camera;
    console Console;
    entity Player;
    entity Crosshair;
    room Room;
    input_controller InputController;
    sound_manager SoundManager;
    uint16 EntityCount;
    entity Entities[NUM_ENTITIES]; // entity.h NUM_ENTITIES
    
};

#define UPDATE(name)void name(real64 DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}


#endif
