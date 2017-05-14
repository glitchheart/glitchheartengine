static void SpawnEnemy(game_state* GameState, glm::vec2 Position, entity* Enemy)
{
    Enemy->Name = "enemy";
    Enemy->Type = Entity_Enemy;
    
    animation* EnemyIdleAnimation = (animation*)malloc(sizeof(animation));
    
    LoadAnimationFromFile("../assets/animations/player_anim_idle_new.pownim", EnemyIdleAnimation, &GameState->RenderState);
    Enemy->Animations.insert(std::pair<char*, animation>(EnemyIdleAnimation->Name, *EnemyIdleAnimation));
    
    animation* EnemyWalkingAnimation = (animation*)malloc(sizeof(animation));
    LoadAnimationFromFile("../assets/animations/player_anim_walk_new.pownim", EnemyWalkingAnimation, &GameState->RenderState);
    Enemy->Animations.insert(std::pair<char*, animation>(EnemyWalkingAnimation->Name, *EnemyWalkingAnimation));
    
    animation* EnemyAttackingAnimation = (animation*)malloc(sizeof(animation));
    
    LoadAnimationFromFile("../assets/animations/player_anim_attack_new.pownim", EnemyAttackingAnimation, &GameState->RenderState);
    Enemy->Animations.insert(std::pair<char*, animation>(EnemyAttackingAnimation->Name, *EnemyAttackingAnimation));
    
    render_entity EnemyRenderEntity = { };
    EnemyRenderEntity.ShaderIndex = Shader_SpriteSheetShader;
    EnemyRenderEntity.TextureHandle = LoadTexture("../assets/textures/new_player.png");
    Enemy->RenderEntity = EnemyRenderEntity;
    Enemy->Rotation = glm::vec3(0, 0, 0);
    Enemy->Position = Position;
    Enemy->Scale = glm::vec3(2, 2, 0);
    Enemy->Velocity = glm::vec2(-2,0);
    
    Enemy->Layer = Layer_Enemy;
    //Enemy->IgnoreLayers = Layer_Enemy;
    
    collision_AABB CollisionAABB;
    Enemy->Center = glm::vec2(0.5f, 0.950f);
    CollisionAABB.Center = glm::vec2(Enemy->Position.x + Enemy->Center.x * Enemy->Scale.x,
                                     Enemy->Position.y + Enemy->Center.y * Enemy->Scale.y);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    Enemy->CollisionAABB = CollisionAABB;
    
    Enemy->Enemy.WalkingSpeed = 5;
    Enemy->Enemy.MaxAlertDistance = 10;
    Enemy->Enemy.MinDistance = 1;
    Enemy->Enemy.AttackCooldown = 1.0f;
    Enemy->Enemy.AIState = AI_Idle;
    
    Enemy->EntityIndex = GameState->EntityCount;
}

static bool32 LoadLevelFromFile(char* FilePath, level* Level, game_state* GameState)
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
            sscanf(LineBuffer, "%f %f", &Level->PlayerStartPosition.x, &Level->PlayerStartPosition.y);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "%d", &MapWidth);
        
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
                    case '0': //empty
                    Data.Type = Tile_None;
                    break;
                    case 'g': //grass
                    Data.Type = Tile_Grass;
                    Data.TextureOffset = glm::vec2(0, 0);
                    Data.IsSolid = false;
                    Data.CollisionAABB = CollisionAABB;
                    break;
                    case 'w': //stone wall
                    Data.Type = Tile_Stone;
                    Data.TextureOffset = glm::vec2(0.8f, 0);
                    Data.IsSolid = true;
                    Data.CollisionAABB = CollisionAABB;
                    break;
                    case 's':
                    Data.Type = Tile_Sand;
                    Data.TextureOffset = glm::vec2(0.6f, 0);
                    Data.IsSolid = false;
                    Data.CollisionAABB = CollisionAABB;
                    break;
                }
                
                Level->Tilemap.Data[IndexWidth][IndexHeight] = Data;
            }
            IndexHeight++;
        }
        
        while(fgets(LineBuffer, 255, File))
        {
            glm::vec2 Pos;
            sscanf(LineBuffer, "enemy %f %f", &Pos.x, &Pos.y);
            SpawnEnemy(GameState, Pos, &GameState->Entities[GameState->EntityCount]);
            GameState->EntityCount++;
        }
        fclose(File);
        return true;
    }
    return false;
}
