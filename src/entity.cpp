void PrintEntityInfo(const entity& Entity)
{
    printf("Entity: Name %s, position x %f y %f, rotation x %f y %f z %f\n", Entity.Name, Entity.Position.x, Entity.Position.y, Entity.Rotation.x, Entity.Rotation.y, Entity.Rotation.z);
}

static void DeleteEntity(game_state* GameState, uint32 EntityIndex)
{
    if(GameState->EntityCount == 0 || GameState->RenderState.RenderEntityCount == 0)
        return;
    
    entity* Entity = &GameState->Entities[EntityIndex];
    
    uint32 RenderEntityHandle = Entity->RenderEntityHandle;
    
    for(int32 RenderIndex = RenderEntityHandle; RenderIndex < GameState->RenderState.RenderEntityCount; RenderIndex++)
    {
        GameState->RenderState.RenderEntities[RenderIndex] = GameState->RenderState.RenderEntities[RenderIndex + 1];
        
        if(GameState->RenderState.RenderEntities[RenderIndex].Entity)
        {
            GameState->RenderState.RenderEntities[RenderIndex].Entity->RenderEntityHandle = RenderIndex;
        }
    }
    
    GameState->RenderState.RenderEntityCount = Max(GameState->RenderState.RenderEntityCount - 1, 0);
    
    if(GameState->EditorState.SelectedEntity)
        GameState->EditorState.SelectedEntity = 0;
    
    for(uint32 Index = EntityIndex; Index < (uint32)GameState->EntityCount - 1; Index++)
    {
        GameState->Entities[Index] = GameState->Entities[Index + 1];
        GameState->Entities[Index].EntityIndex = Index;
        GameState->RenderState.RenderEntities[GameState->Entities[Index].RenderEntityHandle].Entity = &GameState->Entities[Index];
    }
    
    GameState->EntityCount--;
}

static void LoadEntityFiles(game_state* GameState)
{
}

static void LoadEntityData(FILE* File, entity* Entity, game_state* GameState, bool32 IsReload = false)
{
    if(!IsReload)
    {
        Entity->EntityIndex = GameState->EntityCount++;
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
        RenderEntity->ShaderIndex = Shader_Spritesheet;
        RenderEntity->Rendered = true;
        RenderEntity->Entity = &*Entity;
        Entity->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
        RenderEntity->Color = glm::vec4(1, 1, 1, 1);
        
        Entity->HitFlickerTimer = (timer*)malloc(sizeof(timer));
        Entity->HitFlickerTimer->TimerHandle = -1;
        Entity->HitFlickerTimer->TimerMax = 0.05f;
    }
    
    char LineBuffer[255];
    
    while(fgets(LineBuffer, 255, File))
    {
        if(StartsWith(&LineBuffer[0], "#"))
        {
            break;
        }
        else if(StartsWith(&LineBuffer[0], "name"))
        {
            Entity->Name = (char*)malloc(30 * sizeof(char));
            sscanf(LineBuffer, "name %s", Entity->Name);
        }
        else if(StartsWith(&LineBuffer[0], "active"))
        {
            bool32 Active;
            sscanf(LineBuffer, "active %d", &Active);
            Entity->Active = Active;
        }
        else if(StartsWith(&LineBuffer[0], "layer"))
        {
            char* LayerName = (char*)malloc(30 * sizeof(char));
            sscanf(LineBuffer, "layer %s", LayerName);
            
            if(strcmp(LayerName, "Layer_Player"))
            {
                Entity->Layer = Layer_Player;
            }
            else if(strcmp(LayerName, "Layer_Enemy"))
            {
                Entity->Layer = Layer_Enemy;
            }
            
            free(LayerName);
        }
        else if(StartsWith(&LineBuffer[0], "iskinematic"))
        {
            bool32 IsKinematic;
            sscanf(LineBuffer, "iskinematic %d", &IsKinematic);
            Entity->IsKinematic = IsKinematic;
        }
        else if(StartsWith(&LineBuffer[0], "scale"))
        {
            glm::vec3 Scale;
            sscanf(LineBuffer, "scale %f %f", &Scale.x, &Scale.y);
            Entity->Scale = Scale;
        }
        else if(StartsWith(&LineBuffer[0], "center"))
        {
            sscanf(LineBuffer, "center %f %f", &Entity->Center.x, &Entity->Center.y);
        }
        else if(StartsWith(&LineBuffer[0], "health"))
        {
            sscanf(LineBuffer, "health %d", &Entity->Health);
        }
        else if(StartsWith(&LineBuffer[0], "animation"))
        {
            char* AnimationName = (char*)malloc(30 * sizeof(char)); 
            sscanf(LineBuffer, "animation %s", AnimationName);
            PlayAnimation(Entity, AnimationName, GameState);
            free(AnimationName);
        }
        else if(StartsWith(&LineBuffer[0], "hitcooldowntimer"))
        {
            if(!Entity->HitCooldownTimer)
                Entity->HitCooldownTimer = (timer*)malloc(sizeof(timer));
            sscanf(LineBuffer, "hitcooldowntimer %lf", &Entity->HitCooldownTimer->TimerMax);
            Entity->HitCooldownTimer->TimerHandle = -1;
        }
        else if(StartsWith(&LineBuffer[0], "collider"))
        {
            sscanf(LineBuffer, "collider %f %f %f %f %d", &Entity->CollisionAABB.Offset.x, &Entity->CollisionAABB.Offset.y, &Entity->CollisionAABB.Extents.x, &Entity->CollisionAABB.Extents.y, &Entity->CollisionAABB.IsTrigger);
        }
        else if(StartsWith(&LineBuffer[0], "hashittrigger"))
        {
            sscanf(LineBuffer, "hashittrigger %d", &Entity->HasHitTrigger);
        }
        else if(StartsWith(&LineBuffer[0], "hittrigger"))
        {
            sscanf(LineBuffer, "hittrigger %f %f %f %f %d", &Entity->HitTrigger.Offset.x, &Entity->HitTrigger.Offset.y, &Entity->HitTrigger.Extents.x, &Entity->HitTrigger.Extents.y, &Entity->HitTrigger.IsTrigger);
        }
    }
}

