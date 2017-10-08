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

static void UpdatePlayer(input_controller* InputController, game_state* GameState, entity& Player, r64 DeltaTime)
{
    if(KEY(Key_Left))
    {
        Player.Rotation.z += 50.0f * DeltaTime;
    }
    else if(KEY(Key_Right))
    {
        Player.Rotation.z -= 50.0f * DeltaTime;
    }
    
    if(KEY(Key_Space))
    {
        Player.Player.Thrust = 10.0f;
    }
    else
    {
        Player.Player.Thrust = 0.0f;
    }
    
    // Handle physics
    r32 Rotation = Player.Rotation.z * 0.0174532925f;
    Player.Player.Acceleration.x += Player.Player.Thrust * cos(Rotation + 1.57f);
    Player.Player.Acceleration.y += Player.Player.Thrust * sin(Rotation + 1.57f);
    
    Player.Position.x += Player.Velocity.x * DeltaTime;
    Player.Position.y += Player.Velocity.y * DeltaTime;
    Player.Velocity.x += Player.Player.Acceleration.x * 2.0f * DeltaTime;
    Player.Velocity.y += Player.Player.Acceleration.y * 2.0f * DeltaTime;
    Player.Player.Acceleration = math::v3(0.0f, 0.0f, 0.0f);
}

static void RenderPlayer(game_state* GameState, renderer& Renderer, entity& Player)
{
    PushFilledQuad(Renderer, Player.Position + math::v3(1.0f, 1.0f, 0), math::v3(6 * GameState->TileScale, 6 * GameState->TileScale, 0), math::v3(Player.Rotation.x, Player.Rotation.y, Player.Rotation.z), math::rgba(1.0, 1.0, 1.0, 1.0), "spaceship", false);
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
        LoadTextures(Renderer, &GameState->TotalArena);
    }
    
    Assert(GameState);
    
    entity& Player = GameState->Player;
    
    if(!GameState->Loaded)
    {
        GameState->TileScale = 1.0f;
        Player.Player.MovementSpeed = 20.0f;
        Player.Rotation = math::v3();
        
        GameState->Loaded = true;
        
        Renderer.ClearColor = math::rgba(0, 0, 0, 1);
        Renderer.CurrentCameraHandle = 0;
        Renderer.Cameras[0].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[0].ViewportHeight = Renderer.WindowHeight;
    }
    
    UpdatePlayer(InputController, GameState, Player, DeltaTime);
    
    Renderer.Cameras[0].Zoom = 8.0f;
    CameraTransform(Renderer, Renderer.Cameras[0], math::v3(30, 40, 0), math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
    
    DisableDepthTest(Renderer);
    RenderPlayer(GameState, Renderer, Player);
    EnableDepthTest(Renderer);
}