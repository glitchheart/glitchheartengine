void PrintEntityInfo(const entity& Entity)
{
    printf("Entity: Name %s, position x %f y %f, rotation x %f y %f z %f\n", Entity.Name, Entity.Position.x, Entity.Position.y, Entity.Rotation.x, Entity.Rotation.y, Entity.Rotation.z);
}

static void DeleteEntity(game_state* GameState, u32 EntityIndex)
{
    if(GameState->EntityCount == 0 || GameState->RenderState.RenderEntityCount == 0)
        return;
    
    entity* Entity = &GameState->Entities[EntityIndex];
    
    u32 RenderEntityHandle = Entity->RenderEntityHandle;
    
    for(i32 RenderIndex = RenderEntityHandle; RenderIndex < GameState->RenderState.RenderEntityCount; RenderIndex++)
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
    
    for(u32 Index = EntityIndex; Index < (u32)GameState->EntityCount - 1; Index++)
    {
        GameState->Entities[Index] = GameState->Entities[Index + 1];
        GameState->Entities[Index].EntityIndex = Index;
        GameState->RenderState.RenderEntities[GameState->Entities[Index].RenderEntityHandle].Entity = &GameState->Entities[Index];
    }
    
    GameState->EntityCount--;
}

static void LoadEntityData(FILE* File, entity* Entity, game_state* GameState, b32 IsReload = false)
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
        Entity->HitFlickerTimer.Name = "Hit Flicker";
        
        Entity->HitAttackCountIdResetTimer.TimerHandle = -1;
        Entity->HitAttackCountIdResetTimer.TimerMax = 1.0f;
        Entity->HitAttackCountIdResetTimer.Name = "Hit Attack Count Id Reset";
        
        Entity->HealthDecreaseTimer.TimerMax = 0.8;
        Entity->HealthDecreaseTimer.TimerHandle = -1;
        Entity->HealthDecreaseTimer.Name = "Health Decrease";
        Entity->Dead = false;
    }
    else
    {
        Entity->Active = true;
    }
    
    Entity->AttackCount = 0;
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
            b32 Active;
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
            b32 IsKinematic;
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
            sscanf(LineBuffer, "health %hd", &Entity->FullHealth);
            Entity->Health = Entity->FullHealth;
        }
        else if(StartsWith(&LineBuffer[0], "animation"))
        {
            char* AnimationName = (char*)malloc(30 * sizeof(char)); 
            sscanf(LineBuffer, "animation %s", AnimationName);
            PlayAnimation(Entity, AnimationName, GameState);
            free(AnimationName);
        }
        else if(StartsWith(&LineBuffer[0], "renderbuttonoffset"))
        {
            char* AnimationName = (char*)malloc(30 * sizeof(char)); 
            sscanf(LineBuffer, "renderbuttonoffset %f %f", &Entity->RenderButtonOffset.x, &Entity->RenderButtonOffset.y);
        }
        else if(StartsWith(&LineBuffer[0], "staggercooldowntimer"))
        {
            sscanf(LineBuffer, "staggercooldowntimer %lf", &Entity->StaggerCooldownTimer.TimerMax);
            Entity->StaggerCooldownTimer.TimerHandle = -1;
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
        
        else if(StartsWith(&LineBuffer[0], "attacklowframeindex"))
        {
            sscanf(LineBuffer, "attacklowframeindex %d", &Entity->AttackLowFrameIndex);
        }
        else if(StartsWith(&LineBuffer[0], "attackhighframeindex"))
        {
            sscanf(LineBuffer, "attackhighframeindex %d", &Entity->AttackHighFrameIndex);
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
            Entity->HasWeapon = true;
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
        else if(StartsWith(&LineBuffer[0],"pointlight"))
        {
            light_source NewPointLight;
            
            sscanf(LineBuffer, "pointlight type %d active %d radius %f intensity %f color %f %f %f %f atten %f %f %f",
                   &NewPointLight.Type ,&NewPointLight.Active,
                   &NewPointLight.Pointlight.Radius, &NewPointLight.Pointlight.Intensity,&NewPointLight.Color.x,&NewPointLight.Color.y,&NewPointLight.Color.z,&NewPointLight.Color.w, &NewPointLight.Pointlight.ConstantAtten, &NewPointLight.Pointlight.LinearAtten, &NewPointLight.Pointlight.ExponentialAtten);
            NewPointLight.Pointlight.Position = Entity->Position;
            if(Entity->LightSourceHandle == -1)
            {
                Entity->LightSourceHandle = GameState->LightSourceCount;
                GameState->LightSources[GameState->LightSourceCount++] = NewPointLight;
            }
            else
            {
                GameState->LightSources[Entity->LightSourceHandle] = NewPointLight;
            }
            
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
    
    Entity->Enemy.HasLoot = true;
    Entity->Enemy.Loot.HealthPotions = 1;
    
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
            else if(StartsWith(&LineBuffer[0], "experience"))
            {
                sscanf(LineBuffer, "experience %d", &Entity->Enemy.Experience);
            }
            else if(StartsWith(&LineBuffer[0], "wanderingspeed"))
            {
                sscanf(LineBuffer, "wanderingspeed %f", &Entity->Enemy.WanderingSpeed);
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
            else if(StartsWith(&LineBuffer[0], "healthcountposition"))
            {
                sscanf(LineBuffer, "healthcountposition %f %f", &Entity->Enemy.HealthCountStart.x, &Entity->Enemy.HealthCountStart.y);
            }
            else if(StartsWith(&LineBuffer[0], "astarcooldowntimer"))
            {
                Entity->Enemy.AStarPath.AStarCooldownTimer.TimerHandle = -1;
                
                sscanf(LineBuffer, "astarcooldowntimer %lf", &Entity->Enemy.AStarPath.AStarCooldownTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0], "defendingtimer"))
            {
                Entity->Enemy.DefendingTimer.TimerHandle = -1;
                sscanf(LineBuffer, "defendingtimer %lf", &Entity->Enemy.DefendingTimer.TimerMax);
            }
            else if(StartsWith(&LineBuffer[0], "healthbar"))
            {
                Entity->Enemy.Healthbar = (entity_healthbar*)malloc(sizeof(entity_healthbar));
                sscanf(LineBuffer, "healthbar offset %f %f scale %f %f",&Entity->Enemy.Healthbar->Offset.x,
                       &Entity->Enemy.Healthbar->Offset.y,&Entity->Enemy.Healthbar->Scale.x,
                       &Entity->Enemy.Healthbar->Scale.y);
                ui_render_info RenderInfo = {};
                RenderInfo.TextureOffset = glm::vec2(256, 0);
                RenderInfo.FrameSize = glm::vec2(64, 16);
                RenderInfo.ShaderIndex = Shader_Spritesheet;
                Entity->Enemy.Healthbar->RenderInfo = RenderInfo;
            }
        }
    }
}

