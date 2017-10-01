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


static void InitGrid(memory_arena* WorldArena, level* Level, i32 X, i32 Y)
{
    auto& Grid = Level->Grid;
    Grid.Size = math::v2(X, Y);
    Grid.TileScale = 4.0f;
    
    Grid.Grid = PushArray(WorldArena, X * Y, grid_tile*);
    
    for(i32 I = 0; I < X; I++)
    {
        Grid.Grid[I] = PushArray(WorldArena, Y, grid_tile);
    }
    
    card Unwalkable;
    Unwalkable.Type = Suit_None;
    Unwalkable.Rank = -1;
    Unwalkable.TextureName = 0;
    
    for(i32 I = 0;  I < X; I++)
    {
        for(i32 J = 0;  J < X; J++)
        {
            Grid.Grid[I][J].Card = Unwalkable;
        }
    }
}


static void LoadLevel(game_state* GameState, level* Level, const char* FilePath)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    if(File)
    {
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "goal %d", &Level->TargetScore);
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "bg %f %f %f", &Level->BackgroundColor.r, &Level->BackgroundColor.g, &Level->BackgroundColor.b);
        Level->BackgroundColor.a = 1.0f;
        
        i32 Height = 0;
        i32 Width = 0;
        
        if(fgets(LineBuffer, 255, File))
            sscanf(LineBuffer, "x %d y %d", &Height, &Width);
        
        Assert(Height > 0 && Width > 0);
        
        i32 IndexHeight = Height;
        char Line[1024];
        
        InitGrid(&GameState->WorldArena, Level, Height, Width);
        
        card Unwalkable;
        Unwalkable.Type = Suit_None;
        Unwalkable.Rank = -1;
        Unwalkable.TextureName = 0;
        
        card Start;
        Start.Type = Suit_Start;
        Start.Rank = 0;
        Start.TextureName = 0;
        
        card End;
        End.Type = Suit_End;
        End.Rank = 0;
        End.TextureName = 0;
        
        while(IndexHeight > 0)
        {
            fgets(Line, sizeof(Line), File);
            
            char* S = Line;
            
            for(i32 IndexWidth = 0; IndexWidth < Width; IndexWidth++)
            {
                auto Tok = StrSep(&S, " ");
                i32 Rank = 0;
                Suit_Type Suit = Suit_None;
                i32 Offset = 0;
                b32 Card = false;
                
                if(strncmp(Tok, "B1", 2) == 0)
                {
                    Suit = Suit_B1;
                    Offset = 0;
                    Card = true;
                }
                else if(strncmp(Tok, "B2", 2) == 0)
                {
                    Suit = Suit_B2;
                    Offset = 13;
                    Card = true;
                }
                else if(strncmp(Tok, "R1", 2) == 0)
                {
                    Suit = Suit_R1;
                    Offset = 26;
                    Card = true;
                }
                else if(strncmp(Tok, "R2", 2) == 0)
                {
                    Suit = Suit_R2;
                    Offset = 39;
                    Card = true;
                }
                else if(strncmp(Tok, "E", 1) == 0)
                {
                    Suit = Suit_End;
                }
                else if(strncmp(Tok, "S", 1) == 0)
                {
                    Suit = Suit_Start;
                }
                else
                {
                    Suit = Suit_None;
                }
                
                if(Card)
                {
                    Tok = StrSep(&S, " ");
                    Rank = (i32)atoi(Tok);
                    Level->Grid.Grid[IndexWidth][IndexHeight - 1].Card = GameState->Cards[Offset + Rank - 1];
                }
                else
                {
                    switch(Suit)
                    {
                        case Suit_End:
                        {
                            Level->Grid.Grid[IndexWidth][IndexHeight - 1].Card = End;
                        }
                        break;
                        case Suit_Start:
                        {
                            Level->Grid.Grid[IndexWidth][IndexHeight - 1].Card = Start;
                        }
                        break;
                        case Suit_None:
                        {
                            Level->Grid.Grid[IndexWidth][IndexHeight - 1].Card = Unwalkable;
                        }
                        break;
                    }
                }
            }
            IndexHeight--;
        }
        
        fclose(File);
        
        Level->Won = false;
        Level->CurrentScore = 0;
    }
}

static void LoadLevels(const char* FilePath, game_state* GameState)
{
    directory_data DirData = {};
    Platform.GetAllFilesWithExtension(FilePath, "clv", &DirData, false);
    
    for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadLevel(GameState, &GameState->Levels[GameState->LoadedLevels++], DirData.FilePaths[FileIndex]);
    }
    
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

