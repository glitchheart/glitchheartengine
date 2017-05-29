static void PrintEntityInfo(const entity& Entity)
{
    printf("Entity: Name %s, position x %f y %f, rotation x %f y %f z %f\n", Entity.Name, Entity.Position.x, Entity.Position.y, Entity.Rotation.x, Entity.Rotation.y, Entity.Rotation.z);
}

static void InitPlayer(game_state* GameState)
{
    entity* Player = &GameState->Entities[GameState->EntityCount];
    
    Player->Name = "Player";
    Player->Type = Entity_Player;
    Player->Health = 3;
    Player->Player.WalkingSpeed = 8.5f;
    Player->Player.ThrowingSpeed = 18.0f;
    Player->Player.MaxDashTime = 0.2;
    Player->Player.DashSpeed = 30;
    Player->Player.AttackCooldown = 0.3;
    Player->Player.DashCooldown = 0.6;
    Player->Active = true;
    Player->IsKinematic = false;
    Player->CurrentAnimation = 0;
    Player->AnimationInfo.Playing = false;
    Player->AnimationInfo.FrameIndex = 0;
    Player->AnimationInfo.CurrentTime = 0;
    
    render_entity* PlayerRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    PlayerRenderEntity->ShaderIndex = Shader_SpriteSheetShader;
    PlayerRenderEntity->TextureHandle = GameState->RenderState.PlayerTexture;
    PlayerRenderEntity->Rendered = true;
    PlayerRenderEntity->Entity = &*Player;
    Player->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    PlayerRenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    Player->Position = glm::vec2(0, 0);
    Player->Rotation = glm::vec3(0, 0, 0);
    Player->Scale = glm::vec3(2, 2, 0);
    Player->Velocity = glm::vec2(0,0);
    PlayAnimation(Player, &GameState->PlayerIdleAnimation);
    collision_AABB CollisionAABB;
    Player->Center = glm::vec2(0.5f, 0.950f);
    
    Player->Layer = Layer_Player;
    //Player->IgnoreLayers = Layer_Enemy;
    
    CollisionAABB.Center = glm::vec2(Player->Position.x + Player->Center.x * Player->Scale.x,
                                     Player->Position.y + Player->Center.y * Player->Scale.y);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    Player->CollisionAABB = CollisionAABB;
    
    Player->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
    
    entity* PlayerWeapon = &GameState->Entities[GameState->EntityCount];
    PlayerWeapon->Name = "Player weapon";
    PlayerWeapon->Type = Entity_PlayerWeapon;
    PlayerWeapon->Active = true;
    
    render_entity* PlayerWeaponRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    PlayerWeaponRenderEntity->Rendered = true;
    PlayerWeaponRenderEntity->ShaderIndex = Shader_SpriteSheetShader;
    PlayerWeaponRenderEntity->TextureHandle = GameState->RenderState.SwordTopRightTexture;
    PlayerWeaponRenderEntity->Entity = &*PlayerWeapon;
    PlayerWeapon->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    PlayerWeapon->CurrentAnimation = 0;
    PlayerWeapon->AnimationInfo.Playing = false;
    PlayerWeapon->AnimationInfo.FrameIndex = 0;
    PlayerWeapon->AnimationInfo.CurrentTime = 0;
    
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0.5, 0.5);
    CollisionAABB3.Offset = glm::vec2(0.7, 0);
    CollisionAABB3.Extents = glm::vec2(0.5f,1.0f);
    CollisionAABB3.IsTrigger = true;
    PlayerWeapon->CollisionAABB = CollisionAABB3;
    PlayerWeapon->Rotation = glm::vec3(0, 0, 0);
    PlayerWeapon->Scale = glm::vec3(2, 2, 0); 
    PlayerWeapon->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
}