static void EnemyWander(game_state* GameState, entity* Entity)
{
    if(Entity->Enemy.WaypointCount > 0)
    {
        PlayAnimation(Entity, Concat(Entity->Name,"_walk"), GameState);
        
        auto CurrentWaypoint = glm::vec2(Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].X, Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].Y);
        
        auto Distance = glm::distance(CurrentWaypoint, glm::vec2(Entity->Position.x, Entity->Position.y - 0.5f));
        
        if(Distance < 0.01f)
        {
            if(Entity->Enemy.WanderingForward)
            {
                if(Entity->Enemy.WaypointIndex == Entity->Enemy.WaypointCount - 1)
                {
                    Entity->Enemy.WanderingForward = false;
                    Entity->Enemy.WaypointIndex--;
                }
                else
                {
                    Entity->Enemy.WaypointIndex++;
                }
            }
            else
            {
                if(Entity->Enemy.WaypointIndex == 0)
                {
                    Entity->Enemy.WanderingForward = true;
                    Entity->Enemy.WaypointIndex++;
                }
                else
                {
                    Entity->Enemy.WaypointIndex--;
                }
            }
        }
        
        CurrentWaypoint =  glm::vec2(Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].X, Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].Y);
        auto Direction = glm::normalize(CurrentWaypoint - glm::vec2(Entity->Position.x, Entity->Position.y - 0.5f));
        
        Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.WanderingSpeed, Direction.y * Entity->Enemy.WanderingSpeed);
    }
    
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Alerted;
        StartTimer(GameState,Entity->Enemy.Skeleton.AlertedTimer);
    }
}

AI_FUNC(SkeletonIdle)
{
    PlayAnimation(Entity, "skeleton_idle", GameState);
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Alerted;
        StartTimer(GameState,Entity->Enemy.Skeleton.AlertedTimer);
    }
    else
    {
        Enemy.AIState = AI_Wandering;
        PlayAnimation(Entity, "skeleton_walk", GameState);
    }
}

AI_FUNC(SkeletonAlerted)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    if(TimerDone(GameState,Entity->Enemy.Skeleton.AlertedTimer) && DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
        PlayAnimation(Entity, "skeleton_walk", GameState);
    }
}

AI_FUNC(SkeletonFollowing)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(!Player.Dead && Player.Active)
    {
        if(DistanceToPlayer > Entity->Enemy.MaxFollowDistance)
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            Enemy.AIState = AI_Wandering;
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
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
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

AI_FUNC(SkeletonDefending)
{}

AI_FUNC(SkeletonAttacking)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
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
        
        if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex - 2 &&Entity->AnimationInfo.FrameIndex < Entity->AttackHighFrameIndex && !Skeleton.IsAttacking && strcmp(Entity->CurrentAnimation->Name, "skeleton_idle") != 0)
        {
            StartTimer(GameState, Entity->AttackMoveTimer);
            
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
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
                Enemy.AIState = AI_Wandering;
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
    EnemyWander(GameState,Entity);
}

