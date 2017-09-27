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
    i32 X = 1;
    i32 Y = 1;
    Grid.Size = math::v2(X, Y);
    Grid.TileScale = 4.0f;
    grid_tile Tile = {};
    
    Grid.Grid = PushArray(&GameState->WorldArena, X * Y, grid_tile*);
    
    for(i32 I = 0; I < X; I++)
    {
        Grid.Grid[I] = PushArray(&GameState->WorldArena, Y, grid_tile);
    }
    
    Grid.Grid[0][0].Card = GameState->Cards[1];
    //Grid.Grid[1][0].Card = GameState->Cards[1];
    //Grid.Grid[0][1].Card = GameState->Cards[2];
    //Grid.Grid[1][1].Card = GameState->Cards[3];
}

static void AddCard(game_state* GameState, Suit_Type Type, char* TextureName, i32 Rank)
{
    card Card;
    Card.Type = Type;
    Card.TextureName = TextureName;
    Card.Rank = Rank;
    GameState->Cards[GameState->CardCount++] = Card;
}

static void InitializeCards(game_state* GameState)
{
    AddCard(GameState, Suit_B1, "card_b1_1", 1);
    AddCard(GameState, Suit_B1, "card_b1_2", 2);
    AddCard(GameState, Suit_B1, "card_b1_3", 3);
    AddCard(GameState, Suit_B1, "card_b1_4", 4);
    AddCard(GameState, Suit_B1, "card_b1_5", 5);
    AddCard(GameState, Suit_B1, "card_b1_6", 6);
    AddCard(GameState, Suit_B1, "card_b1_7", 7);
    AddCard(GameState, Suit_B1, "card_b1_8", 8);
    AddCard(GameState, Suit_B1, "card_b1_9", 9);
    AddCard(GameState, Suit_B1, "card_b1_10", 10);
    AddCard(GameState, Suit_B1, "card_b1_11", 11);
    AddCard(GameState, Suit_B1, "card_b1_12", 12);
    AddCard(GameState, Suit_B1, "card_b1_13", 13);
    
    
    AddCard(GameState, Suit_B2, "card_b2_1", 1);
    AddCard(GameState, Suit_B2, "card_b2_2", 2);
    AddCard(GameState, Suit_B2, "card_b2_3", 3);
    AddCard(GameState, Suit_B2, "card_b2_4", 4);
    AddCard(GameState, Suit_B2, "card_b2_5", 5);
    AddCard(GameState, Suit_B2, "card_b2_6", 6);
    AddCard(GameState, Suit_B2, "card_b2_7", 7);
    AddCard(GameState, Suit_B2, "card_b2_8", 8);
    AddCard(GameState, Suit_B2, "card_b2_9", 9);
    AddCard(GameState, Suit_B2, "card_b2_10", 10);
    AddCard(GameState, Suit_B2, "card_b2_11", 11);
    AddCard(GameState, Suit_B2, "card_b2_12", 12);
    AddCard(GameState, Suit_B2, "card_b2_13", 13);
    
    AddCard(GameState, Suit_R1, "card_r1_1", 1);
    AddCard(GameState, Suit_R1, "card_r1_2", 2);
    AddCard(GameState, Suit_R1, "card_r1_3", 3);
    AddCard(GameState, Suit_R1, "card_r1_4", 4);
    AddCard(GameState, Suit_R1, "card_r1_5", 5);
    AddCard(GameState, Suit_R1, "card_r1_6", 6);
    AddCard(GameState, Suit_R1, "card_r1_7", 7);
    AddCard(GameState, Suit_R1, "card_r1_8", 8);
    AddCard(GameState, Suit_R1, "card_r1_9", 9);
    AddCard(GameState, Suit_R1, "card_r1_10", 10);
    AddCard(GameState, Suit_R1, "card_r1_11", 11);
    AddCard(GameState, Suit_R1, "card_r1_12", 12);
    AddCard(GameState, Suit_R1, "card_r1_13", 13);
    
    AddCard(GameState, Suit_R2, "card_r2_1", 1);
    AddCard(GameState, Suit_R2, "card_r2_2", 2);
    AddCard(GameState, Suit_R2, "card_r2_3", 3);
    AddCard(GameState, Suit_R2, "card_r2_4", 4);
    AddCard(GameState, Suit_R2, "card_r2_5", 5);
    AddCard(GameState, Suit_R2, "card_r2_6", 6);
    AddCard(GameState, Suit_R2, "card_r2_7", 7);
    AddCard(GameState, Suit_R2, "card_r2_8", 8);
    AddCard(GameState, Suit_R2, "card_r2_9", 9);
    AddCard(GameState, Suit_R2, "card_r2_10", 10);
    AddCard(GameState, Suit_R2, "card_r2_11", 11);
    AddCard(GameState, Suit_R2, "card_r2_12", 12);
    AddCard(GameState, Suit_R2, "card_r2_13", 13);
}