static void LoadPlayerData(game_state* GameState, int32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/player.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] :  &GameState->Entities[GameState->EntityCount];
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File, Entity, GameState, Handle != -1);
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "walkingspeed"))
            {
                sscanf(LineBuffer, "walkingspeed %f", &Entity->Player.WalkingSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "walkingspeed"))
            {
                sscanf(LineBuffer, "throwingspeed %f", &Entity->Player.ThrowingSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "crosshairradius"))
            {
                sscanf(LineBuffer, "crosshairradius %f", &Entity->Player.CrosshairRadius);
            }
            else if(StartsWith(&LineBuffer[0], "targetingsdistance"))
            {
                sscanf(LineBuffer, "targetingsdistance %f", &Entity->Player.TargetingDistance);
            }
            else if(StartsWith(&LineBuffer[0], "attackcooldowntimer"))
            {
                if(!Entity->Player.AttackCooldownTimer)
                    Entity->Player.AttackCooldownTimer = (timer*)malloc(sizeof(timer));
                sscanf(LineBuffer, "attackcooldowntimer %f", &Entity->Player.AttackCooldownTimer->TimerMax);
                Entity->Player.AttackCooldownTimer->TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "lastattacktimer"))
            {
                if(!Entity->Player.LastAttackTimer)
                    Entity->Player.LastAttackTimer = (timer*)malloc(sizeof(timer));
                sscanf(LineBuffer, "lastattacktimer %f", &Entity->Player.LastAttackTimer->TimerMax);
                Entity->Player.LastAttackTimer->TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "dashtimer"))
            {
                if(!Entity->Player.DashTimer)
                    Entity->Player.DashTimer = (timer*)malloc(sizeof(timer));
                sscanf(LineBuffer, "dashtimer %lf", &Entity->Player.DashTimer->TimerMax);
                Entity->Player.DashTimer->TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "dashcooldowntimer"))
            {
                if(!Entity->Player.DashCooldownTimer)
                    Entity->Player.DashCooldownTimer = (timer*)malloc(sizeof(timer));
                sscanf(LineBuffer, "dashcooldowntimer %lf", &Entity->Player.DashCooldownTimer->TimerMax);
                Entity->Player.DashCooldownTimer->TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "attackmovetimer"))
            {
                if(!Entity->Player.AttackMoveTimer)
                    Entity->Player.AttackMoveTimer = (timer*)malloc(sizeof(timer));
                sscanf(LineBuffer, "attackmovetimer %f", &Entity->Player.AttackMoveTimer->TimerMax);
                Entity->Player.AttackMoveTimer->TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "attackmovespeed"))
            {
                sscanf(LineBuffer, "attackmovespeed %f", &Entity->Player.AttackMoveSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "dashspeed"))
            {
                sscanf(LineBuffer, "dashspeed %f", &Entity->Player.DashSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "weaponscale"))
            {
                sscanf(LineBuffer, "weaponscale %f %f", &Entity->Weapon.Scale.x, &Entity->Weapon.Scale.y);
            }
            else if(StartsWith(&LineBuffer[0], "weaponcollider"))
            {
                sscanf(LineBuffer, "weaponcollider %f %f %f %f %d", &Entity->Weapon.CollisionAABB.Offset.x, &Entity->Weapon.CollisionAABB.Offset.y, &Entity->Weapon.CollisionAABB.Extents.x, &Entity->Weapon.CollisionAABB.Extents.y, &Entity->Weapon.CollisionAABB.IsTrigger);
            }
            else if(StartsWith(&LineBuffer[0], "dustcloud"))
            {
                entity* PlayerDustCloud = Handle == -1 ? &GameState->Entities[GameState->EntityCount] : &GameState->Entities[Entity->Player.DustCloudHandle];
                PlayerDustCloud->Name = "Dust cloud";
                PlayerDustCloud->Type = Entity_RenderItem;
                PlayerDustCloud->Active = false;
                
                if(Handle == -1)
                    Entity->Player.DustCloudHandle = GameState->EntityCount;
                
                char* AnimationName = (char*)malloc(30 * sizeof(char));
                
                sscanf(LineBuffer, "dustcloud scale %f %f animation %s", &PlayerDustCloud->Scale.x, &PlayerDustCloud->Scale.y, AnimationName);
                
                PlayAnimation(PlayerDustCloud, AnimationName, GameState);
                free(AnimationName);
                
                if(Handle == -1)
                {
                    render_entity* PlayerDustCloudRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
                    PlayerDustCloudRenderEntity->ShaderIndex = Shader_Spritesheet;
                    PlayerDustCloudRenderEntity->Rendered = true;
                    PlayerDustCloudRenderEntity->Entity = &*PlayerDustCloud;
                    PlayerDustCloud->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
                    PlayerDustCloudRenderEntity->Color = glm::vec4(1, 1, 1, 1);
                    PlayerDustCloud->EntityIndex = GameState->EntityCount++;
                    PlayerDustCloud->Position = glm::vec2(1, 1);
                }
            }
        }
        fclose(File);
    }
}
/*
static void InitPlayer(game_state* GameState, glm::vec2 Position)
{
    entity* Player = &GameState->Entities[GameState->EntityCount];
    
    Player->Name = "Player";
    Player->Type = Entity_Player;
    Player->Health = 3;
    Player->Player.WalkingSpeed = 7.0f;
    Player->Player.ThrowingSpeed = 18.0f;
    Player->Player.CrosshairRadius = 4;
    Player->Player.CrosshairPositionX = Player->Player.CrosshairRadius * 1.0f;
    Player->Player.CrosshairPositionY = 0;
    Player->Player.TargetingDistance = 10;
    Player->Player.TargetedEnemyHandle = -1;
    
    Player->Player.AttackCooldownTimer = (timer*)malloc(sizeof(timer));
    Player->Player.AttackCooldownTimer->TimerHandle = -1;
    Player->Player.AttackCooldownTimer->TimerMax = 0.4;
    
    Player->Player.LastAttackTimer = (timer*)malloc(sizeof(timer));
    Player->Player.LastAttackTimer->TimerHandle = -1;
    Player->Player.LastAttackTimer->TimerMax = 1.0;
    
    Player->Player.PickupCooldownTimer = (timer*)malloc(sizeof(timer));
    Player->Player.PickupCooldownTimer->TimerHandle = -1;
    Player->Player.PickupCooldownTimer->TimerMax = 0.36;
    
    Player->Player.DashTimer = (timer*)malloc(sizeof(timer));
    Player->Player.DashTimer->TimerHandle = -1;
    Player->Player.DashTimer->TimerMax = 0.8f;
    
    Player->Player.DashCooldownTimer = (timer*)malloc(sizeof(timer));
    Player->Player.DashCooldownTimer->TimerHandle = -1;
    Player->Player.DashCooldownTimer->TimerMax = 0.8f;
    
    Player->Player.AttackMoveTimer = (timer*)malloc(sizeof(timer));
    Player->Player.AttackMoveTimer->TimerHandle = -1;
    Player->Player.AttackMoveTimer->TimerMax = 0.07;
    
    // same for all entities
    Player->HitFlickerTimer = (timer*)malloc(sizeof(timer));
    Player->HitFlickerTimer->TimerHandle = -1;
    Player->HitFlickerTimer->TimerMax = 0.05f;
    
    Player->HitCooldownTimer = (timer*)malloc(sizeof(timer));
    Player->HitCooldownTimer->TimerHandle = -1;
    Player->HitCooldownTimer->TimerMax = 0.2;
    
    Player->HitRecoilSpeed = 20;
    
    Player->Player.AttackMoveSpeed = 15;
    Player->Player.DashSpeed = 17;
    Player->Active = true;
    Player->IsKinematic = false;
    Player->CurrentAnimation = 0;
    Player->AnimationInfo.Playing = false;
    Player->AnimationInfo.FrameIndex = 0;
    Player->AnimationInfo.CurrentTime = 0;
    
    collision_AABB* HitTrigger = (collision_AABB*)malloc(sizeof(collision_AABB));
    HitTrigger->Offset = glm::vec2(0, 0);
    HitTrigger->Center = glm::vec2(Player->Position.x + Player->Center.x * Player->Scale.x + HitTrigger->Offset.x,
                                   Player->Position.y + Player->Center.y * Player->Scale.y + HitTrigger->Offset.y);
    HitTrigger->Extents = glm::vec2(0.65f, 1.0f);
    
    
    render_entity* PlayerRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    PlayerRenderEntity->ShaderIndex = Shader_Spritesheet;
    PlayerRenderEntity->Rendered = true;
    PlayerRenderEntity->Entity = &*Player;
    Player->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    PlayerRenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    Player->Position = Position;
    Player->Rotation = glm::vec3(0, 0, 0);
    Player->Scale = glm::vec3(3, 3, 0);
    Player->Velocity = glm::vec2(0,0);
    PlayAnimation(Player, "player_idle_down", GameState);
    collision_AABB CollisionAABB;
    Player->Center = glm::vec2(0, 0.5f);
    
    Player->Layer = Layer_Player;
    
    CollisionAABB.Center = glm::vec2(Player->Position.x + Player->Center.x * Player->Scale.x,
                                     Player->Position.y + Player->Center.y * Player->Scale.y);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    CollisionAABB.Offset = glm::vec2(0, -1.5f);
    CollisionAABB.IsTrigger = false;
    Player->CollisionAABB = CollisionAABB;
    
    // Weapon
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0, 0);
    CollisionAABB3.Offset = glm::vec2(0.2, 0);
    CollisionAABB3.Extents = glm::vec2(1.1f,1.2f);
    CollisionAABB3.IsTrigger = true;
    Player->Weapon.Rotation = glm::vec3(0, 0, 0);
    Player->Weapon.Scale = glm::vec3(2, 2, 0); 
    
    Player->Weapon.CollisionAABB = CollisionAABB3;
    
    Player->EntityIndex = GameState->EntityCount++;
    
    entity* PlayerDustCloud = &GameState->Entities[GameState->EntityCount];
    PlayerDustCloud->Name = "Dust cloud";
    PlayerDustCloud->Type = Entity_RenderItem;
    PlayerDustCloud->Active = false;
    PlayerDustCloud->Rotation = glm::vec3(0, 0, 0);
    PlayerDustCloud->Scale = glm::vec3(1, 1, 0);
    PlayerDustCloud->Velocity = glm::vec2(0,0);
    
    Player->Player.DustCloudHandle = GameState->EntityCount;
    PlayAnimation(PlayerDustCloud, "dust_brown", GameState);
    render_entity* PlayerDustCloudRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    PlayerDustCloudRenderEntity->ShaderIndex = Shader_Spritesheet;
    PlayerDustCloudRenderEntity->Texture = GameState->RenderState.Textures["dust_cloud_brown"];
    PlayerDustCloudRenderEntity->Rendered = true;
    PlayerDustCloudRenderEntity->Entity = &*PlayerDustCloud;
    PlayerDustCloud->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    PlayerDustCloudRenderEntity->Color = glm::vec4(1, 1, 1, 1);
    PlayerDustCloud->EntityIndex = GameState->EntityCount++;
    PlayerDustCloud->Position = glm::vec2(1, 1);
}
*/

