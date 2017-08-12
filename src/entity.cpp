static inline b32 TileIsOccupied(i32 X, i32 Y, game_state* GameState)
{
    return GameState->EntityTilePositions[X][Y] > 0;
}

static inline void AddTimer(const char* LineBuffer, const char* Format, char* Name, timer* Timer)
{
    sscanf(LineBuffer, Format, &Timer->TimerMax);
    Timer->TimerHandle = -1;
    Timer->Name = Name;
}

static inline void PrintEntityInfo(const entity& Entity)
{
    DEBUG_PRINT("Entity: Name %s, position x %f y %f, rotation x %f y %f z %f\n", Entity.Name, Entity.Position.x, Entity.Position.y, Entity.Rotation.x, Entity.Rotation.y, Entity.Rotation.z);
}

static i32 LoadPointlight(game_state* GameState, math::rgba Color = math::v4(), r32 Intensity = 0.0f, r32 ConstantAtten = 0.0f, r32 LinearAtten = 0.0f, r32 ExponentialAtten = 0.0f,math::v3 InitPosition = math::v3(), b32 ShouldGlow = false, r64 GlowTimerMax = 0.0f, r32 GlowIncrease = 0.0f, r32 EmissionIntensity = 0.0f)
{
    light_source LightSource;
    
    LightSource.Type = Light_Pointlight;
    LightSource.Active = true;
    LightSource.Color = Color;
    
    if(ShouldGlow)
    {
        LightSource.Pointlight.GlowTimer.TimerHandle = -1;
        LightSource.Pointlight.GlowTimer.TimerMax = GlowTimerMax;
        LightSource.Pointlight.GlowIncrease = GlowIncrease;
    }
    
    LightSource.Pointlight.Intensity = Intensity;
    LightSource.Pointlight.Position = InitPosition;
    LightSource.Pointlight.ConstantAtten = ConstantAtten;
    LightSource.Pointlight.LinearAtten = LinearAtten;
    LightSource.Pointlight.ExponentialAtten = ExponentialAtten;
    LightSource.Pointlight.EmissionIntensity = EmissionIntensity;
    GameState->LightSources[GameState->LightSourceCount++] = LightSource;
    return GameState->LightSourceCount - 1;
}

static i32 LoadLight(game_state* GameState, char* LineBuffer, math::v3 InitPosition = math::v3(), i32 Handle = -1)
{
    b32 ShouldGlow = false;
    light_source LightSource;
    
    if(StartsWith(LineBuffer, "pointlight"))
    {
        r64 GlowTimerMax = 0.0;
        r32 GlowIncrease;
        sscanf(LineBuffer, "pointlight type %d active %d offset %f %f intensity %f color %f %f %f %f atten %f %f %f emissionintensity %f shouldglow %d glowtimer %lf glowincrease %f",
               &LightSource.Type, &LightSource.Active, &LightSource.Pointlight.Offset.x, &LightSource.Pointlight.Offset.y, &LightSource.Pointlight.Intensity, &LightSource.Color.x, &LightSource.Color.y, &LightSource.Color.z, &LightSource.Color.w, &LightSource.Pointlight.ConstantAtten, &LightSource.Pointlight.LinearAtten, &LightSource.Pointlight.ExponentialAtten, &LightSource.Pointlight.EmissionIntensity, &ShouldGlow, &GlowTimerMax, &GlowIncrease);
        
        if(ShouldGlow)
        {
            LightSource.Pointlight.GlowTimer.TimerHandle = -1;
            LightSource.Pointlight.GlowTimer.TimerMax = GlowTimerMax;
            LightSource.Pointlight.GlowIncrease = GlowIncrease;
            LightSource.Pointlight.IncreasingGlow = true;
        }
    }
    else if(StartsWith(LineBuffer, "ambient"))
    {
        sscanf(LineBuffer, "ambient type %d active %d intensity %f color %f %f %f %f",&LightSource.Type, &LightSource.Active, &LightSource.Ambient.Intensity, &LightSource.Color.x, &LightSource.Color.y, &LightSource.Color.z, &LightSource.Color.w);
    }
    
    if(Handle != -1)
    {
        GameState->LightSources[Handle] = LightSource;
        if(ShouldGlow)
        {
            StartTimer(GameState,GameState->LightSources[Handle].Pointlight.GlowTimer);
        }
        return Handle;
    }
    else
    {
        GameState->LightSources[GameState->LightSourceCount++] = LightSource;
        if(ShouldGlow)
        {
            StartTimer(GameState,GameState->LightSources[GameState->LightSourceCount - 1].Pointlight.GlowTimer);
        }
        return GameState->LightSourceCount - 1;
    }
}

static void DeleteEntity(game_state* GameState, u32 EntityIndex)
{
    if(GameState->EntityCount == 0 || GameState->RenderEntityCount == 0)
        return;
    
    entity* Entity = &GameState->Entities[EntityIndex];
    
    u32 RenderEntityHandle = Entity->RenderEntityHandle;
    
    for(i32 RenderIndex = RenderEntityHandle; RenderIndex < GameState->RenderEntityCount; RenderIndex++)
    {
        GameState->RenderEntities[RenderIndex] = GameState->RenderEntities[RenderIndex + 1];
        
        if(GameState->RenderEntities[RenderIndex].Entity)
        {
            GameState->RenderEntities[RenderIndex].Entity->RenderEntityHandle = RenderIndex;
        }
    }
    
    GameState->RenderEntityCount = Max(GameState->RenderEntityCount - 1, 0);
    
    if(GameState->EditorState.SelectedEntity)
        GameState->EditorState.SelectedEntity = 0;
    
    for(u32 Index = EntityIndex; Index < (u32)GameState->EntityCount - 1; Index++)
    {
        GameState->Entities[Index] = GameState->Entities[Index + 1];
        GameState->Entities[Index].EntityIndex = Index;
        GameState->RenderEntities[GameState->Entities[Index].RenderEntityHandle].Entity = &GameState->Entities[Index];
    }
    
    GameState->EntityCount--;
}

static void DecreaseStamina(entity* Entity, game_state* GameState, i32 Cost) 
{
    i32 NewStamina = Max(0, Entity->Player.Stamina - Cost);
    Entity->Player.StaminaLost = Entity->Player.Stamina - NewStamina;
    Entity->Player.Stamina = NewStamina;
    StartTimer(GameState, Entity->Player.StaminaDecreaseTimer);
    StartTimer(GameState,Entity->Player.StaminaGainCooldownTimer);
}

