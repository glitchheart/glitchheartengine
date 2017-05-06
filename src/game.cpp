#include "game.h"
#include "keycontroller.cpp"

#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "collision.cpp"

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
                    float VelX = 0.0f;
                    float VelY = 0.0f;
                    
                    //player movement
                    if (GetKey(Key_A, GameState))
                    {
                        VelX = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    else if (GetKey(Key_D, GameState))
                    {
                        VelX = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    
                    if (GetKey(Key_W, GameState))
                    {
                        VelY = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    else if (GetKey(Key_S, GameState))
                    {
                        VelY = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                    }
                    
                    if(Entity->Player.IsAttacking && !Entity->Animations[Entity->CurrentAnimation].Playing)
                    {
                        Entity->Player.IsAttacking = false;
                    }
                    
                    if(!Entity->Player.IsAttacking)
                    {
                        if(VelX != 0.0f || VelY != 0.0f)
                            PlayAnimation(Entity, "player_walk");
                        else
                            PlayAnimation(Entity, "player_idle");
                    }
                    
                    Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + VelX, Entity->Position.y + VelY);
                    
                    //Entity->CollisionAABB.Center = Entity->Position;
                    if(!Entity->IsKinematic)
                    {
                        
                        for(uint32 OtherEntityIndex = 0;
                            OtherEntityIndex < GameState->EntityCount;
                            OtherEntityIndex++)
                        {
                            if(OtherEntityIndex != Entity->EntityIndex && !GameState->Entities[OtherEntityIndex].IsKinematic) {
                                collision_AABB Md = {};
                                MinkowskiDifference(&Entity->CollisionAABB, &GameState->Entities[OtherEntityIndex].CollisionAABB,&Md);
                                if(Md.Min.x <= 0 &&
                                   Md.Max.x >= 0 &&
                                   Md.Min.y <= 0 &&
                                   Md.Max.y >= 0)
                                {
                                    Entity->IsColliding = true;
                                    GameState->Entities[OtherEntityIndex].IsColliding = true;
                                    
                                    /*glm::vec2 PenetrationVector;
                                    ClosestPointsOnBoundsToPoint(&Entity->CollisionAABB,GameState->Entities[OtherEntityIndex].Position,
                                                                 &PenetrationVector);
                                                                 
                                    if(PenetrationVector.x != 0 && PenetrationVector.y != 0)
                                    {
                                        glm::vec2 Normalized = glm::normalize(PenetrationVector);
                                        glm::vec2 Tangent = glm::tan(Normalized);
                                        VelX = Tangent.x;
                                        VelY = Tangent.y;
                                    }*/
                                    
                                    Entity->CollisionAABB.Center = Entity->Position;
                                } 
                                
                                else
                                {
                                    Entity->IsColliding = false;
                                    GameState->Entities[OtherEntityIndex].IsColliding = false;
                                    Entity->Position.x += VelX;
                                    Entity->Position.y += VelY;
                                }
                                
                            }
                        }
                    }
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
                
                Entity->Rotation = glm::vec3(0, 0, Degrees);
                
                if(!GameState->EditorUI.On)
                    GameState->Camera.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
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
                Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x, Entity->Position.y);
            }
            break;
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
        if(GameState->RenderState.RenderColliders == 0)
            GameState->RenderState.RenderColliders = 1;
        else
            GameState->RenderState.RenderColliders = 0;
    }
    
#endif
    
    if (GetKeyDown(Key_Enter, GameState) && !GameState->Console.Open)
    {
        PlaySoundEffectOnce(GameState, &GameState->SoundManager.Track01);
    }
    
    UpdateEntities(GameState, DeltaTime);
}
