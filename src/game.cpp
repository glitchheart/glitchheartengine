#include "game.h"
#include "keycontroller.cpp"

#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "collision.cpp"

#define DEBUG

void CheckCollision(game_state* GameState, entity* Entity)
{
    if(!Entity->IsKinematic)
    {
        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Velocity.x, Entity->Position.y + Entity->Velocity.y);
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
                    Entity->CollisionAABB.Center = Entity->Position;
                } 
                else
                {
                    Entity->IsColliding = false;
                    GameState->Entities[OtherEntityIndex].IsColliding = false;
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
                    
                    
                    
                    //Entity->CollisionAABB.Center = Entity->Position;
                    CheckCollision(GameState,Entity);
                    if(!Entity->IsColliding) {
                        Entity->Position.x += Entity->Velocity.x;
                        Entity->Position.y += Entity->Velocity.y;
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
                Entity->Velocity = glm::vec2(0,0);
                //Check collision
                
                CheckCollision(GameState,Entity);
                if(!Entity->IsColliding) {
                    Entity->Position.x += Entity->Velocity.x;
                    Entity->Position.y += Entity->Velocity.y;
                }
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