static void Hit(game_state* GameState, renderer& Renderer, sound_queue* SoundQueue, entity* ByEntity, entity* HitEntity)
{
    if(HitEntity->HitAttackCountId != ByEntity->AttackCount)
    {
        HitEntity->HitAttackCountId = ByEntity->AttackCount;
        StartTimer(GameState, HitEntity->HitAttackCountIdResetTimer);
        
        if(HitEntity->Invincible)
        {
            if(ByEntity->Type == Entity_Player)
            {
                PLAY_SOUND(ShieldImpact);
                StartTimer(GameState, ByEntity->StaggerCooldownTimer);
            }
        }
        else
        {
            //if(TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
            //StartTimer(GameState, GameState->GameCamera.ScreenShakeTimer);
            
            //StartTimer(GameState, HitEntity->StaggerCooldownTimer);
            
            i32 ran = rand() % 2;
            
            if(ran == 0)
            {
                PLAY_SOUND(Splash01, 0.95f);
            }
            else
            {
                PLAY_SOUND(Splash01, 0.85f);
            }
            
            HitEntity->HitRecoilDirection = math::Normalize(HitEntity->Position - ByEntity->Position);
            
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
                    GameState->Entities[0].Player.Will += HitEntity->Enemy.Will;
                }
                
                HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Visible = true;
                HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Position = HitEntity->Enemy.HealthCountStart;
                
                memset(HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Count, 0, 20 * sizeof(char));
                
                sprintf(HitEntity->Enemy.HealthCounts[HitEntity->Enemy.HealthCountIndex].Count, "%d", HitEntity->HealthLost);
                HitEntity->Enemy.HealthCountIndex++;
                if(HitEntity->Enemy.HealthCountIndex == 10)
                    HitEntity->Enemy.HealthCountIndex = 0;
            }
            
            if(HitEntity->Type == Entity_Player)
            {
                PlayAnimation(HitEntity, "swordsman_hit", GameState);
                DecreaseStamina(HitEntity,GameState,HitEntity->Player.HitStaminaCost);
                StartFade(Renderer.Cameras[GameState->GameCameraHandle], Fading_OutIn, 4.0f, math::v3(1, 0, 0), 0.0f, 0.4f);
            }
        }
    }
}