static void SpawnSkeleton(game_state* GameState, glm::vec2 Position)
{
    entity* Skeleton = &GameState->Entities[GameState->EntityCount];
    Skeleton->Name = "skeleton";
    Skeleton->Type = Entity_Enemy;
    Skeleton->Enemy.EnemyType = Enemy_Skeleton;
    
    Skeleton->HitRecoilSpeed = 10;
    Skeleton->Enemy.IsTargeted = false;
    Skeleton->Enemy.TargetingPositionX = -0.5;
    Skeleton->Enemy.TargetingPositionY = 2;
    
    render_entity* SkeletonRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    
    SkeletonRenderEntity->ShaderIndex = Shader_Spritesheet;
    SkeletonRenderEntity->Texture = GameState->RenderState.Textures["skeleton_idle"];
    
    SkeletonRenderEntity->Entity = &*Skeleton;
    Skeleton->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    Skeleton->CurrentAnimation = 0;
    Skeleton->AnimationInfo.Playing = false;
    Skeleton->AnimationInfo.FrameIndex = 0;
    Skeleton->AnimationInfo.CurrentTime = 0;
    PlayAnimation(Skeleton, "skeleton_idle", GameState);
    Skeleton->Rotation = glm::vec3(0, 0, 0);
    Skeleton->Position = Position;
    Skeleton->Scale = glm::vec3(3, 2.58, 1);
    Skeleton->Velocity = glm::vec2(-2,0);
    Skeleton->Active = true;
    Skeleton->IsKinematic = false;
    Skeleton->Layer = Layer_Enemy;
    //Enemy->IgnoreLayers = Layer_Enemy;
    
    collision_AABB CollisionAABB;
    Skeleton->Center = glm::vec2(0, 0.5f);
    CollisionAABB.Center = glm::vec2(Skeleton->Position.x + Skeleton->Center.x * Skeleton->Scale.x,
                                     Skeleton->Position.y + Skeleton->Center.y * Skeleton->Scale.y);
    CollisionAABB.Offset = glm::vec2(0, -1.2f);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    CollisionAABB.IsTrigger = false;
    Skeleton->CollisionAABB = CollisionAABB;
    
    Skeleton->HitTrigger.Center = glm::vec2(Skeleton->Position.x + Skeleton->Center.x * Skeleton->Scale.x,
                                            Skeleton->Position.y + Skeleton->Center.y * Skeleton->Scale.y);
    Skeleton->HitTrigger.Extents = glm::vec2(0.8f, 0.9f);
    Skeleton->HitTrigger.IsTrigger = true;
    Skeleton->HitTrigger.Offset = glm::vec2(0, -0.4f);
    Skeleton->HasHitTrigger = true;
    
    Skeleton->Enemy.WalkingSpeed = 5;
    Skeleton->Enemy.MaxAlertDistance = 5;
    Skeleton->Enemy.MaxFollowDistance = 10;
    Skeleton->Enemy.MinDistanceToPlayer = 2;
    Skeleton->Enemy.AIState = AI_Idle;
    
    Skeleton->Enemy.Skeleton.AttackCooldownTimer = (timer*)malloc(sizeof(timer));
    Skeleton->Enemy.Skeleton.AttackCooldownTimer->TimerHandle = -1;
    Skeleton->Enemy.Skeleton.AttackCooldownTimer->TimerMax = 0.5;
    
    Skeleton->Enemy.Skeleton.ChargingTimer = (timer*)malloc(sizeof(timer));
    Skeleton->Enemy.Skeleton.ChargingTimer->TimerHandle = -1;
    Skeleton->Enemy.Skeleton.ChargingTimer->TimerMax = 0.2f;
    
    Skeleton->HitFlickerTimer = (timer*)malloc(sizeof(timer));
    Skeleton->HitFlickerTimer->TimerHandle = -1;
    Skeleton->HitFlickerTimer->TimerMax = 0.05f;
    
    Skeleton->Enemy.AStarPath.AStarCooldownTimer = (timer*)malloc(sizeof(timer));
    Skeleton->Enemy.AStarPath.AStarCooldownTimer->TimerHandle = -1;
    Skeleton->Enemy.AStarPath.AStarCooldownTimer->TimerMax = 0.6;
    
    Skeleton->RecoilTimer = (timer*)malloc(sizeof(timer));
    Skeleton->RecoilTimer->TimerHandle = -1;
    Skeleton->RecoilTimer->TimerMax = 0.2;
    
    Skeleton->HitCooldownTimer = (timer*)malloc(sizeof(timer));
    Skeleton->HitCooldownTimer->TimerHandle = -1;
    Skeleton->HitCooldownTimer->TimerMax = 0.4;
    
    Skeleton->Health = 4;
    
    // Weapon
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0, 0.5);
    CollisionAABB3.Offset = glm::vec2(0.7, 0);
    CollisionAABB3.Extents = glm::vec2(0.5f,1.0f);
    CollisionAABB3.IsTrigger = true;
    Skeleton->Weapon.CollisionAABB = CollisionAABB3;
    Skeleton->Weapon.Rotation = glm::vec3(0, 0, 0);
    Skeleton->Weapon.Scale = glm::vec3(2, 2, 0); 
    
    Skeleton->Enemy.Healthbar = (entity_healthbar*)malloc(sizeof(entity_healthbar));
    Skeleton->Enemy.Healthbar->Offset = glm::vec2(-0.5f, 2.2f);
    Skeleton->Enemy.Healthbar->Scale = glm::vec3(1.0, 0.25,0 );
    ui_render_info RenderInfo = {};
    RenderInfo.Texture = GameState->RenderState.Textures["4_health"];
    RenderInfo.TextureOffset = glm::vec2(256, 0);
    RenderInfo.FrameSize = glm::vec2(64, 16);
    RenderInfo.ShaderIndex = Shader_Spritesheet;
    Skeleton->Enemy.Healthbar->RenderInfo = RenderInfo;
    
    Skeleton->EntityIndex = GameState->EntityCount++;
}


