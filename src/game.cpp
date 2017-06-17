#include "game.h"
#include "keycontroller.cpp"
#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "entity.cpp"
#include "editor.h"
#include "editor.cpp"
#include "collision.cpp"
#include "level.cpp"
#include "console.cpp"
#include "ai.cpp"

#define DEBUG

static void TickTimers(game_state* GameState, real64 DeltaTime)
{
    for(uint32 Index = 0; Index < NUM_TIMERS; Index++)
    {
        if(GameState->Timers[Index] > 0)
            GameState->Timers[Index] -= DeltaTime;
        else
            GameState->Timers[Index] = 0;
    }
}

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
        
        if(!Entity->Player.IsAttacking && TimerDone(GameState, Entity->Player.DashCooldownTimer) && !Entity->Player.IsDashing && GetActionButtonDown(Action_Dash, GameState))
        {
            PlaySoundEffect(GameState, &GameState->SoundManager.Dash);
            Entity->Player.IsDashing = true;
            StartTimer(GameState, Entity->Player.DashTimer);
        }
        
        if(!Entity->Player.IsDashing)
        {
            if(Entity->Player.IsAttacking && !Entity->AnimationInfo.Playing)
            {
                Entity->Player.IsAttacking = false;
                StartTimer(GameState, Entity->Player.AttackCooldownTimer);
            }
            
            if(!Entity->Player.IsAttacking)
            {
                real32 InputX = GetInputX(GameState);
                real32 InputY = GetInputY(GameState);
                Entity->Velocity.x = InputX * Entity->Player.WalkingSpeed * (real32)DeltaTime;
                Entity->Velocity.y = InputY * Entity->Player.WalkingSpeed * (real32)DeltaTime;
                
                if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                {
                    if(Abs(InputX) < 0.3)
                    {
                        if(Entity->Velocity.y > 0)
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
                }
                else
                {
                    if(Entity->Player.LastKnownDirectionX == 0)
                    {
                        if(Entity->Player.LastKnownDirectionY > 0)
                            PlayAnimation(Entity, "player_idle_up", GameState);
                        else
                            PlayAnimation(Entity, "player_idle_down", GameState);
                    }
                    else
                        PlayAnimation(Entity, "player_idle_right", GameState);
                }
                
                if(Entity->Velocity.x != 0)
                {
                    Entity->IsFlipped = Entity->Velocity.x < 0;
                    
                    if(Entity->LookDirection == Right && Entity->IsFlipped)
                        Entity->LookDirection = Left;
                }
            }
            else
                Entity->Velocity = glm::vec2(0, 0);
            
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
                StartTimer(GameState, Entity->Player.PickupCooldownTimer);
            }
        }
        else
        {
            if(TimerDone(GameState, Entity->Player.DashTimer))
            {
                StartTimer(GameState, Entity->Player.DashCooldownTimer);
                Entity->Player.IsDashing = false;
            }
            else
            {
                Entity->Velocity = glm::vec2(Entity->Player.LastKnownDirectionX * Entity->Player.DashSpeed * DeltaTime, Entity->Player.LastKnownDirectionY * Entity->Player.DashSpeed * DeltaTime);
            }
        }
        
        Entity->Position += Entity->Velocity;
        
        collision_info CollisionInfo;
        CheckCollision(GameState, Entity, &CollisionInfo);
        
        entity* OtherEntity = 0;
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
           GetActionButtonDown(Action_Interact, GameState) && TimerDone(GameState, Entity->Player.PickupCooldownTimer))
        {
            Entity->Player.Pickup = OtherEntity;
            Entity->Player.Pickup->Position = Entity->Position;
            Entity->Player.Pickup->Velocity = glm::vec2(0.0f,0.0f);
            // NOTE(niels): Need to make it kinematic, otherwise
            // there will be an overlap when pressing E to drop
            Entity->Player.Pickup->IsKinematic = true;
        }
        
        if(Entity->Player.Pickup)
        {
            Entity->Player.Pickup->Position = Entity->Position;
        }
        
        //attacking
        if(TimerDone(GameState, Entity->Player.AttackCooldownTimer) && !Entity->Player.IsAttacking && (GetActionButtonDown(Action_Attack, GameState) || GetJoystickKeyDown(Joystick_3, GameState)))
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
            PlaySoundEffect(GameState, &GameState->SoundManager.SwordSlash01);
        }
        
        auto Direction = glm::vec2(pos.x, pos.y) - Entity->Position;
        Direction = glm::normalize(Direction);
        float Degrees = atan2(Direction.y, Direction.x);
        
        if(!GameState->EditorUI.On)
            GameState->GameCamera.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
    }
}

