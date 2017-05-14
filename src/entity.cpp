static void InitPlayer(game_state* GameState)
{
    entity* Player = &GameState->Entities[GameState->EntityCount];
    
    Player->Name = "Player";
    Player->Type = Entity_Player;
    Player->Player.WalkingSpeed = 10.0f;
    
    animation* IdleAnimation = (animation*)malloc(sizeof(animation));
    
    LoadAnimationFromFile("../assets/animations/player_anim_idle_new.pownim", IdleAnimation, &GameState->RenderState);
    Player->Animations.insert(std::pair<char*, animation>(IdleAnimation->Name, *IdleAnimation));
    
    animation* WalkingAnimation = (animation*)malloc(sizeof(animation));
    LoadAnimationFromFile("../assets/animations/player_anim_walk_new.pownim", WalkingAnimation, &GameState->RenderState);
    Player->Animations.insert(std::pair<char*, animation>(WalkingAnimation->Name, *WalkingAnimation));
    
    animation* AttackingAnimation = (animation*)malloc(sizeof(animation));
    LoadAnimationFromFile("../assets/animations/player_anim_attack_new.pownim", AttackingAnimation, &GameState->RenderState);
    Player->Animations.insert(std::pair<char*, animation>(AttackingAnimation->Name, *AttackingAnimation));
    
    render_entity PlayerRenderEntity = { };
    PlayerRenderEntity.ShaderIndex = Shader_SpriteSheetShader;
    
    PlayerRenderEntity.TextureHandle = GameState->RenderState.PlayerTexture;
    
    Player->RenderEntity = PlayerRenderEntity;
    Player->Rotation = glm::vec3(0, 0, 0);
    Player->Scale = glm::vec3(2, 2, 0);
    Player->Velocity = glm::vec2(0,0);
    
    collision_AABB CollisionAABB;
    Player->Center = glm::vec2(0.5f, 0.950f);
    CollisionAABB.Center = glm::vec2(Player->Position.x + Player->Center.x * Player->Scale.x,
                                     Player->Position.y + Player->Center.y * Player->Scale.y);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    Player->CollisionAABB = CollisionAABB;
    
    Player->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
    
    entity* PlayerWeapon = &GameState->Entities[GameState->EntityCount];
    PlayerWeapon->RenderEntity.Rendered = false;
    PlayerWeapon->Name = "Player weapon";
    PlayerWeapon->Type = Entity_PlayerWeapon;
    
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0.5, 0.5);
    CollisionAABB3.Extents = glm::vec2(0.5f,0.5f);
    CollisionAABB3.IsTrigger = true;
    PlayerWeapon->CollisionAABB = CollisionAABB3;
    PlayerWeapon->Rotation = glm::vec3(0, 0, 0);
    PlayerWeapon->Scale = glm::vec3(1, 1, 0);
    
    PlayerWeapon->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
}

static void InitCrosshair(game_state* GameState)
{
    entity* Crosshair = &GameState->Entities[GameState->EntityCount];
    Crosshair->Name = "Crosshair";
    Crosshair->Type = Entity_Crosshair;
    
    Crosshair->IsKinematic = true;
    collision_AABB CollisionAABB2;
    CollisionAABB2.Center = glm::vec2(0,0);
    CollisionAABB2.Extents = glm::vec2(0.5f,0.5f);
    Crosshair->CollisionAABB = CollisionAABB2;
    
    render_entity CrosshairRenderEntity = { };
    CrosshairRenderEntity.ShaderIndex = Shader_Texture;
    CrosshairRenderEntity.TextureHandle = GameState->RenderState.CrosshairTexture;
    Crosshair->RenderEntity = CrosshairRenderEntity;
    Crosshair->Rotation = glm::vec3(0, 0, 0);
    Crosshair->Scale = glm::vec3(1, 1, 0);
    
    Crosshair->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
}

void SpawnMillionBarrels(game_state* GameState)
{
    uint32 OneMillion = 1;
    for(uint32 i = 0; i < OneMillion; i++)
    {
        for(uint32 j = 0; j < OneMillion; j++)
        {
            entity* Barrel = &GameState->Entities[GameState->EntityCount];
            Barrel->Name = "barrel";
            Barrel->Type = Entity_Barrel;
            Barrel->Layer = Layer_Environment;
            Barrel->IgnoreLayers = Layer_Environment;
            render_entity BarrelRenderEntity = { };
            BarrelRenderEntity.ShaderIndex = Shader_Texture;
            BarrelRenderEntity.TextureHandle = GameState->RenderState.BarrelTexture;
            Barrel->RenderEntity = BarrelRenderEntity;
            Barrel->Rotation = glm::vec3(0, 0, 0);
            Barrel->Position = glm::vec2(2 + i,2 + j);
            Barrel->Scale = glm::vec3(2, 2, 0);
            Barrel->Velocity = glm::vec2(0,0);
            Barrel->Center = glm::vec2(0.5, 0.5);
            Barrel->IsStatic = true;
            Barrel->IsKinematic = false;
            Barrel->Pickup = true;
            
            
            collision_AABB CollisionAABB;
            
            CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
            CollisionAABB.Center = glm::vec2(Barrel->Position.x + Barrel->Center.x * Barrel->Scale.x,
                                             Barrel->Position.y + Barrel->Center.y * Barrel->Scale.y);
            //CollisionAABB.Center = glm::vec2(0.5f,0.5f);
            CollisionAABB.IsTrigger = true;
            Barrel->CollisionAABB = CollisionAABB;
            
            Barrel->EntityIndex = GameState->EntityCount;
            GameState->EntityCount++;
        }
    }
}
