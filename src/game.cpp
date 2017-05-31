#include "game.h"
#include "keycontroller.cpp"
#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "entity.cpp"
#include "collision.cpp"
#include "level.cpp"
#include "console.cpp"
#include "ai.cpp"

#define DEBUG

void UpdatePlayer(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX,GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                              GameState->GameCamera.ViewMatrix,
                              GameState->GameCamera.ProjectionMatrix,
                              glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    
    if(Entity->Player.CurrentDashCooldownTime > 0)
        Entity->Player.CurrentDashCooldownTime -= DeltaTime;
    
    if (!GameState->Console.Open)
    {
        // Set the last know direction for dash direction later
        if(Entity->Velocity.x != 0 || Entity->Velocity.y != 0)
        {
            glm::vec2 Direction = glm::normalize(Entity->Velocity);
            Entity->Player.LastKnownDirectionX = Direction.x;
            Entity->Player.LastKnownDirectionY = Direction.y;
        }
        
        if(!Entity->Player.IsAttacking && Entity->Player.CurrentDashCooldownTime <= 0 && !Entity->Player.IsDashing && GetActionButtonDown(Action_Dash, GameState))
        {
            PlaySoundEffect(GameState, &GameState->SoundManager.Dash);
            Entity->Player.IsDashing = true;
        }
        
        if(!Entity->Player.IsDashing)
        {
            if(Entity->Player.IsAttacking && !Entity->AnimationInfo.Playing)
            {
                Entity->Player.IsAttacking = false;
                Entity->Player.CurrentAttackCooldownTime = Entity->Player.AttackCooldown;
            }
            
            if(!Entity->Player.IsAttacking)
            {
                Entity->Velocity.x = GetInputX(GameState) * Entity->Player.WalkingSpeed * (real32)DeltaTime;
                Entity->Velocity.y = GetInputY(GameState) * Entity->Player.WalkingSpeed * (real32)DeltaTime;
                
                if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                {
                    if(Entity->Velocity.x < 0.02 && Entity->Velocity.x > -0.02)
                    {
                        if(Entity->Velocity.y < 0)
                            PlayAnimation(Entity, &GameState->PlayerWalkUpAnimation);
                        else
                            PlayAnimation(Entity, &GameState->PlayerWalkDownAnimation);
                    }
                    else
                        PlayAnimation(Entity, &GameState->PlayerWalkAnimation);
                }
                else
                {
                    PlayAnimation(Entity, &GameState->PlayerIdleAnimation);
                }
                
                if(Entity->Velocity.x != 0)
                    Entity->IsFlipped = Entity->Velocity.x < 0;
            }
            else
                Entity->Velocity = glm::vec2(0,0);
            
            if(GetActionButtonDown(Action_Interact, GameState) && Entity->Player.Pickup)
            {
                Entity->Player.Pickup->IsKinematic = false;
                real32 ThrowingDir = Entity->IsFlipped ? -1.0f : 1.0f;
                glm::vec2 Throw;
                
                glm::vec2 Dir = glm::normalize(Entity->Velocity);
                
                if(Entity->Velocity.x == 0.0f && Entity->Velocity.y == 0.0f)
                {
                    Throw.x = Entity->Player.ThrowingSpeed * ThrowingDir;
                    Throw.y = 0.0f;
                }
                else
                {
                    Throw.x = Dir.x * Entity->Player.ThrowingSpeed;
                    Throw.y = Dir.y * Entity->Player.ThrowingSpeed;
                }
                
                Entity->Player.Pickup->Velocity = Throw;
                Entity->Player.Pickup = NULL;
                Entity->Player.PickupCooldown = 0.8;
            }
        }
        else if(Entity->Player.IsDashing)
        {
            if(Entity->Player.CurrentDashTime == 0)
            {
                Entity->Player.CurrentDashCooldownTime = Entity->Player.DashCooldown;
                Entity->Player.CurrentDashTime = Entity->Player.MaxDashTime;
            }
            
            if(Entity->Player.CurrentDashTime > 0)
            {
                Entity->Velocity = glm::vec2(Entity->Player.LastKnownDirectionX * Entity->Player.DashSpeed * DeltaTime, Entity->Player.LastKnownDirectionY * Entity->Player.DashSpeed * DeltaTime);
                Entity->Player.CurrentDashTime -= DeltaTime;
            }
            else
            {
                Entity->Player.CurrentDashTime = 0;
                Entity->Player.IsDashing = false;
            }
        }
        
        if(Entity->Player.PickupCooldown > 0.0)
        {
            Entity->Player.PickupCooldown -= DeltaTime;
        }
        
        Entity->Position += Entity->Velocity;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        entity* OtherEntity;
        bool32 OtherFound = false;
        
        for(int Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            if(CollisionInfo.Other[Index]->Pickup)
            {
                OtherEntity = CollisionInfo.Other[Index];
                OtherFound = true;
                break;
            }
        }
        
        if(OtherFound && OtherEntity->Pickup &&
           GetActionButtonDown(Action_Interact, GameState) && Entity->Player.PickupCooldown <= 0.0)
        {
            Entity->Player.Pickup = OtherEntity;
            Entity->Player.Pickup->Position = Entity->Position;
            Entity->Player.Pickup->Velocity = glm::vec2(0.0f,0.0f);
            // NOTE(niels): Need to make it kinematic, otherwise
            // there will be an overlap when pressing E to drop
            Entity->Player.Pickup->IsKinematic = true;
            Entity->Player.PickupCooldown = 0.8;
        }
        
        if(Entity->Player.Pickup)
        {
            Entity->Player.Pickup->Position = Entity->Position;
        }
        
        //attacking
        if(Entity->Player.CurrentAttackCooldownTime <= 0 && !Entity->Player.IsAttacking && (GetActionButtonDown(Action_Attack, GameState) || GetJoystickKeyDown(Joystick_3, GameState)))
        {
            PlayAnimation(Entity, &GameState->PlayerAttackAnimation);
            Entity->Player.IsAttacking = true;
            PlaySoundEffect(GameState, &GameState->SoundManager.SwordSlash01);
        }
        
        if(!Entity->Player.IsAttacking && Entity->Player.CurrentAttackCooldownTime > 0)
            Entity->Player.CurrentAttackCooldownTime -= DeltaTime;
        
        auto Direction = glm::vec2(pos.x, pos.y) - Entity->Position;
        Direction = glm::normalize(Direction);
        float Degrees = atan2(Direction.y, Direction.x);
        
        if(!GameState->EditorUI.On)
            GameState->GameCamera.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
    }
}