void UpdateWeapon(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    entity* UsingEntity = &GameState->Entities[Entity->Weapon.EntityHandle];
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
    
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    glm::vec2 Pos = glm::vec2(UsingEntity->Position.x, UsingEntity->Position.y);
    
    Entity->IsFlipped = UsingEntity->IsFlipped;
    
    Entity->CollisionAABB.Extents = glm::vec2(0.7f, 0.7f);
    Entity->CollisionAABB.Offset = glm::vec2(0, -2);
    
    bool32 IsAttacking = false;
    
    switch(UsingEntity->Type)
    {
        case Entity_Player:
        {
            IsAttacking = UsingEntity->Player.IsAttacking;
            
            switch(UsingEntity->LookDirection)
            {
                case Up:
                {
                    Entity->Position = glm::vec2(Pos.x, Pos.y + 1.2f);
                }
                break;
                case Down:
                {
                    Entity->Position = glm::vec2(Pos.x, Pos.y - 1.2f);
                    Entity->CollisionAABB.Offset = glm::vec2(0, -2);
                }
                break;
                case Left:
                {
                    Entity->Position = glm::vec2(Pos.x - 1.3, Pos.y);
                }
                break;
                case Right:
                {
                    Entity->Position = glm::vec2(Pos.x + 1, Pos.y);
                }
                break;
            }
            
        }
        break;
        case Entity_Enemy:
        {
            IsAttacking = UsingEntity->Enemy.IsAttacking;
            
            // @Incomplete: This has to be changed to use the offset
            if(UsingEntity->IsFlipped)
            {
                //Entity->CollisionAABB.Offset = glm::vec2(-0.8, 0);
                Entity->Position = glm::vec2(Pos.x - 1.3f, Pos.y - 1.5f);
            }
            else
            {
                //Entity->CollisionAABB.Offset = glm::vec2(0.7, 0);
                Entity->Position = glm::vec2(Pos.x - 0.5f, Pos.y - 1.5f);
            }
            
        }
        break;
    }
    
    Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->CollisionAABB.Offset.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->CollisionAABB.Offset.y);
    
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
    
    if(IsAttacking)
    {
        for(int32 Index = 0; Index < CollisionInfo.OtherCount; Index++)
        {
            if((UsingEntity->Type == Entity_Player && CollisionInfo.Other[Index]->Type == Entity_Enemy && CollisionInfo.Other[Index]->Enemy.AIState != AI_Hit) ||
               (UsingEntity->Type == Entity_Enemy && CollisionInfo.Other[Index]->Type == Entity_Player && !CollisionInfo.Other[Index]->Player.IsDashing && TimerDone(GameState, CollisionInfo.Other[Index]->HitCooldownTimer)))
            {
                PlaySoundEffect(GameState, &GameState->SoundManager.SwordHit01);
                Hit(GameState, CollisionInfo.Other[Index]);
            }
        }
    }
    
    if(IsAttacking && !Entity->AnimationInfo.Playing)
    {
        Entity->CurrentAnimation = 0;
        PlayAnimation(Entity, "sword_attack", GameState);
        //RenderEntity->Rendered = true;
    }
    else if(!IsAttacking || !Entity->AnimationInfo.Playing)
    {
        RenderEntity->Rendered = false;
    }
}