static void InitializeLevel(game_state* GameState, entity* Player, i32 Level)
{
    GameState->CurrentLevel = Level;
    
    Player->Position = math::v3(0.0f, 0.0f, 0.0f) / GameState->Levels[GameState->CurrentLevel].Grid.TileScale;
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
        LoadLevels(Concat(CARDS_ASSETS, "levels/"), GameState);
        InitializeLevel(GameState, Player, 0);
        
        GameState->IsInitialized = true;
        GameMemory->IsInitialized = true;
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    
    auto Near = -100.0f;
    auto Far = 1000.0f;
    
    CameraTransform(Renderer, Camera, Camera.Position, Camera.Orientation, Camera.Target, Camera.Zoom, Near, Far);
    
    DisableDepthTest(Renderer);
    
    auto Grid = GameState->Levels[GameState->CurrentLevel].Grid;
    
    PushFilledQuad(Renderer, math::v3() - Grid.Size.x * Grid.TileScale * 0.5f, math::v3(2.0f * Grid.Size.x * Grid.TileScale, 2.0f * Grid.Size.y * Grid.TileScale, 0.0f), math::v3(), GameState->Levels[GameState->CurrentLevel].BackgroundColor, 0, false);
    
    auto* B = Renderer.TextureMap["border"];
    auto* N = Renderer.TextureMap["none"];
    auto* C = Renderer.TextureMap["card_r1_9"];
    
    for(i32 I = 0; I < Grid.Size.x; I++)
    {
        for(i32 J = 0; J < Grid.Size.y; J++)
        {
            auto Card = Grid.Grid[I][J].Card;
            
            auto TilePos = math::v3(I * Grid.TileScale, J * Grid.TileScale, 0.0f);
            
            switch(Card.Type)
            {
                case Suit_Start:
                {
                    PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), math::rgba(1.0f), "start", false);
                }
                break;
                case Suit_End:
                {
                    PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), math::rgba(1.0f), "end", false);
                }
                break;
                case Suit_None:
                {
                    PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), math::rgba(1.0f), "none", false);
                }
                break;
                case Suit_B1:
                case Suit_B2:
                case Suit_R1:
                case Suit_R2:
                {
                    char* Texture = Card.TextureName;
                    auto C = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
                    
                    PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), C, Texture, false);
                    
                    auto Walked = Grid.Grid[I][J].Walked;
                    if(Walked) //@Incomplete: Remember empty tiles
                    {
                        C = math::rgba(0.0f, 1.0f, 1.0f, 0.2f);
                        PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), C, 0, false);
                    }
                    
                    
                }
                break;
                
            }
            PushFilledQuad(Renderer, TilePos, math::v3(Grid.TileScale, Grid.TileScale, 1.0f), math::v3(), math::rgba(1.0f, 1.0f, 1.0f, 1.0f), "border", false);
            
            
        }
    } 
    
    PushText(Renderer, ToString(GameState->Levels[GameState->CurrentLevel].TargetScore), math::v3(Renderer.ViewportWidth / 2.0f - 20.0f, Renderer.ViewportHeight / 2.0f + 100.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f), Alignment_Center);
    
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
            
            if(math::Length(Entity->Velocity) > 0.0f && TimerDone(GameState, Player->MoveTimer) )
            {
                auto& NextTile = Grid.Grid
                    [(i32)(Entity->Position.x + Entity->Velocity.x * Grid.TileScale) / (i32)Grid.TileScale]
                    [(i32)(Entity->Position.y + Entity->Velocity.y * Grid.TileScale) / (i32)Grid.TileScale];
                
                if(NextTile.Card.Type != Suit_None)
                {
                    Entity->Position += Entity->Velocity * Grid.TileScale;
                    StartTimer(GameState, Player->MoveTimer);
                }
            }
            
            PushText(Renderer, ToString(GameState->Timers[Player->MoveTimer.TimerHandle]), math::v3(50.0f, 50.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
            
            auto& CurrentTile = Grid.Grid[(i32)Entity->Position.x / (i32)Grid.TileScale][(i32)Entity->Position.y / (i32)Grid.TileScale];
            
            auto PreviouslyWalked = CurrentTile.Walked;
            
            auto& Level = GameState->Levels[GameState->CurrentLevel];
            
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
                            Level.CurrentScore += CurrentTile.Card.Rank;
                        }
                        break;
                        case Suit_B2:
                        {
                            // Add from 13 -> 1
                            Level.CurrentScore += (13 - (CurrentTile.Card.Rank - 1));
                        }
                        break;
                        case Suit_R1:
                        {
                            // Subtract from 1 -> 13
                            Level.CurrentScore -= CurrentTile.Card.Rank;
                        }
                        break;
                        case Suit_R2:
                        {
                            // Subtract from 13 -> 1
                            Level.CurrentScore -= (13 - (CurrentTile.Card.Rank - 1));
                        }
                        break;
                    }
                }
            }
            
            //PushText(Renderer, ToString(Level.CurrentScore), math::v3(Renderer.ViewportWidth / 2.0f - 20.0f, Renderer.ViewportHeight / 2.0f + 200.0f, 0.0f), Font_Inconsolata, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
            
            PushFilledQuad(Renderer, Entity->Position, Entity->Scale, math::v3(), math::rgba(1.0f, 0.0f, 0.0f, 1.0f), "player", false);
            
            if(KEY_DOWN(Key_R))
            {
                InitializeLevel(GameState, Entity, 0);
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
                
                if(KEY_DOWN(Key_Enter))
                {
                    GameState->CurrentLevel++;
                    InitializeLevel(GameState, Entity, Min(GameState->CurrentLevel, GameState->LoadedLevels - 1));
                }
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