void UpdatePlayerWeapon(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    entity* Player = &GameState->Entities[GameState->PlayerIndex];
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
    
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    glm::vec2 Pos = Player->Position;
    
    Entity->IsFlipped = Player->IsFlipped;
    
    Entity->CollisionAABB.Extents = glm::vec2(0.7f, 0.7f);
    
    if(Player->Player.LastKnownDirectionX == 0 && Player->Player.LastKnownDirectionY != 0)
    {
        if(Player->Player.LastKnownDirectionY < 0)
        {
            Entity->CollisionAABB.Offset = glm::vec2(0, -0.2f);
            Entity->Position = glm::vec2(Pos.x, Pos.y - 1.2f);
        }
        else if(Player->Player.LastKnownDirectionY > 0)
        {
            Entity->CollisionAABB.Offset = glm::vec2(0, 0.2f);
            Entity->Position = glm::vec2(Pos.x, Pos.y + 1.4f);
        }
    }
    else
    {
        if(Player->IsFlipped)
        {
            Entity->CollisionAABB.Offset = glm::vec2(-0.2f, 0);
            Entity->Position = glm::vec2(Pos.x - 1.2f, Pos.y);
        }
        else
        {
            Entity->CollisionAABB.Offset = glm::vec2(0.2f, 0);
            Entity->Position = glm::vec2(Pos.x + 1.2f, Pos.y);
        }
    }
    
    Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->CollisionAABB.Offset.y);
    
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
    
    if(GameState->Entities[GameState->PlayerIndex].Player.IsAttacking)
    {
        for(uint32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            if(CollisionInfo.Other[Index]->Type == Entity_Enemy && CollisionInfo.Other[Index]->Enemy.AIState != AI_Hit)
            {
                PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit01);
                Hit(GameState, CollisionInfo.Other[Index]);
            }
        }
    }
    
    if(Player->Player.IsAttacking && !Entity->AnimationInfo.Playing)
    {
        Entity->CurrentAnimation = 0;
        PlayAnimation(Entity, &GameState->SwordAttackAnimation);
        RenderEntity->Rendered = true;
    }
    else if(!Player->Player.IsAttacking || !Entity->AnimationInfo.Playing)
    {
        RenderEntity->Rendered = false;
    }
}