static void LoadEntityData(FILE* File, entity* Entity, game_state* GameState, b32 IsReload = false)
{
    if(!IsReload)
    {
        Entity->EntityIndex = GameState->EntityCount++;
        
        render_entity* RenderEntity = &GameState->RenderEntities[GameState->RenderEntityCount];
        RenderEntity->Shader = Shader_Spritesheet;
        RenderEntity->Rendered = true;
        RenderEntity->Background = false;
        RenderEntity->RenderType = Render_Type_Entity;
        RenderEntity->Entity = &*Entity;
        Entity->RenderEntityHandle = GameState->RenderEntityCount++;
        RenderEntity->Color = math::v4(1, 1, 1, 1);
        
        Entity->LookDirection = South;
        Entity->HitFlickerTimer.TimerHandle = -1;
        Entity->HitFlickerTimer.TimerMax = 0.05f;
        Entity->HitFlickerTimer.Name = "Hit Flicker";
        Entity->HitFlickerFramesLeft = 0;
        Entity->HitFlickerFrameMax = 6;
        
        Entity->RenderButtonOffset = math::v2(0.5f,1.5f);
        Entity->RenderButtonHint = false;
        
        Entity->HitAttackCountIdResetTimer.TimerHandle = -1;
        Entity->HitAttackCountIdResetTimer.TimerMax = 1.0f;
        Entity->HitAttackCountIdResetTimer.Name = "Hit Attack Count Id Reset";
        
        Entity->HealthDecreaseTimer.TimerMax = 0.8;
        Entity->HealthDecreaseTimer.TimerHandle = -1;
        Entity->HealthDecreaseTimer.Name = "Health Decrease";
        Entity->Dead = false;
        Entity->Center = math::v3(0.5, 0.5, 0.5);
        Entity->CurrentAnimation = 0;
        Entity->Active = true;
        Entity->IsKinematic = false;
        Entity->IsColliding = false;
        Entity->IsStatic = false;
        Entity->HasHitTrigger = false;
        Entity->LightSourceHandle = -1;
        Entity->Hit = false;
        Entity->Invincible = false;
        Entity->Health = -1;
        Entity->HitAttackCountId = -1;
        Entity->HasWeapon = false;
        Entity->IsTemporary = false;
        Entity->ShowAttackTiles = false;
        Entity->AnimationInfo.FreezeFrame = false;
        
        Entity->CurrentTile.x = 0;
        Entity->CurrentTile.y = 0;
        
        hit_tile_extents HitExtents;
        HitExtents.StartX = 0;
        HitExtents.EndX = 3;
        HitExtents.StartY = 0;
        HitExtents.EndY = 3;
        Entity->HitExtents[North] = HitExtents;
        
        HitExtents.StartX = 0;
        HitExtents.EndX = 3;
        HitExtents.StartY = -1;
        HitExtents.EndY = 2;
        Entity->HitExtents[NorthEast] = HitExtents;
        
        HitExtents.StartX = 0;
        HitExtents.EndX = 3;
        HitExtents.StartY = -2;
        HitExtents.EndY = 1;
        Entity->HitExtents[East] = HitExtents;
        
        HitExtents.StartX = -1;
        HitExtents.EndX = 2;
        HitExtents.StartY = -2;
        HitExtents.EndY = 1;
        Entity->HitExtents[SouthEast] = HitExtents;
        
        HitExtents.StartX = -2;
        HitExtents.EndX = 1;
        HitExtents.StartY = -2;
        HitExtents.EndY = 1;
        Entity->HitExtents[South] = HitExtents;
        
        HitExtents.StartX = -2;
        HitExtents.EndX = 1;
        HitExtents.StartY = -1;
        HitExtents.EndY = 2;
        Entity->HitExtents[SouthWest] = HitExtents;
        
        HitExtents.StartX = -2;
        HitExtents.EndX = 1;
        HitExtents.StartY = 0;
        HitExtents.EndY = 3;
        Entity->HitExtents[West] = HitExtents;
        
        HitExtents.StartX = -1;
        HitExtents.EndX = 2;
        HitExtents.StartY = 0;
        HitExtents.EndY = 3;
        Entity->HitExtents[NorthWest] = HitExtents;
    }
    else
    {
        Entity->Active = true;
    }
    
    Entity->AttackCount = 0;
    
    char LineBuffer[255];
    
    while(fgets(LineBuffer, 255, File))
    {
        if(StartsWith(LineBuffer, "#"))
        {
            break;
        }
        else if(StartsWith(LineBuffer, "name"))
        {
            Entity->Name = (char*)malloc(30 * sizeof(char));
            sscanf(LineBuffer, "name %s", Entity->Name);
        }
        else if(StartsWith(LineBuffer, "active"))
        {
            b32 Active;
            sscanf(LineBuffer, "active %d", &Active);
            Entity->Active = Active;
        }
        else if(StartsWith(LineBuffer, "layer"))
        {
            char* LayerName = (char*)malloc(30 * sizeof(char));
            sscanf(LineBuffer, "layer %s", LayerName);
            
            if(strcmp(LayerName, "Layer_Player") == 0)
            {
                Entity->Layer = Layer_Player;
            }
            else if(strcmp(LayerName, "Layer_Enemy") == 0)
            {
                Entity->Layer = Layer_Enemy;
            }
            
            free(LayerName);
        }
        else if(StartsWith(LineBuffer, "iskinematic"))
        {
            b32 IsKinematic;
            sscanf(LineBuffer, "iskinematic %d", &IsKinematic);
            Entity->IsKinematic = IsKinematic;
        }
        else if(StartsWith(LineBuffer, "scale"))
        {
            sscanf(LineBuffer, "scale %f", &Entity->Scale);
        }
        else if(StartsWith(LineBuffer, "center"))
        {
            sscanf(LineBuffer, "center %f %f", &Entity->Center.x, &Entity->Center.y);
        }
        else if(StartsWith(LineBuffer, "health"))
        {
            sscanf(LineBuffer, "health %d", &Entity->FullHealth);
            Entity->Health = Entity->FullHealth;
        }
        else if(StartsWith(LineBuffer, "animation"))
        {
            char* AnimationName = (char*)malloc(30 * sizeof(char)); 
            sscanf(LineBuffer, "animation %s", AnimationName);
            PlayAnimation(Entity, AnimationName, GameState);
            free(AnimationName);
        }
        else if(StartsWith(LineBuffer, "renderbuttonoffset"))
        {
            char* AnimationName = (char*)malloc(30 * sizeof(char)); 
            sscanf(LineBuffer, "renderbuttonoffset %f %f", &Entity->RenderButtonOffset.x, &Entity->RenderButtonOffset.y);
        }
        else if(StartsWith(LineBuffer, "staggercooldowntimer"))
        {
            sscanf(LineBuffer, "staggercooldowntimer %lf", &Entity->StaggerCooldownTimer.TimerMax);
            Entity->StaggerCooldownTimer.TimerHandle = -1;
        }
        else if(StartsWith(LineBuffer, "collider"))
        {
            sscanf(LineBuffer, "collider %f %f %f %f %d", &Entity->CollisionAABB.Offset.x, &Entity->CollisionAABB.Offset.y, &Entity->CollisionAABB.Extents.x, &Entity->CollisionAABB.Extents.y, &Entity->CollisionAABB.IsTrigger);
        }
        else if(StartsWith(LineBuffer, "hashittrigger"))
        {
            sscanf(LineBuffer, "hashittrigger %d", &Entity->HasHitTrigger);
        }
        else if(StartsWith(LineBuffer, "hittrigger"))
        {
            sscanf(LineBuffer, "hittrigger %f %f %f %f %d", &Entity->HitTrigger.Offset.x, &Entity->HitTrigger.Offset.y, &Entity->HitTrigger.Extents.x, &Entity->HitTrigger.Extents.y, &Entity->HitTrigger.IsTrigger);
        }
        else if(StartsWith(LineBuffer, "hitrecoilspeed"))
        {
            sscanf(LineBuffer, "hitrecoilspeed %f", &Entity->HitRecoilSpeed);
        }
        else if(StartsWith(LineBuffer, "recoiltimer"))
        {
            Entity->RecoilTimer.TimerHandle = -1;
            sscanf(LineBuffer,"recoiltimer %lf",&Entity->RecoilTimer.TimerMax);
        }
        else if(StartsWith(LineBuffer, "attackmovetimer"))
        {
            sscanf(LineBuffer, "attackmovetimer %lf", &Entity->AttackMoveTimer.TimerMax);
            Entity->AttackMoveTimer.TimerHandle = -1;
        }
        else if(StartsWith(LineBuffer, "attackmovespeed"))
        {
            sscanf(LineBuffer, "attackmovespeed %f", &Entity->AttackMoveSpeed);
        }
        
        else if(StartsWith(LineBuffer, "attacklowframeindex"))
        {
            sscanf(LineBuffer, "attacklowframeindex %d", &Entity->AttackLowFrameIndex);
        }
        else if(StartsWith(LineBuffer, "attackhighframeindex"))
        {
            sscanf(LineBuffer, "attackhighframeindex %d", &Entity->AttackHighFrameIndex);
        }
        else if(StartsWith(LineBuffer, "weaponscale"))
        {
            sscanf(LineBuffer, "weaponscale %f %f", &Entity->Weapon.Scale.x, &Entity->Weapon.Scale.y);
        }
        else if(StartsWith(LineBuffer, "weaponcollider"))
        {
            sscanf(LineBuffer, "weaponcollider %f %f %f %f %d", &Entity->Weapon.CollisionAABB.Offset.x, &Entity->Weapon.CollisionAABB.Offset.y, &Entity->Weapon.CollisionAABB.Extents.x, &Entity->Weapon.CollisionAABB.Extents.y, &Entity->Weapon.CollisionAABB.IsTrigger);
        }
        else if(StartsWith(LineBuffer, "weaponinfo"))
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
            Entity->Weapon.Layer = Entity->Layer;
        }
        else if(StartsWith(LineBuffer,"pointlight"))
        {
            Entity->LightSourceHandle = LoadLight(GameState, LineBuffer, Entity->Position, Entity->LightSourceHandle);
        }
    }
    GameState->EntityPositions[Entity->EntityIndex] = Entity->Position;
}

