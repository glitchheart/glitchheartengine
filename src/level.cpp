static void GenerateRoom(uint32 Width, uint32 Height, room *Room)
{
    for(uint32 i = 0; i < Width; i++)
    {
        for(uint32 j = 0; j < Height; j++)
        {
            if(i == 0 || j == 0 || i == Width - 1 || j == Height - 1)
                Room->Data[i][j] = { Tile_Stone, glm::vec2(0.2f, 0) };
            else
                Room->Data[i][j] = { Tile_Sand, glm::vec2(0.4f, 0) };
            /*
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
        */
        }
    }
}

