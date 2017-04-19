#ifndef WORLD_H
#define WORLD

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

#define NUM_ISLANDS 1
#define ISLAND_SIZE 100
#define REGION_SIZE = 1000

struct island_chunk
{
	uint32 Seed;
	uint32 X;
	uint32 Y;
	tile_data Data[ISLAND_SIZE][ISLAND_SIZE];
};

struct tilemap_data
{
	GLuint TileAtlasTexture;
	island_chunk Chunks[NUM_ISLANDS];
};

#endif