AI_FUNC(MinotaurIdle)
{
    PlayAnimation(Entity, "minotaur_idle", GameState);
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Alerted;
        PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurGrunt02);
        StartTimer(GameState, Entity->Enemy.Minotaur.AlertedTimer);
    }
    else
    {
        Enemy.AIState = AI_Wandering;
        PlayAnimation(Entity, "minotaur_walk", GameState);
    }
}

AI_FUNC(MinotaurAlerted)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    if(TimerDone(GameState,Entity->Enemy.Minotaur.AlertedTimer) && DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
        PlayAnimation(Entity, "minotaur_walk", GameState);
    }
}

AI_FUNC(MinotaurFollowing)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Minotaur = Entity->Enemy.Minotaur;
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(!Player.Dead && Player.Active)
    {
        if(DistanceToPlayer > Entity->Enemy.MaxFollowDistance)
        {
            PlayAnimation(Entity, "minotaur_walk", GameState);
            Enemy.AIState = AI_Wandering;
        }
        else if(DistanceToPlayer < Entity->Enemy.MinDistanceToPlayer)
        {
            PlayAnimation(Entity, "minotaur_idle", GameState);
            StartTimer(GameState, Minotaur.ChargingTimer);
            Enemy.AIState = AI_Charging;
            render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        }
        else if(DistanceToPlayer <= Entity->Enemy.SlowdownDistance)
        {
            PlayAnimation(Entity, "minotaur_walk", GameState);
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.CloseToPlayerSpeed, Direction.y * Entity->Enemy.CloseToPlayerSpeed);
        }
        else
        {
            PlayAnimation(Entity, "minotaur_walk", GameState);
            FindPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
            FollowPath(GameState, Entity, Player, DeltaTime, &Entity->Enemy.AStarPath);
        }
    }
    else
    {
        PlayAnimation(Entity, "minotaur_idle", GameState);
    }
}

AI_FUNC(MinotaurCharging)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Minotaur = Entity->Enemy.Minotaur;
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer >= Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
    }
    else if(TimerDone(GameState, Minotaur.ChargingTimer) && DistanceToPlayer <= Enemy.AttackDistance)
    {
        Enemy.AIState = AI_Attacking;
        PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurGrunt01);
        PlayAnimation(Entity, "minotaur_attack", GameState);
    }
    else
    {
        PlayAnimation(Entity, "minotaur_walk", GameState);
        glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
        Entity->Velocity = glm::vec2((Direction.x + 0.1f) * Entity->Enemy.CloseToPlayerSpeed, (Direction.y + 0.1f) * Entity->Enemy.CloseToPlayerSpeed);
    }
}

AI_FUNC(MinotaurDefending)
{
    auto& RenderEntity = GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
    
    if(!TimerDone(GameState, Entity->Enemy.DefendingTimer))
    {
        Entity->Invincible = true;
        RenderEntity.Color = glm::vec4(0.0f, 0.2f, 0.8f, 1.0f);
    }
    else
    {
        Entity->Invincible = false;
        RenderEntity.Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Entity->Enemy.AIState = AI_Attacking;
        
        PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurGrunt01);
        
        PlayAnimation(Entity, "minotaur_attack", GameState);
    }
}

AI_FUNC(MinotaurAttacking)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Minotaur = Entity->Enemy.Minotaur;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    Enemy.TimesHit = 0;
    
    if(Player.Dead)
    {
        Minotaur.IsAttacking = false;
        Enemy.AIState = AI_Idle;
    }
    else
    {
        if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            glm::vec2 Direction = Minotaur.LastAttackMoveDirection;
            Entity->Velocity = glm::vec2(Direction.x * Entity->AttackMoveSpeed, Direction.y * Entity->AttackMoveSpeed);
        }
        
        if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex - 2 
           && Entity->AnimationInfo.FrameIndex < Entity->AttackHighFrameIndex 
           && !Minotaur.IsAttacking 
           && strcmp(Entity->CurrentAnimation->Name, "minotaur_idle") != 0)
        {
            StartTimer(GameState, Entity->AttackMoveTimer);
            if(Entity->AttackCount == 0)
            {
                Minotaur.LastAttackMoveDirection = glm::normalize(Player.Position - Entity->Position);
            }
            
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
            {
                Minotaur.IsAttacking = true;
                
                Entity->AttackCount++;
                
                StartTimer(GameState, Minotaur.AttackCooldownTimer);
            }
            else
            {
                Minotaur.IsAttacking = false;
            }
        }
        else if(!Entity->AnimationInfo.Playing && strcmp(Entity->CurrentAnimation->Name, "minotaur_attack") == 0)
        {
            PlayAnimation(Entity, "minotaur_idle", GameState);
            
            if(Entity->AttackCount < Minotaur.MaxAttackStreak)
            {
                Minotaur.IsAttacking = false;
                Enemy.AIState = AI_Attacking;
                PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurGrunt01);
                PlayAnimation(Entity, "minotaur_attack", GameState);
            }
        }
        
        if(TimerDone(GameState, Minotaur.AttackCooldownTimer) && Entity->AttackCount == Minotaur.MaxAttackStreak)
        {
            Entity->AttackCount = 0;
            Minotaur.IsAttacking = false;
            
            if(DistanceToPlayer > Entity->Enemy.MinDistanceToPlayer)
            {
                Enemy.AIState = AI_Following;
            }
            else
            {
                Enemy.AIState = AI_Wandering;
            }
        }
    }
}