void UpdateEnemy(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    Entity->Velocity = glm::vec2(0,0);
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
                glm::vec2 EntityPosition = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x,Entity->Position.y + Entity->Center.y * Entity->Scale.y);
                if(Entity->Enemy.AStarCooldown <= 0.0f || !Entity->Enemy.AStarPath || (Entity->Enemy.AStarPathLength <= Entity->Enemy.PathIndex && DistanceToPlayer >= 2.0f)) 
                {
                    Entity->Enemy.PathIndex = Entity->Enemy.AStarPathLength;
                    Entity->Enemy.AStarCooldown = Entity->Enemy.AStarInterval;
                    glm::vec2 StartPosition = EntityPosition;
                    glm::vec2 TargetPosition = glm::vec2(Player.Position.x + Player.Center.x * Player.Scale.x,
                                                         Player.Position.y + Player.Center.y * Player.Scale.y);
                    AStar(Entity,GameState,StartPosition,TargetPosition);
                }
                else
                {
                    Entity->Enemy.AStarCooldown -= DeltaTime;
                }
                
                if(Entity->Enemy.AStarPath && Entity->Enemy.PathIndex < Entity->Enemy.AStarPathLength)
                {
                    glm::vec2 NewPos = Entity->Enemy.AStarPath[Entity->Enemy.PathIndex];
                    real64 DistanceToNode = glm::distance(EntityPosition + glm::vec2(0.0,0.1f), NewPos);
                    if(DistanceToNode > 1.0f) 
                    {
                        glm::vec2 Direction = NewPos - EntityPosition;
                        Direction = glm::normalize(Direction);
                        Entity->Velocity = glm::vec2(Direction.x * Entity->Enemy.WalkingSpeed * DeltaTime, Direction.y * Entity->Enemy.WalkingSpeed * DeltaTime);
                    }
                    else
                    {
                        Entity->Enemy.PathIndex++;
                    }
                }
                if(DistanceToPlayer > Entity->Enemy.MinDistance && DistanceToPlayer < 3.0f)
                {
                    glm::vec2 Direction = Player.Position - Entity->Position;
                    Direction = glm::normalize(Direction);
                    Entity->Velocity =glm::vec2(Direction.x * Entity->Enemy.WalkingSpeed * DeltaTime,
                                                Direction.y * Entity->Enemy.WalkingSpeed * DeltaTime);
                }
                
                Entity->IsFlipped = Abs(Entity->Velocity.x) > 1.0f * DeltaTime;
            }
        }
        break;
        case AI_Attacking:
        {
            if(Entity->Enemy.IsAttacking)
                Entity->Enemy.AttackCooldownCounter += DeltaTime;
            
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
            
            if(Entity->Enemy.AttackCooldownCounter >= Entity->Enemy.AttackCooldown)
            {
                Entity->Enemy.AttackCooldownCounter = 0;
                Entity->Enemy.IsAttacking = false;
            }
        }
        break;
        case AI_Hit:
        {
            PlayAnimation(Entity, &GameState->EnemyHitAnimation);
            if(Entity->HitCooldownLeft <= 0)
            {
                Entity->HitCooldownLeft = 0;
                Entity->Enemy.AIState = AI_Idle;
                
                PlayAnimation(Entity, &GameState->EnemyIdleAnimation);
            }
        }
        break;
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
}

