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

static void LoadLevelFromFile(char* FilePath, level* Level)
{
    //read the file manmain
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    uint32 MapWidth;
    uint32 MapHeight;
    
    Level->Tilemap.RenderEntity.ShaderIndex = Shader_Tile;
    Level->Tilemap.RenderEntity.TextureHandle = LoadTexture("../assets/textures/tiles.png");
    
    if(File)
    {
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%s", &Level->Name);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapWidth);
        
        //type
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapHeight);
        
        Level->Tilemap.Width = MapWidth;
        Level->Tilemap.Height = MapHeight;
        
        Level->Tilemap.Data = (tile_data**)malloc(MapWidth * sizeof(tile_data*));
        
        for(int I = 0; I < MapWidth; I++)
        {
            Level->Tilemap.Data[I] = (tile_data *)malloc(MapHeight * sizeof(tile_data));
        }
        
        char Line[256];
        int IndexHeight = 0;
        
        while (IndexHeight < MapHeight)
        {
            fgets(Line, sizeof(Line), File);
            for(int IndexWidth = 0; IndexWidth < MapWidth; ++IndexWidth) 
            {
                collision_AABB CollisionAABB;
                CollisionAABB.Center = glm::vec2(IndexWidth + 0.5f, IndexHeight + 0.5f);
                CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                tile_data Data;
                
                switch(Line[IndexWidth])
                {
                    case '0':
                    Data.Type = Tile_Grass;
                    Data.TextureOffset = glm::vec2(0, 0);
                    Data.IsSolid = false;
                    Data.CollisionAABB = CollisionAABB;
                    break;
                    case '1':
                    Data.Type = Tile_Stone;
                    Data.TextureOffset = glm::vec2(0.8f, 0);
                    Data.IsSolid = true;
                    Data.CollisionAABB = CollisionAABB;
                    break;
                    case '2':
                    break;
                    case '3':
                    break;
                    case '4':
                    break;
                    case '5':
                    break;
                    case '6':
                    break;
                    case '7':
                    break;
                    case '8':
                    
                    break;
                }
                
                Level->Tilemap.Data[IndexWidth][IndexHeight] = Data;
                //printf("IndexWidth %d IndexHeight %d\n", IndexWidth, IndexHeight);
            }
            IndexHeight++;
        }
        
        printf("DAMN\n");
    }
}
