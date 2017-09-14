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
            PushFilledQuad(Renderer, math::v3(8 * GameState->TileScale * X,  8 * GameState->TileScale * Y, 1), math::v3(8.0f * GameState->TileScale, 8.0f * GameState->TileScale, 0), math::v3(), math::rgba(0.2, 0.5, 0.6, 1.0), 0, false);
        }
    }
}

static void RenderPlayer(game_state* GameState, renderer& Renderer, entity& Player)
{
    PushFilledQuad(Renderer, Player.Position, math::v3(4 * GameState->TileScale, 4 * GameState->TileScale, 0), math::v3(), math::rgba(0, 0, 0, 1), 0, false);
    PushFilledQuad(Renderer, math::v3(Player.Position.x + Player.Player.Offset.x, Player.Position.y + Player.Player.Offset.y, 0), Player.Player.Size, math::v3(), math::rgba(0.2, 0.2, 0.2, 1), 0, false);
}

static void RenderBoxes(game_state* GameState, renderer& Renderer)
{
    auto Position = math::v3((5 * 8 * GameState->TileScale) + 8 * 3 * GameState->TileScale + 1.25f * GameState->TileScale, 8 * 3 * GameState->TileScale + 1.25f * GameState->TileScale, 0);
    PushFilledQuad(Renderer, Position, math::v3(5 * GameState->TileScale, 5 * GameState->TileScale, 0), math::v3(), math::rgba(1, 0, 0, 1), 0, false);
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
        GameState->TileScale = 2;
        Player.Player.MovementSpeed = 40.0f;
        Player.Player.Offset = math::v3(1.5f * GameState->TileScale, 4 * GameState->TileScale, 0);
        Player.Player.Size = math::v3(1 * GameState->TileScale, 5 * GameState->TileScale, 0);
        Player.Player.TargetTile = math::v2i(0, 2);
        GameState->Loaded = true;
        
        Renderer.ClearColor = math::rgba(1, 1, 1, 1);
        Renderer.CurrentCameraHandle = 0;
        Renderer.Cameras[0].Zoom = 4.0f;
        Renderer.Cameras[0].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[0].ViewportHeight = Renderer.WindowHeight;
        CameraTransform(Renderer, Renderer.Cameras[0], math::v3(120, 70, 0), math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
    }
    
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
    
    if(KEY_DOWN(Key_Up))
    {
        Player.Player.Offset = math::v3(1.5f * GameState->TileScale, 4 * GameState->TileScale, 0);
        Player.Player.Size = math::v3(1 * GameState->TileScale, 5 * GameState->TileScale, 0);
    }
    else if(KEY_DOWN(Key_Down))
    {
        Player.Player.Offset = math::v3(1.5f * GameState->TileScale, -5 * GameState->TileScale, 0);
        Player.Player.Size = math::v3(1 * GameState->TileScale, 5* GameState->TileScale, 0);
    }
    else if(KEY_DOWN(Key_Left))
    {
        Player.Player.Offset = math::v3(-5.0f * GameState->TileScale, 1.5f * GameState->TileScale, 0);
        Player.Player.Size = math::v3(5* GameState->TileScale, 1 * GameState->TileScale, 0);
    }
    else if(KEY_DOWN(Key_Right))
    {
        Player.Player.Offset = math::v3(4 * GameState->TileScale, 1.5f * GameState->TileScale, 0);
        Player.Player.Size = math::v3(5 * GameState->TileScale, 1 * GameState->TileScale, 0);
    }
    
    if(Abs(Player.Position.x / GameState->TileScale - Player.Player.TargetTile.x) < 0.07f && Abs(Player.Position.y / GameState->TileScale - Player.Player.TargetTile.y) < 0.07f)
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
    
    DisableDepthTest(Renderer);
    RenderGrid(GameState, Renderer);
    RenderPlayer(GameState, Renderer, Player);
    RenderBoxes(GameState, Renderer);
    EnableDepthTest(Renderer);
}