AI_FUNC(MinotaurHit)
{
    auto& Enemy = Entity->Enemy;
    if(!TimerDone(GameState, Entity->RecoilTimer))
    {
        Entity->Velocity = glm::vec2(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed, Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed);
    }
    
    if(!Entity->AnimationInfo.Playing)
    {
        Enemy.AIState = AI_Following;
        PlayAnimation(Entity, "minotaur_walk", GameState);
    }
}

AI_FUNC(MinotaurDying)
{
    Entity->IsKinematic = true;
    if(!Entity->AnimationInfo.Playing)
    {
        Entity->Dead = true;
        Entity->IsKinematic = true;
    }
}

AI_FUNC(MinotaurWandering)
{
    EnemyWander(GameState,Entity);
    
    if(Entity->Enemy.AIState == AI_Alerted || Entity->Enemy.AIState == AI_Following)
    {
        PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurGrunt02);
    }
}


AI_FUNC(BlobIdle)
{
    auto Player = GameState->Entities[0];
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
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

AI_FUNC(BlobDefending)
{}

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


static void LoadBonfireData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/bonfire.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    Entity->Type = Entity_Bonfire;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File, Entity, GameState, Handle != -1);
        
        if(Handle == -1)
            Entity->Position = glm::vec2(Position.x, Position.y);
        
        fclose(File);
    }
}

static void LoadSkeletonData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/skeleton.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    Entity->Enemy.Skeleton = {};
    Entity->AnimationInfo.FreezeFrame = false;
    Entity->Dead = false;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity, GameState, Handle != -1);
        LoadEnemyData(File,Entity, GameState);
        
        if(Handle == -1)
            Entity->Position = glm::vec2(Position.x, Position.y);
        
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
                Entity->Enemy.Skeleton.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(&LineBuffer[0],"chargingtimer"))
            {
                Entity->Enemy.Skeleton.ChargingTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"chargingtimer %lf",&Entity->Enemy.Skeleton.ChargingTimer.TimerMax);
                Entity->Enemy.Skeleton.ChargingTimer.Name = "Charging";
            }
            else if(StartsWith(&LineBuffer[0],"alertedtimer"))
            {
                Entity->Enemy.Skeleton.AlertedTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"alertedtimer %lf",&Entity->Enemy.Skeleton.AlertedTimer.TimerMax);
                Entity->Enemy.Skeleton.AlertedTimer.Name = "Alerted";
            }
        }
        fclose(File);
    }
    
    if(Entity)
    {
        AI_FUNCS(Skeleton);
    }
}

static void LoadMinotaurData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/minotaur.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    Entity->Enemy.Minotaur = {};
    Entity->AnimationInfo.FreezeFrame = false;
    Entity->Dead = false;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity, GameState, Handle != -1);
        LoadEnemyData(File,Entity, GameState);
        
        if(Handle == -1)
            Entity->Position = Position;
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "#"))
            {
                break;
            }
            else if(StartsWith(&LineBuffer[0],"attackcooldowntimer"))
            {
                Entity->Enemy.Minotaur.AttackCooldownTimer.TimerHandle = -1;
                sscanf(LineBuffer,"attackcooldowntimer %lf",&Entity->Enemy.Minotaur.AttackCooldownTimer.TimerMax);
                Entity->Enemy.Minotaur.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(&LineBuffer[0],"chargingtimer"))
            {
                Entity->Enemy.Minotaur.ChargingTimer.TimerHandle = -1;
                sscanf(LineBuffer,"chargingtimer %lf",&Entity->Enemy.Minotaur.ChargingTimer.TimerMax);
                Entity->Enemy.Minotaur.ChargingTimer.Name = "Charging";
            }
            else if(StartsWith(&LineBuffer[0],"alertedtimer"))
            {
                Entity->Enemy.Minotaur.AlertedTimer.TimerHandle = -1;
                sscanf(LineBuffer,"alertedtimer %lf",&Entity->Enemy.Minotaur.AlertedTimer.TimerMax);
                Entity->Enemy.Minotaur.AlertedTimer.Name = "Alerted";
            }
            else if(StartsWith(&LineBuffer[0],"maxattackstreak"))
            {
                Entity->Enemy.Minotaur.AlertedTimer.TimerHandle = -1;
                sscanf(LineBuffer,"maxattackstreak %d",&Entity->Enemy.Minotaur.MaxAttackStreak);
            }
        }
        fclose(File);
    }
    
    if(Entity)
    {
        AI_FUNCS(Minotaur);
    }
}

