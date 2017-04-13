#include "world.h"
#include "types.h"
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

static void GenerateTilemap(const perlin_noise &Noise, tilemap_data *TilemapData)
{
	for(int i = 0; i < TILEMAP_SIZE; i++)
	{
		for(int j = 0; j < TILEMAP_SIZE; j++)
		{
			real32 N = Noise.Noise[i][j];
			
			if(N < -0.2f)
			{
				TilemapData->Data[i][j] = { Tile_Sand, glm::vec2(0.6f, 0) };
			}
			else if(N < -0.1f)
			{
				TilemapData->Data[i][j] = { Tile_Grass, glm::vec2(0, 0) };;
			}
			else if(N < 0)
			{
				TilemapData->Data[i][j] = { Tile_DarkGrass, glm::vec2(0.4f, 0) };
			}
			else
			{
				TilemapData->Data[i][j] = { Tile_Stone, glm::vec2(0.8f, 0) };
			}
		}
	}
}

static void GenerateNoise(perlin_noise *PerlinNoise, int Width, int Height)
{
	PerlinNoise->Width = Width;
	PerlinNoise->Height = Height;
	PerlinNoise->Noise = (real32**)malloc(sizeof(real32*) * Width);
 
    for(int i = 0; i < Width; i++)
    	PerlinNoise->Noise[i] = (real32*)malloc(sizeof(real32) * Height);

	for(int i = 0; i < Width; i++)
	{
		for(int j = 0; j < Height; j++)
		{
			PerlinNoise->Noise[i][j] = stb_perlin_noise3((real32)i / 100.0f, (real32)j / 100.0f, 0, 0, 0, 0);
		}
	}
}