void UpdateBlob(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    switch(Entity->Blob.AIState)
    {
        case AI_Following:
        {
            FindPath(GameState,Entity,GameState->Entities[GameState->PlayerIndex],GetAStarPath(Entity));
            FollowPath(GameState,Entity,GameState->Entities[GameState->PlayerIndex],DeltaTime,GetAStarPath(Entity));
            /*glm::vec2 Direction = glm::normalize(glm::vec2(GameState->Entities[GameState->PlayerIndex].Position.x - Entity->Position.x, GameState->Entities[GameState->PlayerIndex].Position.y - Entity->Position.y));
            */
            if(Abs(glm::distance(Entity->Position, GameState->Entities[GameState->PlayerIndex].Position)) < 1)
            {
                Entity->Blob.AIState = AI_Charging;
                StartTimer(GameState, Entity->Blob.ExplodeStartTimer);
            }
            
            Entity->Position.x += Entity->Velocity.x * (real32)DeltaTime;
            Entity->Position.y += Entity->Velocity.y * (real32)DeltaTime;
            
            //printf("Velocity: (%f,%f)\n",Entity->Velocity.x,Entity->Velocity.y);
            
            /*Entity->IsFlipped = Direction.x <= 0;*/
        }
        break;
        case AI_Charging:
        {
            if(TimerDone(GameState, Entity->Blob.ExplodeStartTimer))
            {
                Entity->Blob.AIState = AI_Attacking;
                StartTimer(GameState, Entity->Blob.ExplodeCountdownTimer);
            }
            
            if(Abs(glm::distance(Entity->Position, GameState->Entities[GameState->PlayerIndex].Position)) >= 1)
                Entity->Blob.AIState = AI_Following;
        }
        break;
        case AI_Attacking:
        {
            if(TimerDone(GameState, Entity->Blob.ExplodeCountdownTimer))
            {
                Entity->Blob.AIState = AI_Dying;
                PlayAnimation(Entity, "explosion", GameState);
                PlaySoundEffect(GameState, &GameState->SoundManager.Explosion);
            }
        }
        break;
        case AI_Dying:
        {
            if(!Entity->AnimationInfo.Playing)
            {
                DeleteEntity(GameState, Entity->EntityIndex);
            }
        }
        break;
    }
}

