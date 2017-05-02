#include "game.h"
#include "keycontroller.cpp"
#include "keys_glfw.h"
#include "sound.cpp"
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
        
        switch(Entity->Type) {
            case Entity_Player: 
            {
                Entity->CollisionRect.Left = false;
                Entity->CollisionRect.Right = false;
                Entity->CollisionRect.Top = false;
                Entity->CollisionRect.Bottom = false;
                
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
                    bool32 Colliding = false;
                    if(!Entity->IsKinematic) {
                        
                        for(uint32 OtherEntity = 0;
                            OtherEntity < GameState->EntityCount;
                            OtherEntity++)
                        {
                            if(OtherEntity != Entity->EntityIndex &&
                               !GameState->Entities[OtherEntity].IsKinematic && CheckCollision(Entity,&GameState->Entities[OtherEntity]))
                            {
                                Colliding = true;
                            }
                        }
                    }
                    if(!Colliding) {
                        if(VelX > 0 && !Entity->CollisionRect.Right ||
                           VelX < 0 && !Entity->CollisionRect.Left)
                        {
                            Entity->Position.x += VelX;
                        }
                        
                        if(VelY > 0 && !Entity->CollisionRect.Top ||
                           VelY < 0 && !Entity->CollisionRect.Bottom)
                        {
                            Entity->Position.y += VelY;
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
                
                GameState->Camera.ProjectionMatrix = glm::ortho(0.0f,
                                                                static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom),
                                                                static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom),
                                                                0.0f,
                                                                -1.0f,
                                                                1.0f);
                GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f),
                                                              glm::vec3(-Entity->Position.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2,
                                                                        -Entity->Position.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2,
                                                                        0));
            }
            break;
            case Entity_Crosshair:
            {
                Entity->Position = glm::vec2(pos.x - 0.5f, pos.y - 0.5f);
            }break;
        }
    }
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
    
    collision_rect Collider1 = { 0, 0, 10, 10};
    collision_rect Collider2 = {0, 5, 10, 10 };
    
    if (GetKeyDown(Key_Enter, GameState) && !GameState->Console.Open)
    {
        PlaySoundEffectOnce(GameState, &GameState->SoundManager.Track01);
    }
    
    UpdateEntities(GameState, DeltaTime);
    
}
