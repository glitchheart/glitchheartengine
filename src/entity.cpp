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
        
        Entity->HitFlickerTimer.TimerHandle = -1;
        Entity->HitFlickerTimer.TimerMax = 0.05f;
        
        Entity->HitAttackCountIdResetTimer.TimerHandle = -1;
        Entity->HitAttackCountIdResetTimer.TimerMax = 1.0f;
        
        Entity->HealthDecreaseTimer.TimerMax = 0.8;
        Entity->HealthDecreaseTimer.TimerHandle = -1;
        Entity->Dead = false;
    }
    else
    {
        Entity->Active = true;
    }
    
    Entity->AnimationInfo.FreezeFrame = false;
    
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
            sscanf(LineBuffer, "scale %f", &Entity->Scale);
        }
        else if(StartsWith(&LineBuffer[0], "center"))
        {
            sscanf(LineBuffer, "center %f %f", &Entity->Center.x, &Entity->Center.y);
        }
        else if(StartsWith(&LineBuffer[0], "health"))
        {
            sscanf(LineBuffer, "health %d", &Entity->FullHealth);
            Entity->Health = Entity->FullHealth;
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
            sscanf(LineBuffer, "hitcooldowntimer %lf", &Entity->HitCooldownTimer.TimerMax);
            Entity->HitCooldownTimer.TimerHandle = -1;
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
        else if(StartsWith(&LineBuffer[0], "hitrecoilspeed"))
        {
            sscanf(LineBuffer, "hitrecoilspeed %f", &Entity->HitRecoilSpeed);
        }
        else if(StartsWith(&LineBuffer[0], "recoiltimer"))
        {
            Entity->RecoilTimer.TimerHandle = -1;
            sscanf(LineBuffer,"recoiltimer %lf",&Entity->RecoilTimer.TimerMax);
        }
        else if(StartsWith(&LineBuffer[0], "attackmovetimer"))
        {
            sscanf(LineBuffer, "attackmovetimer %lf", &Entity->AttackMoveTimer.TimerMax);
            Entity->AttackMoveTimer.TimerHandle = -1;
        }
        else if(StartsWith(&LineBuffer[0], "attackmovespeed"))
        {
            sscanf(LineBuffer, "attackmovespeed %f", &Entity->AttackMoveSpeed);
        }
        else if(StartsWith(&LineBuffer[0], "weaponscale"))
        {
            sscanf(LineBuffer, "weaponscale %f %f", &Entity->Weapon.Scale.x, &Entity->Weapon.Scale.y);
        }
        else if(StartsWith(&LineBuffer[0], "weaponcollider"))
        {
            sscanf(LineBuffer, "weaponcollider %f %f %f %f %d", &Entity->Weapon.CollisionAABB.Offset.x, &Entity->Weapon.CollisionAABB.Offset.y, &Entity->Weapon.CollisionAABB.Extents.x, &Entity->Weapon.CollisionAABB.Extents.y, &Entity->Weapon.CollisionAABB.IsTrigger);
        }
        else if(StartsWith(&LineBuffer[0], "weaponinfo"))
        {
            sscanf(LineBuffer, "weaponinfo %d up %f %f %f %f down %f %f %f %f left %f %f %f %f right %f %f %f %f", 
                   &Entity->Weapon.Damage,
                   &Entity->WeaponColliderInfo.OffsetUp.x, 
                   &Entity->WeaponColliderInfo.OffsetUp.y, 
                   &Entity->WeaponColliderInfo.ExtentsUp.x, 
                   &Entity->WeaponColliderInfo.ExtentsUp.y, 
                   &Entity->WeaponColliderInfo.OffsetDown.x, 
                   &Entity->WeaponColliderInfo.OffsetDown.y, 
                   &Entity->WeaponColliderInfo.ExtentsDown.x, 
                   &Entity->WeaponColliderInfo.ExtentsDown.y, 
                   &Entity->WeaponColliderInfo.OffsetLeft.x, 
                   &Entity->WeaponColliderInfo.OffsetLeft.y, 
                   &Entity->WeaponColliderInfo.ExtentsLeft.x, 
                   &Entity->WeaponColliderInfo.ExtentsLeft.y, 
                   &Entity->WeaponColliderInfo.OffsetRight.x, 
                   &Entity->WeaponColliderInfo.OffsetRight.y, 
                   &Entity->WeaponColliderInfo.ExtentsRight.x, 
                   &Entity->WeaponColliderInfo.ExtentsRight.y);
        }
    }
}

