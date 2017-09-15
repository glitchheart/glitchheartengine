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
            PushFilledQuad(Renderer, math::v3(8 * GameState->TileScale * X, 8 * GameState->TileScale * Y, 1), math::v3(7.8f * GameState->TileScale, 7.8f * GameState->TileScale, 0), math::v3(), math::rgba(0.5, 0.1, 0.1, 1.0), 0, false);
        }
    }
}

static void RenderPlayer(game_state* GameState, renderer& Renderer, entity& Player)
{
    PushFilledQuad(Renderer, Player.Position + math::v3(1.0f, 1.0f, 0), math::v3(6 * GameState->TileScale, 6 * GameState->TileScale, 0), math::v3(), math::rgba(0.1, 0.4, 0.1, 1), 0, false);
    PushFilledQuad(Renderer, Player.Position + Player.Player.Offset * GameState->TileScale, Player.Player.Size * GameState->TileScale, math::v3(), math::rgba(0.2, 0.2, 0.2, 1), 0, false);
}

static void RenderBoxes(game_state* GameState, renderer& Renderer)
{
    for(i32 Index = 0; Index < 5; Index++)
    {
        entity& Box = GameState->Boxes[Index];
        PushFilledQuad(Renderer, Box.Position * 8.0f + math::v3(1.0f, 1.0f, 0), math::v3(6 * GameState->TileScale, 6 * GameState->TileScale, 0), math::v3(), math::rgba(0.7, 0.4, 0.4, 1), 0, false);
    }
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
        Player.Player.MovementSpeed = 40.0f;
        
        Player.Player.HorizontalSize = math::v3(5, 0.5f, 0);
        Player.Player.VerticalSize = math::v3(0.5f, 5, 0);
        
        Player.Player.UpOffset = math::v3(3.5f, 7.0f, 0);
        Player.Player.DownOffset = math::v3(3.5f, -4.0f, 0);
        Player.Player.LeftOffset = math::v3(-4.0f, 3.5f, 0);
        Player.Player.RightOffset = math::v3(7.0f, 3.5f, 0);
        
        Player.Player.TargetTile = math::v2i(0, 0);
        GameState->Loaded = true;
        
        Renderer.ClearColor = math::rgba(0, 0, 0, 1);
        Renderer.CurrentCameraHandle = 0;
        Renderer.Cameras[0].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[0].ViewportHeight = Renderer.WindowHeight;
        
        // Create test boxes
        entity& Box1 = GameState->Boxes[0];
        entity& Box2 = GameState->Boxes[1];
        entity& Box3 = GameState->Boxes[2];
        entity& Box4 = GameState->Boxes[3];
        entity& Box5 = GameState->Boxes[4];
        
        Box1.Box.Size = math::v3(4, 4, 0);
        Box2.Box.Size = math::v3(4, 4, 0);
        Box3.Box.Size = math::v3(4, 4, 0);
        Box4.Box.Size = math::v3(4, 4, 0);
        Box5.Box.Size = math::v3(4, 4, 0);
        
        Box1.Position = math::v3(2, 6, 0);
        Box2.Position = math::v3(1, 8, 0);
        Box3.Position = math::v3(0, 4, 0);
        Box4.Position = math::v3(4, 0, 0);
        Box5.Position = math::v3(9, 2, 0);
    }
    
    Renderer.Cameras[0].Zoom = 8.0f;
    CameraTransform(Renderer, Renderer.Cameras[0], math::v3(30, 40, 0), math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
    
    auto CurrentDirection = Player.Player.CurrentDirection;
    
    if(!Player.Player.IsMoving)
    {
        if(KEY_DOWN(Key_W) && Player.Player.TargetTile.y < 9)
        {
            Player.Player.CurrentDirection = Up;
            Player.Player.IsMoving = true;
            Player.Player.TargetTile.y += 1;
        }
        else if(KEY_DOWN(Key_S) && Player.Player.TargetTile.y > 0)
        {
            Player.Player.CurrentDirection = Down;
            Player.Player.IsMoving = true;
            Player.Player.TargetTile.y -= 1;
        }
        
        if(KEY_DOWN(Key_A) && Player.Player.TargetTile.x > 0)
        {
            Player.Player.CurrentDirection = Left;
            Player.Player.IsMoving = true;
            Player.Player.TargetTile.x -= 1;
        }
        else if(KEY_DOWN(Key_D) && Player.Player.TargetTile.x < 9)
        {
            Player.Player.CurrentDirection = Right;
            Player.Player.IsMoving = true;
            Player.Player.TargetTile.x += 1;
        }
        
        if(KEY_DOWN(Key_Up))
        {
            Player.Player.Size = Player.Player.VerticalSize;
            Player.Player.Offset = Player.Player.UpOffset;
        }
        else if(KEY_DOWN(Key_Down))
        {
            Player.Player.Size = Player.Player.VerticalSize;
            Player.Player.Offset = Player.Player.DownOffset;
        }
        else if(KEY_DOWN(Key_Left))
        {
            Player.Player.Size = Player.Player.HorizontalSize;
            Player.Player.Offset = Player.Player.LeftOffset;
        }
        else if(KEY_DOWN(Key_Right))
        {
            Player.Player.Size = Player.Player.HorizontalSize;
            Player.Player.Offset = Player.Player.RightOffset;
        }
    }
    
    if(Player.Player.IsMoving)
    {
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
            Player.Position = math::v3(Player.Player.TargetTile.x * 8.0f, Player.Player.TargetTile.y * 8.0f, 0);
            Player.Player.IsMoving = false;
        }
    }
    
    DisableDepthTest(Renderer);
    RenderGrid(GameState, Renderer);
    RenderPlayer(GameState, Renderer, Player);
    RenderBoxes(GameState, Renderer);
    EnableDepthTest(Renderer);
}