void UpdateEnemy(entity* Entity, game_state* GameState, real64 DeltaTime)
{
    Entity->Velocity = glm::vec2(0,0);
    entity Player = GameState->Entities[GameState->PlayerIndex];
    real64 DistanceToPlayer = abs(glm::distance(Entity->Position, Player.Position));
    
    switch(Entity->Enemy.AIState)
    {
        case AI_Sleeping:
        {
            PlayAnimation(Entity, "skeleton_idle", GameState);
        }
        break;
        case AI_Idle:
        {
            PlayAnimation(Entity, "skeleton_idle", GameState);
            if(DistanceToPlayer <= Entity->Enemy.MaxAlertDistance)
            {
                Entity->Enemy.AIState = AI_Following;
                PlayAnimation(Entity, "skeleton_walk", GameState);
            }
        }
        break;
        case AI_Alerted:
        {}
        break;
        case AI_Following:
        {
            if(DistanceToPlayer > Entity->Enemy.MaxAlertDistance)
            {
                PlayAnimation(Entity, "skeleton_idle", GameState);
                Entity->Enemy.AIState = AI_Idle;
            }
            
            else if(DistanceToPlayer < Entity->Enemy.MinDistance)
            {
                PlayAnimation(Entity, "skeleton_idle", GameState);
                StartTimer(GameState, Entity->Enemy.ChargingTimer);
                Entity->Enemy.AIState = AI_Charging;
                render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
            }
            else
            {
                FindPath(GameState,Entity,Player,GetAStarPath(Entity));
                FollowPath(GameState,Entity,Player,DeltaTime,GetAStarPath(Entity));
            }
        }
        break;
        case AI_Charging:
        {
            if(TimerDone(GameState, Entity->Enemy.ChargingTimer))
            {
                Entity->Enemy.AIState = AI_Attacking;
            }
        }
        break;
        case AI_Attacking:
        {
            if(!Entity->Enemy.IsAttacking)
            {
                Entity->Enemy.IsAttacking = true;
                PlayAnimation(Entity, "skeleton_attack", GameState);
                
                StartTimer(GameState, Entity->Enemy.AttackCooldownTimer);
            }
            else if(!Entity->AnimationInfo.Playing)
            {
                PlayAnimation(Entity, "skeleton_idle", GameState);
            }
            
            if(TimerDone(GameState, Entity->Enemy.AttackCooldownTimer))
            {
                if(DistanceToPlayer > Entity->Enemy.MinDistance)
                {
                    Entity->Enemy.IsAttacking = false;
                    Entity->Enemy.AIState = AI_Following;
                }
                else if(DistanceToPlayer > Entity->Enemy.MaxAlertDistance)
                {
                    Entity->Enemy.IsAttacking = false;
                    Entity->Enemy.AIState = AI_Idle;
                }
                else
                {
                    Entity->Enemy.IsAttacking = false;
                    Entity->Enemy.AIState = AI_Charging;
                    StartTimer(GameState, Entity->Enemy.ChargingTimer);
                }
            }
        }
        break;
        case AI_Hit:
        {
            PlayAnimation(Entity, "skeleton_hit", GameState);
            
            if(TimerDone(GameState, Entity->HitCooldownTimer))
            {
                Entity->Enemy.AIState = AI_Idle;
                PlayAnimation(Entity, "skeleton_idle", GameState);
            }
        }
        break;
    }
    
    Entity->Position.x += Entity->Velocity.x * (real32)DeltaTime;
    Entity->Position.y += Entity->Velocity.y * (real32)DeltaTime;
    
    //@Cleanup move this somewhere else, maybe out of switch
    render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Entity->RenderEntityHandle];
    
    RenderEntity->Color = glm::vec4(1, 1, 1, 1);
    
    Entity->Velocity = glm::vec2(0,0);
    
    collision_info CollisionInfo;
    CheckCollision(GameState, Entity, &CollisionInfo);
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
            switch(Entity->Type)
            {
                case Entity_Player: 
                {
                    UpdatePlayer(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_Weapon:
                {
                    UpdateWeapon(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_Enemy:
                {
                    UpdateEnemy(Entity, GameState, DeltaTime);
                }
                break;
                case Entity_Blob:
                {
                    UpdateBlob(Entity, GameState, DeltaTime);
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
    bool32 InToolbar = GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= GameState->RenderState.WindowHeight - 150;
    
    for(uint32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
    {
        button* Button = &GameState->EditorState.Buttons[ButtonIndex];
        Button->Clicked = false;
        
        if(GetMouseButtonDown(Mouse_Left, GameState))
        {
            if(Button->Active && GameState->InputController.MouseX >= Button->ScreenPosition.x && GameState->InputController.MouseX <= Button->ScreenPosition.x + Button->Size.x && 
               GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= Button->ScreenPosition.y && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <= Button->ScreenPosition.y + Button->Size.y)
            {
                Button->Clicked = true;
                
                PlaySoundEffect(GameState, &GameState->SoundManager.ButtonClick);
                StartTimer(GameState, Button->ClickAnimationTimer);
                
                switch(Button->EditorType)
                {
                    case Button_Exit:
                    {
                        GameState->GameMode = Mode_Exit;
                    }
                    break;
                    case Button_SaveAndExit:
                    {
                        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                        ReloadCurrentLevel(GameState);
                        GameState->GameMode = Mode_InGame;
                    }
                    break;
                    case Button_Tilesheet:
                    {
                    }
                    break;
                    case Button_Animation:
                    {
                        GameState->EditorState.Mode = Editor_Animation;
                    }
                    break;
                    case Button_SwitchMode:
                    {
                        if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                            GameState->EditorState.PlacementMode = Editor_Placement_Entity;
                        else
                            GameState->EditorState.PlacementMode = Editor_Placement_Tile;
                    }
                    break;
                }
            }
        }
        
        if(Button->Active)
        {
            if(!TimerDone(GameState, Button->ClickAnimationTimer))
                Button->Color = glm::vec4(0.5, 0.5, 0.5, 1);
            else
                Button->Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
        }
    }
    
    for(int32 Index = 0; Index < 20; Index++)
    {
        textfield* Textfield = &GameState->EditorState.Textfields[Index];
        
        if(GetMouseButtonDown(Mouse_Left, GameState))
        {
            if(Textfield->Active && GameState->InputController.MouseX >= Textfield->ScreenPosition.x && GameState->InputController.MouseX <= Textfield->ScreenPosition.x + Textfield->Size.x && 
               GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= Textfield->ScreenPosition.y && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <= Textfield->ScreenPosition.y + Textfield->Size.y)
            {
                GameState->EditorState.FocusedTextfield = Index;
                /*
                PlaySoundEffect(GameState, &GameState->SoundManager.ButtonClick);
                StartTimer(GameState, Button->ClickAnimationTimer);
                */
            }
        }
        
        if(Index == GameState->EditorState.FocusedTextfield)
        {
            if(GameState->InputController.DeleteCharacter && Textfield->TextIndex != 0)
            {
                Textfield->Text[--Textfield->TextIndex] = 0;
            }
            
            if(GameState->InputController.CurrentCharacter && GameState->InputController.CurrentCharacter != ' ')
            {
                bool32 CanEnter = true;
                
                if(Textfield->OnlyNumber)
                {
                    CanEnter = GameState->InputController.CurrentCharacter == '0' ||
                        GameState->InputController.CurrentCharacter == '1' ||
                        GameState->InputController.CurrentCharacter == '2' ||
                        GameState->InputController.CurrentCharacter == '3' ||
                        GameState->InputController.CurrentCharacter == '4' ||
                        GameState->InputController.CurrentCharacter == '5' ||
                        GameState->InputController.CurrentCharacter == '6' ||
                        GameState->InputController.CurrentCharacter == '7' ||
                        GameState->InputController.CurrentCharacter == '8' ||
                        GameState->InputController.CurrentCharacter == '9';
                }
                
                if(CanEnter && Textfield->TextIndex != TEXTFIELD_LENGTH)
                {
                    Textfield->Text[Textfield->TextIndex++] = GameState->InputController.CurrentCharacter;
                }
            }
        }
    }
    
    switch(GameState->EditorState.Mode)
    {
        case Editor_Normal:
        {
            GameState->EditorState.CreateNewAnimationButton->Active = false;
            if(GetKeyDown(Key_M, GameState))
            {
                if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                    GameState->EditorState.PlacementMode = Editor_Placement_Entity;
                else
                    GameState->EditorState.PlacementMode = Editor_Placement_Tile;
            }
            
            auto Pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                                      GameState->Camera.ViewMatrix,
                                      GameState->Camera.ProjectionMatrix,
                                      glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
            if(!InToolbar)
            {
                switch(GameState->EditorState.PlacementMode)
                {
                    case Editor_Placement_Entity:
                    {
                        if(GetMouseButtonDown(Mouse_Left, GameState))
                        {
                            if(GameState->InputController.MouseX >= GameState->EditorState.ToolbarX)
                            {
                                uint32 Selected = (uint32)(GameState->InputController.MouseY / 65.0f);
                                GameState->EditorState.SelectedTileType = Selected;
                            }
                            else
                            {
                                entity* Selected = 0;
                                
                                for(uint32 EntityIndex = 0;
                                    EntityIndex < GameState->EntityCount;
                                    EntityIndex++)
                                {
                                    entity* Entity = &GameState->Entities[EntityIndex];
                                    
                                    if(Entity->Type != Entity_Weapon && Pos.x >= Entity->Position.x && Pos.y >= Entity->Position.y - Entity->Scale.y && Pos.x < Entity->Position.x + Entity->Scale.x && Pos.y < Entity->Position.y)
                                    {
                                        Selected = Entity;
                                        break;
                                    }
                                }
                                
                                GameState->EditorState.SelectedEntity = Selected;
                            }
                        }
                        
                        if(GameState->EditorState.SelectedEntity && GetMouseButton(Mouse_Left, GameState))
                        {
                            GameState->EditorState.SelectedEntity->Position = glm::vec2(Pos.x - GameState->EditorState.SelectedEntity->Scale.x / 2, Pos.y + GameState->EditorState.SelectedEntity->Scale.y / 2);
                        }
                        
                    }
                    break;
                    case Editor_Placement_Tile:
                    {
                        int32 X = (int32)glm::floor(Pos.x);
                        int32 Y = (int32)glm::floor(Pos.y);
                        GameState->EditorState.TileX = (real32)X;
                        GameState->EditorState.TileY = (real32)glm::ceil(Pos.y);
                        
                        if(GetMouseButton(Mouse_Left, GameState))
                        {
                            if(GameState->InputController.MouseX >= GameState->EditorState.ToolbarX)
                            {
                                uint32 Selected = (uint32)((GameState->RenderState.WindowHeight - GameState->InputController.MouseY + abs(GameState->EditorState.ToolbarScrollOffsetY)) / 60.0f);
                                GameState->EditorState.SelectedTileType = Selected;
                            }
                            
                            else
                            {
                                if(X >= 0 && X < (int32)GameState->CurrentLevel.Tilemap.Width 
                                   && Y >= 0 && Y < (int32)GameState->CurrentLevel.Tilemap.Height)
                                {
                                    tilemap* Tilemap = &GameState->CurrentLevel.Tilemap;
                                    
                                    collision_AABB CollisionAABB;
                                    CollisionAABB.Center = glm::vec2(X + 0.5f, Y + 0.5f);
                                    CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                                    
                                    Tilemap->Data[X][Y] = Tilemap->Tiles[GameState->EditorState.SelectedTileType];
                                    
                                    Tilemap->RenderInfo.Dirty = true;
                                }
                            }
                        }
                        if(GetMouseButton(Mouse_Left,GameState) && GetKey(Key_LeftShift,GameState))
                        {
                            if(X >= 0 && X < (int32)GameState->CurrentLevel.Tilemap.Width && Y >= 0 && Y < (int32)GameState->CurrentLevel.Tilemap.Height)
                            {
                                GameState->CurrentLevel.Tilemap.Data[X][Y] = GameState->CurrentLevel.Tilemap.Tiles[0];
                            }
                        }
                    }
                    break;
                }
            }
            
            // View translation
            if(GameState->InputController.MouseX >= GameState->EditorState.ToolbarX)
            {
                GameState->EditorState.ToolbarScrollOffsetY += (real32)GameState->InputController.ScrollY * GameState->EditorState.ToolbarScrollSpeed * (real32)DeltaTime;
                
                if(GameState->EditorState.ToolbarScrollOffsetY > 0)
                    GameState->EditorState.ToolbarScrollOffsetY = 0.0f;
                else if(GameState->EditorState.ToolbarScrollOffsetY < -((real32)GameState->CurrentLevel.Tilemap.TileCount) * 60)
                    GameState->EditorState.ToolbarScrollOffsetY = -((real32)GameState->CurrentLevel.Tilemap.TileCount) * 60;
            }
            else
            {
                GameState->EditorCamera.Zoom += (real32)GameState->InputController.ScrollY * GameState->EditorState.ZoomingSpeed * (real32)DeltaTime * GameState->EditorCamera.Zoom;
                GameState->EditorCamera.Zoom = Max(Min(GameState->EditorCamera.Zoom, GameState->EditorState.MaxZoom), GameState->EditorState.MinZoom);
            }
            
            if(GetMouseButton(Mouse_Right, GameState))
            {
                if(GameState->EditorState.LastKnownMouseX == 0 && GameState->EditorState.LastKnownMouseY == 0)
                {
                    GameState->EditorState.LastKnownMouseX = (real32)GameState->InputController.MouseX;
                    GameState->EditorState.LastKnownMouseY = (real32)GameState->InputController.MouseY;
                }
                
                glm::vec2 Direction = glm::vec2(GameState->InputController.MouseX - GameState->EditorState.LastKnownMouseX, GameState->InputController.MouseY - GameState->EditorState.LastKnownMouseY);
                
                GameState->EditorCamera.Center -= glm::vec2(Direction.x / GameState->EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime, Direction.y / GameState->EditorCamera.Zoom * -GameState->EditorState.PanningSpeed * DeltaTime);
                
                GameState->EditorState.LastKnownMouseX = (real32)GameState->InputController.MouseX;
                GameState->EditorState.LastKnownMouseY = (real32)GameState->InputController.MouseY;
            }
            else
            {
                GameState->EditorState.LastKnownMouseX = 0;
                GameState->EditorState.LastKnownMouseY = 0;
            }
        }
        break;
        case Editor_Animation:
        {
            GameState->EditorState.CreateNewAnimationButton->Active = true;
            
            if(GameState->EditorState.CreateNewAnimationButton->Clicked)
            {
                GameState->EditorState.LoadedAnimation = (animation*)malloc(sizeof(animation));
            }
            
            if(GameState->EditorState.LoadedAnimation)
            {
                if(!GameState->EditorState.AnimationNameField ||!GameState->EditorState.AnimationNameField->Active)
                {
                    GameState->EditorState.AnimationNameField = &GameState->EditorState.Textfields[0];
                    GameState->EditorState.AnimationFrameWidthField = &GameState->EditorState.Textfields[1];
                    GameState->EditorState.AnimationFrameHeightField = &GameState->EditorState.Textfields[2];
                    GameState->EditorState.AnimationFrameCountField = &GameState->EditorState.Textfields[3];
                    GameState->EditorState.AnimationFrameOffsetXField = &GameState->EditorState.Textfields[4];
                    GameState->EditorState.AnimationFrameOffsetYField = &GameState->EditorState.Textfields[5];
                    
                    GameState->EditorState.AnimationNameField->Active = true;
                    GameState->EditorState.AnimationNameField->Size = glm::vec2(300, 30);
                    GameState->EditorState.AnimationNameField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 660);
                    GameState->EditorState.AnimationNameField->Label = "Name";
                    
                    GameState->EditorState.AnimationFrameWidthField->Active = true;
                    GameState->EditorState.AnimationFrameWidthField->Size = glm::vec2(300, 30);
                    GameState->EditorState.AnimationFrameWidthField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 600);
                    GameState->EditorState.AnimationFrameWidthField->Label = "Frame width";
                    GameState->EditorState.AnimationFrameWidthField->OnlyNumber = true;
                    
                    GameState->EditorState.AnimationFrameHeightField->Active = true;
                    GameState->EditorState.AnimationFrameHeightField->Size = glm::vec2(300, 30);
                    GameState->EditorState.AnimationFrameHeightField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 540);
                    GameState->EditorState.AnimationFrameHeightField->Label = "Frame height";
                    GameState->EditorState.AnimationFrameHeightField->OnlyNumber = true;
                    
                    GameState->EditorState.AnimationFrameCountField->Active = true;
                    GameState->EditorState.AnimationFrameCountField->Size = glm::vec2(300, 30);
                    GameState->EditorState.AnimationFrameCountField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 480);
                    GameState->EditorState.AnimationFrameCountField->Label = "Frame count";
                    GameState->EditorState.AnimationFrameCountField->OnlyNumber = true;
                    
                    GameState->EditorState.AnimationFrameOffsetXField->Active = true;
                    GameState->EditorState.AnimationFrameOffsetXField->Size = glm::vec2(300, 30);
                    GameState->EditorState.AnimationFrameOffsetXField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 420);
                    GameState->EditorState.AnimationFrameOffsetXField->Label = "Frame offset x";
                    GameState->EditorState.AnimationFrameOffsetXField->OnlyNumber = true;
                    
                    GameState->EditorState.AnimationFrameOffsetYField->Active = true;
                    GameState->EditorState.AnimationFrameOffsetYField->Size = glm::vec2(300, 30);
                    GameState->EditorState.AnimationFrameOffsetYField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 360);
                    GameState->EditorState.AnimationFrameOffsetYField->Label = "Frame offset y";
                    GameState->EditorState.AnimationFrameOffsetYField->OnlyNumber = true;
                }
            }
            else
            {
                if(GetKeyDown(Key_Down, GameState))
                {
                    GameState->EditorState.SelectedAnimation++;
                }
                else if(GetKeyDown(Key_Up, GameState))
                {
                    GameState->EditorState.SelectedAnimation--;
                }
                
                if(GameState->EditorState.SelectedAnimation == GameState->Animations.size())
                    GameState->EditorState.SelectedAnimation = 0;
                else if(GameState->EditorState.SelectedAnimation < 0)
                    GameState->EditorState.SelectedAnimation = (int32)GameState->Animations.size() - 1;
            }
            
            //CreateAnimation(GameState->EditorState.LoadedAnimation);
        }
        break;
    }
}