static void LoadEnemyData(FILE* File, entity* Entity, game_state* GameState)
{
    Entity->Enemy.IsTargeted = false;
    Entity->Enemy.AIState = AI_Idle;
    Entity->Type = Entity_Enemy;
    Entity->Enemy.AStarPath = {};
    Entity->Enemy.HealthCountStart = glm::vec2(-10, 50);
    if(File)
    {
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "#"))
            {
                break;
            }
            else if(StartsWith(&LineBuffer[0], "enemytype"))
            {
                sscanf(LineBuffer, "enemytype %d", &Entity->Enemy.EnemyType);
            }
            else if(StartsWith(&LineBuffer[0], "walkingspeed"))
            {
                sscanf(LineBuffer, "walkingspeed %f", &Entity->Enemy.WalkingSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "closetoplayerspeed"))
            {
                sscanf(LineBuffer, "closetoplayerspeed %f", &Entity->Enemy.CloseToPlayerSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "targetingposition"))
            {
                sscanf(LineBuffer, "targetingposition %f %f", &Entity->Enemy.TargetingPositionX, &Entity->Enemy.TargetingPositionY);
            }
            else if(StartsWith(&LineBuffer[0], "maxalertdistance"))
            {
                sscanf(LineBuffer, "maxalertdistance %f", &Entity->Enemy.MaxAlertDistance);
            }
            else if(StartsWith(&LineBuffer[0], "maxfollowdistance"))
            {
                sscanf(LineBuffer, "maxfollowdistance %f", &Entity->Enemy.MaxFollowDistance);
            }
            else if(StartsWith(&LineBuffer[0], "mindistancetoplayer"))
            {
                sscanf(LineBuffer, "mindistancetoplayer %f", &Entity->Enemy.MinDistanceToPlayer);
            }
            else if(StartsWith(&LineBuffer[0], "slowdowndistance"))
            {
                sscanf(LineBuffer, "slowdowndistance %f", &Entity->Enemy.SlowdownDistance);
            }
            else if(StartsWith(&LineBuffer[0], "attackdistance"))
            {
                sscanf(LineBuffer, "attackdistance %f", &Entity->Enemy.AttackDistance);
            }
            else if(StartsWith(&LineBuffer[0], "aistate"))
            {
                sscanf(LineBuffer, "aistate %d", &Entity->Enemy.AIState);
            }
            else if(StartsWith(&LineBuffer[0], "astarcooldowntimer"))
            {
                Entity->Enemy.AStarPath.AStarCooldownTimer.TimerHandle = -1;
                
                sscanf(LineBuffer, "astarcooldowntimer %lf", &Entity->Enemy.AStarPath.AStarCooldownTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0], "healthbar"))
            {
                Entity->Enemy.Healthbar = (entity_healthbar*)malloc(sizeof(entity_healthbar));
                sscanf(LineBuffer, "healthbar offset %f %f scale %f %f",&Entity->Enemy.Healthbar->Offset.x,
                       &Entity->Enemy.Healthbar->Offset.y,&Entity->Enemy.Healthbar->Scale.x,
                       &Entity->Enemy.Healthbar->Scale.y);
                ui_render_info RenderInfo = {};
                RenderInfo.Texture = GameState->RenderState.Textures["4_health"];
                RenderInfo.TextureOffset = glm::vec2(256, 0);
                RenderInfo.FrameSize = glm::vec2(64, 16);
                RenderInfo.ShaderIndex = Shader_Spritesheet;
                Entity->Enemy.Healthbar->RenderInfo = RenderInfo;
            }
        }
    }
}


AI_FUNC(SkeletonIdle)
{
    PlayAnimation(Entity, "skeleton_idle", GameState);
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
        PlayAnimation(Entity, "skeleton_walk", GameState);
    }
}

AI_FUNC(SkeletonAlerted)
{
    
}

AI_FUNC(SkeletonFollowing)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(!Player.Dead && Player.Active)
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
        else if(DistanceToPlayer <= Entity->Enemy.SlowdownDistance)
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.CloseToPlayerSpeed, Direction.y * Entity->Enemy.CloseToPlayerSpeed);
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

AI_FUNC(SkeletonCharging)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer >= Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
    }
    else if(TimerDone(GameState, Skeleton.ChargingTimer) && DistanceToPlayer <= Enemy.AttackDistance)
    {
        Enemy.AIState = AI_Attacking;
        PlayAnimation(Entity, "skeleton_attack", GameState);
    }
    else
    {
        PlayAnimation(Entity, "skeleton_walk", GameState);
        glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
        Entity->Velocity = glm::vec2((Direction.x + 0.1f) * Entity->Enemy.CloseToPlayerSpeed, (Direction.y + 0.1f) * Entity->Enemy.CloseToPlayerSpeed);
    }
}

