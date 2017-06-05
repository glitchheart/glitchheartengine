static bool32 LoadLevelFromFile(char* FilePath, level* Level, game_state* GameState)
{
    //read the file manmain
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    uint32 MapWidth = 0;
    uint32 MapHeight = 0;
    
    Level->Tilemap.RenderEntity.ShaderIndex = Shader_Tile;
    Level->Tilemap.RenderEntity.TextureHandle = GameState->RenderState.TileTexture;
    
    if(File)
    {
        Level->Name = (char*)malloc(sizeof(char) * 30);
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%s", Level->Name);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%f %f", &Level->PlayerStartPosition.x, &Level->PlayerStartPosition.y);
        
        InitPlayer(GameState, Level->PlayerStartPosition);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapWidth);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapHeight);
        
        Assert(MapWidth > 0 && MapHeight > 0);
        
        Level->Tilemap.Width = MapWidth;
        Level->Tilemap.Height = MapHeight;
        
        Level->Tilemap.Data = (tile_data**)malloc(MapWidth * sizeof(tile_data*));
        
        for(uint32 I = 0; I < MapWidth; I++)
        {
            Level->Tilemap.Data[I] = (tile_data *)malloc(MapHeight * sizeof(tile_data));
        }
        
        char Line[256];
        uint32 IndexHeight = 0;
        
        while (IndexHeight < MapHeight)
        {
            fgets(Line, sizeof(Line), File);
            for(uint32 IndexWidth = 0; IndexWidth < MapWidth; ++IndexWidth) 
            {
                collision_AABB CollisionAABB;
                CollisionAABB.Center = glm::vec2(IndexWidth + 0.5f, MapHeight - IndexHeight + 0.5f);
                CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                CollisionAABB.IsTrigger = false;
                tile_data Data;
                
                switch(Line[IndexWidth])
                {
                    case '0': //empty
                    Data.Type = Tile_None;
                    break;
                    case 'g': //grass
                    Data = Level->Tilemap.Tiles[0];
                    Data.CollisionAABB = CollisionAABB;
                    break;
                    case 'w': //stone wall
                    Data = Level->Tilemap.Tiles[1];
                    Data.CollisionAABB = CollisionAABB;
                    break;
                    case 's':
                    Data = Level->Tilemap.Tiles[2];
                    Data.CollisionAABB = CollisionAABB;
                    break;
                }
                
                Level->Tilemap.Data[IndexWidth][IndexHeight] = Data;
            }
            IndexHeight++;
        }
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "enemy"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "enemy %f %f", &Pos.x, &Pos.y);
                SpawnEnemy(GameState, Pos);
            }
            else if(StartsWith(&LineBuffer[0], "barrel"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "barrel %f %f", &Pos.x, &Pos.y);
                SpawnBarrel(GameState, Pos);
            }
        }
        fclose(File);
        return true;
    }
    return false;
}

static void SaveLevelToFile(const char* FilePath, level* Level, game_state* GameState)
{
    FILE* File;
    File = fopen(FilePath, "w");
    if(File)
    {
        fprintf(File, "%s\n", Level->Name);
        
        entity* Player = &GameState->Entities[GameState->PlayerIndex];
        fprintf(File, "%f %f\n", Player->Position.x, Player->Position.y);
        
        fprintf(File, "%d\n", Level->Tilemap.Width);
        fprintf(File, "%d\n", Level->Tilemap.Height);
        
        for(uint32 Y = 0; Y < Level->Tilemap.Height; Y++)
        {
            for(uint32 X = 0; X < Level->Tilemap.Width; X++)
            {
                char Character;
                
                switch(Level->Tilemap.Data[X][Y].Type)
                {
                    case Tile_None:
                    Character = '0';
                    break;
                    case Tile_Grass:
                    Character = 'g';
                    break;
                    case Tile_Stone:
                    Character = 'w';
                    break;
                    case Tile_Sand:
                    Character = 's';
                    break;
                }
                
                fprintf(File, "%c", Character);
            }
            fprintf(File, "\n");
        }
        
        for(uint32 Index = 0; Index < GameState->EntityCount; Index++)
        {
            if(Index != GameState->PlayerIndex)
            {
                const entity* Entity = &GameState->Entities[Index];
                char* TypeName = 0;
                switch(Entity->Type)
                {
                    case Entity_Enemy:
                    TypeName = "enemy";
                    break;
                    case Entity_Barrel:
                    TypeName = "barrel";
                    break;
                }
                
                if(TypeName)
                    fprintf(File, "%s %f %f\n", TypeName, Entity->Position.x, Entity->Position.y);
            }
        }
        
        fclose(File);
    }
}