static void SpawnEnemy(game_state* GameState, glm::vec2 Position)
{
    // Enemy
    entity* Enemy = &GameState->Entities[GameState->EntityCount];
    Enemy->Name = "enemy";
    Enemy->Type = Entity_Enemy;
    
    render_entity* EnemyRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    
    EnemyRenderEntity->ShaderIndex = Shader_SpriteSheetShader;
    EnemyRenderEntity->TextureHandle = GameState->RenderState.PlayerTexture;
    
    EnemyRenderEntity->Entity = &*Enemy;
    Enemy->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    Enemy->CurrentAnimation = 0;
    Enemy->AnimationInfo.Playing = false;
    Enemy->AnimationInfo.FrameIndex = 0;
    Enemy->AnimationInfo.CurrentTime = 0;
    PlayAnimation(Enemy, &GameState->EnemyIdleAnimation);
    Enemy->Rotation = glm::vec3(0, 0, 0);
    Enemy->Position = Position;
    Enemy->Scale = glm::vec3(2, 2, 0);
    Enemy->Velocity = glm::vec2(-2,0);
    Enemy->Active = true;
    Enemy->IsKinematic = false;
    Enemy->Layer = Layer_Enemy;
    //Enemy->IgnoreLayers = Layer_Enemy;
    
    collision_AABB CollisionAABB;
    Enemy->Center = glm::vec2(0.5f, 0.5f);
    CollisionAABB.Center = glm::vec2(Enemy->Position.x + Enemy->Center.x * Enemy->Scale.x,
                                     Enemy->Position.y + Enemy->Center.y * Enemy->Scale.y);
    CollisionAABB.Offset = glm::vec2(0, 0.9);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    Enemy->CollisionAABB = CollisionAABB;
    
    collision_AABB* HitTrigger = (collision_AABB*)malloc(sizeof(collision_AABB));
    HitTrigger->Center = glm::vec2(Enemy->Position.x + Enemy->Center.x * Enemy->Scale.x,
                                   Enemy->Position.y + Enemy->Center.y * Enemy->Scale.y);
    HitTrigger->Extents = glm::vec2(0.5f, 0.7f);
    HitTrigger->IsTrigger;
    Enemy->HitTrigger = HitTrigger;
    
    Enemy->Enemy.WalkingSpeed = 5;
    Enemy->Enemy.MaxAlertDistance = 12;
    Enemy->Enemy.MinDistance = 1;
    Enemy->Enemy.AttackCooldown = 1.0f;
    Enemy->Enemy.AIState = AI_Idle;
    Enemy->Enemy.AStarCooldown = 0.0f;
    Enemy->Enemy.AStarInterval = 0.5f;
    
    Enemy->Health = 2;
    Enemy->HitCooldownTime = 0.4;
    Enemy->EntityIndex = GameState->EntityCount++;
    
    // Weapon
    
    entity* EnemyWeapon = &GameState->Entities[GameState->EntityCount];
    EnemyWeapon->Name = "Enemy weapon";
    EnemyWeapon->Type = Entity_EnemyWeapon;
    
    render_entity* EnemyWeaponRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    EnemyWeaponRenderEntity->Rendered = true;
    EnemyWeaponRenderEntity->ShaderIndex = Shader_SpriteSheetShader;
    EnemyWeaponRenderEntity->TextureHandle = GameState->RenderState.SwordTopRightTexture;
    EnemyWeaponRenderEntity->Entity = &*EnemyWeapon;
    EnemyWeapon->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    EnemyWeapon->CurrentAnimation = 0;
    EnemyWeapon->AnimationInfo.Playing = false;
    EnemyWeapon->AnimationInfo.FrameIndex = 0;
    EnemyWeapon->AnimationInfo.CurrentTime = 0;
    EnemyWeapon->Weapon.EntityHandle = Enemy->EntityIndex;
    
    collision_AABB CollisionAABB3;
    CollisionAABB.Center = glm::vec2(0.5, 0.5);
    CollisionAABB.Offset = glm::vec2(0.7, 0);
    CollisionAABB.Extents = glm::vec2(0.5f,1.0f);
    CollisionAABB.IsTrigger = true;
    EnemyWeapon->CollisionAABB = CollisionAABB;
    EnemyWeapon->Rotation = glm::vec3(0, 0, 0);
    EnemyWeapon->Scale = glm::vec3(4, 4, 0);
    EnemyWeapon->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
}

static void SpawnMillionBarrels(game_state* GameState)
{
    uint32 OneMillion = 1;
    for(uint32 i = 0; i < OneMillion; i++)
    {
        for(uint32 j = 0; j < OneMillion; j++)
        {
            entity* Barrel = &GameState->Entities[GameState->EntityCount];
            Barrel->Name = "barrel";
            Barrel->Active = true;
            Barrel->Type = Entity_Barrel;
            Barrel->Layer = Layer_Environment;
            Barrel->IgnoreLayers = Layer_Environment;
            
            render_entity* BarrelRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
            
            BarrelRenderEntity->ShaderIndex = Shader_Texture;
            BarrelRenderEntity->TextureHandle = GameState->RenderState.BarrelTexture;
            
            BarrelRenderEntity->Entity = &*Barrel;
            Barrel->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
            
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

//@Incomplete: Maybe we will add a weapon type or damage amount
void Hit(game_state* GameState, entity* Entity)

{
    Entity->Health -= 1;
    Entity->HitCooldownLeft = Entity->HitCooldownTime;
    
    printf("Health %d Time left %f\n", Entity->Health, Entity->HitCooldownLeft); 
    
    if(Entity->Type == Entity_Enemy)
    {
        Entity->Enemy.AIState = AI_Hit;
        PlayAnimation(Entity, &GameState->EnemyIdleAnimation);
    }
    
    if(Entity->Health <= 0)
    {
        GameState->RenderState.RenderEntities[Entity->RenderEntityHandle].Rendered = false;
        Entity->Active = false;
    }
}