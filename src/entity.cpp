
void PrintEntityInfo(const entity& Entity)
{
    DEBUG_PRINT("Entity: Name %s, position x %f y %f, rotation x %f y %f z %f\n", Entity.Name, Entity.Position.x, Entity.Position.y, Entity.Rotation.x, Entity.Rotation.y, Entity.Rotation.z);
}

i32 LoadPointlight(game_state* GameState, glm::vec4 Color = glm::vec4(), r32 Intensity = 0.0f, r32 ConstantAtten = 0.0f, r32 LinearAtten = 0.0f, r32 ExponentialAtten = 0.0f,glm::vec2 InitPosition = glm::vec2(), b32 ShouldGlow = false, r64 GlowTimerMax = 0.0f, r32 GlowIncrease = 0.0f)
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
    GameState->LightSources[GameState->LightSourceCount++] = LightSource;
    return GameState->LightSourceCount - 1;
}

i32 LoadLight(game_state* GameState, char* LineBuffer, glm::vec2 InitPosition = glm::vec2(), i32 Handle = -1)
{
    b32 ShouldGlow = false;
    light_source LightSource;
    if(StartsWith(LineBuffer, "pointlight"))
    {
        r64 GlowTimerMax = 0.0;
        r32 GlowIncrease;
        sscanf(LineBuffer, "pointlight type %d active %d intensity %f color %f %f %f %f atten %f %f %f shouldglow %d glowtimer %lf glowincrease %f",
               &LightSource.Type , &LightSource.Active,&LightSource.Pointlight.Intensity,&LightSource.Color.x, &LightSource.Color.y, &LightSource.Color.z,&LightSource.Color.w, &LightSource.Pointlight.ConstantAtten, &LightSource.Pointlight.LinearAtten, &LightSource.Pointlight.ExponentialAtten, &ShouldGlow, &GlowTimerMax, &GlowIncrease);
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

static void DecreaseStamina(entity* Entity, game_state* GameState, i32 Cost) 
{
    i32 NewStamina = Max(0, Entity->Player.Stamina - Cost);
    Entity->Player.StaminaLost = Entity->Player.Stamina - NewStamina;
    Entity->Player.Stamina = NewStamina;
    StartTimer(GameState, Entity->Player.StaminaDecreaseTimer);
    StartTimer(GameState,Entity->Player.StaminaGainCooldownTimer);
}

void Hit(game_state* GameState, sound_queue* SoundQueue, entity* ByEntity, entity* HitEntity)
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
            if(TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
                StartTimer(GameState, GameState->GameCamera.ScreenShakeTimer);
            
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
                StartFade(GameState->GameCamera, Fading_OutIn, 4.0f, glm::vec3(1, 0, 0), 0.0f, 0.4f);
            }
        }
    }
}

static void SpawnShadow(game_state* GameState, glm::vec2 Position, i32* Handle)
{
    *Handle = GameState->ObjectCount;
    
    auto Shadow = &GameState->Objects[GameState->ObjectCount++];
    Shadow->Active = true;
    Shadow->Scale = 1;
    Shadow->Type = Object_Shadow;
    Shadow->Position = glm::vec2(Position.x - 1.4f, Position.y - 1.5f);
    Shadow->UsesTransparency = true;
    
    PlayAnimation(Shadow, "big_shadow", GameState);
    
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    RenderEntity->RenderType = Render_Type_Object;
    Shadow->RenderEntityHandle = GameState->RenderState.RenderEntityCount;
    
    RenderEntity->ShaderIndex = Shader_Spritesheet;
    RenderEntity->Rendered = true;
    RenderEntity->Background = true;
    RenderEntity->RenderType = Render_Type_Object;
    RenderEntity->Object = &*Shadow;
    
    Shadow->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    DEBUG_PRINT("Shadow\n");
    GameState->Objects[GameState->ObjectCount++];
}

static void SpawnLoot(game_state* GameState, glm::vec2 Position, i32* Handle)
{
    *Handle = GameState->ObjectCount;
    
    auto Loot = &GameState->Objects[GameState->ObjectCount++];
    Loot->Active = true;
    Loot->Scale = 1;
    Loot->Type = Object_Loot;
    Loot->Position = glm::vec2(Position.x, Position.y - 0.5f);
    Loot->UsesTransparency = true;
    
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    RenderEntity->RenderType = Render_Type_Object;
    RenderEntity->Texture = GameState->RenderState.Textures["basic_loot"];
    Loot->RenderEntityHandle = GameState->RenderState.RenderEntityCount;
    
    RenderEntity->ShaderIndex = Shader_Texture;
    RenderEntity->Rendered = true;
    RenderEntity->Background = false;
    RenderEntity->RenderType = Render_Type_Object;
    RenderEntity->Object = &*Loot;
    
    Loot->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    DEBUG_PRINT("Loot\n");
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    GameState->Objects[GameState->ObjectCount++];
}

static void SpawnWillDrop(game_state* GameState, glm::vec2 Position, i32* Handle)
{
    *Handle = GameState->ObjectCount;
    
    auto Will  = &GameState->Objects[GameState->ObjectCount++];
    Will->Active = true;
    Will->Scale = 0.5f;
    Will->Type = Object_Will;
    Will->Position = glm::vec2(Position.x, Position.y - 0.5f);
    Will->UsesTransparency = true;
    
    PlayAnimation(Will, "will_glow", GameState);
    
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    RenderEntity->RenderType = Render_Type_Object;
    
    RenderEntity->ShaderIndex = Shader_Spritesheet;
    RenderEntity->Rendered = true;
    RenderEntity->Background = false;
    RenderEntity->RenderType = Render_Type_Object;
    RenderEntity->Object = &*Will;
    
    Will->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    Will->LightSourceHandle = LoadPointlight(GameState, glm::vec4(0.2f, 0.15f, 0.65f, 0.3f), 0.45f, 0.2f, 1.0f,0.2f, GameState->CharacterData.LostWillPosition, true, 0.9, 0.0007f);
    
    GameState->Objects[GameState->ObjectCount++];
}


static void SpawnTree(game_state* GameState, glm::vec2 Position, i32* Handle = 0)
{
    if(Handle)
        *Handle = GameState->ObjectCount;
    
    auto Tree = &GameState->Objects[GameState->ObjectCount++];
    Tree->Active = true;
    Tree->Scale = 1.0f;
    Tree->Type = Object_Tree,
    Tree->Position = glm::vec2(Position.x - 1.4f, Position.y - 1.5f);
    Tree->UsesTransparency = true;
    
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
    RenderEntity->RenderType = Render_Type_Object;
    Tree->RenderEntityHandle = GameState->RenderState.RenderEntityCount;
    
    RenderEntity->ShaderIndex = Shader_Texture;
    RenderEntity->Rendered = true;
    RenderEntity->Background = false;
    RenderEntity->RenderType = Render_Type_Object;
    RenderEntity->Object = &*Tree;
    RenderEntity->Texture = GameState->RenderState.Textures["big_tree"];
    
    Tree->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    GameState->Objects[GameState->ObjectCount++];
}

