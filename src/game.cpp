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
        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y);
        
        glm::vec2 PV;
        
        for(uint32 OtherEntityIndex = 0;
            OtherEntityIndex < GameState->EntityCount;
            OtherEntityIndex++)
        {
            entity OtherEntity = GameState->Entities[OtherEntityIndex];
            
            if(!(OtherEntity.Layer & Entity->IgnoreLayers) && OtherEntityIndex != Entity->EntityIndex && !GameState->Entities[OtherEntityIndex].IsKinematic)
            {
                collision_AABB Md;
                MinkowskiDifference(&GameState->Entities[OtherEntityIndex].CollisionAABB, &Entity->CollisionAABB, &Md);
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
                    
                    switch(Entity->Type)
                    {
                        case Entity_Player:
                        {
                            if(GameState->Entities[OtherEntityIndex].Pickup &&
                               GetKeyDown(Key_E,GameState) && Entity->Player.PickupCooldown <= 0.0)
                            {
                                Entity->Player.Pickup = &GameState->Entities[OtherEntityIndex];
                                Entity->Player.Pickup->Position = Entity->Position;
                                // NOTE(niels): Need to make it kinematic, otherwise
                                // there will be an overlap when pressing E to drop
                                Entity->Player.Pickup->IsKinematic = true;
                                Entity->Player.PickupCooldown = 0.5;
                            }
                            
                        }
                        break;
                        case Entity_Barrel:
                        case Entity_Crosshair:
                        case Entity_Enemy:
                        break;
                    }
                    
                    if(!GameState->Entities[OtherEntityIndex].IsTrigger &&
                       !Entity->IsTrigger && !Entity->IsStatic)
                    {
                        /*
                        Entity->Position += glm::vec2(PenetrationVector.x/XDivider,PenetrationVector.y/YDivider);
                        Entity->CollisionAABB.Center = glm::vec2(Entity->Position.x + Entity->Center.x * Entity->Scale.x + Entity->Velocity.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y + Entity->Velocity.y);
                        Entity->Velocity = glm::vec2(0,0);*/
                    }
                }
            }
        }
        
        
        if(Entity->Type == Entity_Player)
        {
            level* Level = &GameState->CurrentLevel;
            
            int32 XPos = (int32)(Entity->Position.x + Entity->Center.x * Entity->Scale.x);
            int32 YPos = (int32)(Entity->Position.y + Entity->Center.y * Entity->Scale.y);
            
            //@Improvement Is it necessary to go 2 tiles out?
            uint32 MinX = max(0, XPos - 2);
            uint32 MaxX = min(Level->Tilemap.Width, XPos + 2);
            uint32 MinY = max(0, YPos - 2);
            uint32 MaxY = min(Level->Tilemap.Height, YPos + 2);
            
            //check tile collision
            for(int X = MinX; X < MaxX; X++)
            {
                for(int Y = MinY; Y < MaxY; Y++)
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
                    }
                }
            }
        }
        
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
                    if (GetKey(Key_A, GameState))
                    {
                        Entity->Velocity.x = -Entity->Player.WalkingSpeed * (real32)DeltaTime;
                        Entity->RenderEntity.IsFlipped = true;
                    }
                    else if (GetKey(Key_D, GameState))
                    {
                        Entity->Velocity.x = Entity->Player.WalkingSpeed * (real32)DeltaTime;
                        Entity->RenderEntity.IsFlipped = false;
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
                    
                    if(GetKeyDown(Key_E,GameState) && Entity->Player.Pickup)
                    {
                        entity* Pickup = Entity->Player.Pickup;
                        Entity->Player.Pickup->IsKinematic = false;
                        real32 ThrowDir = Entity->RenderEntity.IsFlipped ? -80.0f : 80.0f;
                        Entity->Player.Pickup->Velocity = glm::vec2(ThrowDir * DeltaTime,0);
                        Entity->Player.Pickup = NULL;
                        Entity->Player.PickupCooldown = 0.5;
                    }
                    
                    if(Entity->Player.PickupCooldown > 0.0)
                    {
                        Entity->Player.PickupCooldown -= DeltaTime;
                    }
                    
                    Entity->Position += Entity->Velocity;
                    
                    collision_info CollisionInfo;
                    CheckCollision(GameState, Entity, &CollisionInfo);
                    
                    /*
                    if((Entity->Velocity.x < 0 && !(CollisionInfo.Side & Side_Left)) 
                    || (Entity->Velocity.x > 0 && !(CollisionInfo.Side & Side_Right)))
                    {
                    Entity->Position.x += Entity->Velocity.x;
                    }
                    
                    if((Entity->Velocity.y > 0 && !(CollisionInfo.Side & Side_Top)) || (Entity->Velocity.y < 0 && !(CollisionInfo.Side & Side_Bottom)))
                    {
                    Entity->Position.y += Entity->Velocity.y;
                    }
                    */
                    
                    if(Entity->Player.Pickup)
                    {
                        Entity->Player.Pickup->Position.x += Entity->Velocity.x;
                        Entity->Player.Pickup->Position.y += Entity->Velocity.y;
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
                //CheckCollision(GameState, Entity, &CollisionInfo); //TODO(Daniel) this makes it bug out
                
                if(Entity->Velocity.x > 0.7f * DeltaTime)
                {
                    
                    Entity->Position += Entity->Velocity;
                    Entity->Velocity.x -= 0.7f * DeltaTime;
                }
                else if(Entity->Velocity.x < -0.7f * DeltaTime)
                {
                    Entity->Position += Entity->Velocity;
                    Entity->Velocity.x += 0.7f * DeltaTime;
                }
                else 
                {
                    Entity->Velocity = glm::vec2(0.0f,0.0f);
                }
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
