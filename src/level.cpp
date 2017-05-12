static void GenerateRoom(real32 X, real32 Y, uint32 Width, uint32 Height, room *Room)
{
    Room->X = X;
    Room->Y = Y;
    Room->Width = Width;
    Room->Height = Height;
    
    Room->Data = (tile_data**)malloc(Width * sizeof(tile_data*));
    
    for (uint32 Index = 0; Index < Width; Index++) 
    {
        Room->Data[Index] = (tile_data*)malloc(Height * sizeof(tile_data));
    }
    /*
    for(uint32 i = 0; i < Width; i++)
    {
        for(uint32 j = 0; j < Height; j++)
        {
            if(i == 0 || j == 0 || i == Width - 1 || j == Height - 1)
                Room->Data[i][j] = { Tile_Stone, 0, glm::vec2(0.8,0), false, 0 };
            else
                Room->Data[i][j] = { Tile_Sand, 0, glm::vec2(0, 0), false, 0 };
        }
    }*/
}

static void LoadTilemap(char* FilePath, tilemap* Tilemap)
{
    //read the file manmain
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    uint32 MapWidth;
    uint32 MapHeight;
    uint32** TempMap;
    int Ch;
    
    if(File)
    {
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapWidth);
        
        //type
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapHeight);
        
        Tilemap->Data = (tile_data**)malloc(MapWidth * sizeof(tile_data*));
        
        for(int I = 0; I < MapWidth; I++)
        {
            Tilemap->Data[I] = (tile_data *)malloc(MapHeight * sizeof(tile_data));;
            for(int J = 0; J < MapHeight; J++)
            {
                Ch = getc(File);
                collision_AABB CollisionAABB;
                CollisionAABB.Center = glm::vec2(0,0);
                CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
                tile_data Data;
                
                switch(Ch)
                {
                    case '0':
                    Data.Type = Tile_None;
                    
                    //{ Tile_None, 0, glm::vec2(0,0), false, glm::vec2(0.5, 0.5), CollisionAABB }
                    //Tilemap->Data[I][J] = ;
                    break;
                    case '1':
                    //Tilemap->Data[I][J] = { Tile_Stone, 0, glm::vec2(0.8, 0), false, glm::vec2(0.5, 0.5), CollisionAABB };
                    break;
                    case '2':
                    Tilemap->Data[I][J] = {};
                    break;
                    case '3':
                    Tilemap->Data[I][J] = {};
                    break;
                    case '4':
                    Tilemap->Data[I][J] = {};
                    break;
                    case '5':
                    Tilemap->Data[I][J] = {};
                    break;
                    case '6':
                    Tilemap->Data[I][J] = {};
                    break;
                    case '7':
                    Tilemap->Data[I][J] = {};
                    break;
                    case '8':
                    Tilemap->Data[I][J] = {};
                    break;
                }
                
            }
            
            Ch = getc(File);
        }
    }
}
