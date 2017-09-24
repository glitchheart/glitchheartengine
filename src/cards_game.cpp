#include "cards_game.h"
#include "gmap.cpp"
#include "rendering.cpp"
#include "skeletal_animation.cpp"
#include "keycontroller.cpp"
#include "ui.cpp"
#include "sound.cpp"
#include "collision.cpp"
#if GLITCH_DEBUG
#include "debug.cpp"
#endif

platform_api Platform;

static entity* AddEntity(game_state* GameState, renderer& Renderer, EType Type, math::v3 P = math::v3(), math::quat O = math::quat(),  math::v3 S = math::v3(1.0f))
{
    auto* Entity = &GameState->Entities[GameState->EntityCount++];
    Entity->Position = P;
    Entity->Orientation = O;
    Entity->Scale = S;
    Entity->Type = Type;
    
    return Entity;
}

static void InitGrid(game_state* GameState)
{
    GameState->Grid.Size = math::v2(GRID_X, GRID_Y);
    GameState->Grid.TileScale = 4.0f;
    GameState->Grid.GridOffset = math::v3(10.0f, 8.0f, 0.0f);
    for(i32 I = 0; I < GameState->Grid.Size.x; I++)
    {
        for(i32 J = 0; J < GameState->Grid.Size.y; J++)
        {
            auto& Tile = GameState->Grid.Grid[I][J];
            auto IsEmpty = math::RandomInt(0,4) > 2;
            i32 R = IsEmpty ? 0 : math::RandomInt(1, CARDS + 1);
            Tile.Type = R;
            Tile.Walked = false;
        }
    } 
}

static math::v3 FindNonEmptyPosition(game_state* GameState)
{
    for(i32 I = 0; I < GameState->Grid.Size.x; I++)
    {
        for(i32 J = 0; J < GameState->Grid.Size.y; J++)
        {
            if(GameState->Grid.Grid[I][J].Type != 0)
                return math::v3(I * GameState->Grid.TileScale, J * GameState->Grid.TileScale, 0);
        }
    }
    return math::v3();
}

