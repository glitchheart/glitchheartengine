#include "game.h"
#include "keycontroller.cpp"

#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "collision.cpp"
#include "entity.cpp"
#include "level.cpp"

#include "console.cpp"

#define DEBUG

//@Cleanup move this
void Kill(game_state* GameState, entity* Entity)
{
    GameState->RenderState.RenderEntities[Entity->RenderEntityHandle].Rendered = false;
    Entity->IsDead = true;
}

void CheckCollision(game_state* GameState, entity* Entity, collision_info* CollisionInfo)
{
    if(!Entity->IsKinematic && !Entity->IsDead)
    {
        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y);
        if(Entity->HitTrigger)
        {
            Entity->HitTrigger->Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y);
        }
        
        glm::vec2 PV;
        
        for(uint32 OtherEntityIndex = 0;
            OtherEntityIndex < GameState->EntityCount;
            OtherEntityIndex++)
        {
            entity* OtherEntity = &GameState->Entities[OtherEntityIndex];
            
            if(!(OtherEntity->Layer & Entity->IgnoreLayers) 
               && OtherEntity->EntityIndex != Entity->EntityIndex 
               && !OtherEntity->IsKinematic && !OtherEntity->IsDead)
            {
                if(OtherEntity->HitTrigger && OtherEntity->Type == Entity_Enemy && Entity->Type == Entity_PlayerWeapon)
                {
                    
                    collision_AABB MdHit;
                    MinkowskiDifference(OtherEntity->HitTrigger, &Entity->CollisionAABB, &MdHit);
                    if(MdHit.Min.x <= 0 &&
                       MdHit.Max.x >= 0 &&
                       MdHit.Min.y <= 0 &&
                       MdHit.Max.y >= 0)
                    {
                        OtherEntity->HitTrigger->IsColliding = true;
                        if(!OtherEntity->IsDead && GameState->Entities[GameState->PlayerIndex].Player.IsAttacking)
                        {
                            PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit01);
                            Kill(GameState, OtherEntity);
                        }
                    }
                    else 
                    {
                        OtherEntity->HitTrigger->IsColliding = false;
                    }
                }
                
                collision_AABB Md;
                MinkowskiDifference(&OtherEntity->CollisionAABB, &Entity->CollisionAABB, &Md);
                if(Md.Min.x <= 0 &&
                   Md.Max.x >= 0 &&
                   Md.Min.y <= 0 &&
                   Md.Max.y >= 0)
                {
                    Entity->IsColliding = true;
                    Entity->CollisionAABB.IsColliding = true;
                    
                    if(OtherEntity->Type == Entity_Enemy && Entity->Type == Entity_PlayerWeapon)
                    {
                        if(!OtherEntity->IsDead && GameState->Entities[GameState->PlayerIndex].Player.IsAttacking)
                        {
                            PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit01);
                            Kill(GameState, OtherEntity);
                        }
                    }
                    
                    if(!Entity->CollisionAABB.IsTrigger && !OtherEntity->CollisionAABB.IsTrigger)
                    {
                        OtherEntity->IsColliding = true;
                        OtherEntity->CollisionAABB.IsColliding = true;
                        
                        //calculate what side is colliding
                        auto OtherPosition = OtherEntity->CollisionAABB.Center;
                        auto OtherExtents = OtherEntity->CollisionAABB.Extents;
                        auto Position = Entity->CollisionAABB.Center;
                        auto Extents = Entity->CollisionAABB.Extents;
                        
                        AABBMin(&Md);
                        AABBMax(&Md);
                        AABBSize(&Md);
                        glm::vec2 PenetrationVector;
                        ClosestPointsOnBoundsToPoint(&Md, glm::vec2(0,0), &PenetrationVector);
                        
                        if(glm::abs(PenetrationVector.x) > glm::abs(PenetrationVector.y))
                        {
                            if(PenetrationVector.x > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                            else if(PenetrationVector.x < 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                        }
                        else
                        {
                            if(PenetrationVector.y > 0)
                                CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                            else if(PenetrationVector.y < 0) 
                                CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                        }
                        
                        if(PenetrationVector.x != 0)
                        {
                            PV.x = PenetrationVector.x;
                        }
                        
                        if(PenetrationVector.y != 0)
                        {
                            PV.y = PenetrationVector.y;
                        }
                    }
                    
                    switch(Entity->Type)
                    {
                        case Entity_Player:
                        {
                            if(OtherEntity->Pickup &&
                               GetKeyDown(Key_E,GameState) && Entity->Player.PickupCooldown <= 0.0)
                            {
                                Entity->Player.Pickup = &GameState->Entities[OtherEntityIndex];
                                Entity->Player.Pickup->Position = Entity->Position;
                                Entity->Player.Pickup->Velocity = glm::vec2(0.0f,0.0f);
                                // NOTE(niels): Need to make it kinematic, otherwise
                                // there will be an overlap when pressing E to drop
                                Entity->Player.Pickup->IsKinematic = true;
                                Entity->Player.PickupCooldown = 0.8;
                            }
                            
                        }
                        break;
                        case Entity_Barrel:
                        case Entity_Crosshair:
                        case Entity_Enemy:
                        break;
                    }
                    
                    if(!OtherEntity->CollisionAABB.IsTrigger &&
                       !Entity->CollisionAABB.IsTrigger && !Entity->IsStatic)
                    {
                        /*
                        Entity->Position += glm::vec2(PenetrationVector.x/XDivider,PenetrationVector.y/YDivider);
                        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->Velocity.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->Velocity.y);
                        Entity->Velocity = glm::vec2(0,0);*/
                    }
                }
                else 
                {
                    Entity->IsColliding = false;
                    Entity->CollisionAABB.IsColliding = false;
                }
            }
        }
        
        
        if(Entity->Type == Entity_Player || Entity->Type == Entity_Enemy || Entity->Type == Entity_Barrel)
        {
            level* Level = &GameState->CurrentLevel;
            
            int32 XPos = (int32)(Entity->Position.x + Entity->Center.x * Entity->Scale.x);
            int32 YPos = (int32)(Entity->Position.y + Entity->Center.y * Entity->Scale.y);
            
            //@Improvement Is it necessary to go 2 tiles out?
            uint32 MinX = Max(0, XPos - 2);
            uint32 MaxX = Min((int32)Level->Tilemap.Width, XPos + 2);
            uint32 MinY = Max(0, YPos - 2);
            uint32 MaxY = Min((int32)Level->Tilemap.Height, YPos + 2);
            
            //check tile collision
            for(uint32 X = MinX; X < MaxX; X++)
            {
                for(uint32 Y = MinY; Y < MaxY; Y++)
                {
                    tile_data Tile = Level->Tilemap.Data[X][Y];
                    
                    if(Tile.IsSolid)
                    {
                        //@Cleanup we have to move this to a separate function, because it is used in entity collision and tile collision
                        collision_AABB Md;
                        MinkowskiDifference(&Tile.CollisionAABB, &Entity->CollisionAABB, &Md);
                        if(Md.Min.x <= 0 &&
                           Md.Max.x >= 0 &&
                           Md.Min.y <= 0 &&
                           Md.Max.y >= 0)
                        {
                            Entity->IsColliding = true;
                            Entity->CollisionAABB.IsColliding = true;
                            
                            //calculate what side is colliding
                            auto OtherPosition = Tile.CollisionAABB.Center;
                            auto OtherExtents = Tile.CollisionAABB.Extents;
                            auto Position = Entity->CollisionAABB.Center;
                            auto Extents = Entity->CollisionAABB.Extents;
                            
                            AABBMin(&Md);
                            AABBMax(&Md);
                            AABBSize(&Md);
                            glm::vec2 PenetrationVector;
                            ClosestPointsOnBoundsToPoint(&Md, glm::vec2(0,0), &PenetrationVector);
                            
                            if(glm::abs(PenetrationVector.x) > glm::abs(PenetrationVector.y))
                            {
                                if(PenetrationVector.x > 0)
                                    CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                                else if(PenetrationVector.x < 0)
                                    CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                            }
                            else
                            {
                                if(PenetrationVector.y < 0)
                                    CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                                else if(PenetrationVector.y > 0) 
                                    CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                            }
                            
                            if(PenetrationVector.x != 0)
                            {
                                PV.x = PenetrationVector.x;
                            }
                            
                            if(PenetrationVector.y != 0)
                            {
                                PV.y = PenetrationVector.y;
                            }
                            
                            if(Entity->Type == Entity_Barrel)
                            {
                                Entity->Velocity = glm::vec2(0.0f,0.0f);
                            }
                        }
                    }
                }
            }
        }
        
        if(!Entity->CollisionAABB.IsTrigger)
            Entity->Position += PV;
    }
}

