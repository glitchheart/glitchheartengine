static void SaveTilesheetMetaFile(const char* FilePath, render_state* RenderState, level& Level, bool32 New = false)
{
    FILE* File;
    File = fopen(FilePath, "w");
    
    if(File)
    {
        if(New)
        {
            uint32 TextureIndex = 0;
            
            for(uint32 Index = 0; RenderState->TilesheetCount; Index++)
            {
                if(strcmp(Level.SheetName, RenderState->Tilesheets[Index].Name) == 0)
                {
                    TextureIndex = Index;
                    break;
                }
            }
            
            const texture& Texture = RenderState->Tilesheets[TextureIndex].Texture;
            
            fprintf(File, "%d\n", Texture.Width / 16 * Texture.Height / 16);
            fprintf(File, "%d\n", 16);
            
            uint32 Index = 0;
            for(uint32 Y = 0; Y < Texture.Height / 16; Y++)
            {
                for(uint32 X = 0; X < Texture.Width / 16; X++)
                {
                    fprintf(File, "%d %d %d %d %d %d %f %f\n", Index, (int32)X * 16, (int32)Y * 16, 16, 16, 0, 0.5f, 0.5);
                    Index++;
                }
            }
        }
        else
        {
            fprintf(File, "%d\n", Level.Tilemap.TileCount);
            fprintf(File, "%d\n", Level.Tilemap.TileSize);
            
            for(uint32 Index = 0; Index < Level.Tilemap.TileCount; Index++)
            {
                const tile_data& TileData = Level.Tilemap.Tiles[Index];
                fprintf(File, "%d %d %d %d %d %d %f %f\n", Index, (int32)TileData.TextureOffset.x, (int32)TileData.TextureOffset.y, Level.Tilemap.TileSize, Level.Tilemap.TileSize, TileData.IsSolid, TileData.Center.x, TileData.Center.y); 
            }
        }
        fclose(File);
    }
}

static void LoadTilesheetMetaFile(char* FilePath, level* Level, tilemap* Tilemap, game_state* GameState)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    if(File)
    {
        // Get number of tiles for array allocation
        int NumTiles;
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "%d", &NumTiles);
            Tilemap->Tiles = (tile_data*)malloc(sizeof(tile_data) * NumTiles);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "%d", &Tilemap->TileSize);
        }
        
        int TileIndex = 0;
        // Get each tile
        while(fgets(LineBuffer, 255, File))
        {
            tile_data Data = {};
            glm::vec2 TextureOffset;
            glm::vec2 TextureSize;
            glm::vec2 Center;
            
            sscanf(LineBuffer,"%d %f %f %f %f %d %f %f", &Data.TypeIndex, &TextureOffset.x, &TextureOffset.y, &TextureSize.x, &TextureSize.y, &Data.IsSolid, &Center.x, &Center.y);
            Data.TextureOffset = TextureOffset;
            Data.TextureSize = TextureSize;
            Data.Center = Center;
            Tilemap->Tiles[TileIndex++] = Data;
        }
        Tilemap->TileCount = TileIndex;
        fclose(File);
    }
    else
    {
        SaveTilesheetMetaFile(FilePath, &GameState->RenderState, *Level, true);
        LoadTilesheetMetaFile(FilePath, Level, Tilemap, GameState);
    }
}

static bool32 LoadLevelFromFile(char* FilePath, level* Level, game_state* GameState)
{
    //read the file manmain
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    uint32 MapWidth = 0;
    uint32 MapHeight = 0;
    
    Level->Tilemap.RenderEntity.ShaderIndex = Shader_Tile;
    Level->Tilemap.RenderEntity.Texture = &GameState->RenderState.Tilesheets[Level->TilesheetIndex].Texture;
    
    if(File)
    {
        Level->Name = (char*)malloc(sizeof(char) * 30);
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%s", Level->Name);
        
        Level->SheetName = (char*)malloc(sizeof(char) * 30);
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%s", Level->SheetName);
        
        LoadTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", Level->SheetName), ".tm"), Level, &Level->Tilemap, GameState);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%f %f", &Level->PlayerStartPosition.x, &Level->PlayerStartPosition.y);
        
        InitPlayer(GameState, Level->PlayerStartPosition);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapWidth);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapHeight);
        
        //Assert(MapWidth > 0 && MapHeight > 0);
        
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
            char *Ptr = &Line[0];
            
            for(uint32 IndexWidth = 0; IndexWidth < MapWidth; ++IndexWidth) 
            {
                collision_AABB CollisionAABB;
                CollisionAABB.Center = glm::vec2(IndexWidth + 0.5f, MapHeight - IndexHeight + 0.5f);
                CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                CollisionAABB.IsTrigger = false;
                
                int TypeIndex = (uint32)strtol(Ptr, &Ptr, 10);
                
                tile_data Data = Level->Tilemap.Tiles[TypeIndex];
                Data.CollisionAABB = CollisionAABB;
                
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
        
        Level->Tilemap.RenderInfo.VAO = 0;
        
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
        fprintf(File, "%s\n", Level->SheetName);
        
        entity* Player = &GameState->Entities[GameState->PlayerIndex];
        fprintf(File, "%f %f\n", Player->Position.x, Player->Position.y);
        
        fprintf(File, "%d\n", Level->Tilemap.Width);
        fprintf(File, "%d\n", Level->Tilemap.Height);
        
        for(uint32 Y = 0; Y < Level->Tilemap.Height; Y++)
        {
            for(uint32 X = 0; X < Level->Tilemap.Width; X++)
            {
                fprintf(File, "%d ", Level->Tilemap.Data[X][Y].TypeIndex);
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