static void InitializeLevel(game_state* GameState, entity* Player)
{
    InitGrid(GameState);
    Player->Position = FindNonEmptyPosition(GameState);
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
        GameState->IsInitialized = false;
    }
    
    Assert(GameState);
    
    if(!GameState->IsInitialized || !GameMemory->IsInitialized)
    {
        auto* Player = AddEntity(GameState, Renderer, EType_Player, math::v3(), math::quat(), math::v3(4.0f, 4.0f, 1.0f));
        Player->Player.MoveTimer.TimerMax = 0.05f;
        Player->Player.Speed = 10.0f;
        Renderer.CurrentCameraHandle = 0;
        Renderer.ClearColor = math::rgba(0.3f, 0.3f, 0.3f, 1.0f);
        Renderer.Cameras[Renderer.CurrentCameraHandle].Zoom = 20.0f;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportHeight = Renderer.WindowHeight;
        Renderer.Cameras[Renderer.CurrentCameraHandle].Position = math::v3(32.5f, 15, 0);
        
        sounds Sounds = {};
        //@Incomplete: Get actual sizes, this is retarded
        memcpy(&GameState->Sounds.SoundEffects, SoundEffects, sizeof(sound_effect) * (64 + 32));
        
        LoadTextures(Renderer, &Renderer.TextureArena, Concat(CARDS_ASSETS, "textures/"));
        
        InitializeLevel(GameState, Player);
        
        GameState->IsInitialized = true;
        GameMemory->IsInitialized = true;
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    
    auto Near = -100.0f;
    auto Far = 1000.0f;
    
    CameraTransform(Renderer, Camera, Camera.Position, Camera.Orientation, Camera.Target, Camera.Zoom, Near, Far);
    
    DisableDepthTest(Renderer);
    
    for(i32 I = 0; I < GameState->Grid.Size.x; I++)
    {
        for(i32 J = 0; J < GameState->Grid.Size.y; J++)
        {
            auto Type = GameState->Grid.Grid[I][J].Type;
            
            char* Texture = Type == 0 ? "none" : Concat("card", ToString(Type));
            auto C = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
            
            auto TilePos = math::v3(I * GameState->Grid.TileScale, J * GameState->Grid.TileScale, 0.0f);
            
            PushFilledQuad(Renderer, TilePos, math::v3(GameState->Grid.TileScale, GameState->Grid.TileScale, 1.0f), math::v3(), C, Texture, false);
            
            if(GameState->Grid.Grid[I][J].Type > 0)
            {
                auto Walked = GameState->Grid.Grid[I][J].Walked;
                C = math::rgba(0.0f, 1.0f, 1.0f, 1.0f - (Walked / 4.0f));
                PushFilledQuad(Renderer, TilePos, math::v3(GameState->Grid.TileScale, GameState->Grid.TileScale, 1.0f), math::v3(), C, 0, false);
            }
            
            PushFilledQuad(Renderer, TilePos, math::v3(GameState->Grid.TileScale, GameState->Grid.TileScale, 1.0f), math::v3(), math::rgba(1.0f, 1.0f, 1.0f, 1.0f), "border", false);
        }
    } 
    
    FOR_ENT(Index)
    {
        auto* Entity = &GameState->Entities[Index];
        if(auto* Player = GET_ENT(Entity, Player))
        {
            auto V = math::v3i();
            if(KEY_DOWN(Key_W))
            {
                V.y = 1;
            }
            else if(KEY_DOWN(Key_S))
            {
                V.y = -1;
            }
            
            if(KEY_DOWN(Key_A))
            {
                V.x = -1;
            }
            else if(KEY_DOWN(Key_D))
            {
                V.x = 1;
            }
            
            if(Entity->Position.x / GameState->Grid.TileScale == 0 && V.x < 0)
                V.x = 0;
            
            if(Entity->Position.x / GameState->Grid.TileScale == GameState->Grid.Size.x - 1 && V.x > 0)
                V.x = 0;
            
            if(Entity->Position.y / GameState->Grid.TileScale == 0 && V.y < 0)
                V.y = 0;
            
            if(Entity->Position.y / GameState->Grid.TileScale == GameState->Grid.Size.y - 1&& V.y > 0)
                V.y = 0;
            
            Entity->Velocity = math::v3(V.x, V.y, V.z);
            
            auto NextPos = Entity->Position + Entity->Velocity * GameState->Grid.TileScale;
            
            auto NextTile = GameState->Grid.Grid[(i32)NextPos.x / (i32) GameState->Grid.TileScale][(i32)NextPos.y / (i32)GameState->Grid.TileScale];
            
            if(NextTile.Type == 0)
            {
                Entity->Velocity = math::v3();
            }
            else
            {
                if(math::Length(Entity->Velocity) > 0.0f && TimerDone(GameState, Player->MoveTimer))
                {
                    Entity->Position += Entity->Velocity * GameState->Grid.TileScale;
                    StartTimer(GameState, Player->MoveTimer);
                }
                
                PushText(Renderer, ToString(GameState->Timers[Player->MoveTimer.TimerHandle]), math::v3(50.0f, 50.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
            }
            
            auto& CurrentTile = GameState->Grid.Grid[(i32)Entity->Position.x / (i32)GameState->Grid.TileScale][(i32)Entity->Position.y / (i32)GameState->Grid.TileScale];
            
            if(math::Length(Entity->Velocity) > 0.0f)
                CurrentTile.Walked++;
            
            if(KEY_DOWN(Key_Enter))
            {
                CurrentTile.Type = math::RandomInt(1, CARDS + 1);
            }
            
            PushFilledQuad(Renderer, Entity->Position, Entity->Scale, math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 1.0f), "player", false);
            
            if(KEY_DOWN(Key_R))
            {
                InitializeLevel(GameState, Entity);
            }
        }
    }
    
    EnableDepthTest(Renderer);
    
    if(KEY_DOWN(Key_Escape))
    {
        GameMemory->ExitGame = true;
    }
    
    Renderer.ShowMouseCursor = true;
    
    TickTimers(GameState, DeltaTime);
    
    GameState->PrevMouseX = (r32)InputController->MouseX;
    GameState->PrevMouseY = (r32)InputController->MouseY;
}


