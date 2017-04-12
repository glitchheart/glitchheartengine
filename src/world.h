#ifndef WORLD_H
#define WORLD

#include "types.h"

struct perlin_noise
{
	real32 **Noise;
	uint32 Width;
	uint32 Height;
};

enum Tile_Type
{
	Tile_None,
	Tile_Sand, 
	Tile_Grass, 
	Tile_DarkGrass, 
	Tile_Stone
};

struct world_chunk
{
	Tile_Type** Tiles;
};

struct world
{
	world_chunk Chunks[10];
};

#endif