#include "game.h"
#include "keycontroller.cpp"
#include "keys_glfw.h"
#include "sound.cpp"

extern "C" UPDATE(Update)
{
    if (GetKeyDown(Key_Enter, GameState) && !GameState->Console.Open)
    {
        PlaySoundEffectOnce(GameState, &GameState->SoundManager.Track01);
    }
    
    if (!GameState->Console.Open)
    {
        //player movement
        if (GetKey(Key_A, GameState))
        {
            GameState->Player.Position.x += -GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        else if (GetKey(Key_D, GameState))
        {
            GameState->Player.Position.x += GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        
        if (GetKey(Key_W, GameState))
        {
            GameState->Player.Position.y += -GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        else if (GetKey(Key_S, GameState))
        {
            GameState->Player.Position.y += GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        
        
        if(GetKey(Key_A,GameState))
        {
            printf("Key\n");
        }
        if(GetKeyDown(Key_A,GameState))
        {
            printf("Key Down\n");
        }
        
        if(GetJoystickKey(JOYSTICK_1,GameState))
        {
            printf("Joystick Key\n");
        }
        if(GetJoystickKeyDown(JOYSTICK_1,GameState))
        {
            printf("Joystick Key Down\n");
        }
    }
    
    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX,GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
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