static void LoadEnemyData(FILE* File, entity* Entity, game_state* GameState)
{
    Entity->Enemy.IsTargeted = false;
    Entity->Enemy.AIState = AI_Idle;
    Entity->Type = Entity_Enemy;
    Entity->Enemy.AStarPath = {};
    Entity->Enemy.HealthCountStart = math::v2(-10, 50);
    Entity->Enemy.HasLoot = false;
    Entity->Enemy.Healthbar = 0;
    Entity->Enemy.HealthCountIndex = 0;
    Entity->Enemy.IsTargeted = false;
    Entity->Enemy.WaypointCount = 0;
    Entity->Enemy.WaypointIndex = 0;
    Entity->Enemy.WanderingForward = true;
    
    Entity->Weapon.Layer = (Entity_Layer)4;
    Entity->Weapon.IgnoreLayers = (Entity_Layer)0;
    Entity->IgnoreLayers = (Entity_Layer)0;
    
    Entity->Enemy.HealthCountIndex = 0;
    for(i32 Index = 0; Index < 10; Index++)
    {
        memset(Entity->Enemy.HealthCounts[Index].Count, 0, 20 * sizeof(char));
    }
    
    if(File)
    {
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "#"))
            {
                break;
            }
            else if(StartsWith(LineBuffer, "enemytype"))
            {
                sscanf(LineBuffer, "enemytype %d", &Entity->Enemy.EnemyType);
            }
            else if(StartsWith(LineBuffer, "walkingspeed"))
            {
                sscanf(LineBuffer, "walkingspeed %f", &Entity->Enemy.WalkingSpeed);
            }
            else if(StartsWith(LineBuffer, "will"))
            {
                sscanf(LineBuffer, "will %d", &Entity->Enemy.Will);
            }
            else if(StartsWith(LineBuffer, "wanderingspeed"))
            {
                sscanf(LineBuffer, "wanderingspeed %f", &Entity->Enemy.WanderingSpeed);
            }
            else if(StartsWith(LineBuffer, "closetoplayerspeed"))
            {
                sscanf(LineBuffer, "closetoplayerspeed %f", &Entity->Enemy.CloseToPlayerSpeed);
            }
            else if(StartsWith(LineBuffer, "targetingposition"))
            {
                sscanf(LineBuffer, "targetingposition %f %f", &Entity->Enemy.TargetingPositionX, &Entity->Enemy.TargetingPositionY);
            }
            else if(StartsWith(LineBuffer, "maxalertdistance"))
            {
                sscanf(LineBuffer, "maxalertdistance %f", &Entity->Enemy.MaxAlertDistance);
            }
            else if(StartsWith(LineBuffer, "maxfollowdistance"))
            {
                sscanf(LineBuffer, "maxfollowdistance %f", &Entity->Enemy.MaxFollowDistance);
            }
            else if(StartsWith(LineBuffer, "mindistancetoplayer"))
            {
                sscanf(LineBuffer, "mindistancetoplayer %f", &Entity->Enemy.MinDistanceToPlayer);
            }
            else if(StartsWith(LineBuffer, "slowdowndistance"))
            {
                sscanf(LineBuffer, "slowdowndistance %f", &Entity->Enemy.SlowdownDistance);
            }
            else if(StartsWith(LineBuffer, "attackdistance"))
            {
                sscanf(LineBuffer, "attackdistance %f", &Entity->Enemy.AttackDistance);
            }
            else if(StartsWith(LineBuffer, "aistate"))
            {
                sscanf(LineBuffer, "aistate %d", &Entity->Enemy.AIState);
            }
            else if(StartsWith(LineBuffer, "healthcountposition"))
            {
                sscanf(LineBuffer, "healthcountposition %f %f", &Entity->Enemy.HealthCountStart.x, &Entity->Enemy.HealthCountStart.y);
            }
            else if(StartsWith(LineBuffer, "astarcooldowntimer"))
            {
                Entity->Enemy.AStarPath.AStarCooldownTimer.TimerHandle = -1;
                
                sscanf(LineBuffer, "astarcooldowntimer %lf", &Entity->Enemy.AStarPath.AStarCooldownTimer.TimerMax);
            }
            else if(StartsWith(LineBuffer, "defendingtimer"))
            {
                Entity->Enemy.DefendingTimer.TimerHandle = -1;
                sscanf(LineBuffer, "defendingtimer %lf", &Entity->Enemy.DefendingTimer.TimerMax);
            }
            else if(StartsWith(LineBuffer, "healthbar"))
            {
                Entity->Enemy.Healthbar = (entity_healthbar*)malloc(sizeof(entity_healthbar));
                sscanf(LineBuffer, "healthbar offset %f %f scale %f %f",&Entity->Enemy.Healthbar->Offset.x,
                       &Entity->Enemy.Healthbar->Offset.y,&Entity->Enemy.Healthbar->Scale.x,
                       &Entity->Enemy.Healthbar->Scale.y);
                ui_render_info RenderInfo = {};
                RenderInfo.TextureOffset = math::v2(256, 0);
                RenderInfo.FrameSize = math::v2(64, 16);
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
        auto WalkString = Concat(Entity->Name,"_walk");
        PlayAnimation(Entity, WalkString, GameState);
        free(WalkString);
        
        auto CurrentWaypoint = math::v3(Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].x,
                                        0.0f, Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].z);
        
        auto DistanceToWaypoint = math::Distance(CurrentWaypoint, Entity->Position);
        
        if(DistanceToWaypoint < 0.01f)
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
        
        CurrentWaypoint =  math::v3(Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].x, 0.0f, Entity->Enemy.Waypoints[Entity->Enemy.WaypointIndex].z);
        auto Direction = math::Normalize(CurrentWaypoint - Entity->Position);
        
        Entity->Velocity = math::v3(Direction.x * Entity->Enemy.WanderingSpeed, 0.0f,
                                    Direction.z * Entity->Enemy.WanderingSpeed);
    }
    
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = math::Distance(Entity->Position, Player.Position);
    
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
    r64 DistanceToPlayer = math::Distance(Entity->Position, Player.Position);
    
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
    r64 DistanceToPlayer = math::Distance(Entity->Position, Player.Position);
    
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
    
    r64 DistanceToPlayer = math::Distance(Entity->Position, Player.Position);
    
    if(!Player.Dead && Player.Active)
    {
        if(DistanceToPlayer < Entity->Enemy.MinDistanceToPlayer)
        {
            PlayAnimation(Entity, "skeleton_idle", GameState);
            StartTimer(GameState, Skeleton.ChargingTimer);
            Enemy.AIState = AI_Charging;
            render_entity* RenderEntity = &GameState->RenderEntities[Entity->RenderEntityHandle];
        }
        else if(DistanceToPlayer > Entity->Enemy.MaxFollowDistance)
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            Enemy.AIState = AI_Wandering;
        }
        /*else if(DistanceToPlayer <= Entity->Enemy.SlowdownDistance)
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            math::v2 Direction = math::Normalize(Player.Position - Entity->Position);
            Entity->Velocity = math::v2(Direction.x * Entity->Enemy.CloseToPlayerSpeed, Direction.y * Entity->Enemy.CloseToPlayerSpeed);
        }*/
        else
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            FindPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
            FollowPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
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
    
    r64 DistanceToPlayer = math::Distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer >= Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
    }
    else if(TimerDone(GameState, Skeleton.ChargingTimer) && DistanceToPlayer <= Enemy.AttackDistance)
    {
        Enemy.AIState = AI_Attacking;
        Enemy.LastAttackMoveDirection = math::Normalize(Player.Position - Entity->Position);
        PlayAnimation(Entity, "skeleton_attack", GameState);
    }
    else
    {
        PlayAnimation(Entity, "skeleton_walk", GameState);
        math::v3 Direction = math::Normalize(Player.Position - Entity->Position);
        Entity->Velocity = math::v3((Direction.x + 0.1f) * Entity->Enemy.CloseToPlayerSpeed, 0.0f, (Direction.z + 0.1f) * Entity->Enemy.CloseToPlayerSpeed);
    }
}

