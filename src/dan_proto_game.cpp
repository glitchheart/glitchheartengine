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

static void RenderGrid(game_state* GameState, renderer& Renderer)
{
    for(i32 X = 0; X < 10; X++)
    {
        for(i32 Y = 0; Y < 10; Y++)
        {
            PushFilledQuad(Renderer, math::v3(8 * GameState->TileScale * X, 8 * GameState->TileScale * Y, 1), math::v3(7.8f * GameState->TileScale, 7.8f * GameState->TileScale, 0), math::v3(), math::rgba(0.2, 0.5, 0.6, 1.0), 0, false);
        }
    }
}

static void RenderPlayer(game_state* GameState, renderer& Renderer, entity& Player)
{
    PushFilledQuad(Renderer, Player.Position + math::v3(1.0f, 1.0f, 0), math::v3(6 * GameState->TileScale, 6 * GameState->TileScale, 0), math::v3(), math::rgba(0, 0, 0, 1), 0, false);
    PushFilledQuad(Renderer, Player.Position + math::v3(1.0f, 1.0f, 0), math::v3(1 * GameState->TileScale, 6 * GameState->TileScale, 0), math::v3(), math::rgba(0, 0, 1, 1), 0, false);
    //PushFilledQuad(Renderer, Player.Position + math::v3(1.0f, 1.0f, 0), math::v3(6 * GameState->TileScale, 6 * GameState->TileScale, 0), math::v3(), math::rgba(0, 0, 0, 1), 0, false);
    //PushFilledQuad(Renderer, math::v3(Player.Position.x + Player.Player.Offset.x, Player.Position.y + Player.Player.Offset.y, 0), Player.Player.Size, math::v3(), math::rgba(0.2, 0.2, 0.2, 1), 0, false);
}

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
        GameState->TileScale = 1.0f;
        Player.Player.MovementSpeed = 20.0f;
        Player.Player.Offset = math::v3(1.5f * GameState->TileScale, 4 * GameState->TileScale, 0);
        Player.Player.Size = math::v3(1 * GameState->TileScale, 5 * GameState->TileScale, 0);
        
        Player.Player.TargetTile = math::v2i(0, 1);
        GameState->Loaded = true;
        
        Renderer.ClearColor = math::rgba(1, 1, 1, 1);
        Renderer.CurrentCameraHandle = 0;
        Renderer.Cameras[0].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[0].ViewportHeight = Renderer.WindowHeight;
    }
    
    Renderer.Cameras[0].Zoom = 8.0f;
    CameraTransform(Renderer, Renderer.Cameras[0], math::v3(30, 40, 0), math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
    
    auto CurrentDirection = Player.Player.CurrentDirection;
    
    if(KEY_DOWN(Key_W))
    {
        if(CurrentDirection == Left || CurrentDirection == Right)
        {
            Player.Player.QueuedDirection = Up;
        }
    }
    else if(KEY_DOWN(Key_S))
    {
        if(CurrentDirection == Left || CurrentDirection == Right)
        {
            Player.Player.QueuedDirection = Down;
        }
    }
    
    if(KEY_DOWN(Key_A))
    {
        if(CurrentDirection == Up || CurrentDirection == Down)
        {
            Player.Player.QueuedDirection = Left;
        }
    }
    else if(KEY_DOWN(Key_D))
    {
        if(CurrentDirection == Up || CurrentDirection == Down)
        {
            Player.Player.QueuedDirection = Right;
        }
    }
    
    switch(Player.Player.CurrentDirection)
    {
        case Up:
        {
            Player.Position.y += Player.Player.MovementSpeed * DeltaTime;
        }
        break;
        case Down:
        {
            Player.Position.y += -Player.Player.MovementSpeed * DeltaTime;
        }
        break;
        case Left:
        {
            Player.Position.x += -Player.Player.MovementSpeed * DeltaTime;
        }
        break;
        case Right:
        {
            Player.Position.x += Player.Player.MovementSpeed * DeltaTime;
        }
        break;
    }
    
    if(Abs(Player.Position.x - (r32)Player.Player.TargetTile.x * 8) < 0.08f && Abs(Player.Position.y - (r32)Player.Player.TargetTile.y * 8) < 0.08f)
    {
        Player.Player.CurrentDirection = Player.Player.QueuedDirection;
        
        switch(Player.Player.CurrentDirection)
        {
            case Up:
            {
                Player.Player.TargetTile.y += 1;
                if(Player.Player.TargetTile.y == 10)
                {
                    Player.Player.TargetTile.y -= 2;
                    Player.Player.CurrentDirection = Down;
                    Player.Player.QueuedDirection = Player.Player.CurrentDirection;
                }
            }
            break;
            case Down:
            {
                Player.Player.TargetTile.y -= 1;
                if(Player.Player.TargetTile.y == -1)
                {
                    Player.Player.TargetTile.y += 2;
                    Player.Player.CurrentDirection = Up;
                }
            }
            break;
            case Left:
            {
                Player.Player.TargetTile.x -= 1;
                if(Player.Player.TargetTile.x == -1)
                {
                    Player.Player.TargetTile.x += 2;
                    Player.Player.CurrentDirection = Right;
                }
            }
            break;
            case Right:
            {
                Player.Player.TargetTile.x += 1;
                if(Player.Player.TargetTile.x == 10)
                {
                    Player.Player.TargetTile.x -= 2;
                    Player.Player.CurrentDirection = Left;
                }
            }
            break;
        }
        
        Player.Player.QueuedDirection = Player.Player.CurrentDirection;
    }
    
    DisableDepthTest(Renderer);
    RenderGrid(GameState, Renderer);
    RenderPlayer(GameState, Renderer, Player);
    EnableDepthTest(Renderer);
}