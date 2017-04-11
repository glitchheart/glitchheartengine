#ifndef ENTITY_H
#define ENTITY_H

#include "types.h"
#include "sprite.h"

struct entity
{
	union
	{
		struct
		{
			real32 WalkingSpeed;
			uint16 SpriteHandle;
		} Player;
		struct
		{
			uint16 SpriteHandle;
		} Enemy;
	};
};

#endif