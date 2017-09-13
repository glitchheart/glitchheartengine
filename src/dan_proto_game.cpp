#include "dan_proto_game.h"

#include "gmap.cpp"
#include "rendering.cpp"
#include "skeletal_animation.cpp"
#include "keycontroller.cpp"
#include "ui.cpp"
#include "sound.cpp"
#include "debug.cpp"

#define DEBUG

platform_api Platform;

extern "C" UPDATE(Update)
{
    Platform = GameMemory->PlatformAPI;
    
#if GLITCH_DEBUG
    debug_state* DebugState = GameMemory->DebugState;
#endif
    
    game_state* GameState = GameMemory->GameState;
    
    if(!GameState)
    {
        DEBUG_PRINT("Initializing gamestate\n");
        
        GameState = GameMemory->GameState = BootstrapPushStruct(game_state, TotalArena);
        GameState->Loaded = false;
    }
    Assert(GameState);
    
    entity& Player = GameState->Player;
    if(!GameState->Loaded)
    {
        Player.Player.MovementSpeed = 40.0f;
        Player.Player.Offset = math::v3(-10.5f, 3, 0);
        Player.Player.Size = math::v3(12, 1, 0);
        GameState->Loaded = true;
        
        Renderer.ClearColor = math::rgba(1, 1, 1, 1);
        Renderer.CurrentCameraHandle = 0;
        Renderer.Cameras[0].Zoom = 5.0f;
        Renderer.Cameras[0].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[0].ViewportHeight = Renderer.WindowHeight;
        CameraTransform(Renderer, Renderer.Cameras[0], math::v3(), math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
    }
    
    if(KEY(Key_W))
    {
        Player.Position.y += 40.0f * DeltaTime;
    }
    else if(KEY(Key_S))
    {
        Player.Position.y += -40.0f * DeltaTime;
    }
    
    if(KEY(Key_A))
    {
        Player.Position.x += -40.0f * DeltaTime;
    }
    else if(KEY(Key_D))
    {
        Player.Position.x += 40.0f * DeltaTime;
    }
    
    if(KEY_DOWN(Key_Up))
    {
        Player.Player.Offset = math::v3(3, 4, 0);
        Player.Player.Size = math::v3(1, 12, 0);
    }
    else if(KEY_DOWN(Key_Down))
    {
        Player.Player.Offset = math::v3(3, -10.5f, 0);
        Player.Player.Size = math::v3(1, 12, 0);
    }
    else if(KEY_DOWN(Key_Left))
    {
        Player.Player.Offset = math::v3(-10.5f, 3, 0);
        Player.Player.Size = math::v3(12, 1, 0);
    }
    else if(KEY_DOWN(Key_Right))
    {
        Player.Player.Offset = math::v3(4, 3, 0);
        Player.Player.Size = math::v3(12, 1, 0);
    }
    
    for(i32 X = 0; X < 10; X++)
    {
        for(i32 Y = 0; Y < 10; Y++)
        {
            PushFilledQuad(Renderer, math::v3((5 * 8) + 8 * X, 5 + 8 * Y, 0), math::v3(7.5f, 7.5f, 0), math::v3(), math::rgba(0.2, 0.5, 0.6, 1.0), 0, false);
        }
    }
    
    PushFilledQuad(Renderer, Player.Position, math::v3(8, 8, 0), math::v3(), math::rgba(0, 0, 0, 1), 0, false);
    PushFilledQuad(Renderer, math::v3(Player.Position.x + Player.Player.Offset.x, Player.Position.y + Player.Player.Offset.y, 0), Player.Player.Size, math::v3(), math::rgba(0.2, 0.2, 0.2, 1), 0, false);
}