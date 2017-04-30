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
#include "entity.h"
#include "level.h"
#include "keycontroller.h"
#include "platform_sound.h"
#include "sound.h"
#include "collision.h"

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

#define CONSOLE_BUFFER_SIZE 100
#define HISTORY_BUFFER_LINES 7

struct console
{
    bool Open;
    glm::vec3 EndPosition = glm::vec3(-1, 0.5, 0);
    real32 TimeToAnimate = 0.3f;
    real32 CurrentTime;
    glm::vec4 Color;
    glm::vec3 CursorColor;
    uint32 MaxHeight;
    uint32 BufferIndex;
    char Buffer[CONSOLE_BUFFER_SIZE];
    char HistoryBuffer[HISTORY_BUFFER_LINES][CONSOLE_BUFFER_SIZE + 20]; //NOTE(Daniel) + 20 to make room for : command not found
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
};

#define UPDATE(name)void name(real64 DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}


#endif