void UpdateEntities(game_state* GameState, real64 DeltaTime)
{
    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX,GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                              GameState->Camera.ViewMatrix,
                              GameState->Camera.ProjectionMatrix,
                              glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    
    for(uint32 EntityIndex = 0;
        EntityIndex < GameState->EntityCount;
        EntityIndex++)
    {
        entity* Entity = &GameState->Entities[EntityIndex];
        
        switch(Entity->Type)
        {
            case Entity_Player: 
            {
                if (!GameState->Console.Open)
                {
                    Entity->Velocity = glm::vec2(0,0);
                    
                    //player movement
                    if (GetKey(Key_Left, GameState))
                    {
                        Entity->Velocity.x = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                        Entity->IsFlipped = true;
                    }
                    else if (GetKey(Key_Right, GameState))
                    {
                        Entity->Velocity.x = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                        Entity->IsFlipped = false;
                    }
                    
                    if (GetKey(Key_Up, GameState))
                    {
                        Entity->Velocity.y = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    else if (GetKey(Key_Down, GameState))
                    {
                        Entity->Velocity.y = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    
                    if(Entity->Player.IsAttacking && !Entity->AnimationInfo.Playing)
                    {
                        Entity->Player.IsAttacking = false;
                    }
                    
                    if(!Entity->Player.IsAttacking)
                    {
                        if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                            PlayAnimation(Entity, &GameState->PlayerWalkAnimation);
                        else
                            PlayAnimation(Entity, &GameState->PlayerIdleAnimation);
                    }
                    
                    if(GetKeyDown(Key_E,GameState) && Entity->Player.Pickup)
                    {
                        Entity->Player.Pickup->IsKinematic = false;
                        real32 ThrowingDir = Entity->IsFlipped ? -1.0f : 1.0f;
                        glm::vec2 Throw;
                        if(Entity->Velocity.x == 0.0f && Entity->Velocity.y == 0.0f)
                        {
                            Throw.x = Entity->Player.ThrowingSpeed * ThrowingDir;
                            Throw.y = 0.0f;
                        }
                        if(Entity->Velocity.x > 0)
                        {
                            Throw.x = Entity->Player.ThrowingSpeed;
                        } 
                        else if(Entity->Velocity.x < 0)
                        {
                            Throw.x = -Entity->Player.ThrowingSpeed;
                        }
                        
                        if(Entity->Velocity.y > 0)
                        {
                            Throw.y = Entity->Player.ThrowingSpeed;
                        }
                        else if(Entity->Velocity.y < 0)
                        {
                            Throw.y = -Entity->Player.ThrowingSpeed;
                        }
                        
                        Throw.x = glm::abs(Throw.y) > 0 ? 0.5f * Throw.x : Throw.x;
                        Throw.y = glm::abs(Throw.x) > 0 ? 0.5f * Throw.y : Throw.y;
                        Entity->Player.Pickup->Velocity = glm::vec2(Throw.x,Throw.y);
                        printf("Throw.x: %f, Throw.y: %f\n",Throw.x,Throw.y);
                        Entity->Player.Pickup = NULL;
                        Entity->Player.PickupCooldown = 0.8;
                    }
                    
                    if(Entity->Player.PickupCooldown > 0.0)
                    {
                        Entity->Player.PickupCooldown -= DeltaTime;
                    }
                    
                    Entity->Position += Entity->Velocity;
                    
                    collision_info CollisionInfo;
                    CheckCollision(GameState, Entity, &CollisionInfo);
                    
                    if(Entity->Player.Pickup)
                    {
                        Entity->Player.Pickup->Position = Entity->Position;
                    }
                    
                    //attacking
                    if(!Entity->Player.IsAttacking && GetKeyDown(Key_Z, GameState))
                    {
                        PlayAnimation(Entity, &GameState->PlayerAttackAnimation);
                        Entity->Player.IsAttacking = true;
                        PlaySoundEffect(GameState, &GameState->SoundManager.SwordSlash01);
                    }
                    
                    if(Entity->AnimationInfo.Playing)
                        TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation,DeltaTime);
                    
                    auto Direction = glm::vec2(pos.x, pos.y) - Entity->Position;
                    Direction = glm::normalize(Direction);
                    float Degrees = atan2(Direction.y, Direction.x);
                    
                    if(!GameState->EditorUI.On)
                        GameState->Camera.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
                }
            }
            break;
            case Entity_PlayerWeapon:
            {
                glm::vec2 Pos = GameState->Entities[GameState->PlayerIndex].Position;
                if(GameState->Entities[GameState->PlayerIndex].IsFlipped)
                    Entity->Position = glm::vec2(Pos.x - 0.25f, Pos.y + 0.6f);
                else
                    Entity->Position = glm::vec2(Pos.x + 1.2f, Pos.y + 0.6f);
                
                Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y);
                
                collision_info CollisionInfo;
                CheckCollision(GameState, Entity, &CollisionInfo);
            }
            break;
            case Entity_Crosshair:
            {
                if(!GameState->EditorUI.On)
                {
                    Entity->Position = glm::vec2(pos.x - 0.5f, pos.y - 0.5f);
                    Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
                }
            }
            break;
            case Entity_Enemy:
            {
                if(!Entity->IsDead)
                {
                    entity Player = GameState->Entities[GameState->PlayerIndex];
                    real64 DistanceToPlayer = glm::distance(Entity->Position, Player.Position);
                    switch(Entity->Enemy.AIState)
                    {
                        case AI_Sleeping:
                        {
                            PlayAnimation(Entity, &GameState->PlayerIdleAnimation);
                        }
                        break;
                        case AI_Idle:
                        {
                            PlayAnimation(Entity, &GameState->PlayerIdleAnimation);
                            if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
                            {
                                Entity->Enemy.AIState = AI_Following;
                                PlayAnimation(Entity, &GameState->PlayerWalkAnimation);
                            }
                        }
                        break;
                        case AI_Alerted:
                        {}
                        break;
                        case AI_Following:
                        {
                            //TODO(Daniel) here the pathfinding should happen
                            if(DistanceToPlayer > Entity->Enemy.MaxAlertDistance)
                            {
                                Entity->Enemy.AIState = AI_Idle;
                            }
                            else if(DistanceToPlayer < Entity->Enemy.MinDistance)
                            {
                                Entity->Enemy.AIState = AI_Attacking;
                            }
                            else
                            {
                                glm::vec2 Direction = Player.Position - Entity->Position;
                                Direction = glm::normalize(Direction);
                                Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.WalkingSpeed * DeltaTime, Direction.y * Entity->Enemy.WalkingSpeed * DeltaTime);
                                
                                Entity->IsFlipped = Entity->Velocity.x < 0;
                            }
                        }
                        break;
                        case AI_Attacking:
                        {
                            if(Entity->Enemy.AttackCooldownCounter == 0)
                            {
                                Entity->Enemy.IsAttacking = true;
                                PlayAnimation(Entity, &GameState->PlayerAttackAnimation);
                                PlaySoundEffect(GameState, &GameState->SoundManager.SwordSlash01);
                            }
                            else if(DistanceToPlayer > Entity->Enemy.MinDistance)
                            {
                                Entity->Enemy.IsAttacking = false;
                                Entity->Enemy.AIState = AI_Idle;
                            }
                            else if(!Entity->AnimationInfo.Playing)
                            {
                                PlayAnimation(Entity, &GameState->PlayerIdleAnimation);
                            }
                            
                            Entity->Enemy.AttackCooldownCounter += DeltaTime;
                            
                            if(Entity->Enemy.AttackCooldownCounter >= Entity->Enemy.AttackCooldown)
                            {
                                Entity->Enemy.AttackCooldownCounter = 0;
                                Entity->Enemy.IsAttacking = true;
                            }
                        }
                        break;
                    }
                }
                //Finish the cooldown although we are not in attack-mode. This prevents the enemy from attacking
                //too quickly after the previous attack if switching between states quickly.
                if(Entity->Enemy.AIState != AI_Attacking && Entity->Enemy.AttackCooldownCounter != 0)
                {
                    Entity->Enemy.AttackCooldownCounter += DeltaTime;
                    if(Entity->Enemy.AttackCooldownCounter >= Entity->Enemy.AttackCooldown)
                        Entity->Enemy.AttackCooldownCounter = 0;
                }
                
                Entity->Position.x += Entity->Velocity.x;
                Entity->Position.y += Entity->Velocity.y;
                
                //@Cleanup move this somewhere else, maybe out of switch
                render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
                RenderEntity->Color = glm::vec4(0, 1, 0, 1);
                
                if(Entity->AnimationInfo.Playing)
                    TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
                
                Entity->Velocity = glm::vec2(0,0);
                
                collision_info CollisionInfo;
                CheckCollision(GameState, Entity, &CollisionInfo);
                
                if(!Entity->IsColliding)
                {
                    
                }
            }
            break;
            case Entity_Barrel:
            {
                collision_info CollisionInfo;
                Entity->IsColliding = false;
                Entity->CollisionAABB.IsColliding = false;
                CheckCollision(GameState, Entity, &CollisionInfo);
                
                real32 XVel = 0.0f;
                real32 YVel = 0.0f;
                
                real32 ThrowDiff = 0.2f;
                if(Entity->Velocity.x > ThrowDiff)
                {
                    Entity->Position += glm::vec2(Entity->Velocity.x * DeltaTime,Entity->Velocity.y * DeltaTime);
                    XVel = Entity->Velocity.x - ThrowDiff;
                }
                else if(Entity->Velocity.x < -ThrowDiff)
                {
                    Entity->Position +=glm::vec2(Entity->Velocity.x * DeltaTime,Entity->Velocity.y * DeltaTime);
                    XVel = Entity->Velocity.x + ThrowDiff;
                }
                else 
                {
                    XVel = 0.0f;
                }
                
                if(Entity->Velocity.y > ThrowDiff)
                {
                    Entity->Position +=glm::vec2(Entity->Velocity.x * DeltaTime,Entity->Velocity.y * DeltaTime);
                    YVel = Entity->Velocity.y - ThrowDiff;
                }
                else if(Entity->Velocity.y < -ThrowDiff)
                {
                    Entity->Position +=glm::vec2(Entity->Velocity.x * DeltaTime,Entity->Velocity.y * DeltaTime);
                    YVel = Entity->Velocity.y + ThrowDiff;
                }
                else 
                {
                    YVel = 0.0f;
                }
                
                if(glm::abs(XVel) > 0 || glm::abs(YVel) > 0) {
                    printf("Vel.x: %f, Vel.y: %f\n",XVel, YVel);
                }
                
                Entity->Velocity = glm::vec2(XVel,YVel);
                
            }
        }
    }
    
    switch(GameState->EditorUI.State)
    {
        case State_EntityList:
        {
            auto entity = GameState->Entities[GameState->EditorUI.SelectedIndex];
            GameState->Camera.Center = glm::vec2(entity.Position.x, entity.Position.y);
        }
        break;
    }
    
    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f,
                                                    static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom),
                                                    static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom),
                                                    0.0f,
                                                    -1.0f,
                                                    1.0f);
    
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(-GameState->Camera.Center.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2,
                                                            -GameState->Camera.Center.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2,
                                                            0));
}

