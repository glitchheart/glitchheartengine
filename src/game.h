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

#include "util.h"
#include "error.h"
#include "entity.h"
#include "world.h"
#include "keycontroller.h"
#include "rendering.h"
#include "sound.h"

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

struct camera
{
    uint32 ViewportWidth;
    uint32 ViewportHeight;
    real32 Zoom = 1.0f; //NOTE(Daniel) 1.0 is normal zoom. The higher the number the closer you are zoomed in
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;
};

#define CONSOLE_BUFFER_SIZE 100
#define HISTORY_BUFFER_LINES 7

struct console
{
    bool Open;
    uint32 MaxHeight;
    uint32 BufferIndex;
	uint32 HistoryBufferIndex;
    char Buffer[CONSOLE_BUFFER_SIZE];
    char HistoryBuffer[HISTORY_BUFFER_LINES][CONSOLE_BUFFER_SIZE];
};

struct game_state
{
    render_state RenderState;
    camera Camera;
    console Console;
    entity Player;
    tilemap_data TilemapData;
    input_controller InputController;
    sound_manager SoundManager;
};

#define UPDATE(name)void name(double DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}


#endif
