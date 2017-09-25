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
    auto& Grid = GameState->CurrentLevel.Grid;
    i32 X = 2;
    i32 Y = 2;
    Grid.Size = math::v2(X, Y);
    Grid.TileScale = 4.0f;
    grid_tile Tile = {};
    
    Grid.Grid = PushArray(&GameState->WorldArena, X * Y, grid_tile*);
    
    for(i32 I = 0; I < X; I++)
    {
        Grid.Grid[I] = PushArray(&GameState->WorldArena, Y, grid_tile);
    }
    
    Grid.Grid[0][0].Card = GameState->CurrentLevel.Cards[0];
    Grid.Grid[1][0].Card = GameState->CurrentLevel.Cards[1];
    Grid.Grid[0][1].Card = GameState->CurrentLevel.Cards[2];
    Grid.Grid[1][1].Card = GameState->CurrentLevel.Cards[3];
}

static void AddCard(level* Level, Suit_Type Type, char* TextureName)
{
    card Card;
    Card.Type = Type;
    Card.TextureName = TextureName;
    Level->Cards[Level->CardCount++] = Card;
}

static void InitializeLevel(game_state* GameState, entity* Player)
{
    level Level = {};
    
    AddCard(&Level, Suit_B1, "card1");
    AddCard(&Level, Suit_B1, "card2");
    AddCard(&Level, Suit_B1, "card3");
    
    AddCard(&Level, Suit_B2, "card4");
    AddCard(&Level, Suit_B2, "card5");
    AddCard(&Level, Suit_B2, "card6");
    
    AddCard(&Level, Suit_R1, "card7");
    AddCard(&Level, Suit_R1, "card8");
    AddCard(&Level, Suit_R1, "card9");
    
    AddCard(&Level, Suit_R2, "card10");
    AddCard(&Level, Suit_R2, "card11");
    AddCard(&Level, Suit_R2, "card12");
    
    GameState->CurrentLevel = Level;
    
    InitGrid(GameState);
    
    Player->Position = math::v3(0.0f, -16.0f, 0.0f) / GameState->CurrentLevel.Grid.TileScale;
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
        Renderer.ClearColor = math::rgba(0.0f);
        Renderer.Cameras[Renderer.CurrentCameraHandle].Zoom = 20.0f;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportWidth = Renderer.WindowWidth;
        Renderer.Cameras[Renderer.CurrentCameraHandle].ViewportHeight = Renderer.WindowHeight;
        Renderer.Cameras[Renderer.CurrentCameraHandle].Position = math::v3(5.0f, 0, 0);
        
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
    
    auto Grid = GameState->CurrentLevel.Grid;
    
    PushFilledQuad(Renderer, math::v3() - Grid.Size.x * Grid.TileScale * 0.5f, math::v3(2.0f * Grid.Size.x * Grid.TileScale, 2.0f * Grid.Size.y * Grid.TileScale, 0.0f), math::v3(), math::rgba(1.0f, 0.0f, 1.0f, 0.2f), 0, false);
    
    for(i32 I = 0; I < Grid.Size.x; I++)
    {
        for(i32 J = 0; J < Grid.Size.y; J++)
        {
            auto Card = Grid.Grid[I][J].Card;
            
            char* Texture = Card.TextureName;
            auto C = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
            
            auto TilePos = math::v3(I * Grid.TileScale, J * Grid.TileScale, 0.0f);
            
            PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), C, Texture, false);
            
            auto Walked = Grid.Grid[I][J].Walked;
            if(Walked) //@Incomplete: Remember empty tiles
            {
                C = math::rgba(0.0f, 1.0f, 1.0f, 0.2f);
                PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), C, 0, false);
            }
            
            PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), math::rgba(1.0f, 1.0f, 1.0f, 1.0f), "border", false);
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
            
            if(Entity->Position.x / Grid.TileScale == 0 && V.x < 0)
                V.x = 0;
            
            if(Entity->Position.x / Grid.TileScale == Grid.Size.x - 1 && V.x > 0)
                V.x = 0;
            
            if(Entity->Position.y / Grid.TileScale == 0 && V.y < 0)
                V.y = 0;
            
            if(Entity->Position.y / Grid.TileScale == Grid.Size.y - 1 && V.y > 0)
                V.y = 0;
            
            Entity->Velocity = math::v3(V.x, V.y, V.z);
            
            if(math::Length(Entity->Velocity) > 0.0f && TimerDone(GameState, Player->MoveTimer))
            {
                Entity->Position += Entity->Velocity * Grid.TileScale;
                StartTimer(GameState, Player->MoveTimer);
            }
            
            PushText(Renderer, ToString(GameState->Timers[Player->MoveTimer.TimerHandle]), math::v3(50.0f, 50.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
            
            auto& CurrentTile = Grid.Grid[(i32)Entity->Position.x / (i32)Grid.TileScale][(i32)Entity->Position.y / (i32)Grid.TileScale];
            
            if(math::Length(Entity->Velocity) > 0.0f)
                CurrentTile.Walked = true;
            
            PushFilledQuad(Renderer, Entity->Position, Entity->Scale, math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 1.0f), "player", false);
            
            if(KEY_DOWN(Key_R))
            {
                InitializeLevel(GameState, Entity);
            }
            
            if(KEY_DOWN(Key_Enter))
            {
                //@Incomplete: Check if level was solved here
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