static void LoadEntityData(FILE* File, entity* Entity, game_state* GameState, b32 IsReload = false)
{
    if(!IsReload)
    {
        Entity->EntityIndex = GameState->EntityCount++;
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[GameState->RenderState.RenderEntityCount];
        RenderEntity->ShaderIndex = Shader_Spritesheet;
        RenderEntity->Rendered = true;
        RenderEntity->Background = false;
        RenderEntity->RenderType = Render_Type_Entity;
        RenderEntity->Entity = &*Entity;
        Entity->RenderEntityHandle = GameState->RenderState.RenderEntityCount++;
        RenderEntity->Color = glm::vec4(1, 1, 1, 1);
        
        Entity->LookDirection = South;
        Entity->HitFlickerTimer.TimerHandle = -1;
        Entity->HitFlickerTimer.TimerMax = 0.05f;
        Entity->HitFlickerTimer.Name = "Hit Flicker";
        Entity->HitFlickerFramesLeft = 0;
        Entity->HitFlickerFrameMax = 6;
        
        Entity->RenderButtonOffset = glm::vec2(0.5f,1.5f);
        Entity->RenderButtonHint = false;
        
        Entity->HitAttackCountIdResetTimer.TimerHandle = -1;
        Entity->HitAttackCountIdResetTimer.TimerMax = 1.0f;
        Entity->HitAttackCountIdResetTimer.Name = "Hit Attack Count Id Reset";
        
        Entity->HealthDecreaseTimer.TimerMax = 0.8;
        Entity->HealthDecreaseTimer.TimerHandle = -1;
        Entity->HealthDecreaseTimer.Name = "Health Decrease";
        Entity->Dead = false;
        Entity->Center = glm::vec2(0.5, 0.5);
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
        
        Entity->TilePosition.X = 0;
        Entity->TilePosition.Y = 0;
        Entity->TilePosition.Z = 0;
        
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
    GameState->EntityPositions[GameState->EntityCount] = Entity->Position;
}

static void LoadEnemyData(FILE* File, entity* Entity, game_state* GameState)
{
    Entity->Enemy.IsTargeted = false;
    Entity->Enemy.AIState = AI_Idle;
    Entity->Type = Entity_Enemy;
    Entity->Enemy.AStarPath = {};
    Entity->Enemy.HealthCountStart = glm::vec2(-10, 50);
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
        auto WalkString = Concat(Entity->Name,"_walk");
        PlayAnimation(Entity, WalkString, GameState);
        free(WalkString);
        
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
        if(DistanceToPlayer < Entity->Enemy.MinDistanceToPlayer)
        {
            PlayAnimation(Entity, "skeleton_idle", GameState);
            StartTimer(GameState, Skeleton.ChargingTimer);
            Enemy.AIState = AI_Charging;
            render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        }
        else if(DistanceToPlayer > Entity->Enemy.MaxFollowDistance)
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            Enemy.AIState = AI_Wandering;
        }
        /*else if(DistanceToPlayer <= Entity->Enemy.SlowdownDistance)
        {
            PlayAnimation(Entity, "skeleton_walk", GameState);
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.CloseToPlayerSpeed, Direction.y * Entity->Enemy.CloseToPlayerSpeed);
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
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer >= Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
    }
    else if(TimerDone(GameState, Skeleton.ChargingTimer) && DistanceToPlayer <= Enemy.AttackDistance)
    {
        Enemy.AIState = AI_Attacking;
        Enemy.LastAttackMoveDirection = glm::normalize(Player.Position - Entity->Position);
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
    Entity->ShowAttackTiles = true;
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(Player.Dead)
    {
        Skeleton.IsAttacking = false;
        Entity->ShowAttackTiles = false;
        Enemy.AIState = AI_Idle;
    }
    else
    {
        if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            Entity->Velocity = glm::vec2(Enemy.LastAttackMoveDirection.x * Entity->AttackMoveSpeed, Enemy.LastAttackMoveDirection.y * Entity->AttackMoveSpeed);
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
        
        if(Skeleton.IsAttacking && strcmp(Entity->CurrentAnimation->Name, "skeleton_attack") == 0 && Entity->AnimationInfo.FrameIndex >= Entity->AttackHighFrameIndex)
        {
            Skeleton.IsAttacking = false;
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

static void MinotaurSetAttackMode(entity* Entity, game_state* GameState)
{
    if(Entity->Enemy.AttackMode == 0)
    {
        PlayAnimation(Entity, "minotaur_attack", GameState);
    }
    else
    {
        SpawnShadow(GameState, Entity->Position, &Entity->Enemy.Minotaur.ShadowHandle);
        StartTimer(GameState, Entity->Enemy.Minotaur.JumpAttackTimer);
        PlayAnimation(Entity, "minotaur_detected", GameState);
    }
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
        PLAY_SOUND(MinotaurGrunt02);
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
            FollowPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
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
        PLAY_SOUND(MinotaurGrunt01);
        
        Enemy.LastAttackMoveDirection = glm::normalize(Player.Position - Entity->Position);
        
        MinotaurSetAttackMode(Entity, GameState);
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
        
        PLAY_SOUND(MinotaurGrunt01);
        
        entity& Player = GameState->Entities[GameState->PlayerIndex];
        Entity->Enemy.LastAttackMoveDirection = glm::normalize(Player.Position - Entity->Position);
        
        MinotaurSetAttackMode(Entity, GameState);
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
        
        Enemy.AttackMode++;
        if(Enemy.AttackMode == 2)
        {
            Enemy.AttackMode = 0;
        }
    }
    else
    {
        if(Entity->Enemy.AttackMode == 0) // standard double attack
        {
            if(!TimerDone(GameState, Entity->AttackMoveTimer))
            {
                Entity->Velocity = glm::vec2(Enemy.LastAttackMoveDirection.x * Entity->AttackMoveSpeed, Enemy.LastAttackMoveDirection.y * Entity->AttackMoveSpeed);
            }
            
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex - 2 
               && Entity->AnimationInfo.FrameIndex < Entity->AttackHighFrameIndex 
               && !Minotaur.IsAttacking 
               && strcmp(Entity->CurrentAnimation->Name, "minotaur_idle") != 0)
            {
                StartTimer(GameState, Entity->AttackMoveTimer);
                
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
                    PLAY_SOUND(MinotaurGrunt01);
                    
                    MinotaurSetAttackMode(Entity, GameState);
                }
            }
            
            if(TimerDone(GameState, Minotaur.AttackCooldownTimer) && Entity->AttackCount == Minotaur.MaxAttackStreak)
            {
                Entity->AttackCount = 0;
                Minotaur.IsAttacking = false;
                
                Enemy.AttackMode++;
                
                Enemy.AttackMode = 1;
                
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
        else // jump attack
        {
            if(!TimerDone(GameState, Entity->Enemy.Minotaur.JumpAttackTimer))
            {
                Entity->IsKinematic = true; 
                
                auto Elapsed = ElapsedTimer(GameState, Entity->Enemy.Minotaur.JumpAttackTimer);
                
                if(Elapsed < 0.5)
                    Elapsed = -0.8f;
                
                glm::vec2 JumpDirection = glm::vec2(0, Elapsed);
                
                auto PlayerPosition = GameState->Entities[0].Position;
                
                glm::vec2 Direction = glm::vec2(Enemy.LastAttackMoveDirection.x +  (PlayerPosition.x - Entity->Position.x) / 2, Enemy.LastAttackMoveDirection.y + Elapsed);
                Entity->Velocity = glm::vec2(Direction.x * 7, Direction.y * 7);
                
                GameState->Objects[Entity->Enemy.Minotaur.ShadowHandle].Position += glm::vec2(Direction.x * 7 * DeltaTime, Enemy.LastAttackMoveDirection.y * 7 * DeltaTime);
            }
            else
            {
                if(Entity->IsKinematic)
                {
                    Entity->IsKinematic = false;
                    PLAY_SOUND(MinotaurStomp);
                    StartTimer(GameState, Entity->Enemy.Minotaur.JumpAttackImpactTimer);
                }
                
                if(!TimerDone(GameState, Entity->Enemy.Minotaur.JumpAttackImpactTimer))
                {
                    Entity->CollisionAABB.IsTrigger = true;
                    Entity->CollisionAABB.Extents = Entity->Enemy.Minotaur.ImpactCollisionExtents;
                    collision_info CollisionInfo;
                    CheckCollision(GameState, Entity, &CollisionInfo); //@Incomplete: Remember to check for collision with a bigger collider maybe?
                    
                    for(i32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
                    {
                        if(CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing)
                        {
                            Hit(GameState, SoundQueue, Entity, CollisionInfo.Other[Index]);
                        }
                    }
                }
                else
                {
                    Entity->CollisionAABB.IsTrigger = false;
                    
                    Entity->Enemy.AttackMode = 0;
                    Entity->CollisionAABB.Extents = Entity->Enemy.Minotaur.OldCollisionExtents;
                    
                    if(rand() % 2 == 0 && DistanceToPlayer <= Enemy.AttackDistance) //@Incomplete: Maybe set a specific percentage for this in the .dat-file
                    {
                        Enemy.AIState = AI_Attacking;
                        PLAY_SOUND(MinotaurGrunt01);
                        Enemy.LastAttackMoveDirection = glm::normalize(Player.Position - Entity->Position);
                        MinotaurSetAttackMode(Entity, GameState);
                    }
                    else
                    {
                        Entity->Enemy.AIState = AI_Idle;
                        GameState->Objects[Entity->Enemy.Minotaur.ShadowHandle].Active = false;
                    }
                }
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
        PLAY_SOUND(MinotaurGrunt02);
    }
}


AI_FUNC(WraithIdle)
{
    PlayAnimation(Entity, "wraith_idle", GameState);
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
        PlayAnimation(Entity, "wraith_fly", GameState);
    }
}

AI_FUNC(WraithAlerted)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    if(TimerDone(GameState,Entity->Enemy.Wraith.AlertedTimer) && DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
        PlayAnimation(Entity, "wraith_fly", GameState);
    }
}

AI_FUNC(WraithFollowing)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Wraith = Entity->Enemy.Wraith;
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(!Player.Dead && Player.Active)
    {
        if(DistanceToPlayer > Entity->Enemy.MaxFollowDistance)
        {
            PlayAnimation(Entity, "wraith_fly", GameState);
            Enemy.AIState = AI_Wandering;
        }
        else if(DistanceToPlayer < Entity->Enemy.MinDistanceToPlayer)
        {
            PlayAnimation(Entity, "wraith_idle", GameState);
            StartTimer(GameState, Wraith.ChargingTimer);
            Enemy.AIState = AI_Charging;
            render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        }
        else if(DistanceToPlayer <= Entity->Enemy.SlowdownDistance)
        {
            PlayAnimation(Entity, "wraith_fly", GameState);
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.CloseToPlayerSpeed, Direction.y * Entity->Enemy.CloseToPlayerSpeed);
        }
        else
        {
            PlayAnimation(Entity, "wraith_fly", GameState);
            FindPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
            FollowPath(GameState, Entity, Player, &Entity->Enemy.AStarPath);
        }
    }
    else
    {
        PlayAnimation(Entity, "wraith_idle", GameState);
    }
}

AI_FUNC(WraithCharging)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Wraith = Entity->Enemy.Wraith;
    
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(DistanceToPlayer >= Enemy.MaxAlertDistance)
    {
        Enemy.AIState = AI_Following;
    }
    else if(TimerDone(GameState, Wraith.ChargingTimer) && DistanceToPlayer <= Enemy.AttackDistance)
    {
        Enemy.AIState = AI_Attacking;
        Enemy.LastAttackMoveDirection = glm::normalize(Player.Position - Entity->Position);
        PlayAnimation(Entity, "wraith_attack", GameState);
    }
    else
    {
        PlayAnimation(Entity, "wraith_fly", GameState);
        glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
        Entity->Velocity = glm::vec2((Direction.x + 0.1f) * Entity->Enemy.CloseToPlayerSpeed, (Direction.y + 0.1f) * Entity->Enemy.CloseToPlayerSpeed);
    }
}

AI_FUNC(WraithDefending)
{}

AI_FUNC(WraithAttacking)
{
    entity& Player = GameState->Entities[GameState->PlayerIndex];
    auto& Enemy = Entity->Enemy;
    auto& Wraith = Entity->Enemy.Wraith;
    r64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
    
    if(Player.Dead)
    {
        Wraith.IsAttacking = false;
        Enemy.AIState = AI_Idle;
    }
    else
    {
        if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            Entity->Velocity = glm::vec2(Enemy.LastAttackMoveDirection.x * Entity->AttackMoveSpeed, Enemy.LastAttackMoveDirection.y * Entity->AttackMoveSpeed);
        }
        
        if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex - 2 &&Entity->AnimationInfo.FrameIndex < Entity->AttackHighFrameIndex && !Wraith.IsAttacking && strcmp(Entity->CurrentAnimation->Name, "wraith_idle") != 0)
        {
            StartTimer(GameState, Entity->AttackMoveTimer);
            
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
            {
                Wraith.IsAttacking = true;
                
                Entity->AttackCount++;
                if(Entity->AttackCount == 3)
                    Entity->AttackCount = 0;
                
                StartTimer(GameState, Wraith.AttackCooldownTimer);
            }
        }
        else if(!Entity->AnimationInfo.Playing)
        {
            PlayAnimation(Entity, "wraith_idle", GameState);
        }
        
        if(Wraith.IsAttacking && TimerDone(GameState, Wraith.AttackCooldownTimer))
        {
            if(DistanceToPlayer > Entity->Enemy.MinDistanceToPlayer)
            {
                Wraith.IsAttacking = false;
                Enemy.AIState = AI_Following;
            }
            else if(DistanceToPlayer > Entity->Enemy.MaxAlertDistance)
            {
                Wraith.IsAttacking = false;
                Enemy.AIState = AI_Wandering;
            }
            else
            {
                Wraith.IsAttacking = false;
                Entity->Enemy.AIState = AI_Charging;
                StartTimer(GameState, Wraith.ChargingTimer);
            }
        }
    }
}

AI_FUNC(WraithHit)
{
    auto& Enemy = Entity->Enemy;
    if(!TimerDone(GameState, Entity->RecoilTimer))
    {
        Entity->Velocity = glm::vec2(Entity->HitRecoilDirection.x * Entity->HitRecoilSpeed, Entity->HitRecoilDirection.y * Entity->HitRecoilSpeed);
    }
    
    if(!Entity->AnimationInfo.Playing)
    {
        Enemy.AIState = AI_Idle;
        PlayAnimation(Entity, "wraith_idle", GameState);
    }
}

AI_FUNC(WraithDying)
{
    Entity->IsKinematic = true;
    if(!Entity->AnimationInfo.Playing)
    {
        Entity->Dead = true;
        Entity->IsKinematic = true;
    }
}

AI_FUNC(WraithWandering)
{
    EnemyWander(GameState,Entity);
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
    FollowPath(GameState,Entity,GameState->Entities[GameState->PlayerIndex], &Entity->Enemy.AStarPath);
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
        PLAY_SOUND(Explosion);
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


static void LoadBonfireData(game_state* GameState, sound_queue* SoundQueue, i32 Handle = -1, glm::vec2 Position = glm::vec2(), b32 IsTemporary = false)
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
            DEBUG_PRINT("Entityhandle: %d\n", Entity->EntityIndex);
            Entity->Position = glm::vec2(Position.x, Position.y);
            PLAY_SOUND(Bonfire, 1.0f, Entity->Position.x, Entity->Position.y, 10.0f, true, Entity->EntityIndex);
        }
        
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
    Entity->Enemy.Skeleton.IsAttacking = false;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity, GameState, Handle != -1);
        LoadEnemyData(File,Entity, GameState);
        
        Entity->Enemy.EnemyCollider = Entity->CollisionAABB;
        Entity->Enemy.EnemyCollider.Extents = glm::vec2(Entity->Enemy.EnemyCollider.Extents.x * 3, Entity->Enemy.EnemyCollider.Extents.y * 3);
        
        if(Handle == -1)
            Entity->Position = glm::vec2(Position.x, Position.y);
        
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

static void LoadMinotaurData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/minotaur.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    Entity->Enemy.Minotaur = {};
    Entity->AnimationInfo.FreezeFrame = false;
    Entity->Dead = false;
    Entity->Enemy.Minotaur.IsAttacking = false;
    Entity->Enemy.Minotaur.MaxAttackStreak = 1;
    Entity->Enemy.AttackMode = 0;
    
    if(Handle == -1)
    {
        Entity->Position = Position;
    }
    
    if(File)
    {
        LoadEntityData(File,Entity, GameState, Handle != -1);
        LoadEnemyData(File,Entity, GameState);
        
        Entity->Enemy.Minotaur.OldCollisionExtents = Entity->CollisionAABB.Extents;
        
        if(Handle == -1)
            Entity->Position = Position;
        
        char LineBuffer[255];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "#"))
            {
                break;
            }
            else if(StartsWith(LineBuffer,"attackcooldowntimer"))
            {
                Entity->Enemy.Minotaur.AttackCooldownTimer.TimerHandle = -1;
                sscanf(LineBuffer,"attackcooldowntimer %lf",&Entity->Enemy.Minotaur.AttackCooldownTimer.TimerMax);
                Entity->Enemy.Minotaur.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(LineBuffer,"chargingtimer"))
            {
                Entity->Enemy.Minotaur.ChargingTimer.TimerHandle = -1;
                sscanf(LineBuffer,"chargingtimer %lf",&Entity->Enemy.Minotaur.ChargingTimer.TimerMax);
                Entity->Enemy.Minotaur.ChargingTimer.Name = "Charging";
            }
            else if(StartsWith(LineBuffer,"alertedtimer"))
            {
                Entity->Enemy.Minotaur.AlertedTimer.TimerHandle = -1;
                sscanf(LineBuffer,"alertedtimer %lf",&Entity->Enemy.Minotaur.AlertedTimer.TimerMax);
                Entity->Enemy.Minotaur.AlertedTimer.Name = "Alerted";
            }
            else if(StartsWith(LineBuffer,"maxattackstreak"))
            {
                Entity->Enemy.Minotaur.AlertedTimer.TimerHandle = -1;
                sscanf(LineBuffer,"maxattackstreak %d",&Entity->Enemy.Minotaur.MaxAttackStreak);
            }
            else if(StartsWith(LineBuffer,"jumpattacktimer"))
            {
                Entity->Enemy.Minotaur.JumpAttackTimer.TimerHandle = -1;
                sscanf(LineBuffer,"jumpattacktimer %lf",&Entity->Enemy.Minotaur.JumpAttackTimer.TimerMax);
                Entity->Enemy.Minotaur.JumpAttackTimer.Name = "Jump attack";
            }
            else if(StartsWith(LineBuffer,"jumpattackimpacttimer"))
            {
                Entity->Enemy.Minotaur.JumpAttackImpactTimer.TimerHandle = -1;
                sscanf(LineBuffer,"jumpattackimpacttimer %lf",&Entity->Enemy.Minotaur.JumpAttackImpactTimer.TimerMax);
                Entity->Enemy.Minotaur.JumpAttackImpactTimer.Name = "Jump attack impact";
            }
            else if(StartsWith(LineBuffer,"impactcollisionextents"))
            {
                sscanf(LineBuffer,"impactcollisionextents %f %f", &Entity->Enemy.Minotaur.ImpactCollisionExtents.x, &Entity->Enemy.Minotaur.ImpactCollisionExtents.y);
            }
        }
        fclose(File);
    }
    
    if(Entity)
    {
        AI_FUNCS(Minotaur);
    }
}

static void LoadWraithData(game_state* GameState, i32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/wraith.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] : &GameState->Entities[GameState->EntityCount];
    
    Entity->Enemy.Skeleton = {};
    Entity->AnimationInfo.FreezeFrame = false;
    Entity->Dead = false;
    Entity->Enemy.Wraith.IsAttacking = false;
    
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
            if(StartsWith(LineBuffer, "#"))
            {
                break;
            }
            else if(StartsWith(LineBuffer,"attackcooldowntimer"))
            {
                Entity->Enemy.Skeleton.AttackCooldownTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"attackcooldowntimer %lf",&Entity->Enemy.Wraith.AttackCooldownTimer.TimerMax);
                Entity->Enemy.Wraith.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(LineBuffer,"chargingtimer"))
            {
                Entity->Enemy.Wraith.ChargingTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"chargingtimer %lf",&Entity->Enemy.Wraith.ChargingTimer.TimerMax);
                Entity->Enemy.Wraith.ChargingTimer.Name = "Charging";
            }
            else if(StartsWith(LineBuffer,"alertedtimer"))
            {
                Entity->Enemy.Skeleton.AlertedTimer.TimerHandle = -1;
                
                sscanf(LineBuffer,"alertedtimer %lf",&Entity->Enemy.Wraith.AlertedTimer.TimerMax);
                Entity->Enemy.Wraith.AlertedTimer.Name = "Alerted";
            }
        }
        fclose(File);
    }
    
    if(Entity)
    {
        AI_FUNCS(Wraith);
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
            if(StartsWith(LineBuffer, "#"))
            {
                break;
            }
            else if(StartsWith(LineBuffer,"explodestarttimer"))
            {
                Entity->Enemy.Blob.ExplodeStartTimer.TimerHandle = -1;
                sscanf(LineBuffer,"explodestarttimer %lf",&Entity->Enemy.Blob.ExplodeStartTimer.TimerMax);
                Entity->Enemy.Blob.ExplodeStartTimer.Name = "Explode Start";
            }
            else if(StartsWith(LineBuffer,"explodecountdowntimer"))
            {
                Entity->Enemy.Blob.ExplodeCountdownTimer.TimerHandle = -1;
                sscanf(LineBuffer,"explodecountdowntimer %lf",&Entity->Enemy.Blob.ExplodeCountdownTimer.TimerMax);
                Entity->Enemy.Blob.ExplodeCountdownTimer.Name = "ExplodeCountdown";
            }
            else if(StartsWith(LineBuffer,"explosioncollisionextents"))
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

void PlaceCheckpoint(game_state* GameState, sound_queue* SoundQueue, entity* Entity)
{
    auto CheckpointPos = glm::vec2(Entity->Position.x, Entity->Position.y - 0.5f);
    if(!GameState->CharacterData.HasCheckpoint)
    {
        LoadBonfireData(GameState,SoundQueue, -1,CheckpointPos, true);
        GameState->CharacterData.CheckpointHandle = GameState->EntityCount - 1;
    }
    else
    {
        DEBUG_PRINT("New area\n");
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

static void LoadPlayerData(game_state* GameState, sound_queue* SoundQueue, i32 Handle = -1, glm::vec2 Position = glm::vec2())
{
    FILE* File;
    File = fopen("../assets/entities/player.dat", "r");
    
    entity* Entity = Handle != -1 ? &GameState->Entities[Handle] :  &GameState->Entities[GameState->EntityCount];
    Entity->Type = Entity_Player;
    Entity->Player = {};
    Entity->Player.TargetedEnemyHandle = -1;
    Entity->Player.LastKnownDirectionX = 1.0f;
    Entity->Player.LastKnownDirectionY = 0;
    
    Entity->Player.RenderCrosshair = false;
    Entity->Player.IsAttacking = false;
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
                Entity->Player.AttackCooldownTimer.TimerMax = 0;
                sscanf(LineBuffer, "attackcooldowntimer %lf", &Entity->Player.AttackCooldownTimer.TimerMax);
                Entity->Player.AttackCooldownTimer.TimerHandle = -1;
                Entity->Player.AttackCooldownTimer.Name = "Attack Cooldown";
            }
            else if(StartsWith(LineBuffer, "lastattacktimer"))
            {
                sscanf(LineBuffer, "lastattacktimer %lf", &Entity->Player.LastAttackTimer.TimerMax);
                Entity->Player.LastAttackTimer.TimerHandle = -1;
                Entity->Player.LastAttackTimer.Name = "Last Attack";
            }
            else if(StartsWith(LineBuffer, "dashtimer"))
            {
                sscanf(LineBuffer, "dashtimer %lf", &Entity->Player.DashTimer.TimerMax);
                Entity->Player.DashTimer.TimerHandle = -1;
                Entity->Player.DashTimer.Name = "Dash";
            }
            else if(StartsWith(LineBuffer, "dashcooldowntimer"))
            {
                sscanf(LineBuffer, "dashcooldowntimer %lf", &Entity->Player.DashCooldownTimer.TimerMax);
                Entity->Player.DashCooldownTimer.TimerHandle = -1;
                Entity->Player.DashCooldownTimer.Name = "Dash Cooldown";
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
                sscanf(LineBuffer, "staminagaintimer %lf", &Entity->Player.StaminaGainTimer.TimerMax);
                Entity->Player.StaminaGainTimer.TimerHandle = -1;
                Entity->Player.StaminaGainTimer.Name = "Stamina Gain";
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
                Entity->Player.StaminaGainCooldownTimer.TimerMax = 0;
                sscanf(LineBuffer, "staminagaincooldowntimer %lf", &Entity->Player.StaminaGainCooldownTimer.TimerMax);
                Entity->Player.StaminaGainCooldownTimer.TimerHandle = -1;
                Entity->Player.StaminaGainCooldownTimer.Name = "Stamina Gain Cooldown";
            }
            else if(StartsWith(LineBuffer, "staminagaintimerfast"))
            {
                sscanf(LineBuffer, "staminagaintimerfast %lf", &Entity->Player.StaminaGainTimerFast);
            }
            else if(StartsWith(LineBuffer, "staminagaintimerslow"))
            {
                sscanf(LineBuffer, "staminagaintimerslow %lf", &Entity->Player.StaminaGainTimerSlow);
            }
            else if(StartsWith(LineBuffer, "checkpointplacementtimer"))
            {
                Entity->Player.CheckpointPlacementTimer.TimerMax = 0;
                sscanf(LineBuffer, "checkpointplacementtimer %lf", &Entity->Player.CheckpointPlacementTimer.TimerMax);
                Entity->Player.CheckpointPlacementTimer.TimerHandle = -1;
                Entity->Player.CheckpointPlacementTimer.Name = "Checkpoint Placement";
            }
            else if(StartsWith(LineBuffer, "checkpointplacementcooldowntimer"))
            {
                Entity->Player.CheckpointPlacementCooldownTimer.TimerMax = 0;
                sscanf(LineBuffer, "checkpointplacementcooldowntimer %lf", &Entity->Player.CheckpointPlacementCooldownTimer.TimerMax);
                Entity->Player.CheckpointPlacementCooldownTimer.TimerHandle = -1;
                Entity->Player.CheckpointPlacementCooldownTimer.Name = "Checkpoint Placement";
            }
            else if(StartsWith(LineBuffer, "healthpotiontimer"))
            {
                Entity->Player.HealthPotionTimer.TimerMax = 0;
                sscanf(LineBuffer, "healthpotiontimer %lf", &Entity->Player.HealthPotionTimer.TimerMax);
                Entity->Player.HealthPotionTimer.TimerHandle = -1;
                Entity->Player.HealthPotionTimer.Name = "Health Potion";
            }
        }
        fclose(File);
        
        LoadGame(GameState);
        if(Handle == -1)
        {
            if(GameState->CharacterData.HasCheckpoint)
            {
                Entity->Position = glm::vec2(GameState->CharacterData.CurrentCheckpoint.x - 0.5f,GameState->CharacterData.CurrentCheckpoint.y - 0.5f);
            }
            else
            {
                Entity->Position = Position;
                GameState->CharacterData.CurrentCheckpoint = Position;
                GameState->CharacterData.HasCheckpoint = true;
                
            }
            LoadBonfireData(GameState, SoundQueue, -1,GameState->CharacterData.CurrentCheckpoint, true);
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
            
            if(GameState->CharacterData.HasLostWill)
            {
                SpawnWillDrop(GameState,GameState->CharacterData.LostWillPosition,&GameState->CharacterData.LostWillObjectHandle);
            }
        }
        Entity->Player.Inventory.HasCheckpoint = true;
    }
}

static void CheckWillPickup(game_state* GameState, input_controller* InputController, object_entity* Will,entity* Player)
{
    if(GameState->CharacterData.RenderWillButtonHint && GetActionButtonDown(Action_Interact, InputController))
    {
        Player->Player.Will += GameState->CharacterData.LostWill;
        GameState->CharacterData.HasLostWill = false;
        
        GameState->CharacterData.LostWillPosition = glm::vec2();
        GameState->CharacterData.LostWill = 0;
        GameState->Objects[GameState->CharacterData.LostWillObjectHandle].Active = false;
        auto LightSourceHandle = GameState->Objects[GameState->CharacterData.LostWillObjectHandle].LightSourceHandle;
        GameState->LightSources[LightSourceHandle].Active = false;
        GameState->CharacterData.RenderWillButtonHint= false;
    }
}


static void CheckLootPickup(game_state* GameState, input_controller* InputController,loot* Loot, entity* Player)
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

void UpdatePlayer(entity* Entity, game_state* GameState, sound_queue* SoundQueue,input_controller* InputController, r64 DeltaTime)
{
    if(Entity->Hit)
    {
        Entity->Player.IsAttacking = false;
    }
    
    //Will
    if(GameState->CharacterData.IsInitialized && GameState->CharacterData.HasLostWill)
    {
        auto Will = GameState->Objects[GameState->CharacterData.LostWillObjectHandle];
        if(glm::distance(Entity->Position, Will.Position) < 1.5f)
        {
            GameState->CharacterData.RenderWillButtonHint = true;
            CheckWillPickup(GameState,InputController, &Will,Entity);
        }
        else
        {
            GameState->CharacterData.RenderWillButtonHint = false;
        }
    }
    
    
    // Loot
    if(GameState->CurrentLootCount > 0)
    {
        for(i32 Index = 0; Index < GameState->CurrentLootCount; Index++)
        {
            auto Loot = GameState->CurrentLoot[Index];
            if(Loot.Handle != -1 && glm::distance(Entity->Position, GameState->Objects[Loot.OwnerHandle].Position) < 1.5f)
            {
                GameState->CurrentLoot[Index].RenderButtonHint = true;
                CheckLootPickup(GameState,InputController, &Loot,Entity);
            }
            else
            {
                GameState->CurrentLoot[Index].RenderButtonHint = false;
            }
        }
    }
    
    r32 UsedWalkingSpeed = Entity->Player.WalkingSpeed;
    if(!TimerDone(GameState, Entity->StaggerCooldownTimer) || Entity->Player.TakingHealthPotion)
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
    
    if(Entity->Player.Stamina != GameState->CharacterData.Stamina)
    {
        if(TimerDone(GameState, Entity->Player.StaminaGainTimer))
        {
            Entity->Player.Stamina += 1;
            
            if(Entity->Player.Stamina < GameState->CharacterData.Stamina)
            {
                StartTimer(GameState, Entity->Player.StaminaGainTimer);
            }
            
            Entity->Player.Stamina = Min(Entity->Player.Stamina, (i16)GameState->CharacterData.Stamina);
        }
    }
    
    b32 UsingController = InputController->ControllerPresent;
    
    auto TempPos = glm::unProject(glm::vec3(InputController->MouseX, GameState->RenderState.Viewport[3] - InputController->MouseY, 0),
                                  GameState->Camera.ViewMatrix,
                                  GameState->Camera.ProjectionMatrix,
                                  glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    
    auto Pos = glm::vec2(TempPos.x, TempPos.y);
    
    auto DirectionToMouse = glm::normalize(glm::vec2(Pos.x - Entity->Position.x, Pos.y - Entity->Position.y));
    
    if (Entity->Active && Entity->Health > 0)
    {
        if(!Entity->Player.IsAttacking && !Entity->Hit && !Entity->Player.IsDashing && GetActionButtonDown(Action_Use, InputController) && Entity->Player.Inventory.HealthPotionCount > 0 && TimerDone(GameState,Entity->Player.HealthPotionTimer) && !Entity->Player.TakingHealthPotion)
        {
            //@Incomplete: Play some animation here for drinking!!!
            StartTimer(GameState,Entity->Player.HealthPotionTimer);
            Entity->Player.Inventory.HealthPotionCount--;
            Entity->Player.TakingHealthPotion = true;
        }
        
        if(Entity->Player.TakingHealthPotion && TimerDone(GameState, Entity->Player.HealthPotionTimer))
        {
            PLAY_SOUND(UseHealth);
            Entity->Health = Min((i16)GameState->CharacterData.Health, Entity->Health + 30);
            Entity->Player.TakingHealthPotion = false;
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
            
            if(!Entity->Player.IsAttacking && TimerDone(GameState, Entity->Player.AttackCooldownTimer))
            {
                r32 InputX = GetInputX(InputController);
                r32 InputY = GetInputY(InputController);
                
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
                            if(Moving)
                                PlayAnimation(Entity, "swordsman_walk_shield_up", GameState);
                            else
                                PlayAnimation(Entity, "swordsman_idle_shield_up", GameState);
                        }
                        else
                        {
                            if(Moving)
                                PlayAnimation(Entity, "swordsman_walk_shield_up", GameState);
                            else
                                PlayAnimation(Entity, "swordsman_idle_shield_up", GameState);
                        }
                    }
                    else
                    {
                        if(Moving)
                            PlayAnimation(Entity, "swordsman_walk_shield_up", GameState);
                        else
                            PlayAnimation(Entity, "swordsman_idle_shield_up", GameState);
                    }
                    
                    Entity->IsFlipped = Direction.x < 0;
                }
                else if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                {
                    auto ControllerXValue = UsingController ? InputX : 0.0f;
                    auto ControllerYValue = UsingController ? InputY : 0.0f;
                    
                    auto MouseXValue = DirectionToMouse.x;
                    auto MouseYValue = DirectionToMouse.y;
                    
                    PlayAnimation(Entity, "swordsman_walk_shield_up", GameState);
                    
                    if(Abs(ControllerXValue) < 0.7f && Abs(ControllerYValue) > 0.2f || Abs(MouseXValue) < 0.7f && Abs(MouseYValue) > 0.2f)
                    {
                        if(ControllerYValue > 0 || MouseYValue > 0)
                        {
                            //Entity->LookDirection = North;
                        }
                        else
                        {
                            //Entity->LookDirection = South;
                        }
                    }
                    else
                    {
                        //Entity->LookDirection = East;
                    }
                    
                    if(Abs(ControllerXValue) > InputController->ControllerDeadzone)
                    {
                        Entity->IsFlipped = ControllerXValue < 0;
                    }
                    else if(Abs(MouseXValue) > 0.01f)
                    {
                        Entity->IsFlipped = MouseXValue < 0;
                    }
                }
                else
                {
                    PlayAnimation(Entity, "swordsman_idle_shield_up", GameState);
                    if(Abs(Entity->Player.LastKnownDirectionX) < 0.3)
                    {
                        if(Entity->Player.LastKnownDirectionY > 0)
                        {
                            //Entity->LookDirection = North;
                            //PlayAnimation(Entity, "player_idle_up", GameState);
                        }
                        else
                        {
                            //Entity->LookDirection = South;
                            //PlayAnimation(Entity, "player_idle_down", GameState);
                        }
                    }
                    else
                    {
                        //Entity->LookDirection = East;
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
            
            //if(Entity->LookDirection == East && Entity->IsFlipped)
            //Entity->LookDirection = West;
            
        }
        
        if(Entity->Player.IsDashing && TimerDone(GameState, Entity->Player.DashTimer))
        {
            Entity->Player.IsDashing = false;
            StartTimer(GameState, Entity->Player.DashCooldownTimer);
        }
        
        if(!Entity->Player.IsDashing && TimerDone(GameState, Entity->Player.DashCooldownTimer) && !Entity->Player.IsAttacking  && TimerDone(GameState, Entity->Player.DashTimer) && GetActionButtonDown(Action_Dash, InputController)  && Entity->Player.Stamina >= Entity->Player.RollStaminaCost - Entity->Player.MinDiffStamina)
        {
            PLAY_SOUND(Dash);
            Entity->Player.IsDashing = true;
            
            glm::vec2 Direction = UsingController ? glm::normalize(glm::vec2(GetInputX(InputController), GetInputY(InputController))) : DirectionToMouse;
            
            if(Direction.x == Direction.x && Direction.y == Direction.y && (Direction.x != 0 || Direction.y != 0))
            {
                Entity->Player.LastKnownDirectionX = Direction.x;
                Entity->Player.LastKnownDirectionY = Direction.y;
            }
            
            Entity->Player.DashDirectionX = Entity->Player.LastKnownDirectionX;
            Entity->Player.DashDirectionY = Entity->Player.LastKnownDirectionY;
            StartTimer(GameState, Entity->Player.DashTimer);
            
            DecreaseStamina(Entity,GameState,Entity->Player.RollStaminaCost);
            StartTimer(GameState, Entity->Player.StaminaDecreaseTimer);
            
            PlayAnimation(Entity, "swordsman_roll", GameState);
        }
        else if(Entity->Player.Stamina < Entity->Player.RollStaminaCost - Entity->Player.MinDiffStamina)
        {
            ResetActionButtonQueue(InputController);
        }
        
        Entity->Invincible = Entity->Player.IsDashing;
        
        if(Entity->Player.IsDashing)
        {
            auto XInput = GetInputX(InputController);
            auto YInput = GetInputY(InputController);
            
            auto NewDirection = glm::normalize(glm::vec2(Entity->Player.DashDirectionX + XInput / Entity->Player.DashCounterDivider, Entity->Player.DashDirectionY + YInput / Entity->Player.DashCounterDivider));
            
            Entity->Player.DashDirectionX = NewDirection.x;
            Entity->Player.DashDirectionY = NewDirection.y;
            
            Entity->Velocity = glm::vec2(Entity->Player.DashDirectionX * Entity->Player.DashSpeed * DeltaTime, Entity->Player.DashDirectionY * Entity->Player.DashSpeed * DeltaTime);
        }
        
        if(TimerDone(GameState, Entity->Player.DashCooldownTimer) && !Entity->Player.IsDashing)
        {
            Entity->Player.DashCount = 0;
        }
        
        if(Entity->Player.IsChargingCheckpoint)
            Entity->Velocity = glm::vec2(Entity->Velocity.x * 0.5f, Entity->Velocity.y * 0.5f);
        
        
        if(GameState->CurrentLevel.Type == Level_Isometric)
        {
            if(Entity->Velocity.x == Entity->Velocity.x)
            {
                r32 DX = 0.0f;
                r32 DY = 0.0f;
                
                r32 Scale = 2.0f;
                
                if(Entity->Velocity.x > 0)
                {
                    DX = 1;
                }
                else if(Entity->Velocity.x < 0)
                {
                    DX = -1;
                }
                
                if(Entity->Velocity.y > 0)
                {
                    DY = 1;
                }
                else if(Entity->Velocity.y < 0)
                {
                    DY = -1;
                }
                
                if(DX < 0 && DY > 0)
                {
                    DX = 0;
                    Entity->LookDirection = NorthWest;
                }
                else if(DX == 0.0f && DY > 0)
                {
                    DX = 1.0f;
                    Scale = 1.0f;
                    Entity->LookDirection = North;
                }
                else if(DX > 0 && DY > 0)
                {
                    DY = 0.0f;
                    Scale = 1.0f;
                    Entity->LookDirection = NorthEast;
                }
                else if(DX > 0 && DY == 0.0f)
                {
                    DY = -1.0f;
                    Entity->LookDirection = East;
                }
                else if(DX > 0 && DY < 0)
                {
                    DX = 0.0f;
                    Entity->LookDirection = SouthEast;
                }
                else if(DX == 0.0f && DY < 0)
                {
                    DX = -1.0f;
                    Scale = 1.0f;
                    Entity->LookDirection = South;
                }
                else if(DX < 0 && DY < 0)
                {
                    DY = 0.0f;
                    Scale = 1.0f;
                    Entity->LookDirection = SouthWest;
                }
                else if(DX < 0 && DY == 0.0f)
                {
                    DY = 1.0f;
                    Entity->LookDirection = West;
                }
                
                r32 Length = glm::length(glm::vec2(DX, DY));
                
                glm::vec2 Direction;
                
                if(Length != 0.0f)
                {
                    Direction = glm::normalize(glm::vec2(DX, DY));
                }
                else
                {
                    Direction = glm::vec2(0, 0);
                }
                
                r32 Speed = Entity->Player.WalkingSpeed;
                if(!TimerDone(GameState, Entity->AttackMoveTimer))
                    Speed = Entity->AttackMoveSpeed;
                
                Entity->Position += glm::vec2(Direction.x / Scale * Speed * DeltaTime, Direction.y * Speed * DeltaTime);
            }
            
        }
        else
        {
            if(Entity->Velocity.x == Entity->Velocity.x)
                Entity->Position += Entity->Velocity;
        }
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        
        if(!Entity->Player.IsAttacking && !Entity->Player.IsChargingCheckpoint && !Entity->Player.IsDashing && GetActionButton(Action_Checkpoint, InputController) && TimerDone(GameState, Entity->Player.CheckpointPlacementCooldownTimer) && Entity->Player.Inventory.HasCheckpoint)
        {
            Entity->Player.IsChargingCheckpoint = true;
            StartTimer(GameState,Entity->Player.CheckpointPlacementTimer);
        }
        else if(Entity->Player.IsChargingCheckpoint && TimerDone(GameState,Entity->Player.CheckpointPlacementTimer) && GetActionButton(Action_Checkpoint, InputController))
        {
            Entity->Player.IsChargingCheckpoint = false;
            StartTimer(GameState,Entity->Player.CheckpointPlacementCooldownTimer);
            PlaceCheckpoint(GameState, SoundQueue,Entity);
            Entity->Player.Inventory.HasCheckpoint = false;
        }
        else if(!GetActionButton(Action_Checkpoint, InputController))
        {
            Entity->Player.IsChargingCheckpoint = false;
        }
        
        if(Entity->Player.IsAttacking)
        {
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex)
            {
                StartTimer(GameState, Entity->AttackMoveTimer);
            }
            
            if(Entity->AnimationInfo.FrameIndex >= Entity->AttackHighFrameIndex)
            {
                Entity->Player.IsAttacking = false;
                StartTimer(GameState, Entity->Player.AttackCooldownTimer);
                
                if(Entity->AttackCount == 3)
                {
                    Entity->AttackCount = 0;
                    Entity->Velocity = glm::vec2(0, 0);
                }
            }
        }
        
        //attacking
        if(!Entity->Player.IsChargingCheckpoint && TimerDone(GameState, Entity->Player.AttackCooldownTimer) &&
           TimerDone(GameState, Entity->StaggerCooldownTimer)
           && !Entity->Player.IsAttacking && Entity->Player.Stamina >= Entity->Player.AttackStaminaCost - Entity->Player.MinDiffStamina && (GetActionButtonDown(Action_Attack, InputController)))
        {
            switch(Entity->LookDirection)
            {
                case North:
                case NorthEast:
                case NorthWest:
                {
                    PlayAnimation(Entity, "swordsman_attack_up", GameState);
                }
                break;
                case South:
                case SouthEast:
                case SouthWest:
                {
                    PlayAnimation(Entity, "swordsman_attack_down", GameState);
                }
                break;
                case West:
                case East:
                {
                    PlayAnimation(Entity, "swordsman_attack_right", GameState);
                }
                break;
            }
            
            Entity->Player.IsAttacking = true;
            
            StartTimer(GameState, Entity->Player.LastAttackTimer);
            
            DecreaseStamina(Entity,GameState,Entity->Player.AttackStaminaCost);
            Entity->AttackCount++;
            
            i32 Ran = rand() % 2;
            if(Ran == 0)
            {
                PLAY_SOUND(SwordSlash01, 0.95f);
            }
            else
            {
                PLAY_SOUND(SwordSlash01, 0.85f);
            }
            
        }
        else if(Entity->Player.Stamina < Entity->Player.AttackStaminaCost - Entity->Player.MinDiffStamina)
        {
            
            ResetActionButtonQueue(InputController);
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
        if(GameState->CharacterData.LostWillObjectHandle != -1)
        {
            GameState->LightSources[GameState->CharacterData.LostWillObjectHandle].Active = false;
        }
        
        GameState->CharacterData = GameState->LastCharacterData;
        
        if(Entity->Player.Will > 0)
        {
            GameState->CharacterData.HasLostWill = true;
            GameState->LastCharacterData.HasLostWill = false;
            GameState->CharacterData.LostWill = Entity->Player.Will;
            GameState->CharacterData.LostWillPosition = Entity->Position;
        }
        else
        {
            GameState->CharacterData.HasLostWill = false;
            GameState->LastCharacterData.HasLostWill = false;
            GameState->CharacterData.LostWill = 0;
        }
        
        Entity->Player.Will = 0;
        Entity->Player.Inventory.HealthPotionCount = GameState->CharacterData.HealthPotionCount;
        SaveGame(GameState);
    }
    
    if(InputController->ActionRunning)
    {
        ResetActionButtonQueue(InputController);
    }
    
    InputController->ActionRunning = Entity->Player.IsAttacking || Entity->Player.IsDashing;
}

void UpdateWeapon(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
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
                case North:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetUp.x, WeaponColliderInfo.OffsetUp.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsUp.x, WeaponColliderInfo.ExtentsUp.y);
                }
                break;
                case South:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetDown.x, WeaponColliderInfo.OffsetDown.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsDown.x, WeaponColliderInfo.ExtentsDown.y);
                }
                break;
                case West:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetLeft.x, WeaponColliderInfo.OffsetLeft.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsLeft.x, WeaponColliderInfo.ExtentsLeft.y);
                }
                break;
                case East:
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
            switch(Entity->Enemy.EnemyType)
            {
                case Enemy_Skeleton:
                {
                    IsAttacking = Entity->Enemy.Skeleton.IsAttacking;
                }
                break;
                case Enemy_Minotaur:
                {
                    IsAttacking = Entity->Enemy.Minotaur.IsAttacking;
                }
                break;
                case Enemy_Wraith:
                {
                    IsAttacking = Entity->Enemy.Wraith.IsAttacking;
                }
                break;
            }
            
            auto WeaponColliderInfo = Entity->WeaponColliderInfo;
            
            switch(Entity->LookDirection)
            {
                case North:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetUp.x, WeaponColliderInfo.OffsetUp.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsUp.x, WeaponColliderInfo.ExtentsUp.y);
                }
                break;
                case South:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetDown.x, WeaponColliderInfo.OffsetDown.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsDown.x, WeaponColliderInfo.ExtentsDown.y);
                }
                break;
                case West:
                {
                    Entity->Weapon.CollisionAABB.Offset = glm::vec2(WeaponColliderInfo.OffsetLeft.x, WeaponColliderInfo.OffsetLeft.y);
                    Entity->Weapon.CollisionAABB.Extents = glm::vec2(WeaponColliderInfo.ExtentsLeft.x, WeaponColliderInfo.ExtentsLeft.y);
                }
                break;
                case East:
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
    
    if(IsAttacking && Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
    {
        hit_tile_extents HitExtents = Entity->HitExtents[Entity->LookDirection];
        
        for(i32 X = HitExtents.StartX; X < HitExtents.EndX; X++)
        {
            for(i32 Y = HitExtents.StartY; Y < HitExtents.EndY; Y++)
            {
                if(Entity->TilePosition.X + X >= 0 && Entity->TilePosition.Y + Y >= 0 && Entity->TilePosition.X + X < GameState->CurrentLevel.Tilemap.Width && Entity->TilePosition.Y + Y < GameState->CurrentLevel.Tilemap.Height)
                {
                    for(i32 EntityIndex = 0; EntityIndex < 20; EntityIndex++)
                    {
                        i32 Handle = GameState->EntityTilePositions[Entity->TilePosition.X + X][Entity->TilePosition.Y + Y].Entities[EntityIndex] - 1;
                        
                        if(Handle >= 0 && Handle < GameState->EntityCount)
                        {
                            entity* HitEntity = &GameState->Entities[Handle];
                            if(HitEntity->Active && !HitEntity->Dead && (Entity->Type == Entity_Enemy && HitEntity->Type == Entity_Player) || (Entity->Type == Entity_Player && HitEntity->Type == Entity_Enemy))
                                Hit(GameState, SoundQueue, Entity, HitEntity);
                        }
                    }
                }
            }
        }
        
        //collision_info CollisionInfo;
        //CheckWeaponCollision(GameState, &Entity->Weapon, &CollisionInfo);
        
        /*for(i32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
        if((Entity->Type == Entity_Player && CollisionInfo.Other[Index]->Type == Entity_Enemy && CollisionInfo.Other[Index]->Enemy.AIState != AI_Dying) ||
        (Entity->Type == Entity_Enemy && CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing))
        {
        if(Entity->AnimationInfo.FrameIndex >= Entity->AttackLowFrameIndex && Entity->AnimationInfo.FrameIndex <= Entity->AttackHighFrameIndex)
        {
        Hit(GameState, Entity, CollisionInfo.Other[Index]);
        }
        }
        }*/
    }
}