extern "C" UPDATE(Update)
{
    CheckConsoleInput(GameState, DeltaTime);
    CheckEditorUIInput(GameState, DeltaTime);
    
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
            SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
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
            GameState->LevelPath = "../assets/levels/level_new.plv";
            
            GameState->EditorCamera.Zoom = 3.0f; // @Cleanup: We might not want to reset these values every time we load a level
            GameState->EditorCamera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
            GameState->EditorCamera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
            
            GameState->GameMode = Mode_InGame;
        }
        
        LoadLevelFromFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
        
        GameState->GameCamera.Zoom = 4.0f;
        GameState->GameCamera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
        GameState->GameCamera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
        
        GameState->IsInitialized = true;
        GameState->ShouldReload = false;
    }
    else
    
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
        ToggleMuteSound(GameState);
    }
    
    if(GetKeyDown(Key_F4, GameState))
    {
        GameState->RenderState.RenderPaths = !GameState->RenderState.RenderPaths;
    }
    
    
    if(GetKeyDown(Key_F5, GameState))
    {
        TogglePauseSound(GameState);
    }
    
    
    if(GetKeyDown(Key_F6, GameState))
    {
        StopSound(GameState);
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
            {
                UpdateEntities(GameState, DeltaTime);
                TickTimers(GameState, DeltaTime);
            }
            GameState->Camera = GameState->GameCamera;
        }
        break;
        case Mode_Editor:
        {
            EditorUpdateEntities(GameState, DeltaTime);
            GameState->Camera = GameState->EditorCamera;
            TickTimers(GameState, DeltaTime); // @Cleanup: I don't remember why this has to be called individually for each mode. It might be okay to call it at the end of the Update-function
        }
        break;
    }
    
    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f,
                                                    static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom),
                                                    0.0f,
                                                    static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom),
                                                    -1.0f,
                                                    1.0f);
    
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(-GameState->Camera.Center.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2,
                                                            -GameState->Camera.Center.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2,
                                                            0));
    GameState->InputController.CurrentCharacter = 0;
}