AI_FUNC(SkeletonAttacking)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(Player.Dead)
    {
        Skeleton.IsAttacking = false;
        Enemy.AIState = AI_Idle;
    }
    else
    {
        if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            Entity->Velocity = glm::vec2(Direction.x * Entity->AttackMoveSpeed, Direction.y * Entity->AttackMoveSpeed);
        }
        
        if(Entity->AnimationInfo.FrameIndex >= 6 &&Entity->AnimationInfo.FrameIndex < 14 && !Skeleton.IsAttacking && strcmp(Entity->CurrentAnimation->Name, "skeleton_idle") != 0)
        {
            StartTimer(GameState, Entity->AttackMoveTimer);
            
            if(Entity->AnimationInfo.FrameIndex >= 8)
            {
                Skeleton.IsAttacking = true;
                
                Entity->AttackCount++;
                if(Entity->AttackCount == 3)
                    Entity->AttackCount = 0;
                
                StartTimer(GameState, Skeleton.AttackCooldownTimer);
            }
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
}

AI_FUNC(SkeletonHit)
{
    auto& Enemy = Entity->Enemy;
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

AI_FUNC(SkeletonDying)
{
    Entity->IsKinematic = true;
    if(!Entity->AnimationInfo.Playing)
    {
        Entity->Dead = true;
        Entity->IsKinematic = true;
    }
}

AI_FUNC(SkeletonWandering)
{
    
}

AI_FUNC(BlobIdle)
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

AI_FUNC(BlobAlerted)
{
}

AI_FUNC(BlobFollowing)
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

AI_FUNC(BlobCharging)
{
    if(TimerDone(GameState, Entity->Enemy.Blob.ExplodeStartTimer))
    {
        Entity->Enemy.AIState = AI_Attacking;
        StartTimer(GameState, Entity->Enemy.Blob.ExplodeCountdownTimer);
    }
    
    if(Abs(glm::distance(Entity->Position, GameState->Entities[GameState->PlayerIndex].Position)) >= 1)
        Entity->Enemy.AIState = AI_Following;
}

AI_FUNC(BlobAttacking)
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

AI_FUNC(BlobHit)
{
}

AI_FUNC(BlobWandering)
{
}

AI_FUNC(BlobDying)
{
    Entity->CollisionAABB.Extents = glm::vec2(Entity->Enemy.Blob.ExplosionCollisionExtentsX, Entity->Enemy.Blob.ExplosionCollisionExtentsY);
    if(!Entity->AnimationInfo.Playing)
    {
        Entity->Active = false;
        //DeleteEntity(GameState, Entity->EntityIndex);
    }
}


static void LoadSkeletonData(game_state* GameState, int32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/skeleton.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    Entity->Enemy.Skeleton = {};
    Entity->Position = glm::vec2(0, 0);
    Entity->AnimationInfo.FreezeFrame = false;
    Entity->Dead = false;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity,GameState, Handle != -1);
        LoadEnemyData(File,Entity,GameState);
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "#"))
            {
                break;
            }
            else if(StartsWith(&LineBuffer[0],"attackcooldowntimer"))
            {
                Entity->Enemy.Skeleton.AttackCooldownTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"attackcooldowntimer %lf",&Entity->Enemy.Skeleton.AttackCooldownTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0],"chargingtimer"))
            {
                Entity->Enemy.Skeleton.ChargingTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"chargingtimer %lf",&Entity->Enemy.Skeleton.ChargingTimer.TimerMax);
            }
        }
        fclose(File);
    }
    
    if(Entity)
    {
        AI_FUNCS(Skeleton);
    }
}

static void LoadBlobData(game_state* GameState, int32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/blob.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity,GameState, Handle != -1);
        LoadEnemyData(File,Entity,GameState);
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "#"))
            {
                break;
            }
            else if(StartsWith(&LineBuffer[0],"pickupthrowtimer"))
            {
                Entity->Enemy.Blob.PickupThrowTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"pickupthrowtimer %lf",&Entity->Enemy.Blob.PickupThrowTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0],"explodestarttimer"))
            {
                Entity->Enemy.Blob.ExplodeStartTimer.TimerHandle = -1;
                sscanf(LineBuffer,"explodestarttimer %lf",&Entity->Enemy.Blob.ExplodeStartTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0],"explodecountdowntimer"))
            {
                Entity->Enemy.Blob.ExplodeCountdownTimer.TimerHandle = -1;
                sscanf(LineBuffer,"explodecountdowntimer %lf",&Entity->Enemy.Blob.ExplodeCountdownTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0],"explosioncollisionextents"))
            {
                sscanf(LineBuffer,"explosioncollisionextents %f %f",&Entity->Enemy.Blob.ExplosionCollisionExtentsX, &Entity->Enemy.Blob.ExplosionCollisionExtentsY);
            }
        }
        fclose(File);
    }
    
    if(Entity)
    {
        AI_FUNCS(Blob);
    }
    
}