static void InitializeLevel(game_state* GameState, entity* Player)
{
    level Level = {};
    
    Level.CurrentScore = 0;
    Level.TargetScore = 2;
    
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
        Renderer.Cameras[Renderer.CurrentCameraHandle].Position = math::v3(5.0f, 5.0f, 0);
        
        sounds Sounds = {};
        //@Incomplete: Get actual sizes, this is retarded
        memcpy(&GameState->Sounds.SoundEffects, SoundEffects, sizeof(sound_effect) * (64 + 32));
        
        LoadTextures(Renderer, &Renderer.TextureArena, Concat(CARDS_ASSETS, "textures/"));
        
        InitializeCards(GameState);
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
    
    PushText(Renderer, ToString(GameState->CurrentLevel.TargetScore), math::v3(Renderer.ViewportWidth / 2.0f - 20.0f, Renderer.ViewportHeight / 2.0f + 100.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f), Alignment_Center);
    
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
            
            auto PreviouslyWalked = CurrentTile.Walked;
            
            auto& Level = GameState->CurrentLevel;
            
            if(math::Length(Entity->Velocity) > 0.0f)
            {
                CurrentTile.Walked = true;
                if(!PreviouslyWalked)
                {
                    switch(CurrentTile.Card.Type)
                    {
                        case Suit_B1:
                        {
                            // Add from 1 -> 13
                            DEBUG_PRINT("Black 1\n");
                            Level.CurrentScore += CurrentTile.Card.Rank;
                        }
                        break;
                        case Suit_B2:
                        {
                            // Add from 13 -> 1
                            DEBUG_PRINT("Black 2\n");
                            Level.CurrentScore += (13 - CurrentTile.Card.Rank);
                        }
                        break;
                        case Suit_R1:
                        {
                            // Subtract from 1 -> 13
                            DEBUG_PRINT("Red 1\n");
                            Level.CurrentScore -= CurrentTile.Card.Rank;
                        }
                        break;
                        case Suit_R2:
                        {
                            // Subtract from 13 -> 1
                            DEBUG_PRINT("Red 2\n");
                            Level.CurrentScore -= (13 - CurrentTile.Card.Rank);
                        }
                        break;
                    }
                }
            }
            
            //PushText(Renderer, ToString(Level.CurrentScore), math::v3(Renderer.ViewportWidth / 2.0f - 20.0f, Renderer.ViewportHeight / 2.0f + 200.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
            
            
            PushFilledQuad(Renderer, Entity->Position, Entity->Scale, math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 1.0f), "player", false);
            
            if(KEY_DOWN(Key_R))
            {
                InitializeLevel(GameState, Entity);
            }
            
            if(KEY_DOWN(Key_Enter))
            {
                if(Level.TargetScore == Level.CurrentScore)
                {
                    //InitializeLevel(GameState, Entity);
                    Level.Won = true;
                }
                //@Incomplete: Check if level was solved here
            }
            
            if(Level.Won)
            {
                PushText(Renderer, "YOU WON!!!", math::v3(Renderer.ViewportWidth / 2.0f - 20.0f, Renderer.ViewportHeight / 2.0f + 200.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
                
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


