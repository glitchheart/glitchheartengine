static void UpdateTileData(i32 SelectedTypeIndex, b32 IsSolid, tilemap* Tilemap)
{
    for(u32 X = 0; X < Tilemap->Width; X++)
    {
        for(u32 Y = 0; Y < Tilemap->Height; Y++)
        {
            if(Tilemap->Data[1][X][Y].TypeIndex == SelectedTypeIndex)
                Tilemap->Data[1][X][Y].IsSolid = IsSolid;
        }
    }
}

static void SaveTilesheetMetaFile(const char* FilePath, render_state* RenderState, level& Level, b32 New = false)
{
    FILE* File;
    File = fopen(FilePath, "w");
    
    if(File)
    {
        if(New)
        {
            u32 TextureIndex = 0;
            
            for(u32 Index = 0; RenderState->TilesheetCount; Index++)
            {
                if(strcmp(Level.SheetName, RenderState->Tilesheets[Index].Name) == 0)
                {
                    TextureIndex = Index;
                    break;
                }
            }
            
            const texture& Texture = RenderState->Tilesheets[TextureIndex].Texture;
            
            fprintf(File, "%d\n", Texture.Width / RenderState->Tilesheets[TextureIndex].TileWidth * Texture.Height / RenderState->Tilesheets[TextureIndex].TileHeight);
            
            fprintf(File, "%d\n", 16);
            
            i32 Index = 0;
            
            for(u32 Y = 0; Y < (u32)Texture.Height / RenderState->Tilesheets[TextureIndex].TileHeight; Y++)
            {
                for(u32 X = 0; X < (u32)Texture.Width / RenderState->Tilesheets[TextureIndex].TileWidth; X++)
                {
                    fprintf(File, "%d %d %d %d %d %d %f %f\n", Index, (i32)X * RenderState->Tilesheets[TextureIndex].TileWidth, (i32)Y * RenderState->Tilesheets[TextureIndex].TileHeight, RenderState->Tilesheets[TextureIndex].TileWidth, RenderState->Tilesheets[TextureIndex].TileHeight, 0, 0.5f, 0.5);
                    Index++;
                }
            }
        }
        else
        {
            fprintf(File, "%d\n", Level.Tilemap.TileCount);
            fprintf(File, "%d %d\n", Level.Tilemap.TileWidth, Level.Tilemap.TileHeight);
            
            for(u32 Index = 0; Index < Level.Tilemap.TileCount; Index++)
            {
                const tile_data& TileData = Level.Tilemap.Tiles[Index];
                fprintf(File, "%d %d %d %d %d %d %f %f\n", Index, (i32)TileData.TextureOffset.x, (i32)TileData.TextureOffset.y, Level.Tilemap.TileWidth, Level.Tilemap.TileHeight, TileData.IsSolid, TileData.Center.x, TileData.Center.y); 
            }
        }
        fclose(File);
    }
}

static void LoadTilesheetMetaFile(char* FilePath, level* Level, tilemap* Tilemap, game_state* GameState)
{
    if(FileExists(FilePath))
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
                sscanf(LineBuffer, "%d %d", &Tilemap->TileWidth, &Tilemap->TileHeight);
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
            printf("Tilemap tile count %d\n", TileIndex);
            Tilemap->TileCount = TileIndex;
            fclose(File);
        }
    }
    else
    {
        SaveTilesheetMetaFile(FilePath, &GameState->RenderState, *Level, true);
        LoadTilesheetMetaFile(FilePath, Level, Tilemap, GameState);
    }
}

