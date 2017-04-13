#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "entity.h"

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
};

#endif