static void LoadBlobData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
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
                Entity->Enemy.Blob.PickupThrowTimer.Name = "Pickup Throw";
            }
            else if(StartsWith(&LineBuffer[0],"explodestarttimer"))
            {
                Entity->Enemy.Blob.ExplodeStartTimer.TimerHandle = -1;
                sscanf(LineBuffer,"explodestarttimer %lf",&Entity->Enemy.Blob.ExplodeStartTimer.TimerMax);
                Entity->Enemy.Blob.ExplodeStartTimer.Name = "Explode Start";
            }
            else if(StartsWith(&LineBuffer[0],"explodecountdowntimer"))
            {
                Entity->Enemy.Blob.ExplodeCountdownTimer.TimerHandle = -1;
                sscanf(LineBuffer,"explodecountdowntimer %lf",&Entity->Enemy.Blob.ExplodeCountdownTimer.TimerMax);
                Entity->Enemy.Blob.ExplodeCountdownTimer.Name = "ExplodeCountdown";
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

static void LoadPlayerData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
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
    Entity->Player.RenderCrosshair = false;
    
    if(Handle == -1)
    {
        if(GameState->CharacterData.HasCheckpoint)
        {
            Entity->Position = GameState->CharacterData.CurrentCheckpoint;
        }
        else
        {
            LoadBonfireData(GameState,-1,Position);
            GameState->CharacterData.CurrentCheckpoint = Position;
            GameState->CharacterData.HasCheckpoint = true;
        }
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
                Entity->Player.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(&LineBuffer[0], "lastattacktimer"))
            {
                sscanf(LineBuffer, "lastattacktimer %lf", &Entity->Player.LastAttackTimer.TimerMax);
                Entity->Player.LastAttackTimer.TimerHandle = -1;
                Entity->Player.LastAttackTimer.Name = "Last Attack";
            }
            else if(StartsWith(&LineBuffer[0], "pickupcooldowntimer"))
            {
                sscanf(LineBuffer, "pickupcooldowntimer %lf", &Entity->Player.PickupCooldownTimer.TimerMax);
                Entity->Player.PickupCooldownTimer.TimerHandle = -1;
                Entity->Player.PickupCooldownTimer.Name = "Pickup Cooldown";
            }
            else if(StartsWith(&LineBuffer[0], "dashtimer"))
            {
                sscanf(LineBuffer, "dashtimer %lf", &Entity->Player.DashTimer.TimerMax);
                Entity->Player.DashTimer.TimerHandle = -1;
                Entity->Player.DashTimer.Name = "Dash";
            }
            else if(StartsWith(&LineBuffer[0], "dashcooldowntimer"))
            {
                sscanf(LineBuffer, "dashcooldowntimer %lf", &Entity->Player.DashCooldownTimer.TimerMax);
                Entity->Player.DashCooldownTimer.TimerHandle = -1;
                Entity->Player.DashCooldownTimer.Name = "Dash Cooldown";
            }
            else if(StartsWith(&LineBuffer[0], "dashspeed"))
            {
                sscanf(LineBuffer, "dashspeed %f", &Entity->Player.DashSpeed);
            }
            else if(StartsWith(&LineBuffer[0], "stamina "))
            {
                sscanf(LineBuffer, "stamina %hd", &Entity->Player.FullStamina);
                Entity->Player.Stamina = Entity->Player.FullStamina;
            }
            else if(StartsWith(&LineBuffer[0], "staminagaintimer "))
            {
                sscanf(LineBuffer, "staminagaintimer %lf", &Entity->Player.StaminaGainTimer.TimerMax);
                Entity->Player.StaminaGainTimer.TimerHandle = -1;
                Entity->Player.StaminaGainTimer.Name = "Stamina Gain";
            }
            else if(StartsWith(&LineBuffer[0], "hitstaminacost"))
            {
                sscanf(LineBuffer, "hitstaminacost %hd", &Entity->Player.HitStaminaCost);
            }
            else if(StartsWith(&LineBuffer[0], "rollstaminacost"))
            {
                sscanf(LineBuffer, "rollstaminacost %hd", &Entity->Player.RollStaminaCost);
            }
            else if(StartsWith(&LineBuffer[0], "attackstaminacost"))
            {
                sscanf(LineBuffer, "attackstaminacost %hd", &Entity->Player.AttackStaminaCost);
            }
            else if(StartsWith(&LineBuffer[0], "mindiffstamina"))
            {
                sscanf(LineBuffer, "mindiffstamina %hd", &Entity->Player.MinDiffStamina);
            }
            else if(StartsWith(&LineBuffer[0], "staminagaincooldowntimer"))
            {
                Entity->Player.StaminaGainCooldownTimer.TimerMax = 0;
                sscanf(LineBuffer, "staminagaincooldowntimer %lf", &Entity->Player.StaminaGainCooldownTimer.TimerMax);
                Entity->Player.StaminaGainCooldownTimer.TimerHandle = -1;
                Entity->Player.StaminaGainCooldownTimer.Name = "Stamina Gain Cooldown";
            }
            else if(StartsWith(&LineBuffer[0], "staminagaintimerfast"))
            {
                sscanf(LineBuffer, "staminagaintimerfast %lf", &Entity->Player.StaminaGainTimerFast);
            }
            else if(StartsWith(&LineBuffer[0], "staminagaintimerslow"))
            {
                sscanf(LineBuffer, "staminagaintimerslow %lf", &Entity->Player.StaminaGainTimerSlow);
            }
        }
        fclose(File);
        
        if(GameState->CharacterData.Level != 0)
        {
            Entity->Player.Level = GameState->CharacterData.Level;
            if(GameState->CharacterData.HasCheckpoint)
            {
                Entity->Position = GameState->CharacterData.CurrentCheckpoint;
            }
            Entity->Health = GameState->LastCharacterData.Health;
            Entity->FullHealth = GameState->LastCharacterData.Health;
            Entity->Player.Stamina = GameState->LastCharacterData.Stamina;
            Entity->Player.FullStamina = GameState->LastCharacterData.Stamina;
            Entity->Weapon.Damage = GameState->LastCharacterData.Strength;
        }
    }
}