static void LoadPlayerData(game_state* GameState, int32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/player.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] :  &GameState->Entities[GameState->EntityCount];
    Entity->Type = Entity_Player;
    Entity->Player.TargetedEnemyHandle = -1;
    Entity->Player.LastKnownDirectionX = 1.0f;
    Entity->Player.LastKnownDirectionY = 0;
    Entity->Player.Pickup = {};
    Entity->Player = {};
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        Entity->Dead = false;
        LoadEntityData(File, Entity, GameState, Handle != -1);
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "walkingspeed"))
            {
                sscanf(LineBuffer, "walkingspeed %f", &Entity->Player.WalkingSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "throwingspeed"))
            {
                sscanf(LineBuffer, "throwingspeed %f", &Entity->Player.ThrowingSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "crosshairradius"))
            {
                sscanf(LineBuffer, "crosshairradius %f", &Entity->Player.CrosshairRadius);
            }
            else if(StartsWith(&LineBuffer[0], "dashcounterdivider"))
            {
                sscanf(LineBuffer, "dashcounterdivider %f", &Entity->Player.DashCounterDivider);
            }
            else if(StartsWith(&LineBuffer[0], "targetingdistance"))
            {
                sscanf(LineBuffer, "targetingdistance %f", &Entity->Player.TargetingDistance);
            }
            else if(StartsWith(&LineBuffer[0], "attackcooldowntimer"))
            {
                Entity->Player.AttackCooldownTimer.TimerMax = 0;
                sscanf(LineBuffer, "attackcooldowntimer %lf", &Entity->Player.AttackCooldownTimer.TimerMax);
                Entity->Player.AttackCooldownTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "lastattacktimer"))
            {
                sscanf(LineBuffer, "lastattacktimer %lf", &Entity->Player.LastAttackTimer.TimerMax);
                Entity->Player.LastAttackTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "pickupcooldowntimer"))
            {
                sscanf(LineBuffer, "pickupcooldowntimer %lf", &Entity->Player.PickupCooldownTimer.TimerMax);
                Entity->Player.PickupCooldownTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "dashtimer"))
            {
                sscanf(LineBuffer, "dashtimer %lf", &Entity->Player.DashTimer.TimerMax);
                Entity->Player.DashTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "dashcooldowntimer"))
            {
                sscanf(LineBuffer, "dashcooldowntimer %lf", &Entity->Player.DashCooldownTimer.TimerMax);
                Entity->Player.DashCooldownTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "dashspeed"))
            {
                sscanf(LineBuffer, "dashspeed %f", &Entity->Player.DashSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "stamina "))
            {
                sscanf(LineBuffer, "stamina %d", &Entity->Player.FullStamina);
                Entity->Player.Stamina = Entity->Player.FullStamina;
            }
            else if(StartsWith(&LineBuffer[0], "staminagaintimer"))
            {
                sscanf(LineBuffer, "staminagaintimer %lf", &Entity->Player.StaminaGainTimer.TimerMax);
                printf("We did it!\n");
                Entity->Player.StaminaGainTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "staminadecreasetimer"))
            {
                sscanf(LineBuffer, "staminadecreasetimer %lf", &Entity->Player.StaminaDecreaseTimer.TimerMax);
                printf("LOADED IT\n");
                Entity->HealthDecreaseTimer.TimerHandle = -1;
            }
            else if(StartsWith(&LineBuffer[0], "hitstaminacost"))
            {
                sscanf(LineBuffer, "hitstaminacost %d", &Entity->Player.HitStaminaCost);
            }
            else if(StartsWith(&LineBuffer[0], "rollstaminacost"))
            {
                sscanf(LineBuffer, "rollstaminacost %d", &Entity->Player.RollStaminaCost);
            }
            else if(StartsWith(&LineBuffer[0], "attackstaminacost"))
            {
                sscanf(LineBuffer, "attackstaminacost %d", &Entity->Player.AttackStaminaCost);
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
                
                sscanf(LineBuffer, "dustcloud scale %f animation %s", &PlayerDustCloud->Scale, AnimationName);
                
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
    Wraith->Scale = 2;
    Wraith->Velocity = glm::vec2(-2,0);
    Wraith->Active = true;
    Wraith->IsKinematic = false;
    Wraith->Layer = Layer_Enemy;
    
    collision_AABB CollisionAABB;
    Wraith->Center = glm::vec2(0.5, 0.5f);
    CollisionAABB.Center = glm::vec2(Wraith->Position.x + Wraith->Center.x * Wraith->Scale,
                                     Wraith->Position.y + Wraith->Center.y * Wraith->Scale);
    CollisionAABB.Offset = glm::vec2(0, -1.2f);
    CollisionAABB.Extents = glm::vec2(0.3f, 0.15f);
    CollisionAABB.IsTrigger = false;
    Wraith->CollisionAABB = CollisionAABB;
    
    Wraith->HitTrigger.Center = glm::vec2(Wraith->Position.x + Wraith->Center.x * Wraith->Scale,
                                          Wraith->Position.y + Wraith->Center.y * Wraith->Scale);
    Wraith->HitTrigger.Extents = glm::vec2(0.8f, 0.9f);
    Wraith->HitTrigger.IsTrigger;
    Wraith->HitTrigger.Offset = glm::vec2(0, -0.4f);
    Wraith->HasHitTrigger = true;
    
    Wraith->Enemy.WalkingSpeed = 5;
    Wraith->Enemy.MaxAlertDistance = 5;
    Wraith->Enemy.MaxFollowDistance = 10;
    Wraith->Enemy.MinDistanceToPlayer = 2;
    Wraith->Enemy.AIState = AI_Idle;
    
    Wraith->Enemy.Wraith.AttackCooldownTimer.TimerHandle = -1;
    Wraith->Enemy.Wraith.AttackCooldownTimer.TimerMax = 0.5;
    
    Wraith->Enemy.Wraith.ChargingTimer.TimerHandle = -1;
    Wraith->Enemy.Wraith.ChargingTimer.TimerMax = 0.2f;
    
    Wraith->HitFlickerTimer.TimerHandle = -1;
    Wraith->HitFlickerTimer.TimerMax = 0.05f;
    
    Wraith->Enemy.AStarPath.AStarCooldownTimer.TimerHandle = -1;
    Wraith->Enemy.AStarPath.AStarCooldownTimer.TimerMax = 0.6;
    
    Wraith->RecoilTimer.TimerHandle = -1;
    Wraith->RecoilTimer.TimerMax = 0.2;
    
    Wraith->HitCooldownTimer.TimerHandle = -1;
    Wraith->HitCooldownTimer.TimerMax = 0.4;
    
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

static void SpawnBarrel(game_state* GameState, glm::vec2 Position)
{
    entity* Barrel = &GameState->Entities[GameState->EntityCount];
    Barrel->Name = "barrel";
    Barrel->Active = true;
    Barrel->Type = Entity_Barrel;
    Barrel->Layer = Layer_Environment;
    Barrel->IgnoreLayers = Layer_Environment;
    
    Barrel->Pickup.PickupThrowTimer.TimerHandle = -1;
    Barrel->Pickup.PickupThrowTimer.TimerMax = 1.5f;
    
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
    Barrel->Scale = 2;
    Barrel->Velocity = glm::vec2(0,0);
    Barrel->Center = glm::vec2(0.5, 0.5);
    Barrel->IsStatic = true;
    Barrel->IsKinematic = false;
    Barrel->IsPickup = true;
    
    collision_AABB CollisionAABB;
    
    CollisionAABB.Extents = glm::vec2(0.5f,0.5f);
    CollisionAABB.Center = glm::vec2(Barrel->Position.x + Barrel->Center.x * Barrel->Scale,
                                     Barrel->Position.y + Barrel->Center.y * Barrel->Scale);
    CollisionAABB.IsTrigger = true;
    Barrel->CollisionAABB = CollisionAABB;
    
    Barrel->EntityIndex = GameState->EntityCount;
    GameState->EntityCount++;
}


static void DecreaseStamina(entity* Entity, game_state* GameState, int32 Cost) 
{
    int32 NewStamina = Max(0, Entity->Player.Stamina - Cost);
    Entity->Player.StaminaLost = Entity->Player.Stamina - NewStamina;
    Entity->Player.Stamina = NewStamina;
    StartTimer(GameState, Entity->Player.StaminaDecreaseTimer);
}

void Hit(game_state* GameState, entity* ByEntity, entity* HitEntity)
{
    if(HitEntity->HitAttackCountId != ByEntity->AttackCount)
    {
        StartTimer(GameState, GameState->GameCamera.ScreenShakeTimer);
        StartTimer(GameState, HitEntity->HitCooldownTimer);
        PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit02);
        HitEntity->HitRecoilDirection = glm::normalize(HitEntity->Position - ByEntity->Position);
        
        int32 Damage = ByEntity->Weapon.Damage > HitEntity->Health ? HitEntity->Health : ByEntity->Weapon.Damage;
        HitEntity->Health -= Damage;
        
        HitEntity->Hit = true;
        HitEntity->HitAttackCountId = ByEntity->AttackCount;
        HitEntity->HitFlickerFramesLeft = HitEntity->HitFlickerFrameMax;
        
        StartTimer(GameState, HitEntity->HealthDecreaseTimer);
        HitEntity->HealthLost = Damage;
        
        StartTimer(GameState, HitEntity->HitFlickerTimer);
        StartTimer(GameState, HitEntity->HitAttackCountIdResetTimer);
        
        if(HitEntity->Type == Entity_Enemy)
        {
            HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Visible = true;
            HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Position = HitEntity->Enemy.HealthCountStart;
            sprintf(HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Count, "%d", HitEntity->HealthLost);
            HitEntity->Enemy.HealthCountIndex++;
            if(HitEntity->Enemy.HealthCountIndex == 10)
                HitEntity->Enemy.HealthCountIndex = 0;
        }
        
        if(HitEntity->Type == Entity_Player)
        {
            DecreaseStamina(HitEntity,GameState,HitEntity->Player.HitStaminaCost);
            StartFade(GameState->GameCamera, Fading_OutIn, 4.0f, glm::vec3(1, 0, 0), 0.0f, 0.4f);
        }
    }
}

