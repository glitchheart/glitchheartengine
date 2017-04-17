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

#define TILEMAP_SIZE 1

#define CHUNK_SIZE 50

struct tile_chunk
{
	uint32 Seed;
	uint32 X;
	uint32 Y;
	tile_data Data[CHUNK_SIZE][CHUNK_SIZE];
};

struct tilemap_data
{
	GLuint TileAtlasTexture;
	tile_chunk Chunks[TILEMAP_SIZE][TILEMAP_SIZE];
};

#endif