static void DecreaseStamina(entity* Entity, game_state* GameState, i16 Cost) 
{
    i16 NewStamina = Max(0, Entity->Player.Stamina - Cost);
    Entity->Player.StaminaLost = Entity->Player.Stamina - NewStamina;
    Entity->Player.Stamina = NewStamina;
    StartTimer(GameState, Entity->Player.StaminaDecreaseTimer);
    StartTimer(GameState,Entity->Player.StaminaGainCooldownTimer);
}

void Hit(game_state* GameState, entity* ByEntity, entity* HitEntity)
{
    if(HitEntity->HitAttackCountId != ByEntity->AttackCount)
    {
        HitEntity->HitAttackCountId = ByEntity->AttackCount;
        StartTimer(GameState, HitEntity->HitAttackCountIdResetTimer);
        
        if(HitEntity->Invincible)
        {
            if(ByEntity->Type == Entity_Player)
            {
                PlaySoundEffect(GameState, &GameState->SoundManager.ShieldImpact);
                StartTimer(GameState, ByEntity->StaggerCooldownTimer);
            }
        }
        else
        {
            if(TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
                StartTimer(GameState, GameState->GameCamera.ScreenShakeTimer);
            
            StartTimer(GameState, HitEntity->StaggerCooldownTimer);
            PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit02);
            HitEntity->HitRecoilDirection = glm::normalize(HitEntity->Position - ByEntity->Position);
            
            i16 Damage = ByEntity->Weapon.Damage > HitEntity->Health ? (i16)HitEntity->Health : (i16)ByEntity->Weapon.Damage;
            HitEntity->Health -= Damage;
            
            HitEntity->Hit = true;
            HitEntity->HitAttackCountId = ByEntity->AttackCount;
            HitEntity->HitFlickerFramesLeft = HitEntity->HitFlickerFrameMax;
            
            StartTimer(GameState, HitEntity->HealthDecreaseTimer);
            HitEntity->HealthLost = Damage;
            
            StartTimer(GameState, HitEntity->HitFlickerTimer);
            
            if(HitEntity->Type == Entity_Enemy)
            {
                if(HitEntity->Health <= 0)
                {
                    GameState->Entities[0].Player.Experience += HitEntity->Enemy.Experience;
                }
                
                HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Visible = true;
                HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Position = HitEntity->Enemy.HealthCountStart;
                sprintf(HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Count, "%d", HitEntity->HealthLost);
                HitEntity->Enemy.HealthCountIndex++;
                if(HitEntity->Enemy.HealthCountIndex == 10)
                    HitEntity->Enemy.HealthCountIndex = 0;
            }
            
            if(HitEntity->Type == Entity_Player)
            {
                PlayAnimation(HitEntity, "swordsman_hit", GameState);
                DecreaseStamina(HitEntity,GameState,HitEntity->Player.HitStaminaCost);
                StartFade(GameState->GameCamera, Fading_OutIn, 4.0f, glm::vec3(1, 0, 0), 0.0f, 0.4f);
            }
        }
    }
}