void UpdatePlayer(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    if(Entity->Player.Stamina != Entity->Player.FullStamina)
    {
        if(TimerDone(GameState, Entity->Player.StaminaGainTimer))
        {
            Entity->Player.Stamina += 1;
            
            if(Entity->Player.Stamina < Entity->Player.FullStamina)
            {
                StartTimer(GameState, Entity->Player.StaminaGainTimer);
                printf("Stamina gain timer max %f\n", Entity->Player.StaminaGainTimer.TimerMax);
            }
        }
    }
    
    if(Entity->Hit)
        Entity->Player.IsAttacking = false;
    
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
        if(!Entity->Player.IsAttacking && !Entity->Hit && !Entity->Player.IsDashing && GetActionButtonDown(Action_Use, GameState))
        {
            PlaySoundEffect(GameState, &GameState->SoundManager.UseHealth);
            Entity->Health = Min(Entity->FullHealth, Entity->Health + 30);
        }
        
        if(Entity->Player.IsDefending && !Entity->AnimationInfo.Playing)
        {
            Entity->Player.IsDefending = false;
        }
        
        if(!TimerDone(GameState, Entity->HitCooldownTimer))
        {
            PlayAnimation(Entity, "swordsman_hit", GameState);
            Entity->Velocity = glm::vec2(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed * DeltaTime, Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed * DeltaTime);
        }
        else
        {
            if(!Entity->Player.IsDashing && !Entity->Player.IsDefending)
            {
                glm::vec2 Direction = UsingController ? glm::normalize(Entity->Velocity) : DirectionToMouse;
                if(Direction.x == Direction.x && Direction.y == Direction.y && (Direction.x != 0 || Direction.y != 0))
                {
                    Entity->Player.LastKnownDirectionX = Direction.x;
                    Entity->Player.LastKnownDirectionY = Direction.y;
                }
                
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
                                    PlayAnimation(Entity, "swordsman_walk", GameState);
                                else
                                    PlayAnimation(Entity, "swordsman_idle", GameState);
                            }
                            else
                            {
                                Entity->LookDirection = Down;
                                if(Moving)
                                    PlayAnimation(Entity, "swordsman_walk", GameState);
                                else
                                    PlayAnimation(Entity, "swordsman_idle", GameState);
                            }
                        }
                        else
                        {
                            Entity->LookDirection = Right;
                            if(Moving)
                                PlayAnimation(Entity, "swordsman_walk", GameState);
                            else
                                PlayAnimation(Entity, "swordsman_idle", GameState);
                        }
                        
                        Entity->IsFlipped = Direction.x < 0;
                        
                        if(Entity->LookDirection == Right && Entity->IsFlipped)
                            Entity->LookDirection = Left;
                    }
                    else if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                    {
                        auto XValue = UsingController ? InputX : DirectionToMouse.x;
                        auto YValue = UsingController ? InputY : DirectionToMouse.y;
                        
                        PlayAnimation(Entity, "swordsman_walk", GameState);
                        
                        if(Abs(XValue) < 0.7f && Abs(YValue) > 0.2f)
                        {
                            if(YValue > 0)
                            {
                                Entity->LookDirection = Up;
                            }
                            else
                            {
                                Entity->LookDirection = Down;
                                //PlayAnimation(Entity, "player_run_down", GameState);
                            }
                        }
                        else
                        {
                            Entity->LookDirection = Right;
                            //PlayAnimation(Entity, "player_run_right", GameState);
                        }
                        
                        if(XValue != 0)
                        {
                            Entity->IsFlipped = XValue < 0;
                        }
                    }
                    else
                    {
                        PlayAnimation(Entity, "swordsman_idle", GameState);
                        if(Abs(Entity->Player.LastKnownDirectionX) < 0.3)
                        {
                            if(Entity->Player.LastKnownDirectionY > 0)
                            {
                                Entity->LookDirection = Up;
                                //PlayAnimation(Entity, "player_idle_up", GameState);
                            }
                            else
                            {
                                Entity->LookDirection = Down;
                                //PlayAnimation(Entity, "player_idle_down", GameState);
                            }
                        }
                        else
                        {
                            Entity->LookDirection = Right;
                            //PlayAnimation(Entity, "player_idle_right", GameState);
                        }
                        Entity->IsFlipped = Entity->Player.LastKnownDirectionX < 0;
                        
                    }
                }
                else if(!TimerDone(GameState, Entity->AttackMoveTimer))
                {
                    glm::vec2 Vel;
                    real32 AttackMoveSpeed = Entity->AttackMoveSpeed;
                    
                    if(Entity->Player.LastKnownDirectionX != 0 || Entity->Player.LastKnownDirectionY != 0)
                    {
                        Vel = glm::vec2(Entity->Player.LastKnownDirectionX * AttackMoveSpeed * DeltaTime, Entity->Player.LastKnownDirectionY * AttackMoveSpeed * DeltaTime);
                    }
                    
                    Entity->Velocity = Vel;
                }
                else
                {
                    Entity->Velocity = glm::vec2(0, 0);
                }
                
                if(Entity->LookDirection == Right && Entity->IsFlipped)
                    Entity->LookDirection = Left;
                
                // Pickup
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
            
            if(Entity->Player.IsDashing && TimerDone(GameState, Entity->Player.DashTimer))
            {
                Entity->Player.IsDashing = false;
                StartTimer(GameState, Entity->Player.DashCooldownTimer);
            }
            
            if(!Entity->Player.Pickup && !Entity->Player.IsAttacking  && TimerDone(GameState, Entity->Player.DashTimer) && GetActionButtonDown(Action_Dash, GameState))
            {
                if(!Entity->Player.IsDashing && TimerDone(GameState, Entity->Player.DashCooldownTimer))
                {
                    PlaySoundEffect(GameState, &GameState->SoundManager.Dash);
                    Entity->Player.IsDashing = true;
                    Entity->Player.DashDirectionX = Entity->Player.LastKnownDirectionX;
                    Entity->Player.DashDirectionY = Entity->Player.LastKnownDirectionY; 
                    StartTimer(GameState, Entity->Player.DashTimer);
                    
                    DecreaseStamina(Entity,GameState,Entity->Player.RollStaminaCost);
                    StartTimer(GameState, Entity->Player.StaminaDecreaseTimer);
                    
                    PlayAnimation(Entity, "swordsman_roll", GameState);
                }
            }
            
            if(Entity->Player.IsDashing)
            {
                auto XInput = GetInputX(GameState);
                auto YInput = GetInputY(GameState);
                
                auto NewDirection = glm::normalize(glm::vec2(Entity->Player.DashDirectionX + XInput / Entity->Player.DashCounterDivider, Entity->Player.DashDirectionY + YInput / Entity->Player.DashCounterDivider));
                
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
                    PlayAnimation(Entity, "swordsman_attack_up", GameState);
                }
                break;
                case Down:
                {
                    PlayAnimation(Entity, "swordsman_attack_down", GameState);
                }
                break;
                case Left:
                case Right:
                {
                    PlayAnimation(Entity, "swordsman_attack_right", GameState);
                }
                break;
            }
            
            Entity->Player.IsAttacking = true;
            StartTimer(GameState, Entity->Player.LastAttackTimer);
            
            DecreaseStamina(Entity,GameState,Entity->Player.AttackStaminaCost);
            StartTimer(GameState, Entity->AttackMoveTimer);
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
    else if(!Entity->Dead)
    {
        Entity->Dead = true;
        PlayAnimation(Entity, "swordsman_death", GameState);
        Entity->AnimationInfo.FreezeFrame = true;
    }
}

