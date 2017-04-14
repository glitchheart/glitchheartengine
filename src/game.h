#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "types.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include "entity.h"
#include "world.h"
#include "keycontroller.h"
#include "rendering.h"

#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

struct camera
{
	uint32 ViewportWidth;
	uint32 ViewportHeight;
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
};

struct game_state
{
	render_state RenderState;
	camera Camera;
	entity Player;
	tilemap_data TilemapData;
	input_controller InputController;
};



#define UPDATE(name)void name(double DeltaTime, game_state* GameState) 
typedef UPDATE(update);
UPDATE(UpdateStub)
{
}

#endif