void UpdatePlayer(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    if(Entity->Player.LastMilestone == 0 && Entity->Player.Experience >= GameState->StatData[Entity->Player.Level].Milestones[0].MilestonePoint)
    {
        Entity->Player.LastMilestone++;
        GameState->StatGainModeOn = true;
    }
    else if(Entity->Player.LastMilestone == 1 && Entity->Player.Experience >= GameState->StatData[Entity->Player.Level].Milestones[1].MilestonePoint)
    {
        Entity->Player.LastMilestone++;
        GameState->StatGainModeOn = true;
    }
    
    if(Entity->Player.Experience >= GameState->StatData[Entity->Player.Level].ExperienceForLevel)
    {
        Entity->Player.Experience -= GameState->StatData[Entity->Player.Level].ExperienceForLevel;
        Entity->Player.Level++;
        Entity->Player.LastMilestone = 0;
    }
    
    r32 UsedWalkingSpeed = Entity->Player.WalkingSpeed;
    if(!TimerDone(GameState, Entity->StaggerCooldownTimer))
    {
        UsedWalkingSpeed = UsedWalkingSpeed / 2;
    }
    
    if(TimerDone(GameState,Entity->Player.StaminaGainCooldownTimer))
    {
        Entity->Player.StaminaGainTimer.TimerMax =Entity->Player.StaminaGainTimerFast;
    }
    else
    {
        Entity->Player.StaminaGainTimer.TimerMax = Entity->Player.StaminaGainTimerSlow;
    }
    
    if(Entity->Player.Stamina != Entity->Player.FullStamina)
    {
        if(TimerDone(GameState, Entity->Player.StaminaGainTimer))
        {
            Entity->Player.Stamina += 1;
            
            if(Entity->Player.Stamina < Entity->Player.FullStamina)
            {
                StartTimer(GameState, Entity->Player.StaminaGainTimer);
            }
            
            Entity->Player.Stamina = Min(Entity->Player.Stamina,Entity->Player.FullStamina);
        }
    }
    
    if(Entity->Hit)
        Entity->Player.IsAttacking = false;
    
    b32 UsingController = GameState->InputController.ControllerPresent;
    
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
        
        if(!TimerDone(GameState, Entity->StaggerCooldownTimer))
        {
            Entity->Velocity = glm::vec2(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed * DeltaTime, Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed * DeltaTime);
        }
        
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
                r32 InputX = GetInputX(GameState);
                r32 InputY = GetInputY(GameState);
                
                Entity->Velocity.x = InputX * UsedWalkingSpeed * (r32)DeltaTime;
                Entity->Velocity.y = InputY * UsedWalkingSpeed * (r32)DeltaTime;
                
                // @Cleanup: This section really needs a cleanup
                if(Entity->Player.TargetedEnemyHandle != -1)
                {
                    b32 Moving = Entity->Velocity.x != 0 || Entity->Velocity.y != 0;
                    
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
                        }
                    }
                    else
                    {
                        Entity->LookDirection = Right;
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
                r32 AttackMoveSpeed = Entity->AttackMoveSpeed;
                
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
                r32 ThrowingDir = Entity->IsFlipped ? -1.0f : 1.0f;
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
        
        if(!Entity->Player.Pickup && !Entity->Player.IsAttacking  && TimerDone(GameState, Entity->Player.DashTimer) && GetActionButtonDown(Action_Dash, GameState)  && Entity->Player.Stamina >= Entity->Player.RollStaminaCost - Entity->Player.MinDiffStamina)
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
        
        Entity->Invincible = Entity->Player.IsDashing;
        
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
        if(!Entity->Player.Pickup && TimerDone(GameState, Entity->Player.AttackCooldownTimer) &&
           TimerDone(GameState, Entity->StaggerCooldownTimer)
           && !Entity->Player.IsAttacking && (GetActionButtonDown(Action_Attack, GameState) || GetJoystickKeyDown(Joystick_3, GameState)))
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
                    i32 Closest = -1;
                    r32 ClosestDistance = 2000.0f;
                    
                    for(u32 Index = 0; Index < GameState->EntityCount; Index++)
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
                i32 NextTarget = -1;
                
                if(Entity->Player.TargetedEnemyHandle + 1 < GameState->EntityCount)
                {
                    for(u32 Index = Entity->Player.TargetedEnemyHandle + 1; Index < GameState->EntityCount; Index++)
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
                    for(i32 Index = 0; Index < Entity->Player.TargetedEnemyHandle - 1; Index++)
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

void UpdateWeapon(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    b32 IsAttacking = false;
    
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
        for(i32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            if( (Entity->Type == Entity_Player && CollisionInfo.Other[Index]->Type == Entity_Enemy && CollisionInfo.Other[Index]->Enemy.AIState != AI_Hit && CollisionInfo.Other[Index]->Enemy.AIState != AI_Dying) ||
               (Entity->Type == Entity_Enemy && CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing && !CollisionInfo.Other[Index]->Hit && TimerDone(GameState, CollisionInfo.Other[Index]->StaggerCooldownTimer)))
            {
                if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
                {
                    Hit(GameState, Entity, CollisionInfo.Other[Index]);
                }
            }
        }
    }
}

void UpdateAI(entity* Entity, game_state* GameState, r64 DeltaTime)
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
        case AI_Defending:
        {
            Entity->Enemy.Defending(Entity,GameState,DeltaTime);
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



void UpdateBlob(entity* Entity, game_state* GameState, r64 DeltaTime)
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
            for(i32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
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
    
    Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
    Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
}

void UpdateSkeleton(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    auto& Player = GameState->Entities[0];
    Entity->RenderButtonHint = Entity->Dead && glm::distance(Player.Position, Entity->Position) < 1.5f;
    
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
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
        
        glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
        
        if(Entity->Enemy.AIState != AI_Attacking)
        {
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
        }
        
        if(Entity->Velocity.x == 0.0f && Entity->Velocity.y == 0.0f)
            Entity->IsFlipped = Direction.x < 0;
        else
            Entity->IsFlipped = Entity->Velocity.x < 0;
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = glm::vec2(0,0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

void UpdateMinotaur(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Minotaur = Entity->Enemy.Minotaur;
    
    auto& Player = GameState->Entities[0];
    Entity->RenderButtonHint = Entity->Dead && glm::distance(Player.Position, Entity->Position) < 1.5f;
    
    if(Entity->Active && !Entity->Dead)
    {
        entity& Player = GameState->Entities[GameState->PlayerIndex];
        
        if(Entity->Hit)
        {
            PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurHit);
            Enemy.TimesHit++;
            
            if(Entity->Health <= 0)
            {
                PlaySoundEffect(GameState, &GameState->SoundManager.MinotaurDeath);
                PlayAnimation(Entity, "minotaur_death", GameState);
                Entity->AnimationInfo.FreezeFrame = true;
                Enemy.AIState = AI_Dying;
            }
            else if(strcmp(Entity->CurrentAnimation->Name, "minotaur_attack") != 0 && Enemy.AIState != AI_Dying)
            {
                PlayAnimation(Entity, "minotaur_detected", GameState);
                Enemy.AIState = AI_Hit;
                Minotaur.IsAttacking = false;
                Entity->HitRecoilDirection = glm::normalize(Entity->Position - Player.Position);
                StartTimer(GameState, Entity->RecoilTimer);
            }
        }
        
        Entity->Velocity = glm::vec2(0,0);
        
        if(Enemy.TimesHit == 2 && Entity->Health > 0)
        {
            Enemy.AIState = AI_Defending;
            Enemy.TimesHit = 0;
            StartTimer(GameState, Enemy.DefendingTimer);
        }
        
        UpdateAI(Entity,GameState,DeltaTime);
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
        
        if(Entity->Enemy.AIState != AI_Attacking && !Entity->Enemy.Minotaur.IsAttacking)
        {
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            
            if(Abs(Direction.x) < 0.6f)
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
            
            Entity->IsFlipped = Direction.x > 0;
        }
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = glm::vec2(0,0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

void UpdateBarrel(entity* Entity, game_state* GameState, r64 DeltaTime)
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
        
        b32 HasHitEnemy = false;
        
        if(Entity->Velocity.x != 0 || Entity->Velocity.y != 0)
        {
            for(i32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
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
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
    }
}

static void UpdateStaticEntity(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
}

void UpdateGeneral(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    if(Entity->LightSourceHandle != -1)
    {
        GameState->LightSources[Entity->LightSourceHandle].Pointlight.Position = Entity->Position;
    }
    
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
        for(i32 HealthCountIndex = 0; HealthCountIndex < 10; HealthCountIndex++)
        {
            auto& HealthCount = Entity->Enemy.HealthCounts[HealthCountIndex];
            
            if(HealthCount.Visible)
            {
                HealthCount.Position.y += 100 * (r32)DeltaTime;
                
                if(HealthCount.Position.y - Entity->Enemy.HealthCountStart.y >= 70)
                {
                    HealthCount.Visible = false;
                }
            }
        }
    }
}

void UpdateEntities(game_state* GameState, r64 DeltaTime)
{
    for(u32 EntityIndex = 0;
        EntityIndex < GameState->EntityCount;
        EntityIndex++)
    {
        entity* Entity = &GameState->Entities[EntityIndex];
        
        if(Entity->Active)
        {
            UpdateGeneral(Entity, GameState, DeltaTime);
            
            switch(Entity->Type)
            {
                case Entity_Player: 
                {
                    if(!GameState->GodModeOn)
                    {
                        UpdatePlayer(Entity, GameState, DeltaTime);
                        UpdateWeapon(Entity, GameState, DeltaTime);
                    }
                }
                break;
                case Entity_Enemy:
                {
                    
                    switch(Entity->Enemy.EnemyType)
                    {
                        case Enemy_Skeleton:
                        {
                            UpdateSkeleton(Entity, GameState, DeltaTime);
                            
                            if(!Entity->Dead)
                            {
                                UpdateWeapon(Entity, GameState, DeltaTime);
                            }
                        }
                        break;
                        case Enemy_Minotaur:
                        {
                            UpdateMinotaur(Entity, GameState, DeltaTime);
                            
                            if(!Entity->Dead)
                            {
                                UpdateWeapon(Entity, GameState, DeltaTime);
                            }
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
                case Entity_Bonfire:
                {
                    UpdateStaticEntity(Entity, GameState, DeltaTime);
                }
                break;
            }
            
            if(Entity->Active && Entity->CurrentAnimation && Entity->AnimationInfo.Playing)
                TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
        }
    }
}