static void SpawnWraith(game_state* GameState, glm::vec2 Position)
{
    entity* Wraith = &GameState->Entities[GameState->EntityCount];
    Wraith->Name = "skeleton";
    Wraith->Type = Entity_Enemy;
    Wraith->Enemy.EnemyType = Enemy_Wraith;
    
    Wraith->HitRecoilSpeed = 10;
    Wraith->Enemy.IsTargeted = false;
    Wraith->Enemy.TargetingPositionX = -0.5;
    Wraith->Enemy.TargetingPositionY = 2;
    
    render_entity* WraithRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    
    WraithRenderEntity->ShaderIndex = Shader_Spritesheet;
    WraithRenderEntity->Texture = GameState->RenderState.Textures["wraith_idle"];
    
    WraithRenderEntity->Entity = &*Wraith;
    Wraith->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    Wraith->CurrentAnimation = 0;
    Wraith->AnimationInfo.Playing = false;
    Wraith->AnimationInfo.FrameIndex = 0;
    Wraith->AnimationInfo.CurrentTime = 0;
    PlayAnimation(Wraith, "wraith_idle", GameState);
    Wraith->Rotation = glm::vec3(0, 0, 0);
    Wraith->Position = Position;
    Wraith->Scale = glm::vec3(4, 4, 1);
    Wraith->Velocity = glm::vec2(-2,0);
    Wraith->Active = true;
    Wraith->IsKinematic = false;
    Wraith->Layer = Layer_Enemy;
    //Enemy->IgnoreLayers = Layer_Enemy;
    
    collision_AABB CollisionAABB;
    Wraith->Center = glm::vec2(0.5, 0.5f);
    CollisionAABB.Center = glm::vec2(Wraith->Position.x + Wraith->Center.x * Wraith->Scale.x,
                                     Wraith->Position.y + Wraith->Center.y * Wraith->Scale.y);
    CollisionAABB.Offset = glm::vec2(0, -1.2f);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    CollisionAABB.IsTrigger = false;
    Wraith->CollisionAABB = CollisionAABB;
    
    Wraith->HitTrigger.Center = glm::vec2(Wraith->Position.x + Wraith->Center.x * Wraith->Scale.x,
                                          Wraith->Position.y + Wraith->Center.y * Wraith->Scale.y);
    Wraith->HitTrigger.Extents = glm::vec2(0.8f, 0.9f);
    Wraith->HitTrigger.IsTrigger;
    Wraith->HitTrigger.Offset = glm::vec2(0, -0.4f);
    Wraith->HasHitTrigger = true;
    
    Wraith->Enemy.WalkingSpeed = 5;
    Wraith->Enemy.MaxAlertDistance = 5;
    Wraith->Enemy.MaxFollowDistance = 10;
    Wraith->Enemy.MinDistanceToPlayer = 2;
    Wraith->Enemy.AIState = AI_Idle;
    
    Wraith->Enemy.Wraith.AttackCooldownTimer = (timer*)malloc(sizeof(timer));
    Wraith->Enemy.Wraith.AttackCooldownTimer->TimerHandle = -1;
    Wraith->Enemy.Wraith.AttackCooldownTimer->TimerMax = 0.5;
    
    Wraith->Enemy.Wraith.ChargingTimer = (timer*)malloc(sizeof(timer));
    Wraith->Enemy.Wraith.ChargingTimer->TimerHandle = -1;
    Wraith->Enemy.Wraith.ChargingTimer->TimerMax = 0.2f;
    
    Wraith->HitFlickerTimer = (timer*)malloc(sizeof(timer));
    Wraith->HitFlickerTimer->TimerHandle = -1;
    Wraith->HitFlickerTimer->TimerMax = 0.05f;
    
    Wraith->Enemy.AStarPath.AStarCooldownTimer = (timer*)malloc(sizeof(timer));
    Wraith->Enemy.AStarPath.AStarCooldownTimer->TimerHandle = -1;
    Wraith->Enemy.AStarPath.AStarCooldownTimer->TimerMax = 0.6;
    
    Wraith->RecoilTimer = (timer*)malloc(sizeof(timer));
    Wraith->RecoilTimer->TimerHandle = -1;
    Wraith->RecoilTimer->TimerMax = 0.2;
    
    Wraith->HitCooldownTimer = (timer*)malloc(sizeof(timer));
    Wraith->HitCooldownTimer->TimerHandle = -1;
    Wraith->HitCooldownTimer->TimerMax = 0.4;
    
    Wraith->Health = 4;
    
    // Weapon
    collision_AABB CollisionAABB3;
    CollisionAABB3.Center = glm::vec2(0, 0.5);
    CollisionAABB3.Offset = glm::vec2(0.7, 0);
    CollisionAABB3.Extents = glm::vec2(0.5f, 1.0f);
    CollisionAABB3.IsTrigger = true;
    Wraith->Weapon.CollisionAABB = CollisionAABB3;
    Wraith->Weapon.Rotation = glm::vec3(0, 0, 0);
    Wraith->Weapon.Scale = glm::vec3(2, 2, 0); 
    
    Wraith->Enemy.Healthbar = (entity_healthbar*)malloc(sizeof(entity_healthbar));
    Wraith->Enemy.Healthbar->Offset = glm::vec2(-0.5f, 2.2f);
    Wraith->Enemy.Healthbar->Scale = glm::vec3(1.0, 0.25,0 );
    ui_render_info RenderInfo = {};
    RenderInfo.Texture = GameState->RenderState.Textures["4_health"];
    RenderInfo.TextureOffset = glm::vec2(256, 0);
    RenderInfo.FrameSize = glm::vec2(64, 16);
    RenderInfo.ShaderIndex = Shader_Spritesheet;
    Wraith->Enemy.Healthbar->RenderInfo = RenderInfo;
    
    Wraith->EntityIndex = GameState->EntityCount++;
}

static void SpawnBlob(game_state* GameState, glm::vec2 Position)
{
    // Enemy
    entity* Blob = &GameState->Entities[GameState->EntityCount];
    Blob->Name = "blob";
    Blob->Type = Entity_Enemy;
    Blob->Enemy.EnemyType = Enemy_Blob;
    Blob->Enemy.MinDistanceToPlayer = 2;
    
    render_entity* BlobRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    
    BlobRenderEntity->ShaderIndex = Shader_Spritesheet;
    BlobRenderEntity->Texture = GameState->RenderState.Textures["blob"];
    
    BlobRenderEntity->Entity = &*Blob;
    Blob->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    Blob->CurrentAnimation = 0;
    Blob->AnimationInfo.Playing = false;
    Blob->AnimationInfo.FrameIndex = 0;
    Blob->AnimationInfo.CurrentTime = 0;
    PlayAnimation(Blob, "blob", GameState);
    
    Blob->Active = true;
    Blob->Position = Position;
    Blob->Rotation = glm::vec3(0, 0, 0);
    Blob->Scale = glm::vec3(2, 2, 1);
    Blob->Velocity = glm::vec2(0, 0);
    Blob->IsKinematic = false;
    Blob->Layer = Layer_Enemy;
    
    Blob->HitFlickerTimer = (timer*)malloc(sizeof(timer));
    Blob->HitFlickerTimer->TimerHandle = -1;
    Blob->HitFlickerTimer->TimerMax = 0.05f;
    
    Blob->Enemy.MaxAlertDistance = 10;
    Blob->Enemy.MaxFollowDistance = 20;
    Blob->Enemy.AIState = AI_Following;
    Blob->Enemy.AStarPath.AStarCooldownTimer = (timer*)malloc(sizeof(timer));
    Blob->Enemy.AStarPath.AStarCooldownTimer->TimerHandle = -1;
    Blob->Enemy.AStarPath.AStarCooldownTimer->TimerMax = 0.6;
    Blob->Enemy.Blob.PickupThrowTimer = (timer*)malloc(sizeof(timer));
    Blob->Enemy.Blob.PickupThrowTimer->TimerHandle = -1;
    Blob->Enemy.Blob.PickupThrowTimer->TimerMax = 1.5f;
    
    Blob->Enemy.Blob.ExplodeStartTimer = (timer*)malloc(sizeof(timer));
    Blob->Enemy.Blob.ExplodeStartTimer->TimerHandle = -1;
    Blob->Enemy.Blob.ExplodeStartTimer->TimerMax = 0.5;
    Blob->Enemy.Blob.ExplodeCountdownTimer = (timer*)malloc(sizeof(timer));
    Blob->Enemy.Blob.ExplodeCountdownTimer->TimerHandle = -1;
    Blob->Enemy.Blob.ExplodeCountdownTimer->TimerMax = 1.0;
    
    collision_AABB CollisionAABB;
    Blob->Center = glm::vec2(0, 0.5f);
    CollisionAABB.Center = glm::vec2(Blob->Position.x + Blob->Center.x * Blob->Scale.x,
                                     Blob->Position.y + Blob->Center.y * Blob->Scale.y);
    CollisionAABB.Offset = glm::vec2(-0.5, -0.9);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    CollisionAABB.IsTrigger = false;
    Blob->CollisionAABB = CollisionAABB;
    Blob->Enemy.Blob.ExplosionCollisionExtentsX = 1.0f;
    Blob->Enemy.Blob.ExplosionCollisionExtentsY = 1.0f;
    
    Blob->HitTrigger.Extents = glm::vec2(0.5f, 0.7f);
    Blob->HitTrigger.IsTrigger = true;
    Blob->HasHitTrigger = true;
    
    Blob->Health = 1;
    Blob->EntityIndex = GameState->EntityCount++;
    
    Blob->Enemy.Healthbar = (entity_healthbar*)malloc(sizeof(entity_healthbar));
    Blob->Enemy.Healthbar->Offset = glm::vec2(-0.5f, 2.2f);
    Blob->Enemy.Healthbar->Scale = glm::vec3(1.0, 0.25,0 );
    ui_render_info RenderInfo = {};
    RenderInfo.Texture = GameState->RenderState.Textures["4_health"];
    RenderInfo.TextureOffset = glm::vec2(256, 0);
    RenderInfo.FrameSize = glm::vec2(64, 16);
    RenderInfo.ShaderIndex = Shader_Spritesheet;
    Blob->Enemy.Healthbar->RenderInfo = RenderInfo;
}