void UpdateWeapon(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    bool32 IsAttacking = false;
    
    switch(Entity->Type)
    {
        case Entity_Player:
        {
            IsAttacking = Entity->Player.IsAttacking;
            auto WeaponColliderInfo = Entity->WeaponColliderInfo;
            
            switch(Entity->LookDirection)
            {
                case Up:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetUp.x, WeaponColliderInfo.OffsetUp.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsUp.x, WeaponColliderInfo.ExtentsUp.y);
                }
                break;
                case Down:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetDown.x, WeaponColliderInfo.OffsetDown.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsDown.x, WeaponColliderInfo.ExtentsDown.y);
                }
                break;
                case Left:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetLeft.x, WeaponColliderInfo.OffsetLeft.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsLeft.x, WeaponColliderInfo.ExtentsLeft.y);
                }
                break;
                case Right:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetRight.x, WeaponColliderInfo.OffsetRight.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsRight.x, WeaponColliderInfo.ExtentsRight.y);
                }
                break;
            }
        }
        break;
        case Entity_Enemy:
        {
            IsAttacking = Entity->Enemy.Skeleton.IsAttacking;
            auto WeaponColliderInfo = Entity->WeaponColliderInfo;
            
            switch(Entity->LookDirection)
            {
                case Up:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetUp.x, WeaponColliderInfo.OffsetUp.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsUp.x, WeaponColliderInfo.ExtentsUp.y);
                }
                break;
                case Down:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetDown.x, WeaponColliderInfo.OffsetDown.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsDown.x, WeaponColliderInfo.ExtentsDown.y);
                }
                break;
                case Left:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetLeft.x, WeaponColliderInfo.OffsetLeft.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsLeft.x, WeaponColliderInfo.ExtentsLeft.y);
                }
                break;
                case Right:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetRight.x, WeaponColliderInfo.OffsetRight.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsRight.x, WeaponColliderInfo.ExtentsRight.y);
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
            if((Entity->Type == Entity_Player && CollisionInfo.Other[Index]->Type == Entity_Enemy && CollisionInfo.Other[Index]->Enemy.AIState != AI_Hit && CollisionInfo.Other[Index]->Enemy.AIState != AI_Dying) ||
               (Entity->Type == Entity_Enemy && CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing && !CollisionInfo.Other[Index]->Hit && TimerDone(GameState, CollisionInfo.Other[Index]->HitCooldownTimer)))
            {
                Hit(GameState, Entity, CollisionInfo.Other[Index]);
            }
        }
    }
}

