#ifndef ENTITY_H
#define ENTITY_H

#include "sprite.h"

struct player
{
	sprite spr;
};

struct entity
{
	union
	{
		struct
		{
			sprite spr;
		} Player;

		struct
		{
			int shit;
			bool crap;
		} SecondType;

		struct
		{
			int shit;
			bool crap;
		} ThirdType;

		struct
		{
			int shit;
			bool crap;
		} FourthType;
	};
};

#endif