static void SpawnBarrel(game_state* GameState, glm::vec2 Position)
{
    entity* Barrel = &GameState->Entities[GameState->EntityCount];
    Barrel->Name = "barrel";
    Barrel->Active = true;
    Barrel->Type = Entity_Barrel;
    Barrel->Layer = Layer_Environment;
    Barrel->IgnoreLayers = Layer_Environment;
    
    Barrel->Pickup.PickupThrowTimer = (timer*)malloc(sizeof(timer));
    Barrel->Pickup.PickupThrowTimer->TimerHandle = -1;
    Barrel->Pickup.PickupThrowTimer->TimerMax = 1.5f;
    
    render_entity* BarrelRenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    
    BarrelRenderEntity->ShaderIndex = Shader_Spritesheet;
    BarrelRenderEntity->Entity = &*Barrel;
    BarrelRenderEntity->Texture = GameState->RenderState.Textures["barrel_sheet"];
    Barrel->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    
    Barrel->CurrentAnimation = 0;
    Barrel->AnimationInfo.Playing = false;
    Barrel->AnimationInfo.FrameIndex = 0;
    Barrel->AnimationInfo.CurrentTime = 0;
    PlayAnimation(Barrel, "barrel_standing", GameState);
    
    Barrel->Rotation = glm::vec3(0, 0, 0);
    Barrel->Position = Position;
    Barrel->Scale = glm::vec3(2, 2, 0);
    Barrel->Velocity = glm::vec2(0,0);
    Barrel->Center = glm::vec2(0.5, 0.5);
    Barrel->IsStatic = true;
    Barrel->IsKinematic = false;
    Barrel->IsPickup = true;
    
    collision_AABB CollisionAABB;
    
    CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
    CollisionAABB.Center = glm::vec2(Barrel->Position.x + Barrel->Center.x * Barrel->Scale.x,
                                     Barrel->Position.y + Barrel->Center.y * Barrel->Scale.y);
    CollisionAABB.IsTrigger = true;
    Barrel->CollisionAABB = CollisionAABB;
    
    Barrel->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
}

//@Incomplete: Maybe we will add a weapon type or damage amount
void Hit(game_state* GameState, entity* ByEntity, entity* HitEntity)
{
    if(HitEntity->HitAttackCountId != ByEntity->AttackCount)
    {
        StartTimer(GameState, GameState->GameCamera.ScreenShakeTimer);
        StartTimer(GameState, HitEntity->HitCooldownTimer);
        PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit02);
        HitEntity->HitRecoilDirection = glm::normalize(HitEntity->Position - ByEntity->Position);
        HitEntity->Health -= 1;
        
        HitEntity->Hit = true;
        HitEntity->HitAttackCountId = ByEntity->AttackCount;
        HitEntity->HitFlickerFramesLeft = HitEntity->HitFlickerFrameMax;
        StartTimer(GameState, HitEntity->HitFlickerTimer);
        
        if(HitEntity->Type == Entity_Player)
        {
            StartFade(GameState->GameCamera, Fading_OutIn, 4.0f, glm::vec3(1, 0, 0), 0.0f, 0.4f);
        }
    }
}


