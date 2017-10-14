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

static void InitializePlayer(game_state* GameState, renderer& Renderer, entity& Player)
{
    Player.Player.BulletCount = 0;
    LoadTexture("../assets/textures/spaceship.png", Renderer, &GameState->TotalArena, &Player.TextureHandle);
}

static void CreateLevel(game_state* GameState, i32 ChunkWidth, i32 ChunkHeight)
{
    GameState->Level.ChunkWidth = ChunkWidth;
    GameState->Level.ChunkHeight = ChunkHeight;
    
    GameState->Level.Chunks = PushArray(&GameState->WorldArena, ChunkWidth, level_chunk*);
    
    for(i32 Index = 0; Index < ChunkHeight; Index++)
    {
        GameState->Level.Chunks[Index] = PushArray(&GameState->WorldArena, ChunkHeight, level_chunk);
    }
}

static void AddBullet(entity& Player)
{
    auto Dir = RotateByAngle(math::v2(0, 1.0f), Player.Rotation.z * 0.0174532925f);
    Player.Player.Bullets[Player.Player.BulletCount].Direction = math::v3(Dir.x, Dir.y, 0.0f);
    
    Player.Player.Bullets[Player.Player.BulletCount].Position = math::v3(Player.Position.x + 4.0f, Player.Position.y + 4.0f, 0.0f) + Player.Player.Bullets[Player.Player.BulletCount].Direction * 4.0f;
    Player.Player.Bullets[Player.Player.BulletCount].Speed = 35.0f;
    Player.Player.BulletCount++;
}

static void UpdatePlayer(input_controller* InputController, game_state* GameState, entity& Player, r64 DeltaTime)
{
    if(KEY(Key_Left))
    {
        Player.Rotation.z += 120.0f * DeltaTime;
    }
    else if(KEY(Key_Right))
    {
        Player.Rotation.z -= 120.0f * DeltaTime;
    }
    
    if(KEY(Key_Space))
    {
        Player.Player.Thrust = 10.0f;
    }
    else
    {
        Player.Player.Thrust = 0.0f;
        Player.Velocity.x *= (1.0f - 0.3f * DeltaTime);
        Player.Velocity.y *= (1.0f - 0.3f * DeltaTime);
    }
    
    if(KEY_DOWN(Key_Z))
    {
        AddBullet(Player);
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
    
    auto Lowest = GameState->LowestPositionInWorld;
    auto Highest = GameState->HighestPositionInWorld;
    
    // If the player gets out of viewport bounds
    if(Player.Position.x < Lowest.x - 1.0f)
    {
        Player.Position.x = Highest.x - 1.0f;
    }
    else if(Player.Position.x > Highest.x + 1.0f)
    {
        Player.Position.x = Lowest.x + 1.0f;
    }
    
    if(Player.Position.y < Lowest.y - 1.0f)
    {
        Player.Position.y = Highest.y - 1.0f;
    }
    else if(Player.Position.y > Highest.y + 1.0f)
    {
        Player.Position.y = Lowest.y + 1.0f;
    }
    
    for(i32 Index = 0; Index < Player.Player.BulletCount; Index++)
    {
        bullet& Bullet = Player.Player.Bullets[Index];
        Bullet.Position.x += Bullet.Direction.x * Bullet.Speed * DeltaTime;
        Bullet.Position.y += Bullet.Direction.y * Bullet.Speed * DeltaTime;
    }
}

static void RenderPlayer(game_state* GameState, renderer& Renderer, entity& Player)
{
    PushFilledQuad(Renderer, Player.Position, math::v3(8, 8, 0), math::v3(Player.Rotation.x, Player.Rotation.y, Player.Rotation.z), math::rgba(1.0, 1.0, 1.0, 1.0), Player.TextureHandle, false);
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
        GameState = GameMemory->GameState = BootstrapPushStruct(game_state, TotalArena);
        GameState->Loaded = false;
        LoadFont(Renderer, Concat("../assets/", "/fonts/pixelart.ttf"), 60, "Pixelart");
        InitializePlayer(GameState, Renderer, GameState->Player);
        CreateLevel(GameState, 20, 20);
        LoadTexture("../assets/textures/bullet_simple.png", Renderer, &GameState->TotalArena, &GameState->BulletTextureHandle);
        LoadTexture("../assets/textures/enemy_star.png", Renderer, &GameState->TotalArena, &GameState->StarEnemyTextureHandle);
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
        
        Renderer.Cameras[0].Zoom = 8.0f;
        CameraTransform(Renderer, Renderer.Cameras[0], Player.Position, math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
        GameState->LowestPositionInWorld = math::UnProject(math::v3(0, Renderer.Viewport[3]  - Renderer.ViewportHeight, 0),
                                                           Renderer.Cameras[0].ViewMatrix,
                                                           Renderer.Cameras[0].ProjectionMatrix,
                                                           math::v4i(0, 0, Renderer.Viewport[2], Renderer.Viewport[3]));
        GameState->HighestPositionInWorld = UnProject(math::v3(Renderer.ViewportWidth, Renderer.Viewport[3], 0),
                                                      Renderer.Cameras[0].ViewMatrix,
                                                      Renderer.Cameras[0].ProjectionMatrix,
                                                      math::v4i(0, 0, Renderer.Viewport[2], Renderer.Viewport[3]));
    }
    
    UpdatePlayer(InputController, GameState, Player, DeltaTime);
    CameraTransform(Renderer, Renderer.Cameras[0], Player.Position, math::quat(), math::v3(), Renderer.Cameras[0].Zoom);
    
    DisableDepthTest(Renderer);
    RenderPlayer(GameState, Renderer, Player);
    PushText(Renderer, "GEGENSCHEIN", math::v3(Renderer.ViewportWidth / 2, Renderer.ViewportHeight / 2, 0), GameState->PixelArtFontHandle, math::rgba(1.0f, 1.0f, 1.0f, 1.0f), Alignment_Center);
    
    for(i32 Index = 0; Index < Player.Player.BulletCount; Index++)
    {
        PushFilledQuad(Renderer, Player.Player.Bullets[Index].Position, math::v3(2.0f, 2.0f, 0.0f), math::v3(), math::rgba(1.0, 1.0, 1.0, 1.0), GameState->BulletTextureHandle, false);
    }
    
    EnableDepthTest(Renderer);
}