void UpdateEnemyWeapon(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    //@Cleanup: This attack code is the almost identical to the player. Do something about it!
    entity* Enemy = &GameState->Entities[Entity->Weapon.EntityHandle];
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
    
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    glm::vec2 Pos = Enemy->Position;
    
    Entity->IsFlipped = Enemy->IsFlipped;
    
    if(Enemy->IsFlipped)
    {
        Entity->CollisionAABB.Offset = glm::vec2(-0.8, 0);
        Entity->Position = glm::vec2(Pos.x - 1.3f, Pos.y - 1.5f);
    }
    else
    {
        Entity->CollisionAABB.Offset = glm::vec2(0.7, 0);
        Entity->Position = glm::vec2(Pos.x - 0.5f, Pos.y - 1.5f);
    }
    
    Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->CollisionAABB.Offset.y);
    
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
    
    if(Enemy->Enemy.IsAttacking)
    {
        for(uint32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            if(CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing)
            {
                PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit01);
                Hit(GameState, CollisionInfo.Other[Index]);
            }
        }
    }
    
    if(Enemy->Enemy.IsAttacking)
    {
        if(Enemy->Enemy.AttackCooldownCounter == 0)
        {
            Entity->CurrentAnimation = 0;
            PlayAnimation(Entity, &GameState->SwordTopRightAnimation);
            RenderEntity->Rendered = true;
        }
    }
    
    if(!Entity->AnimationInfo.Playing)
    {
        RenderEntity->Rendered = false;
    }
}