AI_FUNC(SkeletonDefending)
{}

AI_FUNC(SkeletonAttacking)
{
    Entity->ShowAttackTiles = true;
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    r64 DistanceToPlayer = math::Distance(Entity->Position, Player.Position);
    
    if(Player.Dead)
    {
        Entity->IsAttacking = false;
        Entity->ShowAttackTiles = false;
        Enemy.AIState = AI_Idle;
    }
    else
    {
        if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            Entity->Velocity = math::v3(Enemy.LastAttackMoveDirection.x * Entity->AttackMoveSpeed,
                                        0.0f,
                                        Enemy.LastAttackMoveDirection.z * Entity->AttackMoveSpeed);
        }
        
        if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex - 2 &&Entity->AnimationInfo.FrameIndex < Entity->AttackHighFrameIndex && !Entity->IsAttacking && strcmp(Entity->CurrentAnimation->Name, "skeleton_idle") != 0)
        {
            StartTimer(GameState, Entity->AttackMoveTimer);
            
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
            {
                Entity->IsAttacking = true;
                
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
        
        if(Entity->IsAttacking && strcmp(Entity->CurrentAnimation->Name, "skeleton_attack") == 0 && Entity->AnimationInfo.FrameIndex >= Entity->AttackHighFrameIndex)
        {
            Entity->IsAttacking = false;
            Entity->ShowAttackTiles = false;
            if(DistanceToPlayer > Entity->Enemy.MinDistanceToPlayer)
            {
                Enemy.AIState = AI_Following;
            }
            else if(DistanceToPlayer > Entity->Enemy.MaxAlertDistance)
            {
                Enemy.AIState = AI_Wandering;
            }
            else
            {
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
        Entity->Velocity = math::v3(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed, 
                                    Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed, Entity->HitRecoilDirection.z * Entity->HitRecoilSpeed);
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

static void LoadBonfireData(game_state* GameState, sound_queue* SoundQueue, i32 Handle = -1, math::v3 Position = math::v3(), b32 IsTemporary = false)
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
        Entity->IsTemporary = IsTemporary;
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "#"))
            {
                break;
            }
        }
        
        StartTimer(GameState,GameState->LightSources[Entity->LightSourceHandle].Pointlight.GlowTimer);
        
        if(Handle == -1)
        {
            Entity->Position = math::v3(Position.x, Position.y, Position.z);
            PLAY_SOUND(Bonfire, 1.0f, Entity->Position.x, Entity->Position.z, 10.0f, true, Entity->EntityIndex);
        }
        
        fclose(File);
    }
}

static void LoadSkeletonData(game_state* GameState, i32 Handle = -1, math::v3 Position = math::v3())
{
    FILE* File;
    File = fopen("../assets/entities/skeleton.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    Entity->Enemy.Skeleton = {};
    Entity->AnimationInfo.FreezeFrame = false;
    Entity->Dead = false;
    Entity->IsAttacking = false;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity, GameState, Handle != -1);
        LoadEnemyData(File,Entity, GameState);
        
        Entity->Enemy.EnemyCollider = Entity->CollisionAABB;
        Entity->Enemy.EnemyCollider.Extents = math::v3(Entity->Enemy.EnemyCollider.Extents.x * 3, 
                                                       Entity->Enemy.EnemyCollider.Extents.y * 3,
                                                       Entity->Enemy.EnemyCollider.Extents.z * 3);
        
        if(Handle == -1)
            Entity->Position = math::v3(Position.x, Position.y, Position.z);
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "#"))
            {
                break;
            }
            else if(StartsWith(LineBuffer,"attackcooldowntimer"))
            {
                Entity->Enemy.Skeleton.AttackCooldownTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"attackcooldowntimer %lf",&Entity->Enemy.Skeleton.AttackCooldownTimer.TimerMax);
                Entity->Enemy.Skeleton.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(LineBuffer,"chargingtimer"))
            {
                Entity->Enemy.Skeleton.ChargingTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"chargingtimer %lf",&Entity->Enemy.Skeleton.ChargingTimer.TimerMax);
                Entity->Enemy.Skeleton.ChargingTimer.Name = "Charging";
            }
            else if(StartsWith(LineBuffer,"alertedtimer"))
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