void UpdateAI(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    switch(Entity->Enemy.AIState)
    {
        case AI_Idle:
        {
            Entity->Enemy.Idle(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Alerted:
        {
            Entity->Enemy.Alerted(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Following:
        {
            Entity->Enemy.Following(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Charging:
        {
            Entity->Enemy.Charging(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Attacking:
        {
            Entity->Enemy.Attacking(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Hit:
        {
            Entity->Enemy.Hit(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Dying:
        {
            Entity->Enemy.Dying(Entity,GameState,DeltaTime);
        }
        break;
        case AI_Wandering:
        {
            Entity->Enemy.Wandering(Entity,GameState,DeltaTime);
        }
        break;
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
    
    UpdateAI(Entity,GameState,DeltaTime);
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
    
    if(Entity->AnimationInfo.FrameIndex < 15)
    {
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
            if(Entity->Health <= 0)
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
        
        UpdateAI(Entity,GameState,DeltaTime);
        
        Entity->Position.x += Entity->Velocity.x * (real32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (real32)DeltaTime;
        
        if(Entity->Enemy.AIState != AI_Attacking)
        {
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
        }
        
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
    
    if(TimerDone(GameState, Entity->HitAttackCountIdResetTimer))
    {
        Entity->HitAttackCountId = -1;
    }
    
    if(TimerDone(GameState, Entity->HealthDecreaseTimer))
        Entity->HealthLost = 0;
    
    if(Entity->Type == Entity_Enemy)
    {
        for(int32 HealthCountIndex = 0; HealthCountIndex < 10; HealthCountIndex++)
        {
            auto& HealthCount = Entity->Enemy.HealthCounts[HealthCountIndex];
            
            if(HealthCount.Visible)
            {
                HealthCount.Position.y += 100 * DeltaTime;
                
                if(HealthCount.Position.y - Entity->Enemy.HealthCountStart.y >= 70)
                {
                    HealthCount.Visible = false;
                }
            }
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
                    if(!Entity->Dead)
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