void UpdateBarrel(entity* Entity, game_state* GameState, real64 DeltaTime)
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
    
    Entity->Velocity = glm::vec2(XVel,YVel);
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
            if(Entity->HitCooldownLeft > 0)
            {
                Entity->HitCooldownLeft -= DeltaTime;
            }
            
            switch(Entity->Type)
            {
                case Entity_Player: 
                {
                    UpdatePlayer(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_PlayerWeapon:
                {
                    UpdatePlayerWeapon(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_Enemy:
                {
                    UpdateEnemy(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_EnemyWeapon:
                {
                    UpdateEnemyWeapon(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_Barrel:
                {
                    UpdateBarrel(Entity, GameState, DeltaTime);
                }
            }
            
            if(Entity->CurrentAnimation && Entity->AnimationInfo.Playing)
                TickAnimation(&Entity->AnimationInfo, Entity->CurrentAnimation, DeltaTime);
        }
    }
    
    switch(GameState->EditorUI.State)
    {
        case State_EntityList:
        {
            auto entity = GameState->Entities[GameState->EditorUI.SelectedIndex];
            GameState->GameCamera.Center = glm::vec2(entity.Position.x, entity.Position.y);
        }
        break;
    }
}

static void EditorUpdateEntities(game_state* GameState, real64 DeltaTime)
{
    auto Pos = glm::unProject(glm::vec3(GameState->InputController.MouseX,GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                              GameState->Camera.ViewMatrix,
                              GameState->Camera.ProjectionMatrix,
                              glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    
    if(GetMouseButtonDown(Mouse_Left, GameState))
    {
        entity* Selected = 0;
        
        for(uint32 EntityIndex = 0;
            EntityIndex < GameState->EntityCount;
            EntityIndex++)
        {
            entity* Entity = &GameState->Entities[EntityIndex];
            
            if(Entity->Type != Entity_PlayerWeapon && Entity->Type != Entity_EnemyWeapon && Pos.x >= Entity->Position.x && Pos.y >= Entity->Position.y && Pos.x < Entity->Position.x + Entity->Scale.x && Pos.y < Entity->Position.y + Entity->Scale.y)
            {
                Selected = Entity;
                break;
            }
        }
        
        GameState->EditorState.SelectedEntity = Selected;
    }
    
    if(GameState->EditorState.SelectedEntity && GetMouseButton(Mouse_Left, GameState))
    {
        GameState->EditorState.SelectedEntity->Position = glm::vec2(Pos.x - GameState->EditorState.SelectedEntity->Scale.x / 2, Pos.y -  GameState->EditorState.SelectedEntity->Scale.y / 2);
    }
    
    // View translation
    GameState->EditorCamera.Zoom += GameState->InputController.ScrollY * GameState->EditorState.ZoomingSpeed * DeltaTime;
    
    if(GetMouseButton(Mouse_Right, GameState))
    {
        if(GameState->EditorState.LastKnownMouseX == 0 && GameState->EditorState.LastKnownMouseY == 0)
        {
            GameState->EditorState.LastKnownMouseX = GameState->InputController.MouseX;
            GameState->EditorState.LastKnownMouseY = GameState->InputController.MouseY;
        }
        
        glm::vec2 Direction = glm::vec2(GameState->InputController.MouseX - GameState->EditorState.LastKnownMouseX, GameState->InputController.MouseY - GameState->EditorState.LastKnownMouseY);
        
        GameState->EditorCamera.Center -= glm::vec2(Direction.x / GameState->EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime, Direction.y / GameState->EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime);
        
        GameState->EditorState.LastKnownMouseX = GameState->InputController.MouseX;
        GameState->EditorState.LastKnownMouseY = GameState->InputController.MouseY;
    }
    else
    {
        GameState->EditorState.LastKnownMouseX = 0;
        GameState->EditorState.LastKnownMouseY = 0;
    }
}

extern "C" UPDATE(Update)
{
    CheckConsoleInput(GameState, DeltaTime);
    
    if((GetKey(Key_LeftCtrl, GameState) || GetKey(Key_RightCtrl, GameState)) && GetKeyDown(Key_E, GameState))
    {
        if(GameState->GameMode == Mode_InGame)
        {
            ReloadCurrentLevel(GameState);
            GameState->GameMode = Mode_Editor;
            GameState->Paused = false;
            GameState->EditorCamera.Center = GameState->GameCamera.Center;
        }
        else
        {
            ReloadCurrentLevel(GameState);
            GameState->GameMode = Mode_InGame;
        }
    }
    
    if(!GameState->IsInitialized)
    {
        if(!GameState->ShouldReload)
        {
            LoadAnimations(GameState);
            InitCommands();
            GameState->LevelPath = "../assets/levels/level2.plv";
            
            GameState->EditorCamera.Zoom = 3.0f; // @Cleanup: We might not want to reset these values every time we load a level
            GameState->EditorCamera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
            GameState->EditorCamera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
            
            GameState->GameMode = Mode_InGame;
        }
        
        InitPlayer(GameState);
        LoadLevelFromFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
        //@Cleanup this should be in the level file
        SpawnMillionBarrels(GameState);
        
        GameState->GameCamera.Zoom = 3.0f;
        GameState->GameCamera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
        GameState->GameCamera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
        
        GameState->IsInitialized = true;
        GameState->ShouldReload = false;
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
    
    
    if(GetKeyDown(Key_F3, GameState))
    {
        GameState->SoundManager.Muted = !GameState->SoundManager.Muted;
    }
    
    if(GetKeyDown(Key_F4, GameState))
    {
        GameState->RenderState.RenderPaths = !GameState->RenderState.RenderPaths;
    }
    
    if(GameState->GameMode == Mode_InGame && GetKey(Key_LeftCtrl, GameState) && GetKeyDown(Key_P, GameState))
    {
        GameState->Paused = !GameState->Paused;
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
    
    switch(GameState->GameMode)
    {
        case Mode_InGame:
        case Mode_MainMenu:
        {
            if(!GameState->Paused)
                UpdateEntities(GameState, DeltaTime);
            GameState->Camera = GameState->GameCamera;
        }
        break;
        case Mode_Editor:
        {
            EditorUpdateEntities(GameState, DeltaTime);
            GameState->Camera = GameState->EditorCamera;
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
