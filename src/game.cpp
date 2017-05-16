#include "game.h"
#include "keycontroller.cpp"

#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "entity.cpp"
#include "collision.cpp"
#include "level.cpp"

#include "console.cpp"

#define DEBUG

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
                        
                        Throw.x = Abs(Throw.y) > 0 ? 0.5f * Throw.x : Throw.x;
                        Throw.y = Abs(Throw.x) > 0 ? 0.5f * Throw.y : Throw.y;
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
                entity* Player = &GameState->Entities[GameState->PlayerIndex];
                render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
                
                RenderEntity->Color = glm::vec4(1, 1, 1, 1);
                
                glm::vec2 Pos = Player->Position;
                
                Entity->IsFlipped = Player->IsFlipped;
                
                if(Player->IsFlipped)
                    Entity->Position = glm::vec2(Pos.x - 2.0f, Pos.y - 1.5f);
                else
                    Entity->Position = glm::vec2(Pos.x, Pos.y - 1.5f);
                
                Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y);
                
                collision_info CollisionInfo;
                CheckCollision(GameState, Entity, &CollisionInfo);
                
                if(Player->Player.IsAttacking && !Entity->AnimationInfo.Playing)
                {
                    Entity->CurrentAnimation = 0;
                    PlayAnimation(Entity, &GameState->SwordTopRightAnimation);
                    RenderEntity->Rendered = true;
                }
                else if(!Entity->AnimationInfo.Playing)
                {
                    RenderEntity->Rendered = false;
                }
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
                
                if(Abs(XVel) > 0 || Abs(YVel) > 0) {
                    printf("Vel.x: %f, Vel.y: %f\n",XVel, YVel);
                }
                
                Entity->Velocity = glm::vec2(XVel,YVel);
                
            }
        }
        
        if(Entity->CurrentAnimation && Entity->AnimationInfo.Playing)
            TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
        
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
