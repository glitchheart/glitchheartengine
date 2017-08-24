                              static void LoadOrthogonalTilemapBuffer(renderer& Renderer, tilemap& Tilemap, memory_arena* TempArena)
                              {
                                  r32* VertexBuffer = PushArray(TempArena, 16 * Tilemap.Width * Tilemap.Height, r32);
                                  
                                  i32 Size = 0;
                                  
                                  texture_data* TextureData = Renderer.TextureMap[Tilemap.TextureName];
                                  
                                  r32 Width = (r32)TextureData->Width;
                                  r32 Height = (r32)TextureData->Height;
                                  
                                  for(i32 X = 0; X < Tilemap.Width; X++)
                                  {
                                      for(i32 Y = 0; Y < Tilemap.Height; Y++)
                                      {
                                          tile_data* Tile = &Tilemap.Data[0][X][Y];
                                          
                                          if(Tile->TypeIndex != -1)
                                          {
                                              r32 TexCoordX = (Tile->TextureOffset.x) / Width;
                                              r32 TexCoordY = (Tile->TextureOffset.y) / Height; 
                                              r32 TexCoordXHigh = (Tile->TextureOffset.x + 16) / Width;
                                              r32 TexCoordYHigh = (Tile->TextureOffset.y + 16) / Height;
                                              
                                              r32 CorrectY = (r32)Y;
                                              
                                              VertexBuffer[Size++] = (r32)X;
                                              VertexBuffer[Size++] = (r32)CorrectY + 1.0f;
                                              VertexBuffer[Size++] = (r32)TexCoordX;
                                              VertexBuffer[Size++] =  (r32)TexCoordY;
                                              VertexBuffer[Size++] = (r32)X + 1;
                                              VertexBuffer[Size++] = (r32)CorrectY + 1;
                                              VertexBuffer[Size++] = (r32)TexCoordXHigh;
                                              VertexBuffer[Size++] =  (r32)TexCoordY;
                                              VertexBuffer[Size++] = (r32)X + 1;
                                              VertexBuffer[Size++] = (r32)CorrectY;
                                              VertexBuffer[Size++] = (r32)TexCoordXHigh;
                                              VertexBuffer[Size++] = (r32)TexCoordYHigh;
                                              VertexBuffer[Size++] = (r32)X;
                                              VertexBuffer[Size++] = (r32)CorrectY;
                                              VertexBuffer[Size++] =(r32)TexCoordX;
                                              VertexBuffer[Size++] = (r32)TexCoordYHigh;
                                          }
                                      }
                                  }
                                  
                                  Tilemap.BufferHandle = 0;
                                  LoadBuffer(Renderer, VertexBuffer, Size, &Tilemap.BufferHandle);
                              }
                              
                              static void LoadTilemapBuffer(renderer& Renderer, tilemap& Tilemap, memory_arena* TempArena)
                              {
                                  r32* VertexBuffer = PushArray(TempArena, 16 * Tilemap.Width * Tilemap.Height, r32);
                                  
                                  i32 Size = 0;
                                  
                                  texture_data* TextureData = Renderer.TextureMap[Tilemap.TextureName];
                                  
                                  r32 Width = (r32)TextureData->Width;
                                  r32 Height = (r32)TextureData->Height;
                                  
                                  for(i32 X = 0; X < Tilemap.Width; X++)
                                  {
                                      for(i32 Y = 0; Y < Tilemap.Height; Y++)
                                      {
                                          tile_data* Tile = &Tilemap.Data[0][X][Y];
                                          
                                          if(Tile->TypeIndex != -1)
                                          {
                                              r32 TexCoordX = (Tile->TextureOffset.x) / Width;
                                              r32 TexCoordY = (Tile->TextureOffset.y) / Height; 
                                              r32 TexCoordXHigh = (Tile->TextureOffset.x + Tilemap.TileWidth) / Width;
                                              r32 TexCoordYHigh = (Tile->TextureOffset.y + Tilemap.TileHeight) / Height; 
                                              
                                              math::v2 CorrectPosition = ToIsometric(math::v2(X, Y));
                                              r32 CorrectX = CorrectPosition.x;
                                              r32 CorrectY = CorrectPosition.y;
                                              
                                              VertexBuffer[Size++] = (r32)CorrectX;
                                              VertexBuffer[Size++] = (r32)CorrectY + 0.5f;
                                              VertexBuffer[Size++] = (r32)TexCoordX;
                                              VertexBuffer[Size++] =  (r32)TexCoordY;
                                              VertexBuffer[Size++] = (r32)CorrectX + 1;
                                              VertexBuffer[Size++] = (r32)CorrectY + 0.5f;
                                              VertexBuffer[Size++] = (r32)TexCoordXHigh;
                                              VertexBuffer[Size++] =  (r32)TexCoordY;
                                              VertexBuffer[Size++] = (r32)CorrectX + 1;
                                              VertexBuffer[Size++] = (r32)CorrectY;
                                              VertexBuffer[Size++] = (r32)TexCoordXHigh;
                                              VertexBuffer[Size++] = (r32)TexCoordYHigh;
                                              VertexBuffer[Size++] = (r32)CorrectX;
                                              VertexBuffer[Size++] = (r32)CorrectY;
                                              VertexBuffer[Size++] =(r32)TexCoordX;
                                              VertexBuffer[Size++] = (r32)TexCoordYHigh;
                                          }
                                      }
                                  };
                                  
                                  Tilemap.BufferHandle = 0;
                                  LoadBuffer(Renderer, VertexBuffer, Size, &Tilemap.BufferHandle);
                              }
                              
                              static void UpdateTileData(i32 SelectedTypeIndex, b32 IsSolid, tilemap* Tilemap)
                              {
                                  for(i32 X = 0; X < Tilemap->Width; X++)
                                  {
                                      for(i32 Y = 0; Y < Tilemap->Height; Y++)
                                      {
                                          if(Tilemap->Data[1][X][Y].TypeIndex == SelectedTypeIndex)
                                              Tilemap->Data[1][X][Y].IsSolid = IsSolid;
                                      }
                                  }
                              }
                              
                              static void SaveTilesheetMetaFile(const char* FilePath, renderer& Renderer, level& Level, b32 New = false)
                              {
                                  // @Incomplete: Holy moly we did some crazy stuff when we were young
                                  /*
                                      FILE* File;
                                  File = fopen(FilePath, "w");
                                  
                                  if(File)
                                  {
                                      if(New)
                                      {
                                          texture_data* TextureData = Renderer.TextureMap[Level.SheetName];
                                          
                                          fprintf(File, "%d\n", TextureData->Width / 64 * TextureData->Height / 32);
                                          
                                          fprintf(File, "%d\n", 16);
                                          
                                          i32 Index = 0;
                                          
                                          for(u32 Y = 0; Y < (u32)Texture.Height / RenderState.Tilesheets[TextureIndex].TileHeight; Y++)
                                          {
                                              for(u32 X = 0; X < (u32)Texture.Width / RenderState.Tilesheets[TextureIndex].TileWidth; X++)
                                              {
                                                  fprintf(File, "%d %d %d %d %d %d %f %f\n", Index, (i32)X * RenderState.Tilesheets[TextureIndex].TileWidth, (i32)Y * RenderState.Tilesheets[TextureIndex].TileHeight, RenderState.Tilesheets[TextureIndex].TileWidth, RenderState.Tilesheets[TextureIndex].TileHeight, 0, 0.5f, 0.5);
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
                                  }*/
                              }
                              
                              static void LoadTilesheetMetaFile(char* FilePath, level* Level, tilemap* Tilemap, game_state* GameState, renderer& Renderer)
                              {
                                  if(Platform.FileExists(FilePath))
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
                                              Tilemap->Tiles = PushArray(&GameState->WorldArena, NumTiles, tile_data);
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
                                              math::v2 TextureOffset;
                                              math::v2 TextureSize;
                                              math::v2 Center;
                                              
                                              sscanf(LineBuffer,"%d %f %f %f %f %d %f %f", &Data.TypeIndex, &TextureOffset.x, &TextureOffset.y, &TextureSize.x, &TextureSize.y, &Data.IsSolid, &Center.x, &Center.y);
                                              Data.TextureOffset = TextureOffset;
                                              Data.TextureSize = TextureSize;
                                              Data.Center = Center;
                                              Tilemap->Tiles[TileIndex++] = Data;
                                          }
                                          
                                          Tilemap->TileCount = TileIndex;
                                          fclose(File);
                                      }
                                  }
                                  else
                                  {
                                      SaveTilesheetMetaFile(FilePath, Renderer, *Level, true);
                                      LoadTilesheetMetaFile(FilePath, Level, Tilemap, GameState, Renderer);
                                  }
                              }
                              
                              static b32 LoadLevelFromFile(char* FilePath, level* Level, game_state* GameState, transient_state* TranState, renderer& Renderer, sound_queue* SoundQueue)
                              {
                                  //read the file manmain
                                  FILE* File;
                                  File = fopen(FilePath, "r");
                                  char LineBuffer[255];
                                  
                                  u32 MapWidth = 0;
                                  u32 MapHeight = 0;
                                  
                                  if(File)
                                  {
                                      Level->Name = PushString(&GameState->WorldArena, 30);
                                      if(fgets(LineBuffer, 255, File))
                                          sscanf(LineBuffer, "%s", Level->Name);
                                      
                                      Level->SheetName = PushString(&GameState->WorldArena, 30);
                                      if(fgets(LineBuffer, 255, File))
                                          sscanf(LineBuffer, "%s", Level->SheetName);
                                      
                                      Level->Tilemap.TextureName = Level->SheetName;
                                      
                                      LoadTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", Level->SheetName, &TranState->TranArena), ".tm", &TranState->TranArena), Level, &Level->Tilemap, GameState, Renderer);
                                      
                                      if(fgets(LineBuffer, 255, File))
                                          sscanf(LineBuffer, "%f %f %f", &Level->PlayerStartPosition.x, &Level->PlayerStartPosition.y, &Level->PlayerStartPosition.z);
                                      
                                      LoadPlayerData(GameState, TranState, SoundQueue, -1, math::Floor(Level->PlayerStartPosition));
                                      
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
                                          Level->Tilemap.Data[Layer] = PushArray(&GameState->WorldArena, MapWidth, tile_data*);
                                          GameState->EntityTilePositions = PushArray(&GameState->WorldArena, MapWidth, i32*);
                                      }
                                      
                                      for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
                                      {
                                          for(u32 I = 0; I < MapWidth; I++)
                                          {
                                              Level->Tilemap.Data[Layer][I] = PushArray(&GameState->WorldArena, MapHeight, tile_data);
                                              GameState->EntityTilePositions[I] = PushArray(&GameState->WorldArena, MapHeight, i32);
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
                                                  
                                                  tile_data Data;
                                                  
                                                  if(TypeIndex > 0)
                                                  {
                                                      Data = Level->Tilemap.Tiles[TypeIndex - 1];
                                                      collision_AABB CollisionAABB;
                                                      CollisionAABB.Center = math::v3(IndexWidth + 0.5f, 0.0f, MapHeight - IndexHeight - 0.5f);
                                                      CollisionAABB.Extents = math::v3(0.5, 0.5, 0.5);
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
                                      
                                      LoadOrthogonalTilemapBuffer(Renderer, Level->Tilemap, &TranState->TranArena);
                                      
                                      u32 PathIndex = 0;
                                      
                                      while(fgets(LineBuffer, 255, File))
                                      {
                                          if(StartsWith(LineBuffer, "skeleton"))
                                          {
                                              math::v3 Pos;
                                              Pos = math::Floor(Pos);
                                              sscanf(LineBuffer, "skeleton %f %f %f%n", &Pos.x, &Pos.y, &Pos.z, &PathIndex);
                                              LoadSkeletonData(GameState, TranState, -1, Pos);
                                          }
                                          else if(StartsWith(LineBuffer, "bonfire"))
                                          {
                                              math::v3 Pos;
                                              Pos = math::Floor(Pos);
                                              sscanf(LineBuffer, "bonfire %f %f %f", &Pos.x, &Pos.y, &Pos.z);
                                              LoadBonfireData(GameState, TranState, SoundQueue, -1, Pos);
                                          }
                                          
                                          if(PathIndex != 0)
                                          {
                                              auto& Entity = GameState->Entities[GameState->EntityCount - 1];
                                              
                                              i32 WaypointCount = 0;
                                              char* PathPtr = LineBuffer;
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
                                                      Entity.Enemy.Waypoints[Index].x = (u32)strtol(PathPtr, &PathPtr, 10);
                                                      Entity.Enemy.Waypoints[Index].z = (u32)strtol(PathPtr, &PathPtr, 10);
                                                  }
                                              }
                                          }
                                      }
                                      
                                      fclose(File);
                                      
                                      //@Incomplete: Daniel says this is dumb.
                                      /*
                                      for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
                                      {
                                          Level->Tilemap.RenderInfo.VAOS[Layer] = 0;
                                      }
                                      Level->Tilemap.RenderInfo.Dirty = true;
                                      */
                                      Renderer.Cameras[GameState->GameCameraHandle].Center = math::v3(GameState->Entities[0].Position.x, GameState->Entities[0].Position.y, GameState->Entities[0].Position.z);
                                      
                                      return true;
                                  }
                                  return false;
                              }
                              
                              static void SaveLevelToFile(const char* FilePath, level* Level, game_state* GameState, transient_state* TranState, renderer& Renderer, b32 New = false)
                              {
                                  FILE* File;
                                  File = fopen(FilePath, "w");
                                  
                                  if(File)
                                  {
                                      SaveTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", Level->SheetName, &TranState->TranArena),".tm", &TranState->TranArena), Renderer, *Level, New);
                                      
                                      fprintf(File, "%s\n", Level->Name);
                                      fprintf(File, "%s\n", Level->SheetName);
                                      
                                      entity* Player = &GameState->Entities[GameState->PlayerIndex];
                                      fprintf(File, "%f %f %f\n", Player->Position.x, Player->Position.y, Player->Position.z);
                                      
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
                                          for(i32 Y = 0; Y < Level->Tilemap.Height; Y++)
                                          {
                                              for(i32 X = 0; X < Level->Tilemap.Width; X++)
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
                                          for(i32 Index = 0; Index < GameState->EntityCount; Index++)
                                          {
                                              if(Index != GameState->PlayerIndex)
                                              {
                                                  entity* Entity = &GameState->Entities[Index];
                                                  char* TypeName = 0;
                                                  if(!IsSet(Entity, EFlag_IsTemporary))
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
                                                                      fprintf(File, "%d %d\n", Entity->Enemy.Waypoints[Index].x, Entity->Enemy.Waypoints[Index].y);
                                                                  }
                                                                  else
                                                                  {
                                                                      fprintf(File, "%d %d ", Entity->Enemy.Waypoints[Index].x, Entity->Enemy.Waypoints[Index].y);
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
                              
                              static void CreateNewLevelWithSize(char* FilePath, u32 Width, u32 Height, level* NewLevel, renderer& Renderer, game_state* GameState, transient_state* TranState, sound_queue* SoundQueue)
                              {
                                  NewLevel->Tilemap.Width = Width;
                                  NewLevel->Tilemap.Height = Height;
                                  
                                  NewLevel->TilesheetIndex = 0;
                                  
                                  for(i32 Layer = 0; Layer < TILEMAP_LAYERS; Layer++)
                                  {
                                      NewLevel->Tilemap.Data[Layer] = PushArray(&GameState->WorldArena, Width, tile_data*);
                                      
                                      for(u32 I = 0; I < Width; I++)
                                      {
                                          NewLevel->Tilemap.Data[Layer][I] = PushArray(&GameState->WorldArena, Height, tile_data);
                                          NewLevel->Tilemap.Data[Layer][I]->TypeIndex = 0;
                                      }
                                  }
                                  
                                  
                                  SaveLevelToFile(FilePath, NewLevel, GameState, TranState, Renderer, true);
                                  LoadTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", NewLevel->SheetName, &TranState->TranArena), ".tm", &TranState->TranArena), NewLevel, &NewLevel->Tilemap, GameState, Renderer);
                                  LoadLevelFromFile(FilePath, NewLevel, GameState, TranState, Renderer, SoundQueue);
}