static b32 LoadLevelFromFile(char* FilePath, level* Level, game_state* GameState)
{
    //read the file manmain
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    u32 MapWidth = 0;
    u32 MapHeight = 0;
    
    Level->Tilemap.RenderEntity.ShaderIndex = Shader_Tile;
    
    if(File)
    {
        Level->Name = (char*)malloc(sizeof(char) * 30);
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%s", Level->Name);
        
        Level->SheetName = (char*)malloc(sizeof(char) * 30);
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%s", Level->SheetName);
        
        for(i32 Index = 0; Index < (i32)GameState->RenderState.TilesheetCount; Index++)
        {
            if(strcmp(Level->SheetName, GameState->RenderState.Tilesheets[Index].Name) == 0)
            {
                Level->TilesheetIndex = Index;
                break;
            }
        }
        
        Level->Tilemap.RenderEntity.Texture = &GameState->RenderState.Tilesheets[Level->TilesheetIndex].Texture;
        
        LoadTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", Level->SheetName), ".tm"), Level, &Level->Tilemap, GameState);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%f %f", &Level->PlayerStartPosition.x, &Level->PlayerStartPosition.y);
        
        LoadPlayerData(GameState, -1, Level->PlayerStartPosition);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapWidth);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapHeight);
        
        if(fgets(LineBuffer, 255, File))
        {
            char LevelType[20];
            sscanf(LineBuffer, "%s", LevelType);
            
            if(StartsWith(LevelType, "orthogonal"))
            {
                Level->Type = Level_Orthogonal;
            }
            else if(StartsWith(LevelType, "isometric"))
            {
                Level->Type = Level_Isometric;
            }
        }
        
        Assert(MapWidth > 0 && MapHeight > 0);
        
        Level->Tilemap.Width = MapWidth;
        Level->Tilemap.Height = MapHeight;
        
        for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
        {
            Level->Tilemap.Data[Layer] = (tile_data**)malloc(MapWidth * sizeof(tile_data*));
        }
        
        
        for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
        {
            for(u32 I = 0; I < MapWidth; I++)
            {
                Level->Tilemap.Data[Layer][I] = (tile_data *)calloc(MapHeight, sizeof(tile_data));
            }
        }
        
        char Line[1024];
        Assert(MapWidth < 1024 * 2 + 1);
        u32 IndexHeight = 0;
        
        for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
        {
            while (IndexHeight < MapHeight)
            {
                fgets(Line, sizeof(Line), File);
                char *Ptr = &Line[0];
                
                for(u32 IndexWidth = 0; IndexWidth < MapWidth; ++IndexWidth) 
                {
                    i32 TypeIndex = (u32)strtol(Ptr, &Ptr, 10);
                    
                    tile_data Data = Level->Tilemap.Tiles[TypeIndex];
                    
                    if(TypeIndex > 0)
                    {
                        collision_AABB CollisionAABB;
                        CollisionAABB.Center = glm::vec2(IndexWidth + 0.5f, MapHeight - IndexHeight - 0.5f);
                        CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                        CollisionAABB.IsTrigger = false;
                        
                        Data.CollisionAABB = CollisionAABB;
                        
                        Data.TypeIndex = TypeIndex - 1;
                    }
                    else
                    {
                        Data.TypeIndex = -1;
                    }
                    
                    Level->Tilemap.Data[Layer][IndexWidth][MapHeight - IndexHeight - 1] = Data;
                }
                IndexHeight++;
            }
            IndexHeight = 0;
        }
        
        u32 PathIndex = 0;
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "skeleton"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "skeleton %f %f%n", &Pos.x, &Pos.y, &PathIndex);
                LoadSkeletonData(GameState, -1, Pos);
            }
            else if(StartsWith(&LineBuffer[0], "minotaur"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "minotaur %f %f%n", &Pos.x, &Pos.y, &PathIndex);
                LoadMinotaurData(GameState, -1, Pos);
            }
            
            else if(StartsWith(&LineBuffer[0], "blob"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "blob %f %f%n", &Pos.x, &Pos.y, &PathIndex);
                LoadBlobData(GameState, -1, Pos);
            }
            else if(StartsWith(&LineBuffer[0], "wraith"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "wraith %f %f%n", &Pos.x, &Pos.y, &PathIndex);
                LoadWraithData(GameState, -1, Pos);
            }
            else if(StartsWith(&LineBuffer[0], "barrel"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "barrel %f %f", &Pos.x, &Pos.y);
            }
            else if(StartsWith(&LineBuffer[0], "bonfire"))
            {
                glm::vec2 Pos;
                sscanf(LineBuffer, "bonfire %f %f", &Pos.x, &Pos.y);
                LoadBonfireData(GameState, -1, Pos);
            }
            
            if(PathIndex != 0)
            {
                auto& Entity = GameState->Entities[GameState->EntityCount - 1];
                
                i32 WaypointCount = 0;
                char* PathPtr = &LineBuffer[0];
                PathPtr += PathIndex + 1;
                
                i32 Consumed = 0;
                sscanf(PathPtr, "path %d%n", &WaypointCount, &Consumed);
                PathPtr += Consumed + 1;
                
                if(WaypointCount > 0)
                {
                    Assert(WaypointCount <= 10);
                    Entity.Enemy.WaypointCount = WaypointCount;
                    
                    for(i32 Index = 0; Index < WaypointCount; Index++)
                    {
                        Entity.Enemy.Waypoints[Index].X = (u32)strtol(PathPtr, &PathPtr, 10);
                        Entity.Enemy.Waypoints[Index].Y = (u32)strtol(PathPtr, &PathPtr, 10);
                    }
                }
            }
        }
        
        fclose(File);
        
        for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
        {
            Level->Tilemap.RenderInfo.VAOS[Layer] = 0;
        }
        Level->Tilemap.RenderInfo.Dirty = true;
        
        GameState->Camera.Center = GameState->Entities[0].Position;
        
        return true;
    }
    return false;
}

