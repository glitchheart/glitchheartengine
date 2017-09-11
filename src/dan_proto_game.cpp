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
        Player.Player.Offset = math::v3(-105, 30, 0);
        Player.Player.Size = math::v3(150, 20, 0);
        GameState->Loaded = true;
    }
    
    if(KEY(Key_W))
    {
        Player.Position.y += 400.0f * DeltaTime;
    }
    else if(KEY(Key_S))
    {
        Player.Position.y += -400.0f * DeltaTime;
    }
    
    if(KEY(Key_A))
    {
        Player.Position.x += -400.0f * DeltaTime;
    }
    else if(KEY(Key_D))
    {
        Player.Position.x += 400.0f * DeltaTime;
    }
    
    if(KEY_DOWN(Key_Up))
    {
        Player.Player.Offset = math::v3(30, 40, 0);
        Player.Player.Size = math::v3(20, 150, 0);
    }
    else if(KEY_DOWN(Key_Down))
    {
        Player.Player.Offset = math::v3(30, -105, 0);
        Player.Player.Size = math::v3(20, 150, 0);
    }
    else if(KEY_DOWN(Key_Left))
    {
        Player.Player.Offset = math::v3(-105, 30, 0);
        Player.Player.Size = math::v3(150, 20, 0);
    }
    else if(KEY_DOWN(Key_Right))
    {
        Player.Player.Offset = math::v3(40, 30, 0);
        Player.Player.Size = math::v3(150, 20, 0);
    }
    
    for(i32 X = 0; X < 10; X++)
    {
        for(i32 Y = 0; Y < 10; Y++)
        {
            PushFilledQuad(Renderer, math::v3(Renderer.WindowWidth / 2 - 500 + 100 * X, 10 + 100 * Y, 0), math::v3(95, 95, 0), math::v3(), math::rgba(0.2, 0.5, 0.6, 1.0));
        }
    }
    
    PushFilledQuad(Renderer, math::v3(Player.Position.x + Player.Player.Offset.x, Player.Position.y + Player.Player.Offset.y, 0), Player.Player.Size, math::v3(), math::rgba(0, 0, 1, 1));
    PushFilledQuad(Renderer, Player.Position, math::v3(80, 80, 0), math::v3(), math::rgba(1, 1, 1, 1));
}