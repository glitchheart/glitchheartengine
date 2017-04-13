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

#define WORLD_SIZE 10
#define TILE_CHUNK_SIZE 32

struct tile_chunk
{
	GLuint VAO;
	GLuint VBO;
	glm::vec2 Offset;
	tile_data Data[TILE_CHUNK_SIZE][TILE_CHUNK_SIZE];
};

struct world_data
{
	uint32 Width = WORLD_SIZE;
	uint32 Height = WORLD_SIZE;
	tile_chunk TileChunks[WORLD_SIZE][WORLD_SIZE];
};

#endif