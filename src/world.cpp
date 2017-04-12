#include "world.h"
#include "types.h"
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

static void GenerateWorldChunk(const perlin_noise &Noise, world_chunk *Chunk)
{
	Chunk->Tiles = (Tile_Type**)malloc(sizeof(Tile_Type*) * Noise.Width);
 
    for(int i = 0; i < Noise.Width; i++)
    	Chunk->Tiles[i] = (Tile_Type*)malloc(sizeof(Tile_Type) * Noise.Height);
	
	for(int i = 0; i < Noise.Width; i++)
	{
		for(int j = 0; j < Noise.Height; j++)
		{
			real32 N = Noise.Noise[i][j];

			if(N < -0.2f)
			{
				Chunk->Tiles[i][j] = Tile_Sand;
				std::cout << "SAND" << std::endl;
			}
			else if(N < -0.1f)
			{
				Chunk->Tiles[i][j] = Tile_Grass;
				std::cout << "GRASS" << std::endl;
			}
			else if(N < 0)
			{
				Chunk->Tiles[i][j] = Tile_DarkGrass;
				std::cout << "DARKGRASS" << std::endl;
			}
			else
			{
				Chunk->Tiles[i][j] = Tile_Stone;
				std::cout << "STONE" << std::endl;
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
			printf("Noise: %f\n", PerlinNoise->Noise[i][j]);
		}
	}
}