static void SaveLevelToFile(const char* FilePath, level* Level, game_state* GameState, b32 New = false)
{
    FILE* File;
    File = fopen(FilePath, "w");
    
    if(File)
    {
        SaveTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", Level->SheetName),".tm"), &GameState->RenderState, *Level, New);
        
        fprintf(File, "%s\n", Level->Name);
        fprintf(File, "%s\n", Level->SheetName);
        
        entity* Player = &GameState->Entities[GameState->PlayerIndex];
        fprintf(File, "%f %f\n", Player->Position.x, Player->Position.y);
        
        fprintf(File, "%d\n", Level->Tilemap.Width);
        fprintf(File, "%d\n", Level->Tilemap.Height);
        
        if(Level->Type == Level_Orthogonal)
        {
            fprintf(File,"orthogonal\n");
        }
        else
        {
            fprintf(File,"isometric\n");
        }
        
        for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
        {
            for(u32 Y = 0; Y < Level->Tilemap.Height; Y++)
            {
                for(u32 X = 0; X < Level->Tilemap.Width; X++)
                {
                    if(X == Level->Tilemap.Width - 1)
                        fprintf(File, "%d", Level->Tilemap.Data[Layer][X][Level->Tilemap.Height - Y - 1].TypeIndex + 1);
                    else
                        fprintf(File, "%d ", Level->Tilemap.Data[Layer][X][Level->Tilemap.Height - Y - 1].TypeIndex + 1);
                }
                fprintf(File, "\n");
            }
        }
        
        if(!New)
        {
            for(u32 Index = 0; Index < GameState->EntityCount; Index++)
            {
                if(Index != GameState->PlayerIndex)
                {
                    const entity* Entity = &GameState->Entities[Index];
                    char* TypeName = 0;
                    if(!Entity->IsTemporary)
                    {
                        switch(Entity->Type)
                        {
                            case Entity_Enemy:
                            {
                                switch(Entity->Enemy.EnemyType)
                                {
                                    case Enemy_Skeleton:
                                    {
                                        TypeName = "skeleton";
                                    }
                                    break;
                                    case Enemy_Blob:
                                    {
                                        TypeName = "blob";
                                    }
                                    break;
                                    case Enemy_Wraith:
                                    {
                                        TypeName = "wraith";
                                    }
                                    break;
                                    case Enemy_Minotaur:
                                    {
                                        TypeName = "minotaur";
                                    }
                                    break;
                                }
                            }
                            break;
                            case Entity_Barrel:
                            {
                                TypeName = "barrel";
                            }
                            break;
                            case Entity_Bonfire:
                            {
                                TypeName = "bonfire";
                            }
                            break;
                        }
                        
                        if(TypeName)
                        {
                            if(Entity->Type == Entity_Enemy && Entity->Enemy.WaypointCount > 0)
                            {
                                fprintf(File, "%s %f %f path %d ", TypeName, Entity->Position.x, Entity->Position.y, Entity->Enemy.WaypointCount);
                                for(i32 Index = 0; Index < Entity->Enemy.WaypointCount; Index++)
                                {
                                    if(Index == Entity->Enemy.WaypointCount - 1)
                                    {
                                        fprintf(File, "%d %d\n", Entity->Enemy.Waypoints[Index].X, Entity->Enemy.Waypoints[Index].Y);
                                    }
                                    else
                                    {
                                        fprintf(File, "%d %d ", Entity->Enemy.Waypoints[Index].X, Entity->Enemy.Waypoints[Index].Y);
                                    }
                                }
                            }
                            else
                            {
                                fprintf(File, "%s %f %f\n", TypeName, Entity->Position.x, Entity->Position.y);
                            }
                        }
                    }
                }
            }
        }
        
        fclose(File);
    }
}

static void CreateNewLevelWithSize(char* FilePath, u32 Width, u32 Height, level* NewLevel, game_state* GameState)
{
    NewLevel->Tilemap.Width = Width;
    NewLevel->Tilemap.Height = Height;
    
    NewLevel->TilesheetIndex = 0;
    
    for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
    {
        NewLevel->Tilemap.Data[Layer] = (tile_data**)calloc(Width, sizeof(tile_data*));
        
        for(u32 I = 0; I < Width; I++)
        {
            NewLevel->Tilemap.Data[Layer][I] = (tile_data *)calloc(Height, sizeof(tile_data));
            NewLevel->Tilemap.Data[Layer][I]->TypeIndex = 0;
        }
    }
    
    
    SaveLevelToFile(FilePath, NewLevel, GameState, true);
    LoadTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", NewLevel->SheetName), ".tm"), NewLevel, &NewLevel->Tilemap, GameState);
    LoadLevelFromFile(FilePath, NewLevel, GameState);
}