void PlaceCheckpoint(game_state* GameState, sound_queue* SoundQueue, entity* Entity)
{
    auto CheckpointPos = math::v3(Entity->Position.x + 1, Entity->Position.y, Entity->Position.z + 1);
    
    if(!GameState->CharacterData.HasCheckpoint)
    {
        LoadBonfireData(GameState, SoundQueue, -1, CheckpointPos, true);
        GameState->CharacterData.CheckpointHandle = GameState->EntityCount - 1;
    }
    else
    {
        GameState->Entities[GameState->CharacterData.CheckpointHandle].Position = CheckpointPos;
    }
    
    GameState->LastCharacterData.CurrentCheckpoint = CheckpointPos;
    GameState->LastCharacterData.CheckpointHandle = GameState->CharacterData.CheckpointHandle;
    GameState->CharacterData.CurrentCheckpoint = CheckpointPos;
    GameState->CharacterData.HasCheckpoint = true;
    GameState->Entities[0].Player.Inventory.HealthPotionCount = GameState->CharacterData.HealthPotionCount;
    GameState->LastCharacterData.HasCheckpoint = true;
    SaveGame(GameState);
}

static void LoadPlayerData(game_state* GameState, sound_queue* SoundQueue, i32 Handle = -1, math::v3 Position = math::v3())
{
    FILE* File;
    File = fopen("../assets/entities/player.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] 
        : &GameState->Entities[GameState->EntityCount];
    Entity->Type = Entity_Player;
    Entity->Player = {};
    Entity->Player.TargetedEnemyHandle = -1;
    Entity->Player.LastKnownDirectionX = 1.0f;
    Entity->Player.LastKnownDirectionY = 0;
    
    Entity->Player.RenderCrosshair = false;
    Entity->IsAttacking = false;
    Entity->Player.IsDashing = false;
    Entity->Player.IsDefending = false;
    Entity->IgnoreLayers = (Entity_Layer)0;
    Entity->Layer = (Entity_Layer)1;
    Entity->Weapon.IgnoreLayers = (Entity_Layer)0;
    
    if(File)
    {
        Entity->Dead = false;
        LoadEntityData(File, Entity, GameState, Handle != -1);
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "walkingspeed"))
            {
                sscanf(LineBuffer, "walkingspeed %f", &Entity->Player.WalkingSpeed);
            }
            else if(StartsWith(LineBuffer, "throwingspeed"))
            {
                sscanf(LineBuffer, "throwingspeed %f", &Entity->Player.ThrowingSpeed);
            }
            else if(StartsWith(LineBuffer, "crosshairradius"))
            {
                sscanf(LineBuffer, "crosshairradius %f", &Entity->Player.CrosshairRadius);
            }
            else if(StartsWith(LineBuffer, "dashcounterdivider"))
            {
                sscanf(LineBuffer, "dashcounterdivider %f", &Entity->Player.DashCounterDivider);
            }
            else if(StartsWith(LineBuffer, "targetingdistance"))
            {
                sscanf(LineBuffer, "targetingdistance %f", &Entity->Player.TargetingDistance);
            }
            else if(StartsWith(LineBuffer, "attackcooldowntimer"))
            {
                AddTimer(LineBuffer, "attackcooldowntimer %lf", "Attack Cooldown",
                         &Entity->Player.AttackCooldownTimer);
            }
            else if(StartsWith(LineBuffer, "lastattacktimer"))
            {
                AddTimer(LineBuffer, "lastattacktimer %lf", "Last Attack",
                         &Entity->Player.LastAttackTimer);
            }
            else if(StartsWith(LineBuffer, "dashtimer"))
            {
                AddTimer(LineBuffer, "dashtimer %lf", "Dash",&Entity->Player.DashTimer);
            }
            else if(StartsWith(LineBuffer, "dashcooldowntimer"))
            {
                AddTimer(LineBuffer, "dashcooldowntimer %lf", "Dash Cooldown",
                         &Entity->Player.DashCooldownTimer);
            }
            else if(StartsWith(LineBuffer, "dashspeed"))
            {
                sscanf(LineBuffer, "dashspeed %f", &Entity->Player.DashSpeed);
            }
            else if(StartsWith(LineBuffer, "stamina "))
            {
                sscanf(LineBuffer, "stamina %d", &GameState->CharacterData.Stamina);
                Entity->Player.Stamina = (i16)GameState->CharacterData.Stamina;
            }
            else if(StartsWith(LineBuffer, "staminagaintimer "))
            {
                AddTimer(LineBuffer, "staminagaintimer %lf", "Stamina Gain",
                         &Entity->Player.StaminaGainTimer);
            }
            else if(StartsWith(LineBuffer, "hitstaminacost"))
            {
                sscanf(LineBuffer, "hitstaminacost %d", &Entity->Player.HitStaminaCost);
            }
            else if(StartsWith(LineBuffer, "rollstaminacost"))
            {
                sscanf(LineBuffer, "rollstaminacost %d", &Entity->Player.RollStaminaCost);
            }
            else if(StartsWith(LineBuffer, "attackstaminacost"))
            {
                sscanf(LineBuffer, "attackstaminacost %d", &Entity->Player.AttackStaminaCost);
            }
            else if(StartsWith(LineBuffer, "mindiffstamina"))
            {
                sscanf(LineBuffer, "mindiffstamina %d", &Entity->Player.MinDiffStamina);
            }
            else if(StartsWith(LineBuffer, "staminagaincooldowntimer"))
            {
                AddTimer(LineBuffer, "staminagaincooldowntimer %lf", "Stamina Gain Cooldown",
                         &Entity->Player.StaminaGainCooldownTimer);
            }
            else if(StartsWith(LineBuffer, "staminagaintimerfast"))
            {
                sscanf(LineBuffer, "staminagaintimerfast %lf", 
                       &Entity->Player.StaminaGainTimerFast);
            }
            else if(StartsWith(LineBuffer, "staminagaintimerslow"))
            {
                sscanf(LineBuffer, "staminagaintimerslow %lf", 
                       &Entity->Player.StaminaGainTimerSlow);
            }
            else if(StartsWith(LineBuffer, "checkpointplacementtimer"))
            {
                AddTimer(LineBuffer, "checkpointplacementtimer %lf", "Checkpoint Placement",
                         &Entity->Player.CheckpointPlacementTimer);
            }
            else if(StartsWith(LineBuffer, "checkpointplacementcooldowntimer"))
            {
                AddTimer(LineBuffer, "checkpointplacementcooldowntimer %lf", 
                         "Checkpoint Placement Cooldown",
                         &Entity->Player.CheckpointPlacementCooldownTimer);
            }
            else if(StartsWith(LineBuffer, "healthpotiontimer"))
            {
                AddTimer(LineBuffer, "healthpotiontimer %lf", "Health Potion",
                         &Entity->Player.HealthPotionTimer);
            }
        }
        fclose(File);
        
        LoadGame(GameState);
        
        if(Handle == -1)
        {
            if(GameState->CharacterData.HasCheckpoint)
            {
                Entity->Position = math::v3(GameState->CharacterData.CurrentCheckpoint.x - 1,
                                            0.0f,
                                            GameState->CharacterData.CurrentCheckpoint.z - 1);
            }
            else
            {
                printf("Entity position: (%f, %f, %f)\n", Position.x, Position.y, Position.z);
                
                Entity->Position = Position;
                GameState->CharacterData.CurrentCheckpoint = Position + math::v3(1, 0, 1);
                GameState->CharacterData.HasCheckpoint = true;
                
            }
            
            Entity->Position = math::v3(Entity->Position.x, Entity->Position.y, Entity->Position.z);
            
            LoadBonfireData(GameState, SoundQueue, -1, GameState->CharacterData.CurrentCheckpoint, true);
            GameState->CharacterData.CheckpointHandle = GameState->EntityCount - 1;
            GameState->LastCharacterData.CheckpointHandle = GameState->EntityCount - 1;
            GameState->LastCharacterData.CurrentCheckpoint = GameState->CharacterData.CurrentCheckpoint;
            GameState->LastCharacterData.HasCheckpoint = true;
            Entity->Player.Inventory.HealthPotionCount = GameState->CharacterData.HealthPotionCount;
            
            if(GameState->CharacterData.Health == 0)
            {
                GameState->CharacterData.Health = Entity->Health;
                GameState->CharacterData.Stamina = Entity->Player.Stamina;
                GameState->CharacterData.Strength = Entity->Weapon.Damage;
            }
        }
        
        Entity->Player.Inventory.HasCheckpoint = true;
        Entity->CurrentTile = math::v2i((i32)Entity->Position.x, (i32)Entity->Position.z);
        
    }
}