void UpdatePlayer(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    bool32 UsingController = GameState->InputController.ControllerPresent;
    
    auto TempPos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                                  GameState->Camera.ViewMatrix,
                                  GameState->Camera.ProjectionMatrix,
                                  glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    auto Pos = glm::vec2(TempPos.x, TempPos.y);
    
    auto DirectionToMouse = glm::normalize(glm::vec2(Pos.x - Entity->Position.x, Pos.y - Entity->Position.y));
    
    if(TimerDone(GameState, Entity->Player.LastAttackTimer) && TimerDone(GameState, Entity->Player.AttackCooldownTimer))
        Entity->AttackCount = 0;
    
    if (Entity->Active && Entity->Health > 0)
    {
        if(!TimerDone(GameState, Entity->HitCooldownTimer))
        {
            Entity->Velocity = glm::vec2(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed * DeltaTime, Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed * DeltaTime);
        }
        else
        {
            if(!Entity->Player.IsDashing)
            {
                glm::vec2 Direction = UsingController ? glm::normalize(Entity->Velocity) : DirectionToMouse;
                Entity->Player.LastKnownDirectionX = Direction.x;
                Entity->Player.LastKnownDirectionY = Direction.y;
                
                if(Entity->Player.IsAttacking && !Entity->AnimationInfo.Playing)
                {
                    Entity->Player.IsAttacking = false;
                    
                    if(Entity->AttackCount == 3)
                    {
                        Entity->AttackCount = 0;
                        Entity->Velocity = glm::vec2(0, 0);
                        StartTimer(GameState, Entity->Player.AttackCooldownTimer);
                    }
                }
                
                if(!Entity->Player.IsAttacking)
                {
                    real32 InputX = GetInputX(GameState);
                    real32 InputY = GetInputY(GameState);
                    Entity->Velocity.x = InputX * Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    Entity->Velocity.y = InputY * Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    
                    // @Cleanup: This section really needs a cleanup
                    if(Entity->Player.TargetedEnemyHandle != -1)
                    {
                        bool32 Moving = Entity->Velocity.x != 0 || Entity->Velocity.y != 0;
                        
                        auto Direction = glm::normalize(GameState->Entities[Entity->Player.TargetedEnemyHandle].Position - Entity->Position);
                        
                        if(Direction.x < 0.7)
                        {
                            if(Direction.y > 0)
                            {
                                Entity->LookDirection = Up;
                                if(Moving)
                                    PlayAnimation(Entity, "player_run_up", GameState);
                                else
                                    PlayAnimation(Entity, "player_idle_up", GameState);
                            }
                            else
                            {
                                Entity->LookDirection = Down;
                                if(Moving)
                                    PlayAnimation(Entity, "player_run_down", GameState);
                                else
                                    PlayAnimation(Entity, "player_idle_down", GameState);
                            }
                        }
                        else
                        {
                            Entity->LookDirection = Right;
                            if(Moving)
                                PlayAnimation(Entity, "player_run_right", GameState);
                            else
                                PlayAnimation(Entity, "player_idle_right", GameState);
                        }
                        
                        Entity->IsFlipped = Direction.x < 0;
                        
                        if(Entity->LookDirection == Right && Entity->IsFlipped)
                            Entity->LookDirection = Left;
                    }
                    else if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                    {
                        
                        auto XValue = UsingController ? InputX : DirectionToMouse.x;
                        auto YValue = UsingController ? Entity->Velocity.y : DirectionToMouse.y;
                        
                        if(Abs(XValue) < 0.7)
                        {
                            if(YValue > 0)
                            {
                                Entity->LookDirection = Up;
                                PlayAnimation(Entity, "player_run_up", GameState);
                            }
                            else
                            {
                                Entity->LookDirection = Down;
                                PlayAnimation(Entity, "player_run_down", GameState);
                            }
                        }
                        else
                        {
                            Entity->LookDirection = Right;
                            PlayAnimation(Entity, "player_run_right", GameState);
                        }
                        
                        if(XValue != 0)
                        {
                            Entity->IsFlipped = XValue < 0;
                            
                            if(Entity->LookDirection == Right && Entity->IsFlipped)
                                Entity->LookDirection = Left;
                        }
                    }
                    else
                    {
                        if(Abs(Entity->Player.LastKnownDirectionX) < 0.3)
                        {
                            if(Entity->Player.LastKnownDirectionY > 0)
                                PlayAnimation(Entity, "player_idle_up", GameState);
                            else
                                PlayAnimation(Entity, "player_idle_down", GameState);
                        }
                        else
                            PlayAnimation(Entity, "player_idle_right", GameState);
                        
                        Entity->IsFlipped = Entity->Player.LastKnownDirectionX < 0;
                        
                        if(Entity->LookDirection == Right && Entity->IsFlipped)
                            Entity->LookDirection = Left;
                    }
                }
                else if(!TimerDone(GameState, Entity->Player.AttackMoveTimer))
                {
                    glm::vec2 Vel;
                    real32 AttackMoveSpeed = Entity->Player.AttackMoveSpeed;
                    
                    if(Entity->Player.LastKnownDirectionX != 0 || Entity->Player.LastKnownDirectionY != 0)
                    {
                        Vel = glm::vec2(Entity->Player.LastKnownDirectionX * AttackMoveSpeed * DeltaTime, Entity->Player.LastKnownDirectionY * AttackMoveSpeed * DeltaTime);
                    }
                    else
                    {
                        switch(Entity->LookDirection)
                        {
                            case Up:
                            {
                                Vel = glm::vec2(0, AttackMoveSpeed * DeltaTime);
                            }
                            break;
                            case Down:
                            {
                                
                                Vel = glm::vec2(0, -AttackMoveSpeed * DeltaTime);
                            }
                            break;
                            case Left:
                            {
                                
                                Vel = glm::vec2(-AttackMoveSpeed * DeltaTime, 0);
                            }
                            break;
                            case Right:
                            {
                                Vel = glm::vec2(AttackMoveSpeed * DeltaTime, 0);
                            }
                            break;
                        }
                    }
                    
                    Entity->Velocity = Vel;
                }
                else
                {
                    Entity->Velocity = glm::vec2(0, 0);
                }
                
                if(GetActionButtonDown(Action_Interact, GameState) && Entity->Player.Pickup)
                {
                    GameState->Entities[Entity->Player.TargetedEnemyHandle].Enemy.IsTargeted = false;
                    Entity->Player.TargetedEnemyHandle = -1;
                    
                    Entity->Player.Pickup->IsKinematic = false;
                    real32 ThrowingDir = Entity->IsFlipped ? -1.0f : 1.0f;
                    glm::vec2 Throw;
                    
                    glm::vec2 Dir = glm::normalize(glm::vec2(Entity->Player.CrosshairPositionX, Entity->Player.CrosshairPositionY));
                    Throw.x = Dir.x * Entity->Player.ThrowingSpeed;
                    Throw.y = Dir.y * Entity->Player.ThrowingSpeed;
                    
                    Entity->Player.Pickup->Velocity = Throw;
                    
                    if(Entity->Player.Pickup->Type == Entity_Barrel)
                    {
                        StartTimer(GameState, Entity->Player.Pickup->Pickup.PickupThrowTimer);
                        PlayAnimation(Entity->Player.Pickup, "barrel_thrown", GameState);
                        PlaySoundEffect(GameState, &GameState->SoundManager.Throw);
                        Entity->Player.Pickup = NULL;
                        StartTimer(GameState, Entity->Player.PickupCooldownTimer);
                    }
                    else if(Entity->Player.Pickup->Type == Entity_Enemy && Entity->Player.Pickup->Enemy.EnemyType == Enemy_Blob)
                    {
                        StartTimer(GameState, Entity->Player.Pickup->Enemy.Blob.PickupThrowTimer);
                        PlaySoundEffect(GameState, &GameState->SoundManager.Throw);
                        Entity->Player.Pickup = NULL;
                        StartTimer(GameState, Entity->Player.PickupCooldownTimer);
                    }
                }
            }
            
            if(TimerDone(GameState, Entity->Player.DashTimer))
            {
                Entity->Player.IsDashing = false;
            }
            
            if(!Entity->Player.Pickup && !Entity->Player.IsAttacking  && TimerDone(GameState, Entity->Player.DashTimer) && GetActionButtonDown(Action_Dash, GameState))
            {
                PlaySoundEffect(GameState, &GameState->SoundManager.Slide01);
                Entity->Player.DashCount++;
                
                if(Entity->Player.DashCount < 4)
                {
                    Entity->Player.IsDashing = true;
                    Entity->Player.DashDirectionX = Entity->Player.LastKnownDirectionX;
                    Entity->Player.DashDirectionY = Entity->Player.LastKnownDirectionY; 
                    StartTimer(GameState, Entity->Player.DashTimer);
                    StartTimer(GameState, Entity->Player.DashCooldownTimer);
                }
            }
            
            if(Entity->Player.IsDashing)
            {
                auto XInput = GetInputX(GameState);
                auto YInput = GetInputY(GameState);
                
                auto NewDirection = glm::normalize(glm::vec2(Entity->Player.DashDirectionX + XInput / 500.0f, Entity->Player.DashDirectionY + YInput / 500.0f));
                
                Entity->Player.DashDirectionX = NewDirection.x;
                Entity->Player.DashDirectionY = NewDirection.y;
                
                Entity->Velocity = glm::vec2(Entity->Player.DashDirectionX * Entity->Player.DashSpeed * DeltaTime, Entity->Player.DashDirectionY * Entity->Player.DashSpeed * DeltaTime);
                
            }
            
            if(TimerDone(GameState, Entity->Player.DashCooldownTimer) && !Entity->Player.IsDashing)
            {
                Entity->Player.DashCount = 0;
            }
        }
        
        if(Entity->Velocity.x == Entity->Velocity.x)
            Entity->Position += Entity->Velocity;
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        if(Entity->Player.Pickup)
        {
            Entity->Player.Pickup->Position = glm::vec2(Entity->Position.x - 0.5f, Entity->Position.y);
            
            // If the player has targeted an enemy, the crosshair should be targeted at it
            if(Entity->Player.TargetedEnemyHandle != -1)
            {
                auto Direction = GameState->Entities[Entity->Player.TargetedEnemyHandle].Position - Entity->Position;
                
                Entity->Player.CrosshairPositionX = Direction.x * 0.9f; // @Incomplete: Not sure this is the best way, but it works fine
                Entity->Player.CrosshairPositionY = Direction.y * 0.9f;
            }
            else
            {
                auto Input = UsingController ? glm::normalize(glm::vec2(GetInputX(GameState, Stick_Right), GetInputY(GameState, Stick_Right))) : DirectionToMouse;
                
                if(Input.x == Input.x || Input.y == Input.y) // NaN check
                {
                    Entity->Player.CrosshairPositionX = Input.x * Entity->Player.CrosshairRadius;
                    Entity->Player.CrosshairPositionY = Input.y * Entity->Player.CrosshairRadius;
                }
            }
        }
        else
        {
            if(Entity->Player.TargetedEnemyHandle != -1)
            {
                GameState->Entities[Entity->Player.TargetedEnemyHandle].Enemy.IsTargeted = false;
                Entity->Player.TargetedEnemyHandle = -1;
            }
            
            Entity->Player.RenderCrosshair = false;
        }
        
        //attacking
        if(!Entity->Player.Pickup && TimerDone(GameState, Entity->Player.AttackCooldownTimer) && !Entity->Player.IsAttacking && (GetActionButtonDown(Action_Attack, GameState) || GetJoystickKeyDown(Joystick_3, GameState)))
        {
            switch(Entity->LookDirection)
            {
                case Up:
                {
                    PlayAnimation(Entity, "player_attack_up", GameState);
                }
                break;
                case Down:
                {
                    PlayAnimation(Entity, "player_attack_down", GameState);
                }
                break;
                case Left:
                case Right:
                {
                    PlayAnimation(Entity, "player_attack_right", GameState);
                }
                break;
            }
            
            Entity->Player.IsAttacking = true;
            StartTimer(GameState, Entity->Player.LastAttackTimer);
            
            StartTimer(GameState, Entity->Player.AttackMoveTimer);
            Entity->AttackCount++;
            PlaySoundEffect(GameState, &GameState->SoundManager.SwordSlash01);
        }
        
        if(Entity->Player.Pickup)
        {
            if(GetActionButtonDown(Action_Target, GameState))
            {
                if(Entity->Player.TargetedEnemyHandle == -1)
                {
                    int32 Closest = -1;
                    real32 ClosestDistance = 2000.0f;
                    
                    for(uint32 Index = 0; Index < GameState->EntityCount; Index++)
                    {
                        if(!GameState->Entities[Index].Dead &&
                           GameState->Entities[Index].Type == Entity_Enemy && GameState->Entities[Index].Enemy.EnemyType == Enemy_Skeleton) // @Incomplete: We need a way to easily determine whether it's an enemy or not
                        {
                            auto Distance = glm::distance(Entity->Position, GameState->Entities[Index].Position);
                            if(Distance <= Entity->Player.TargetingDistance && Distance < ClosestDistance)
                            {
                                ClosestDistance = Distance;
                                Closest = Index;
                            }
                        }
                    }
                    
                    if(Closest != -1)
                    {
                        Entity->Player.TargetedEnemyHandle = Closest;
                        GameState->Entities[Closest].Enemy.IsTargeted = true;
                    }
                }
                else
                {
                    GameState->Entities[Entity->Player.TargetedEnemyHandle].Enemy.IsTargeted = false;
                    Entity->Player.TargetedEnemyHandle = -1;
                }
            }
            
            if(GetActionButtonDown(Action_SwitchTarget, GameState))
            {
                int32 NextTarget = -1;
                
                if(Entity->Player.TargetedEnemyHandle + 1 < GameState->EntityCount)
                {
                    for(uint32 Index = Entity->Player.TargetedEnemyHandle + 1; Index < GameState->EntityCount; Index++)
                    {
                        if(GameState->Entities[Index].Health > 0 && GameState->Entities[Index].Type == Entity_Enemy)
                        {
                            auto Distance = glm::distance(Entity->Position, GameState->Entities[Index].Position);
                            if(Distance <= Entity->Player.TargetingDistance)
                            {
                                NextTarget = Index;
                                break;
                            }
                        }
                    }
                }
                
                if(NextTarget == -1 && Entity->Player.TargetedEnemyHandle > 0)
                {
                    for(int32 Index = 0; Index < Entity->Player.TargetedEnemyHandle - 1; Index++)
                    {
                        if(GameState->Entities[Index].Health > 0 && GameState->Entities[Index].Type == Entity_Enemy)
                        {
                            auto Distance = glm::distance(Entity->Position, GameState->Entities[Index].Position);
                            if(Distance <= Entity->Player.TargetingDistance)
                            {
                                NextTarget = Index;
                                break;
                            }
                        }
                    }
                }
                
                if(NextTarget != -1)
                {
                    GameState->Entities[Entity->Player.TargetedEnemyHandle].Enemy.IsTargeted = false;
                    GameState->Entities[NextTarget].Enemy.IsTargeted = true;
                    Entity->Player.TargetedEnemyHandle = NextTarget;
                }
            }
        }
        
        auto Direction = glm::vec2(Pos.x, Pos.y) - Entity->Position;
        Direction = glm::normalize(Direction);
        float Degrees = atan2(Direction.y, Direction.x);
        
        GameState->GameCamera.CenterTarget = glm::vec2(Entity->Position.x, Entity->Position.y);
    }
    else
    {
        Entity->Dead = true;
        PlayAnimation(Entity, "player_death", GameState);
    }
    
    if(Entity->Dead && !Entity->AnimationInfo.Playing)
    {
        Entity->Active = false;
    }
    
    auto& DustCloud = GameState->Entities[Entity->Player.DustCloudHandle];
    DustCloud.Position = Entity->Position;
    DustCloud.Active = Entity->Player.IsDashing;
}

