#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "types.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/noise.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include "al.h"
#include "alc.h"
#include <time.h> 
#include <stdio.h>


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

struct console
{
	bool Open;
	uint32 MaxHeight;
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