void CheckLootPickup(game_state* GameState, input_controller* InputController,loot* Loot, entity* Player)
{
    if(Loot->RenderButtonHint && GetActionButtonDown(Action_Interact, InputController))
    {
        b32 CanLoot = false;
        switch(Loot->Type)
        {
            case Loot_Health:
            {
                Player->Player.Inventory.HealthPotionCount = Player->Player.Inventory.HealthPotionCount;
                CanLoot = true;
            }
            break;
            case Loot_Checkpoint:
            {
                Player->Player.Inventory.HasCheckpoint = true;
                CanLoot = true;
            }
            break;
            case Loot_LevelItem:
            {
                if(Player->Player.Will >= GameState->StatData[GameState->CharacterData.Level].WillForLevel)
                {
                    GameState->StatGainModeOn = true;
                    CanLoot = true;
                }
            }
            break;
        }
        if(CanLoot)
        {
            GameState->Entities[Loot->OwnerHandle].Enemy.HasLoot = false;
            Loot->RenderButtonHint= false;
            GameState->Objects[Loot->OwnerHandle].Active = false;
            for(i32 Index = Loot->Handle; Index < GameState->CurrentLootCount; Index++)
            {
                GameState->CurrentLoot[Index] = GameState->CurrentLoot[Index + 1];
            }
            GameState->CurrentLootCount--;
        }
    }
}

Look_Direction DetermineDirection(r32* DXPtr, r32* DYPtr)
{
    r32 DX = *DXPtr;
    r32 DY = *DYPtr;
    Look_Direction NewDirection = North;
    
    if(DX < 0 && DY > 0)
    {
        DX = 0;
        NewDirection = NorthWest;
    }
    else if(DX == 0.0f && DY > 0)
    {
        DX = 1.0f;
        NewDirection = North;
    }
    else if(DX > 0 && DY > 0)
    {
        DY = 0.0f;
        NewDirection = NorthEast;
    }
    else if(DX > 0 && DY == 0.0f)
    {
        DY = -1.0f;
        NewDirection = East;
    }
    else if(DX > 0 && DY < 0)
    {
        DX = 0.0f;
        NewDirection = SouthEast;
    }
    else if(DX == 0.0f && DY < 0)
    {
        DX = -1.0f;
        NewDirection = South;
    }
    else if(DX < 0 && DY < 0)
    {
        DY = 0.0f;
        NewDirection = SouthWest;
    }
    else if(DX < 0 && DY == 0.0f)
    {
        DY = 1.0f;
        NewDirection = West;
    }
    
    *DXPtr = DX;
    *DYPtr = DY;
    
    return NewDirection;
}

void DetermineDeltaForDirection(Look_Direction LookDirection, r32* DX, r32* DY)
{
    switch(LookDirection)
    {
        case North:
        {
            *DX = 1.0f;
            *DY = 1.0f;
        }
        break;
        case South:
        {
            *DX = -1.0f;
            *DY = -1.0f;
        }
        break;
        case East:
        {
            *DX = 1.0f;
            *DY = -1.0f;
        }
        break;
        case West:
        {
            *DX = -1.0f;
            *DY = 1.0f;
        }
        break;
        case NorthEast:
        {
            *DX = 1.0f;
            *DY = 0.0f;
        }
        break;
        case NorthWest:
        {
            *DX = 0.0f;
            *DY = 1.0f;
        }
        break;
        case SouthEast:
        {
            *DX = 0.0f;
            *DY = -1.0f;
        }
        break;
        case SouthWest:
        {
            *DX = -1.0f;
            *DY = 0.0f;
        }
        break;
    }
}

void UpdatePlayer(entity* Entity, game_state* GameState, renderer& Renderer, sound_queue* SoundQueue, input_controller* InputController, r64 DeltaTime)
{
    // Collision check
    //collision_info CollisionInfo;
    //CheckCollision(GameState, Entity, &CollisionInfo);
    
    // Input + movement
    r32 XInput = GetInputX(InputController);
    r32 YInput = GetInputY(InputController);
    
    r32 Speed = Entity->Player.WalkingSpeed;
    
    Entity->Velocity.x = XInput * Speed;
    Entity->Velocity.z = YInput * Speed;
    
    Entity->Velocity = math::YRotate(45) * Entity->Velocity;
    
    
    Entity->Position += math::v3(Entity->Velocity.x * DeltaTime, Entity->Velocity.y * DeltaTime, -Entity->Velocity.z * DeltaTime);
    Entity->Position = math::v3(Entity->Position.x, Entity->Position.y, Entity->Position.z);
    
    // Update camera if centered on player
    Renderer.Cameras[GameState->GameCameraHandle].CenterTarget = Entity->Position;
}