void UpdateWeapon(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    bool32 IsAttacking = false;
    
    switch(Entity->Type)
    {
        case Entity_Player:
        {
            IsAttacking = Entity->Player.IsAttacking;
            
            switch(Entity->LookDirection)
            {
                case Up:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(-1.0f, 0.8f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(1.5f, 1.5f);
                }
                break;
                case Down:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(-1.0f, -1.0f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(1.5f, 0.5f);
                }
                break;
                case Left:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(-2.0f, 0.5f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(0.5f, 1.5f);
                }
                break;
                case Right:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(0, 0.5f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(0.5f, 1.5f);
                }
                break;
            }
            
        }
        break;
        case Entity_Enemy:
        {
            IsAttacking = Entity->Enemy.Skeleton.IsAttacking;
            
            switch(Entity->LookDirection)
            {
                case Up:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(-1.0f, 1.5f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(1.0f, 0.5f);
                }
                break;
                case Down:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(-1.0f, -1.0f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(1.0f, 0.5f);
                }
                break;
                case Left:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(-2.0f, 0.2f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(0.5f, 1.0f);
                }
                break;
                case Right:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(0, 0.2f);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(0.5f, 1.0f);
                }
                break;
            }
        }
        break;
    }
    
    Entity->Weapon.CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Weapon.Center.x * Entity->Weapon.Scale.x + Entity->Weapon.CollisionAABB.Offset.x, Entity->Position.y + Entity->Weapon.Center.y * Entity->Weapon.Scale.y + Entity->Weapon.CollisionAABB.Offset.y);
    
    collision_info CollisionInfo;
    CheckWeaponCollision(GameState, &Entity->Weapon, &CollisionInfo);
    
    if(IsAttacking)
    {
        for(int32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            if((Entity->Type == Entity_Player && CollisionInfo.Other[Index]->Type == Entity_Enemy && CollisionInfo.Other[Index]->Enemy.AIState != AI_Hit && CollisionInfo.Other[Index]->Enemy.AIState != AI_Dying && !CollisionInfo.Other[Index]->Hit) ||
               (Entity->Type == Entity_Enemy && CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing && !CollisionInfo.Other[Index]->Hit && TimerDone(GameState, CollisionInfo.Other[Index]->HitCooldownTimer)))
            {
                Hit(GameState, Entity, CollisionInfo.Other[Index]);
            }
        }
    }
}

void UpdateBlob(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    if(!Entity->Enemy.Blob.InPickupMode)
    {
        auto& Player = GameState->Entities[0];
        
        Entity->RenderButtonHint = !Entity->IsKinematic && glm::distance(Player.Position, Entity->Position) < 1.5f;
        
        if(TimerDone(GameState, Entity->Enemy.Blob.PickupThrowTimer) && Entity->RenderButtonHint && GetActionButtonDown(Action_Interact, GameState) && !Player.Player.Pickup)
        {
            Player.Player.Pickup = Entity;
            Player.Player.RenderCrosshair = true;
            Entity->Position = Player.Position;
            Entity->Velocity = glm::vec2(0.0f,0.0f);
            Entity->IsKinematic = true;
            Entity->Enemy.Blob.InPickupMode = true;
            Entity->RenderButtonHint = false;
        }
    }
    else
    {
        if(TimerDone(GameState, Entity->Enemy.Blob.PickupThrowTimer))
        {
            Entity->Velocity = glm::vec2();
        }
    }
    
    switch(Entity->Enemy.AIState)
    {
        case AI_Idle:
        {
            auto Player = GameState->Entities[0];
            real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
            
            if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
            {
                Entity->Enemy.AIState = AI_Following;
            }
            else
            {
                Entity->Velocity = glm::vec2();
            }
        }
        break;
        case AI_Following:
        {
            FindPath(GameState,Entity,GameState->Entities[GameState->PlayerIndex], &Entity->Enemy.AStarPath);
            FollowPath(GameState,Entity,GameState->Entities[GameState->PlayerIndex],DeltaTime, &Entity->Enemy.AStarPath);
            if(Abs(glm::distance(Entity->Position, GameState->Entities[GameState->PlayerIndex].Position)) < Entity->Enemy.MinDistanceToPlayer)
            {
                Entity->Enemy.AIState = AI_Charging;
                StartTimer(GameState, Entity->Enemy.Blob.ExplodeStartTimer);
            }
            else
            {
                Entity->Enemy.AIState = AI_Idle;
            }
        }
        break;
        case AI_Charging:
        {
            if(TimerDone(GameState, Entity->Enemy.Blob.ExplodeStartTimer))
            {
                Entity->Enemy.AIState = AI_Attacking;
                StartTimer(GameState, Entity->Enemy.Blob.ExplodeCountdownTimer);
            }
            
            if(Abs(glm::distance(Entity->Position, GameState->Entities[GameState->PlayerIndex].Position)) >= 1)
                Entity->Enemy.AIState = AI_Following;
        }
        break;
        case AI_Attacking:
        {
            if(TimerDone(GameState, Entity->Enemy.Blob.ExplodeCountdownTimer))
            {
                Entity->Enemy.AIState = AI_Dying;
                PlayAnimation(Entity, "explosion", GameState);
                Entity->Health = 0;
                Entity->Velocity = glm::vec2();
                if(Entity->Enemy.Blob.InPickupMode)
                {
                    auto& Player = GameState->Entities[0];
                    Player.Player.Pickup = 0;
                }
                PlaySoundEffect(GameState, &GameState->SoundManager.Explosion);
            }
        }
        break;
        case AI_Dying:
        {
            Entity->CollisionAABB.Extents = glm::vec2(Entity->Enemy.Blob.ExplosionCollisionExtentsX, Entity->Enemy.Blob.ExplosionCollisionExtentsY);
            if(!Entity->AnimationInfo.Playing)
            {
                Entity->Active = false;
                printf("HOT DAMN\n");
                //DeleteEntity(GameState, Entity->EntityIndex);
            }
        }
        break;
    }
    
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
    
    if(Entity->Enemy.AIState == AI_Dying)
    {
        for(int32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            auto Other = CollisionInfo.Other[Index];
            if(Other->Type != Entity_Barrel)
            {
                Hit(GameState, Entity, CollisionInfo.Other[Index]);
            }
        }
    }
    else if(!TimerDone(GameState, Entity->Enemy.Blob.PickupThrowTimer))
    {
        if(CollisionInfo.OtherCount > 0)
        {
            Entity->Enemy.AIState = AI_Dying;
            PlayAnimation(Entity, "explosion", GameState);
            Entity->Health = 0;
            Entity->Velocity = glm::vec2();
            if(Entity->Enemy.Blob.InPickupMode)
            {
                auto& Player = GameState->Entities[0];
                Player.Player.Pickup = 0;
            }
            PlaySoundEffect(GameState, &GameState->SoundManager.Explosion);
        }
    }
    
    Entity->Position.x += Entity->Velocity.x * (real32)DeltaTime;
    Entity->Position.y += Entity->Velocity.y * (real32)DeltaTime;
}

