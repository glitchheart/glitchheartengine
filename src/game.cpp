#include "game.h"
#include "keycontroller.cpp"
#include "keys_glfw.h"
#include "sound.cpp"
#include "animation.cpp"
#include "collision.cpp"

#define DEBUG

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
    
    if(CheckCollision(&Collider1, &Collider2))
    {
        //printf("COLLIDING\n");
    }
    
    if (GetKeyDown(Key_Enter, GameState) && !GameState->Console.Open)
    {
        PlaySoundEffectOnce(GameState, &GameState->SoundManager.Track01);
    }
    
    if (!GameState->Console.Open)
    {
        float VelX = 0.0f;
        float VelY = 0.0f;
        
        //player movement
        if (GetKey(Key_A, GameState))
        {
            VelX = -GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        else if (GetKey(Key_D, GameState))
        {
            VelX =GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        
        if (GetKey(Key_W, GameState))
        {
            VelY = -GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        else if (GetKey(Key_S, GameState))
        {
            VelY = GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        
        if(GameState->Player.player.IsAttacking && !GameState->Player.Animations[GameState->Player.CurrentAnimation].Playing)
        {
            GameState->Player.player.IsAttacking = false;
        }
        
        if(!GameState->Player.player.IsAttacking)
        {
            if(VelX != 0.0f || VelY != 0.0f)
                PlayAnimation(&GameState->Player, "player_walk");
            else
                PlayAnimation(&GameState->Player, "player_idle");
        }
        
        GameState->Player.Position.x += VelX;
        GameState->Player.Position.y += VelY;
    }
    
    //attacking
    if(!GameState->Player.player.IsAttacking && GetMouseButtonDown(Mouse_Left, GameState))
    {
        PlayAnimation(&GameState->Player, "player_attack");
        GameState->Player.player.IsAttacking = true;
    }
    
    if(GameState->Player.CurrentAnimation)
        TickAnimation(&GameState->Player.Animations[GameState->Player.CurrentAnimation],DeltaTime);
    
    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                              GameState->Camera.ViewMatrix,
                              GameState->Camera.ProjectionMatrix,
                              glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    
    auto direction = glm::vec2(pos.x, pos.y) - GameState->Player.Position;
    direction = glm::normalize(direction);
    float degrees = atan2(direction.y, direction.x);
    
    GameState->Player.Rotation = glm::vec3(0, 0, degrees);
    GameState->Crosshair.Position = glm::vec2(pos.x - 0.5f, pos.y - 0.5f);
    
    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f,
                                                    static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom),
                                                    static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom),
                                                    0.0f,
                                                    -1.0f,
                                                    1.0f);
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(-GameState->Player.Position.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2,
                                                            -GameState->Player.Position.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2,
                                                            0));
}
