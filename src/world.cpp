#include "world.h"
#include "types.h"
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

static void __GenerateVAO(tile_chunk *Chunk, GLfloat *Buffer, size_t LengthOfBuffer)
{
	glGenVertexArrays(1, &Chunk->VAO);
    glBindVertexArray(Chunk->VAO);

    glGenBuffers(1, &Chunk->VBO);

    glBindBuffer(GL_ARRAY_BUFFER, Chunk->VBO);
    glBufferData(GL_ARRAY_BUFFER, LengthOfBuffer, Buffer, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

static void GenerateWorld(const perlin_noise &Noise, world_data *WorldData)
{
	for(int a = 0; a < WORLD_SIZE; a++)
	{
		for(int b = 0; b < WORLD_SIZE; b++)
		{
			GLfloat VertexBuffer[TILE_CHUNK_SIZE * TILE_CHUNK_SIZE * 16];
			
			tile_chunk Chunk = {};

			for(int i = 0; i < TILE_CHUNK_SIZE; i++)
			{
				for(int j = 0; j < TILE_CHUNK_SIZE; j++)
				{
					int x = a * TILE_CHUNK_SIZE + i;
					int y = b * TILE_CHUNK_SIZE + j;

					real32 N = Noise.Noise[x][y];

					if(N < -0.2f)
					{
						Chunk.Data[i][j] = { Tile_Sand, glm::vec2(0.6f, 0) };
					}
					else if(N < -0.1f)
					{
						Chunk.Data[i][j] = { Tile_Grass, glm::vec2(0, 0) };;
					}
					else if(N < 0)
					{
						Chunk.Data[i][j] = { Tile_DarkGrass, glm::vec2(0.4f, 0) };
					}
					else
					{
						Chunk.Data[i][j] = { Tile_Stone, glm::vec2(0.8f, 0) };
					}
					
					VertexBuffer[(i + j) * 4] = 		x;
					VertexBuffer[(i + j) * 4 + 1] = 	y;
					VertexBuffer[(i + j) * 4 + 2] = 	Chunk.Data[i][j].TextureOffset.x;
					VertexBuffer[(i + j) * 4 + 3] = 	Chunk.Data[i][j].TextureOffset.y;
				}
			}
			
			Chunk.Offset = glm::vec2(a * TILE_CHUNK_SIZE, b * TILE_CHUNK_SIZE);
			
			// __GenerateVAO(&Chunk, x&VertexBuffer[0], TILE_CHUNK_SIZE * TILE_CHUNK_SIZE * 16);
			
			WorldData->TileChunks[a][b] = Chunk;
		}
	}
}

static void GenerateTileChunk(const perlin_noise &Noise, tile_chunk *Chunk)
{
	for(int i = 0; i < TILE_CHUNK_SIZE; i++)
	{
		for(int j = 0; j < TILE_CHUNK_SIZE; j++)
		{
			real32 N = Noise.Noise[i][j];

			if(N < -0.2f)
			{
				Chunk->Data[i][j] = { Tile_Sand, glm::vec2(0.6f, 0) };
			}
			else if(N < -0.1f)
			{
				Chunk->Data[i][j] = { Tile_Grass, glm::vec2(0, 0) };;
			}
			else if(N < 0)
			{
				Chunk->Data[i][j] = { Tile_DarkGrass, glm::vec2(0.4f, 0) };
			}
			else
			{
				Chunk->Data[i][j] = { Tile_Stone, glm::vec2(0.8f, 0) };
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