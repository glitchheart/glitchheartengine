#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"
#include <glm/glm.hpp>

enum Entity_Enum
{
	Entity_Player,
	Entity_PalmTree,
	Entity_Enemy,

	Entity_Max
};

struct entity
{
	Entity_Enum Type;
	glm::vec2 Position;
	glm::vec2 Rotation;
	uint32 TextureHandle;
	uint32 ShaderIndex;
	union
	{
		struct
		{
			real32 WalkingSpeed;
		} player;
		struct
		{
		} palm_tree;
		struct
		{
		} enemy;
	};
};

#define NUM_ENTITIES 10

//TODO(daniel) Put this to use
struct entity_manager
{
    uint16 Count;
    entity Entities[NUM_ENTITIES];
};

#endif