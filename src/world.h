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

struct tile_data
{
	Tile_Type Type;
	glm::vec2 TextureOffset;
};

//tile chunks are 32 x 32 tiles
struct tile_chunk
{
	tile_data Data[32][32];
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