extern "C" UPDATE(Update)
{
    if(!GameState->IsInitialized)
    {
        LoadAnimations(GameState);
        
        InitPlayer(GameState);
        //InitCrosshair(GameState);
        
        LoadLevelFromFile("../assets/levels/level_02.plv", &GameState->CurrentLevel, GameState);
        
        //@Cleanup this should be in the level file
        SpawnMillionBarrels(GameState);
        
        GameState->Camera.Zoom = 2.5f;
        GameState->Camera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
        GameState->Camera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
        
        GameState->GameMode = Mode_InGame;
        InitCommands();
        GameState->IsInitialized = true;
    }
    
#ifdef DEBUG
    if(GetKeyDown(Key_F1, GameState))
    {
        GameState->RenderState.RenderColliders = !GameState->RenderState.RenderColliders;
    }
    
    if(GetKeyDown(Key_F2, GameState))
    {
        GameState->RenderState.RenderFPS = !GameState->RenderState.RenderFPS;
    }
    
#endif
    
    if (GetKeyDown(Key_Escape, GameState) && !GameState->Console.Open)
    {
        switch(GameState->GameMode)
        {
            case Mode_MainMenu:
            {
                //StopSoundEffect(GameState, &GameState->SoundManager.MainMenuTrack);
                GameState->GameMode = Mode_InGame;
            }
            break;
            case Mode_InGame:
            {
                //PlaySoundEffect(GameState, &GameState->SoundManager.MainMenuTrack);
                GameState->GameMode = Mode_MainMenu;
            }
            break;
        }
    }
    
    if(GameState->GameMode == Mode_MainMenu)
    {
        if(GetKeyDown(Key_Up, GameState))
            GameState->MainMenu.SelectedIndex -= 1;
        else if(GetKeyDown(Key_Down, GameState))
            GameState->MainMenu.SelectedIndex += 1;
        
        if(GameState->MainMenu.SelectedIndex < 0)
            GameState->MainMenu.SelectedIndex = GameState->MainMenu.OptionCount - 1;
        else if(GameState->MainMenu.SelectedIndex == (int32)GameState->MainMenu.OptionCount)
            GameState->MainMenu.SelectedIndex = 0;
        
        if(GetKeyDown(Key_Enter, GameState))
        {
            char* Selection = GameState->MainMenu.Options[GameState->MainMenu.SelectedIndex];
            
            if(strcmp(Selection, "Exit") == 0) //TODO(Daniel) do an enumeration instead
            {
                GameState->GameMode = Mode_Exit;
            }
            else if(strcmp(Selection, "Continue") == 0)
            {
                GameState->GameMode = Mode_InGame;
            }
            GameState->MainMenu.SelectedIndex = 0;
        }
    }
    
    UpdateEntities(GameState, DeltaTime);
    CheckConsoleInput(GameState, DeltaTime);
}
