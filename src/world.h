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

#define TILEMAP_SIZE 200

struct tilemap_data
{
	GLuint TileAtlasTexture;
	tile_data Data[TILEMAP_SIZE][TILEMAP_SIZE];
};

#endif