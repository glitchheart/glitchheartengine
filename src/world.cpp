static void GenerateNoise(perlin_noise *PerlinNoise, int Width, int Height, int Seed)
{
    PerlinNoise->Width = Width;
    PerlinNoise->Height = Height;
    PerlinNoise->Noise = (real32 **)malloc(sizeof(real32 *) * Width);

    for (int i = 0; i < Width; i++)
    {
		PerlinNoise->Noise[i] = (real32 *)malloc(sizeof(real32) * Height);
    }

    for (int i = 0; i < Width; i++)
    {
		for (int j = 0; j < Height; j++)
		{
			PerlinNoise->Noise[i][j] = glm::perlin(glm::vec2(((real32)i + Seed) / 100.0f + 0.5f, ((real32)j + Seed) / 100.0f + 0.5f));//glm::vec2(((real32)i + (real32)Seed) + 0.01f, ((real32)j + (real32)Seed) + 0.01f));

			real32 DistanceX = std::abs((real32)i - (real32)Width * 0.5f);
			real32 DistanceY = std::abs((real32)j - (real32)Height * 0.5f);
			real32 Distance = (real32)sqrt(DistanceX * DistanceX + DistanceY * DistanceY);

			real32 MaxWidth = Width * 0.5f - 10.0f;
			real32 Delta = Distance / MaxWidth;
			real32 Gradient = Delta * Delta;

			auto GRAAD = (real32)std::max(0.0f, 1.0f - Gradient);

			PerlinNoise->Noise[i][j] *= GRAAD;
		}
    }
}

static void __GenerateIsland(tile_chunk* Chunk, glm::vec2 Position, uint32 Width, uint32 Height)
{
	perlin_noise PerlinNoise;
	GenerateNoise(&PerlinNoise, Width, Height, Chunk->Seed);

	for(int i = 0; i < Width; i++)
	{
		for(int j = 0; j < Height; j++)
		{
			real32 elevation = PerlinNoise.Noise[i][j];
			
			printf("%g\n", elevation);

			if(elevation >= 0.4)
				Chunk->Data[i][j] = { Tile_Stone, glm::vec2(0.8f, 0) };
			else if(elevation >= 0.22)
				Chunk->Data[i][j] = { Tile_Stone, glm::vec2(0.8f, 0) };
			else if(elevation >= 0.1)
				Chunk->Data[i][j] = { Tile_Grass, glm::vec2(0, 0) };
			else if(elevation >= 0.3)
				Chunk->Data[i][j] = { Tile_Sand, glm::vec2(0.4f, 0) };
			else
				Chunk->Data[i][j] = { Tile_None, glm::vec2(0, 0) };
		}
	}
}

//start with 3 x 3 grid of chunks
//generate a seed for each
//put seed into tile_chunk struct with noise / tile_data array data

static void GenerateTilemap(tilemap_data *TilemapData)
{
	srand (time(NULL));

	for(int i = 0; i < TILEMAP_SIZE; i++)
	{
		for(int j = 0; j < TILEMAP_SIZE; j++)
		{
			TilemapData->Chunks[i][j] = {};
			TilemapData->Chunks[i][j].Seed = 100 + (rand() % (int)(400000 / 2 - 100 + 1)); //TODO(Daniel) Create random function and seed generation function
			TilemapData->Chunks[i][j].X = i;
			TilemapData->Chunks[i][j].Y = j;

			__GenerateIsland(&TilemapData->Chunks[i][j], glm::vec2(i * CHUNK_SIZE, j * CHUNK_SIZE), CHUNK_SIZE, CHUNK_SIZE);
		}
	}
}