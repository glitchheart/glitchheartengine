static void GenerateRoom(real32 X, real32 Y, uint32 Width, uint32 Height, room *Room)
{
    Room->X = X;
    Room->Y = Y;
    Room->Width = Width;
    Room->Height = Height;
    
    Room->Data = (tile_data**)malloc(Width * sizeof(tile_data*));
    
    for (int Index = 0; Index < Width; Index++) 
    {
        Room->Data[Index] = (tile_data*)malloc(Height * sizeof(tile_data));
    }
    
    for(uint32 i = 0; i < Width; i++)
    {
        for(uint32 j = 0; j < Height; j++)
        {
            if(i == 0 || j == 0 || i == Width - 1 || j == Height - 1)
                Room->Data[i][j] = { Tile_Stone, glm::vec2(0.8f, 0) };
            else
                Room->Data[i][j] = { Tile_Sand, glm::vec2(0, 0) };
        }
    }
}