static void UpdateAI(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    switch(Entity->Enemy.AIState)
    {
        case AI_Idle:
        {
            Entity->Enemy.Idle(Entity,GameState, SoundQueue, DeltaTime);
        }
        break;
        case AI_Alerted:
        {
            Entity->Enemy.Alerted(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Following:
        {
            Entity->Enemy.Following(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Charging:
        {
            Entity->Enemy.Charging(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Defending:
        {
            Entity->Enemy.Defending(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Attacking:
        {
            Entity->Enemy.Attacking(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Hit:
        {
            Entity->Enemy.Hit(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Dying:
        {
            Entity->Enemy.Dying(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
        case AI_Wandering:
        {
            Entity->Enemy.Wandering(Entity,GameState, SoundQueue,DeltaTime);
        }
        break;
    }
}

static void UpdateSkeleton(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    auto Player = &GameState->Entities[0];
    Entity->RenderButtonHint = Entity->Enemy.HasLoot && Entity->Dead && math::Distance(Player->Position, Entity->Position) < 1.5f;
    
    if(Entity->Active && !Entity->Dead)
    {
        if(Entity->Hit)
        {
            if(Entity->Health <= 0)
            {
                Entity->IsAttacking = false;
                PlayAnimation(Entity, "skeleton_dead", GameState);
                Entity->AnimationInfo.FreezeFrame = true;
                Enemy.AIState = AI_Dying;
                SaveGame(GameState);
            }
            else if(strcmp(Entity->CurrentAnimation->Name, "skeleton_attack") != 0 && Enemy.AIState != AI_Dying)
            {
                PlayAnimation(Entity, "monster_hit_south", GameState);
                Enemy.AIState = AI_Hit;
                Entity->HitRecoilDirection = math::Normalize(Entity->Position - Player->Position);
                StartTimer(GameState, Entity->RecoilTimer);
            }
            else
            {
                PlayAnimation(Entity, "monster_idle_south", GameState);
            }
        }
        
        Entity->Velocity = math::v3(0, 0, 0); //@Cleanup: This is not good. Do this in AI
        
        //UpdateAI(Entity,GameState, SoundQueue,DeltaTime);
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
        Entity->Position.z += Entity->Velocity.z * (r32)DeltaTime;
        
        render_entity* RenderEntity = &GameState->RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = math::v3(0, 0, 0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

static void UpdateStaticEntity(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
}

static void UpdateGeneral(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    GameState->RenderEntities[Entity->RenderEntityHandle].Rendered = math::Distance(Entity->Position, GameState->Entities[0].Position) < 15;
    
    GameState->EntityTilePositions[Entity->CurrentTile.x][Entity->CurrentTile.y] = 0;
    
    auto& RenderEntity = GameState->RenderEntities[Entity->RenderEntityHandle];
    
    if(Entity->HitFlickerFramesLeft > 0 && TimerDone(GameState, Entity->HitFlickerTimer))
    {
        Entity->HitFlickerFramesLeft--;
        
        if(Entity->HitFlickerFramesLeft % 2 == 0)
        {
            RenderEntity.Color = math::v4(1, 0, 0, 1);
        }
        else
            RenderEntity.Color = math::v4(1, 1, 1, 1);
        
        StartTimer(GameState, Entity->HitFlickerTimer);
    }
    else if(Entity->HitFlickerFramesLeft == 0)
    {
        RenderEntity.Color = math::v4(1, 1, 1, 1);
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


static void LightGlow(game_state* GameState, i32 LightHandle)
{
    auto Light = &GameState->LightSources[LightHandle].Pointlight;
    if(!TimerDone(GameState,Light->GlowTimer))
    {
        GameState->LightSources[LightHandle].Pointlight.Intensity += Light->IncreasingGlow ? Light->GlowIncrease : -Light->GlowIncrease;
    }
    else
    {
        Light->IncreasingGlow = !Light->IncreasingGlow;
        StartTimer(GameState,Light->GlowTimer);
    }
}

static void UpdateObjects(game_state* GameState, r64 DeltaTime)
{
    for(i32 Index = 0; Index < GameState->ObjectCount; Index++)
    {
        auto& Object = GameState->Objects[Index];
        
        if(Object.Active && Object.CurrentAnimation && Object.AnimationInfo.Playing)
            TickAnimation(&Object.AnimationInfo, Object.CurrentAnimation, DeltaTime);
        
        if(Object.Type == Object_Will)
        {
            LightGlow(GameState, Object.LightSourceHandle);
            
        }
    }
}

static void UpdateEntities(game_state* GameState, renderer& Renderer, input_controller* InputController, sound_queue* SoundQueue, r64 DeltaTime)
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
                    
                    UpdatePlayer(Entity, GameState, Renderer, SoundQueue, InputController, DeltaTime);
                    
                }
                break;
                case Entity_Enemy:
                {
                    switch(Entity->Enemy.EnemyType)
                    {
                        case Enemy_Skeleton:
                        {
                            UpdateSkeleton(Entity, GameState, SoundQueue, DeltaTime);
                        }
                        break;
                    }
                }
                break;
                case Entity_Bonfire:
                {
                    UpdateStaticEntity(Entity, GameState, DeltaTime);
                    
                    LightGlow(GameState,Entity->LightSourceHandle);
                }
                break;
            }
            
            if(Entity->Active && Entity->CurrentAnimation && Entity->AnimationInfo.Playing)
                TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
        }
        GameState->EntityPositions[EntityIndex] = Entity->Position;
    }
    
    /*if(GameState->ClearTilePositionFrame)
    {
    for(i32 X = 0; X < GameState->CurrentLevel.Tilemap.Width; X++)
    {
    for(i32 Y = 0; Y < GameState->CurrentLevel.Tilemap.Height; Y++)
    {
    GameState->EntityTilePositions[X][Y] = 0;
    }
    }
    }*/
}