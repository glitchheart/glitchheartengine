#include "game.h"
#include "keycontroller.cpp"

#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "collision.cpp"

#define DEBUG

void CheckCollision(game_state* GameState, entity* Entity, collision_info* CollisionInfo)
{
    Entity->IsColliding = false;
    
    if(!Entity->IsKinematic)
    {
        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Velocity.x, Entity->Position.y + Entity->Velocity.y);
        for(uint32 OtherEntityIndex = 0;
            OtherEntityIndex < GameState->EntityCount;
            OtherEntityIndex++)
        {
            if(OtherEntityIndex != Entity->EntityIndex && !GameState->Entities[OtherEntityIndex].IsKinematic) {
                collision_AABB Md = {};
                MinkowskiDifference(&Entity->CollisionAABB, &GameState->Entities[OtherEntityIndex].CollisionAABB, &Md);
                if(Md.Min.x <= 0 &&
                   Md.Max.x >= 0 &&
                   Md.Min.y <= 0 &&
                   Md.Max.y >= 0)
                {
                    Entity->IsColliding = true;
                    GameState->Entities[OtherEntityIndex].IsColliding = true;
                    
                    //calculate what side is colliding
                    auto OtherPosition = GameState->Entities[OtherEntityIndex].CollisionAABB.Center;
                    auto OtherExtents = GameState->Entities[OtherEntityIndex].CollisionAABB.Extents;
                    auto Position = Entity->CollisionAABB.Center;
                    auto Extents = Entity->CollisionAABB.Extents;
                    
                    real32 Left = Position.x - Extents.x;
                    real32 Right = Position.x + Extents.x;
                    real32 Top = Position.y - Extents.y;
                    real32 Bottom = Position.y + Extents.y;
                    real32 OtherLeft = OtherPosition.x - OtherExtents.x;
                    real32 OtherRight = OtherPosition.x + OtherExtents.x;
                    real32 OtherTop = OtherPosition.y - OtherExtents.y;
                    real32 OtherBottom = OtherPosition.y + OtherExtents.y;
                    
                    if(Right > OtherLeft && Left < OtherLeft && Right-OtherLeft < Bottom - OtherTop && Right - OtherLeft < OtherBottom - Top)
                    {
                        //Player collides from left side of the object
                        CollisionInfo->Side = CollisionInfo->Side | Side_Left;
                    }
                    else if(Left < OtherRight && Right > OtherRight && OtherRight-Left < Bottom-OtherTop && OtherRight-Left < OtherBottom-Top)
                    {
                        //Player collides from right side of the object
                        CollisionInfo->Side = CollisionInfo->Side | Side_Right;
                    }
                    else if(Bottom > OtherTop && Top < OtherTop)
                    {
                        //Player collides from top side of the object
                        CollisionInfo->Side = CollisionInfo->Side | Side_Top;
                    }
                    else if(Top < OtherBottom && Bottom > OtherBottom)
                    {
                        //Player collides from bottom side of the object
                        CollisionInfo->Side = CollisionInfo->Side | Side_Bottom;
                    }
                } 
                else
                {
                    if(!Entity->IsColliding) 
                    {
                        Entity->IsColliding = false;
                    }
                    if(!GameState->Entities[OtherEntityIndex].IsColliding) 
                    {
                        GameState->Entities[OtherEntityIndex].IsColliding = false;
                    }
                }
                
            }
        }
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
                    if (GetKey(Key_A, GameState))
                    {
                        Entity->Velocity.x = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    else if (GetKey(Key_D, GameState))
                    {
                        Entity->Velocity.x = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    
                    if (GetKey(Key_W, GameState))
                    {
                        Entity->Velocity.y = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    else if (GetKey(Key_S, GameState))
                    {
                        Entity->Velocity.y = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    
                    if(Entity->Player.IsAttacking && !Entity->Animations[Entity->CurrentAnimation].Playing)
                    {
                        Entity->Player.IsAttacking = false;
                    }
                    
                    if(!Entity->Player.IsAttacking)
                    {
                        if(Entity->Velocity.x != 0.0f || Entity->Velocity.y != 0.0f)
                            PlayAnimation(Entity, "player_walk");
                        else
                            PlayAnimation(Entity, "player_idle");
                    }
                    
                    collision_info CollisionInfo;
                    CheckCollision(GameState, Entity, &CollisionInfo);
                    
                    if(Entity->IsColliding)
                    {
                        if(CollisionInfo.Side & Side_Left || CollisionInfo.Side & Side_Right)
                        {
                            Entity->Position.y += Entity->Velocity.y;
                        }
                        else if(CollisionInfo.Side & Side_Top || CollisionInfo.Side & Side_Bottom)
                        {
                            Entity->Position.x += Entity->Velocity.x;
                        }
                        else
                        {
                            Entity->Position.x += Entity->Velocity.x;
                            Entity->Position.y += Entity->Velocity.y;
                        }
                    }
                    else
                    {
                        Entity->Position.x += Entity->Velocity.x;
                        Entity->Position.y += Entity->Velocity.y;
                    }
                    
                    //attacking
                    if(!Entity->Player.IsAttacking && GetMouseButtonDown(Mouse_Left, GameState))
                    {
                        PlayAnimation(Entity, "player_attack");
                        Entity->Player.IsAttacking = true;
                    }
                    
                    if(Entity->CurrentAnimation)
                        TickAnimation(&Entity->Animations[Entity->CurrentAnimation],DeltaTime);
                    
                    auto Direction = glm::vec2(pos.x, pos.y) - Entity->Position;
                    Direction = glm::normalize(Direction);
                    float Degrees = atan2(Direction.y, Direction.x);
                    
                    Entity->Rotation = glm::vec3(0, 0, Degrees + 1.37079633);
                    
                    if(!GameState->EditorUI.On)
                        GameState->Camera.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
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
                Entity->Velocity = glm::vec2(0,0);
                
                collision_info CollisionInfo;
                CheckCollision(GameState, Entity, &CollisionInfo);
                
                if(!Entity->IsColliding)
                {
                    Entity->Position.x += Entity->Velocity.x;
                    Entity->Position.y += Entity->Velocity.y;
                }
            }
            break;
            case Entity_Barrel:
            {
                collision_info CollisionInfo;
                CheckCollision(GameState, Entity, &CollisionInfo);
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
    
    if(GetKeyDown(Key_Enter, GameState))
    {
        PlaySoundEffectOnce(GameState, &GameState->SoundManager.Track01);
    }
    
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
        else if(GameState->MainMenu.SelectedIndex == GameState->MainMenu.OptionCount)
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
}