void UpdateAI(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
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

void UpdateBlob(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    
    UpdateAI(Entity,GameState, SoundQueue, DeltaTime);
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
                    Hit(GameState, SoundQueue, Entity, CollisionInfo.Other[Index]);
                }
            }
        }
    }
    
    Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
    Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
}

static void DetermineLoot(game_state* GameState, entity* Entity)
{
    b32 HasLoot = false;
    loot Loot;
    i32 RNG = rand() % 100;
    if(RNG > 90)
    {
        HasLoot = true;
        Loot.Type = Loot_Checkpoint;
    }
    else if(RNG > 0 && RNG < 10)
    {
        HasLoot = true;
        Loot.Type = Loot_LevelItem;
    }
    
    if(HasLoot)
    {
        Entity->Enemy.HasLoot = HasLoot;
        SpawnLoot(GameState, Entity->Position, &Loot.OwnerHandle);
        
        Loot.Handle = GameState->CurrentLootCount;
        GameState->CurrentLoot[GameState->CurrentLootCount++] = Loot;
    }
}

void UpdateSkeleton(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Skeleton = Entity->Enemy.Skeleton;
    
    auto Player = &GameState->Entities[0];
    Entity->RenderButtonHint = Entity->Enemy.HasLoot && Entity->Dead && glm::distance(Player->Position, Entity->Position) < 1.5f;
    
    if(Entity->Active && !Entity->Dead)
    {
        if(Entity->Hit)
        {
            if(Entity->Health <= 0)
            {
                Skeleton.IsAttacking = false;
                PlayAnimation(Entity, "skeleton_dead", GameState);
                Entity->AnimationInfo.FreezeFrame = true;
                Enemy.AIState = AI_Dying;
                SaveGame(GameState);
                DetermineLoot(GameState,Entity);
            }
            else if(strcmp(Entity->CurrentAnimation->Name, "skeleton_attack") != 0 && Enemy.AIState != AI_Dying)
            {
                PlayAnimation(Entity, "skeleton_hit", GameState);
                Enemy.AIState = AI_Hit;
                Entity->HitRecoilDirection = glm::normalize(Entity->Position - Player->Position);
                StartTimer(GameState, Entity->RecoilTimer);
            }
        }
        
        Entity->Velocity = glm::vec2(0,0); //@Cleanup: This is not good. Do this in AI
        
        UpdateAI(Entity,GameState, SoundQueue,DeltaTime);
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
        
        glm::vec2 Direction = glm::normalize(Player->Position - Entity->Position);
        
        if(Entity->Enemy.AIState != AI_Attacking && !Entity->Enemy.Skeleton.IsAttacking)
        {
            glm::vec2 Direction = glm::normalize(Player->Position - Entity->Position);
            
            if(Abs(Direction.x) < 0.6f)
            {
                if(Direction.y > 0)
                {
                    Entity->LookDirection = North;
                }
                else
                {
                    Entity->LookDirection = South;
                }
            }
            else
            {
                if(Direction.x < 0)
                    Entity->LookDirection = West;
                else
                    Entity->LookDirection = East;
            }
            
            Entity->IsFlipped = Direction.x < 0;
        }
        else if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            Entity->IsFlipped = Entity->Velocity.x < 0;
        }
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = glm::vec2(0,0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

void UpdateMinotaur(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Minotaur = Entity->Enemy.Minotaur;
    
    auto Player = &GameState->Entities[0];
    
    Entity->RenderButtonHint = Entity->Enemy.HasLoot &&  Entity->Dead && glm::distance(Player->Position, Entity->Position) < 1.5f;
    
    if(Entity->Active && !Entity->Dead)
    {
        if(Entity->Hit)
        {
            PLAY_SOUND(MinotaurHit);
            
            Enemy.TimesHit++;
            
            if(Entity->Health <= 0)
            {
                PLAY_SOUND(MinotaurDeath);
                
                PlayAnimation(Entity, "minotaur_death", GameState);
                Entity->AnimationInfo.FreezeFrame = true;
                Enemy.AIState = AI_Dying;
                SaveGame(GameState);
                DetermineLoot(GameState, Entity);
            }
            else if(strcmp(Entity->CurrentAnimation->Name, "minotaur_attack") != 0 && Enemy.AIState != AI_Dying)
            {
                PlayAnimation(Entity, "minotaur_detected", GameState);
                Enemy.AIState = AI_Hit;
                Minotaur.IsAttacking = false;
                Entity->HitRecoilDirection = glm::normalize(Entity->Position - Player->Position);
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
        
        UpdateAI(Entity,GameState, SoundQueue,DeltaTime);
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
        
        if(Entity->Enemy.AIState != AI_Attacking && !Entity->Enemy.Minotaur.IsAttacking)
        {
            glm::vec2 Direction = glm::normalize(Player->Position - Entity->Position);
            
            if(Abs(Direction.x) < 0.6f)
            {
                if(Direction.y > 0)
                {
                    Entity->LookDirection = North;
                }
                else
                {
                    Entity->LookDirection = South;
                }
            }
            else
            {
                if(Direction.x < 0)
                    Entity->LookDirection = West;
                else
                    Entity->LookDirection = East;
            }
            
            Entity->IsFlipped = Direction.x > 0;
        }
        else if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            Entity->IsFlipped = Entity->Velocity.x > 0;
        }
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = glm::vec2(0,0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

void UpdateWraith(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    auto& Enemy = Entity->Enemy;
    auto& Wraith = Entity->Enemy.Wraith;
    
    auto& Player = GameState->Entities[0];
    Entity->RenderButtonHint = Entity->Dead && glm::distance(Player.Position, Entity->Position) < 1.5f;
    
    if(Entity->Active && !Entity->Dead)
    {
        entity& Player = GameState->Entities[GameState->PlayerIndex];
        
        if(Entity->Hit)
        {
            if(Entity->Health <= 0)
            {
                Wraith.IsAttacking = false;
                PlayAnimation(Entity, "wraith_death", GameState);
                Entity->AnimationInfo.FreezeFrame = true;
                Enemy.AIState = AI_Dying;
            }
            else if(strcmp(Entity->CurrentAnimation->Name, "wraith_attack") != 0 && Enemy.AIState != AI_Dying)
            {
                //PlayAnimation(Entity, "skeleton_hit", GameState);
                Enemy.AIState = AI_Hit;
                Entity->HitRecoilDirection = glm::normalize(Entity->Position - Player.Position);
                StartTimer(GameState, Entity->RecoilTimer);
            }
        }
        
        Entity->Velocity = glm::vec2(0,0); //@Cleanup: This is not good. Do this in AI
        
        UpdateAI(Entity, GameState, SoundQueue, DeltaTime);
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
        
        glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
        
        if(Entity->Enemy.AIState != AI_Attacking && !Entity->Enemy.Wraith.IsAttacking)
        {
            glm::vec2 Direction = glm::normalize(Player.Position - Entity->Position);
            
            if(Abs(Direction.x) < 0.6f)
            {
                if(Direction.y > 0)
                {
                    Entity->LookDirection = North;
                }
                else
                {
                    Entity->LookDirection = South;
                }
            }
            else
            {
                if(Direction.x < 0)
                    Entity->LookDirection = West;
                else
                    Entity->LookDirection = East;
            }
            
            Entity->IsFlipped = Direction.x < 0;
        }
        else if(!TimerDone(GameState, Entity->AttackMoveTimer))
        {
            Entity->IsFlipped = Entity->Velocity.x < 0;
        }
        
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
        
        Entity->Velocity = glm::vec2(0,0);
        
        Entity->Hit = false;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        Entity->Enemy.Healthbar->CurrentFrame = 4 - Entity->Health;
    }
}

void UpdateBarrel(entity* Entity, game_state* GameState, sound_queue* SoundQueue, r64 DeltaTime)
{
    if(Entity->Active)
    {
        collision_info CollisionInfo;
        Entity->IsColliding = false;
        Entity->CollisionAABB.IsColliding = false;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        b32 HasHitEnemy = false;
        
        if(Entity->Velocity.x != 0 || Entity->Velocity.y != 0)
        {
            for(i32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
            {
                if(CollisionInfo.Other[Index]->Type == Entity_Enemy)
                {
                    Hit(GameState, SoundQueue, Entity, CollisionInfo.Other[Index]);
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
                PLAY_SOUND(BarrelBreak);
            }
        }
        
        if(!Entity->AnimationInfo.Playing)
        {
            Entity->Active = false;
        }
        
        auto& Player = GameState->Entities[0];
        
        Entity->RenderButtonHint = !Entity->IsKinematic && glm::distance(Player.Position, Entity->Position) < 1.5f;
        
        Entity->Position.x += Entity->Velocity.x * (r32)DeltaTime;
        Entity->Position.y += Entity->Velocity.y * (r32)DeltaTime;
    }
}

static void UpdateStaticEntity(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
}

void UpdateTilePosition(entity& Entity, game_state* GameState, r64 DeltaTime)
{
    r32 CartesianX = glm::floor(Entity.Position.x - 0.5f);
    r32 CartesianY = glm::ceil(Entity.Position.y - 0.5f);
    
    if(CartesianX >= 0 && CartesianX < GameState->CurrentLevel.Tilemap.Width && CartesianY >= 0 && CartesianY < GameState->CurrentLevel.Tilemap.Height)
    {
        Entity.TilePosition.X = (i32)CartesianX;
        Entity.TilePosition.Y = (i32)CartesianY;
        Assert(GameState->EntityTilePositions[Entity.TilePosition.X][Entity.TilePosition.Y].Count < 20);
        Entity.TilePosition.Z = GameState->EntityTilePositions[Entity.TilePosition.X][Entity.TilePosition.Y].Count++;
        
        GameState->EntityTilePositions[Entity.TilePosition.X][Entity.TilePosition.Y].Entities[Entity.TilePosition.Z] = Entity.EntityIndex + 1;
    }
}

void UpdateGeneral(entity* Entity, game_state* GameState, r64 DeltaTime)
{
    GameState->RenderState.RenderEntities[Entity->RenderEntityHandle].Rendered = glm::distance(Entity->Position, GameState->Entities[0].Position) < 15;
    
    GameState->EntityTilePositions[Entity->TilePosition.X][Entity->TilePosition.Y].Entities[Entity->TilePosition.Z] = 0;
    
    if(Entity->LightSourceHandle != -1)
    {
        GameState->LightSources[Entity->LightSourceHandle].Pointlight.Position = glm::vec2(Entity->Position.x - Entity->Center.x, Entity->Position.y);
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


void LightGlow(game_state* GameState, i32 LightHandle)
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

void UpdateObjects(game_state* GameState, r64 DeltaTime)
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

void UpdateEntities(game_state* GameState, input_controller* InputController, sound_queue* SoundQueue, r64 DeltaTime)
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
                        UpdatePlayer(Entity, GameState, SoundQueue, InputController, DeltaTime);
                        UpdateWeapon(Entity, GameState, SoundQueue, DeltaTime);
                    }
                }
                break;
                case Entity_Enemy:
                {
                    switch(Entity->Enemy.EnemyType)
                    {
                        case Enemy_Skeleton:
                        {
                            UpdateSkeleton(Entity, GameState, SoundQueue, DeltaTime);
                            
                            if(!Entity->Dead)
                            {
                                UpdateWeapon(Entity, GameState, SoundQueue, DeltaTime);
                            }
                        }
                        break;
                        case Enemy_Minotaur:
                        {
                            UpdateMinotaur(Entity, GameState, SoundQueue, DeltaTime);
                            
                            if(!Entity->Dead)
                            {
                                UpdateWeapon(Entity, GameState, SoundQueue, DeltaTime);
                            }
                        }
                        break;
                        case Enemy_Wraith:
                        {
                            UpdateWraith(Entity, GameState, SoundQueue, DeltaTime);
                        }
                        break;
                        case Enemy_Blob:
                        {
                            UpdateBlob(Entity, GameState, SoundQueue, DeltaTime);
                        }
                        break;
                    }
                }
                break;
                case Entity_Barrel:
                {
                    UpdateBarrel(Entity, GameState, SoundQueue, DeltaTime);
                }
                case Entity_Bonfire:
                {
                    UpdateStaticEntity(Entity, GameState, DeltaTime);
                    
                    LightGlow(GameState,Entity->LightSourceHandle);
                }
                break;
            }
            
            if(GameState->ClearTilePositionFrame)
            {
                for(i32 X = 0; X < GameState->CurrentLevel.Tilemap.Width; X++)
                {
                    for(i32 Y = 0; Y < GameState->CurrentLevel.Tilemap.Height; Y++)
                    {
                        GameState->EntityTilePositions[X][Y].Count = 0;
                    }
                }
            }
            else
            {
                if(Entity->Active && !Entity->Dead)
                    UpdateTilePosition(*Entity, GameState, DeltaTime);
            }
            
            if(Entity->Active && Entity->CurrentAnimation && Entity->AnimationInfo.Playing)
                TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
        }
        GameState->EntityPositions[EntityIndex] = Entity->Position;
    }
}