void UpdateSkeleton(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    if(Entity->Active && !Entity->Dead)
    {
        entity& Player = GameState->Entities[GameState->PlayerIndex];
        
        if(Entity->Hit)
        {
            if(Entity->Health == 0)
            {
                PlayAnimation(Entity, "skeleton_dead", GameState);
                Entity->AnimationInfo.FreezeFrame = true;
                Enemy.AIState = AI_Dying;
            }
            else if(strcmp(Entity->CurrentAnimation->Name, "skeleton_attack") != 0 && Enemy.AIState != AI_Dying)
            {
                PlayAnimation(Entity, "skeleton_hit", GameState);
                Enemy.AIState = AI_Hit;
                Entity->HitRecoilDirection = glm::normalize(Entity->Position - Player.Position);
                StartTimer(GameState, Entity->RecoilTimer);
            }
        }
        
        Entity->Velocity = glm::vec2(0,0);
        
        real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
        
        switch(Enemy.AIState)
        {
            case AI_Idle:
            {
                PlayAnimation(Entity, "skeleton_idle", GameState);
                
                if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
                {
                    Enemy.AIState = AI_Following;
                    PlayAnimation(Entity, "skeleton_walk", GameState);
                }
            }
            break;
            case AI_Alerted:
            {}
            break;
            case AI_Following:
            {
                if(Player.Active)
                {
                    if(DistanceToPlayer > Entity->Enemy.MaxFollowDistance)
                    {
                        PlayAnimation(Entity, "skeleton_idle", GameState);
                        Enemy.AIState = AI_Idle;
                    }
                    else if(DistanceToPlayer < Entity->Enemy.MinDistanceToPlayer)
                    {
                        PlayAnimation(Entity, "skeleton_idle", GameState);
                        StartTimer(GameState, Skeleton.ChargingTimer);
                        Enemy.AIState = AI_Charging;
                        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
                    }
                    else
                    {
                        PlayAnimation(Entity, "skeleton_walk", GameState);
                        FindPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
                        FollowPath(GameState, Entity, Player, DeltaTime, &Entity->Enemy.AStarPath);
                    }
                }
                else
                {
                    PlayAnimation(Entity, "skeleton_idle", GameState);
                }
            }
            break;
            case AI_Charging:
            {
                if(TimerDone(GameState, Skeleton.ChargingTimer))
                {
                    Enemy.AIState = AI_Attacking;
                    PlayAnimation(Entity, "skeleton_attack", GameState);
                }
            }
            break;
            case AI_Attacking:
            {
                if(Entity->AnimationInfo.FrameIndex >= 6 && Entity->AnimationInfo.FrameIndex < 14 && !Skeleton.IsAttacking && strcmp(Entity->CurrentAnimation->Name, "skeleton_idle") != 0)
                {
                    Skeleton.IsAttacking = true;
                    
                    Entity->AttackCount++;
                    if(Entity->AttackCount == 3)
                        Entity->AttackCount = 0;
                    
                    StartTimer(GameState, Skeleton.AttackCooldownTimer);
                }
                else if(!Entity->AnimationInfo.Playing)
                {
                    PlayAnimation(Entity, "skeleton_idle", GameState);
                }
                
                if(Skeleton.IsAttacking && TimerDone(GameState, Skeleton.AttackCooldownTimer))
                {
                    if(DistanceToPlayer > Entity->Enemy.MinDistanceToPlayer)
                    {
                        Skeleton.IsAttacking = false;
                        Enemy.AIState = AI_Following;
                    }
                    else if(DistanceToPlayer > Entity->Enemy.MaxAlertDistance)
                    {
                        Skeleton.IsAttacking = false;
                        Enemy.AIState = AI_Idle;
                    }
                    else
                    {
                        Skeleton.IsAttacking = false;
                        Entity->Enemy.AIState = AI_Charging;
                        StartTimer(GameState, Skeleton.ChargingTimer);
                    }
                }
            }
            break;
            case AI_Hit:
            {
                if(!TimerDone(GameState, Entity->RecoilTimer))
                {
                    Entity->Velocity = glm::vec2(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed, Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed);
                }
                
                if(!Entity->AnimationInfo.Playing)
                {
                    Enemy.AIState = AI_Idle;
                    PlayAnimation(Entity, "skeleton_idle", GameState);
                }
            }
            break;
            case AI_Dying:
            {
                Entity->IsKinematic = true;
                if(!Entity->AnimationInfo.Playing)
                {
                    Entity->Dead = true;
                    Entity->IsKinematic = true;
                }
            }
            break;
        }
        
        Entity->Position.x += Entity->Velocity.x * (real32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (real32)DeltaTime;
        
        //if(Entity->Enemy.AIState != AI_Attacking)
        //{
        glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
        
        if(Abs(Direction.x) < 0.4f)
        {
            if(Direction.y > 0)
            {
                Entity->LookDirection = Up;
            }
            else
            {
                Entity->LookDirection = Down;
            }
        }
        else
        {
            if(Direction.x < 0)
                Entity->LookDirection = Left;
            else
                Entity->LookDirection = Right;
        }
        
        Entity->IsFlipped = Direction.x < 0;
        //}
        
        // @Cleanup: move this somewhere else, maybe out of switch
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = glm::vec2(0,0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

void UpdateBarrel(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    if(Entity->Active)
    {
        collision_info CollisionInfo;
        Entity->IsColliding = false;
        Entity->CollisionAABB.IsColliding = false;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        if(TimerDone(GameState, Entity->Pickup.PickupThrowTimer))
        {
            Entity->Velocity = glm::vec2();
        }
        
        bool32 HasHitEnemy = false;
        
        if(Entity->Velocity.x != 0 || Entity->Velocity.y != 0)
        {
            for(int32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
            {
                if(CollisionInfo.Other[Index]->Type == Entity_Enemy)
                {
                    Hit(GameState, Entity, CollisionInfo.Other[Index]);
                    HasHitEnemy = true;
                }
            }
        }
        
        if(!Entity->Dead)
        {
            if(HasHitEnemy || (!Entity->AnimationInfo.Playing && strcmp(Entity->CurrentAnimation->Name, "barrel_thrown") == 0))
            {
                PlayAnimation(Entity, "barrel_destroy", GameState);
                Entity->Dead = true;
                Entity->Velocity = glm::vec2(0, 0);
                PlaySoundEffect(GameState, &GameState->SoundManager.BarrelBreak);
            }
        }
        
        if(!Entity->AnimationInfo.Playing)
        {
            Entity->Active = false;
        }
        
        auto& Player = GameState->Entities[0];
        
        Entity->RenderButtonHint = !Entity->IsKinematic && glm::distance(Player.Position, Entity->Position) < 1.5f;
        
        if(TimerDone(GameState, Entity->Pickup.PickupThrowTimer) && Entity->RenderButtonHint && GetActionButtonDown(Action_Interact, GameState) && !Player.Player.Pickup)
        {
            Player.Player.Pickup = Entity;
            Player.Player.RenderCrosshair = true;
            Entity->Position = Player.Position;
            Entity->Velocity = glm::vec2(0.0f,0.0f);
            Entity->IsKinematic = true;
        }
        
        Entity->Position.x += Entity->Velocity.x * (real32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (real32)DeltaTime;
    }
}

void UpdateGeneral(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    auto& RenderEntity = GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
    
    if(Entity->HitFlickerTimer)
    {
        if(Entity->HitFlickerFramesLeft > 0 && TimerDone(GameState, Entity->HitFlickerTimer))
        {
            Entity->HitFlickerFramesLeft--;
            
            if(Entity->HitFlickerFramesLeft % 2 == 0)
            {
                RenderEntity.Color = glm::vec4(1, 0, 0, 1);
            }
            else
                RenderEntity.Color = glm::vec4(1, 1, 1, 1);
            
            StartTimer(GameState, Entity->HitFlickerTimer);
        }
        else if(Entity->HitFlickerFramesLeft == 0)
        {
            RenderEntity.Color = glm::vec4(1, 1, 1, 1);
        }
    }
}

void UpdateEntities(game_state* GameState, real64 DeltaTime)
{
    for(uint32 EntityIndex = 0;
        EntityIndex < GameState->EntityCount;
        EntityIndex++)
    {
        entity* Entity = &GameState->Entities[EntityIndex];
        
        if(Entity->Active)
        {
            switch(Entity->Type)
            {
                case Entity_Player: 
                {
                    UpdatePlayer(Entity, GameState, DeltaTime);
                    UpdateWeapon(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_Enemy:
                {
                    switch(Entity->Enemy.EnemyType)
                    {
                        case Enemy_Skeleton:
                        {
                            UpdateSkeleton(Entity, GameState, DeltaTime);
                            UpdateWeapon(Entity, GameState, DeltaTime);
                        }
                        break;
                        case Enemy_Blob:
                        {
                            UpdateBlob(Entity, GameState, DeltaTime);
                        }
                        break;
                    }
                }
                break;
                case Entity_Barrel:
                {
                    UpdateBarrel(Entity, GameState, DeltaTime);
                }
            }
            
            UpdateGeneral(Entity, GameState, DeltaTime);
            
            if(Entity->Active && Entity->CurrentAnimation && Entity->AnimationInfo.Playing)
                TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
        }
    }
}
