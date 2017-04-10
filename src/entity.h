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
			uint16 spriteHandle;
		} Player;
		struct
		{
			uint16 spriteHandle;
		} Enemy;
	};
};

#endif