#include "game.h"
#include "rendering.cpp"
#include "keycontroller.cpp"
#include "sound.cpp"
#define ANIMATION_GAME
#include "animation.cpp"
#include "collision.cpp"
#include "console.cpp"
#include "ai.cpp"
#include "entity.cpp"
#include "level.cpp"
#include "editor.cpp"

#define DEBUG

static inline void TickTimers(game_state* GameState, r64 DeltaTime)
{
    for(u32 Index = 0; Index < NUM_TIMERS; Index++)
    {
        if(GameState->Timers[Index] > 0)
            GameState->Timers[Index] -= DeltaTime;
        else
        {
            GameState->Timers[Index] = 0;
        }
    }
}

static void LoadGameDataFile(game_state* GameState)
{
    FILE* File;
    File = fopen("../assets/game_data.dat", "r");
    
    if(File)
    {
        char LineBuffer[256];
        
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(&LineBuffer[0], "levels"))
            {
                i32 NumLevels = 0;
                sscanf(LineBuffer, "levels %d", &NumLevels);
                
                Assert(NumLevels <= 10);
                
                for(i32 Index = 0; Index < NumLevels; Index++)
                {
                    fgets(LineBuffer, 255, File);
                    
                    i32 WillNeeded = 0;
                    
                    sscanf(LineBuffer, "%d", &WillNeeded);
                    
                    GameState->StatData[Index].WillForLevel = WillNeeded;
                }
            }
        }
        
        fclose(File);
    }
}

int CompareFunction(const void* a, const void* b)
{
    render_entity APtr = *(render_entity*)a;
    render_entity BPtr = *(render_entity*)b;
    
    math::v2 PositionA = APtr.RenderType == Render_Type_Entity ? APtr.Entity->Position : APtr.Object->Position;
    math::v2 PositionB = BPtr.RenderType == Render_Type_Entity ? BPtr.Entity->Position : BPtr.Object->Position;
    
    PositionA = ToIsometric(PositionA);
    PositionB = ToIsometric(PositionB);
    
    if(APtr.Background || PositionA.y > PositionB.y)
        return -1;
    if(BPtr.Background || PositionA.y < PositionB.y)
        return 1;
    return 0;
}

static void PushTilemapRenderCommands(game_state* GameState)
{
    PushBuffer(GameState->Renderer, GameState->CurrentLevel.Tilemap.BufferHandle, GameState->CurrentLevel.Tilemap.RenderEntity.Texture->Name);
}

static void PushEntityRenderCommands(game_state* GameState)
{
    qsort(GameState->RenderState.RenderEntities, GameState->RenderState.RenderEntityCount, sizeof(render_entity), CompareFunction);
    
    for(i32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++) 
    {
        render_entity* RenderEntity = &GameState->RenderState.RenderEntities[Index];
        
        if(RenderEntity->RenderType == Render_Type_Entity)
            RenderEntity->Entity->RenderEntityHandle = Index;
        else if(RenderEntity->RenderType == Render_Type_Object)
            RenderEntity->Object->RenderEntityHandle = Index;
        
        b32 Active = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Active : RenderEntity->Object->Active;
        math::v2 Position = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Position : RenderEntity->Object->Position;
        math::v2 Center = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Center : RenderEntity->Object->Center;
        r32 EntityScale = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Scale : RenderEntity->Object->Scale;
        i32 LightSourceHandle = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->LightSourceHandle : RenderEntity->Object->LightSourceHandle;
        
        animation* CurrentAnimation =  RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->CurrentAnimation : RenderEntity->Object->CurrentAnimation;
        animation_info AnimationInfo = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->AnimationInfo : RenderEntity->Object->AnimationInfo;
        
        math::v2 CurrentPosition;
        math::v3 CurrentScale;
        math::v2 CurrentFrame;
        math::v2 CurrentTextureOffset;
        math::rgba CurrentColor;
        char* CurrentTexture;
        
        if(CurrentAnimation)
        {
            r32 WidthInUnits = (r32)CurrentAnimation->FrameSize.x / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = (r32)CurrentAnimation->FrameSize.y / (r32)PIXELS_PER_UNIT;
            
            math::v3 Scale = math::v3(WidthInUnits * EntityScale, HeightInUnits * EntityScale, 1);
            
            auto CorrectPos = ToIsometric(Position);
            
            CorrectPos.x -= CurrentAnimation->Center.x * Scale.x;
            CorrectPos.y -= CurrentAnimation->Center.y * Scale.y;
            
            // @Cleanup: Move these to a global variable or similar
            r32 TileWidthHalf = 0.5f;
            r32 TileHeightHalf = 0.25f;
            
            CorrectPos.x += TileWidthHalf; //We want the sprite to be centered in the tile
            CorrectPos.y += TileHeightHalf;
            
            if(LightSourceHandle != -1)
            {
                GameState->LightSources[LightSourceHandle].Pointlight.RenderPosition = CorrectPos + math::v2(CurrentAnimation->Center.x * Scale.x, CurrentAnimation->Center.y * Scale.y);
            }
            
            CurrentPosition = CorrectPos;
            CurrentScale = Scale;
            
            animation* Animation = CurrentAnimation;
            CurrentTexture = Animation->Texture->Name;
            
            CurrentTextureOffset = math::v2(Animation->Frames[AnimationInfo.FrameIndex].X, Animation->Frames[AnimationInfo.FrameIndex].Y);
            
            CurrentColor = RenderEntity->Color;
            CurrentFrame = Animation->FrameSize;
        } 
        else 
        {
            auto CorrectPos = ToIsometric(math::v2(Position.x, Position.y));
            
            r32 CorrectX = CorrectPos.x;
            r32 CorrectY = CorrectPos.y;
            
            CurrentPosition = CorrectPos;
            
            r32 WidthInUnits = RenderEntity->Texture->Width / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = RenderEntity->Texture->Height / (r32)PIXELS_PER_UNIT;
            
            CurrentScale = math::v3(WidthInUnits * EntityScale, HeightInUnits * EntityScale, 1);
            
            CurrentTexture = RenderEntity->Texture->Name;
            
            CurrentFrame = math::v2(RenderEntity->Texture->Width, RenderEntity->Texture->Height);
        }
        
        // @Incomplete: Glow and special parameters
        PushSprite(GameState->Renderer, CurrentPosition, CurrentScale, CurrentFrame, CurrentTextureOffset, CurrentTexture, CurrentColor);
    }
}

extern "C" UPDATE(Update)
{
    game_state* GameState = (game_state*)GameMemory->PermanentStorage;
    Assert(GameState);
    
    if(!GameState->IsInitialized)
    {
        model Model;
        LoadOBJFile("../assets/test.obj", &Model);
        
        if(GameState->ShouldReload)
        {
            GameState->InitialZoom = GameMemory->ConfigData.Zoom;
            GameState->LevelPath = GameMemory->ConfigData.StartingLevelFilePath;
            GameState->ShouldReload = GameMemory->ShouldReload;
            GameState->Console = {};
            GameState->RenderGame = true;
            GameState->RenderLight = true;
            GameState->ClearTilePositionFrame = false;
            GameState->StatGainModeOn = false;
            GameState->SelectedGainIndex = 0;
            GameState->CurrentLootCount = 0;
            GameState->AIDebugModeOn = false;
            GameState->GodModeOn = false;
            GameState->GodModePanSpeed = 10.0f;
            GameState->GodModeZoomSpeed = 45.0f;
            GameState->GodModeMinZoom = 5.0f;
            GameState->GodModeMaxZoom = 100.0f;
            GameState->PlayerState = Player_Alive;
            
            GameState->DeathScreenTimer.TimerMax = 1.0f;
            GameState->DeathScreenTimer.TimerHandle = -1;
            
            GameState->EditorState.MenuOptions[0] = "Game";
            GameState->EditorState.MenuOptions[1] = "Level editor";
            GameState->EditorState.MenuOptions[2] = "Animation";
            GameState->EditorState.Loaded = false;
            GameState->EditorState.Mode = Editor_Level;
            GameState->EditorState.PlacementMode = Editor_Placement_Tile;
            GameState->EditorState.SelectedTileType = 1;
            GameState->EditorState.CurrentTilemapLayer = 1;
            GameState->EditorState.RenderAllLayers = true;
            GameState->EditorState.ZoomingSpeed = 50;
            GameState->EditorState.PanningSpeed = 500;
            GameState->EditorState.MinZoom = 5;
            GameState->EditorState.MaxZoom = 100;
            GameState->EditorState.RenderedTileSize = 30.0f;
            GameState->EditorState.TileBrushSize = math::v2(1, 1);
            GameState->EditorState.ToolbarScrollSpeed = 30000;
            GameState->EditorState.ToolbarScrollOffsetY = 0.0f;
            GameState->EditorState.IsInCreateWaypointMode = false;
            GameState->EditorState.SelectedAnimation = 0;
            
            sound_manager SoundManager = {};
            memcpy(&SoundManager.SoundEffects, SoundEffects, sizeof(sound_effect) * (64 + 32));
            SoundManager.Muted = GameMemory->ConfigData.Muted;
            SoundManager.SFXGain = GameMemory->ConfigData.SFXVolume;
            SoundManager.MusicGain = GameMemory->ConfigData.MusicVolume;
            GameState->SoundManager = SoundManager;
            
            LoadGameDataFile(GameState);
            srand((u32)time(NULL));
            
            LoadAnimations(GameState);
            InitCommands();
            
            GameState->EditorCamera.Zoom = GameState->InitialZoom; 
            GameState->EditorCamera.ViewportWidth = GameState->RenderState.WindowWidth;
            GameState->EditorCamera.ViewportHeight = GameState->RenderState.WindowHeight;
            
            GameState->GameMode = Mode_InGame;
            GameState->ShouldReload = false;
            GameMemory->ShouldReload = false;
            PLAY_TRACK(Brugt);
        }
        
        LoadLevelFromFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, SoundQueue);
        
        GameState->GameCamera.Zoom = GameState->InitialZoom;
        GameState->GameCamera.ViewportWidth = GameState->RenderState.WindowWidth;
        GameState->GameCamera.ViewportHeight = GameState->RenderState.WindowHeight;
        GameState->GameCamera.FollowSpeed = 10.0f; 
        GameState->GameCamera.FadingSpeed = 0.6f;
        
        StartFade(GameState->GameCamera, Fading_In, 0.6f, math::v3(0, 0, 0), 1.0f, 0.0f);
        
        // @Incomplete: This is not the right value, it is only set so high to remove smooth following as of now, since it needs to be done a little differently
        
        if(GameState->CurrentLevel.Type == Level_Isometric)
        {
            auto PlayerPos = GameState->Entities[0].Position;
            GameState->GameCamera.Center = math::v2((PlayerPos.x + PlayerPos.y) * 0.5f,(PlayerPos.x - PlayerPos.y) * 0.25f);; // Set center to player's position!
            GameState->GameCamera.CenterTarget = math::v2((PlayerPos.x + PlayerPos.y) * 0.5f,(PlayerPos.x - PlayerPos.y) * 0.25f);
        }
        else
        {
            GameState->GameCamera.Center = GameState->Entities[0].Position; // Set center to player's position!
            GameState->GameCamera.CenterTarget = GameState->Entities[0].Position;
        }
        
        
        GameState->IsInitialized = true;
        GameMemory->IsInitialized = true;
    }
    
    if(KEY_DOWN(Key_Q) && KEY(Key_LeftCtrl))
    {
        GameState->GameMode = Mode_Exit;
    }
    
    if(GameState->ReloadData)
    {
        if(GameState->ReloadData->ReloadPlayerFile)
        {
            LoadPlayerData(GameState, SoundQueue, 0);
            GameState->ReloadData->ReloadPlayerFile = false;
        }
        
        if(GameState->ReloadData->ReloadSkeletonFile)
        {
            for(u32 EntityIndex = 0; EntityIndex < GameState->EntityCount; EntityIndex++)
            {
                auto Entity = GameState->Entities[EntityIndex];
                if(Entity.Type == Entity_Enemy && Entity.Enemy.EnemyType == Enemy_Skeleton && !Entity.Dead)
                {
                    LoadSkeletonData(GameState,EntityIndex);
                }
            }
            GameState->ReloadData->ReloadSkeletonFile = false;
        }
        
        if(GameState->ReloadData->ReloadMinotaurFile)
        {
            for(u32 EntityIndex = 0; EntityIndex < GameState->EntityCount; EntityIndex++)
            {
                auto Entity = GameState->Entities[EntityIndex];
                if(Entity.Type == Entity_Enemy && Entity.Enemy.EnemyType == Enemy_Minotaur && !Entity.Dead)
                {
                    LoadMinotaurData(GameState, EntityIndex);
                }
            }
            GameState->ReloadData->ReloadMinotaurFile = false;
        }
        
        if(GameState->ReloadData->ReloadBonfireFile)
        {
            for(u32 EntityIndex = 0; EntityIndex < GameState->EntityCount; EntityIndex++)
            {
                auto Entity = GameState->Entities[EntityIndex];
                if(Entity.Type == Entity_Bonfire)
                {
                    LoadBonfireData(GameState, SoundQueue, EntityIndex);
                }
            }
            GameState->ReloadData->ReloadBonfireFile = false;
        }
    }
    if(GameState->Console.Open)
    {
        if(GameState->Console.BufferIndex < CONSOLE_BUFFER_SIZE - 1 && InputController->CurrentCharacter != 0)
            GameState->Console.Buffer[GameState->Console.BufferIndex++] = InputController->CurrentCharacter;
    }
    
    CheckConsoleInput(GameState, InputController, DeltaTime);
    
    if(GameState->GameMode == Mode_Editor)
        CheckEditorUIInput(GameState,InputController, DeltaTime);
    
    if((KEY(Key_LeftCtrl) || KEY(Key_RightCtrl)))
    {
        if(KEY_DOWN(Key_E)) // Editor-mode on/off
        {
            if(GameState->GameMode == Mode_InGame)
            {
                GameState->GodModeOn = false;
                ReloadCurrentLevel(GameState);
                GameState->GameMode = Mode_Editor;
                GameState->Paused = false;
                GameState->RenderLight = false;
                GameState->EditorCamera.Center = GameState->GameCamera.Center;
            }
            else
            {
                GameState->RenderLight = true;
                GameState->GodModeOn = false;
                SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                ReloadCurrentLevel(GameState);
                GameState->GameMode = Mode_InGame;
            }
        }
        else if(KEY_DOWN(Key_G)) // God-mode on/off
        {
            GameState->GodModeOn = !GameState->GodModeOn;
            
            if(!GameState->GodModeOn)
            {
                auto Player = GameState->Entities[0];
                GameState->GameCamera.Center = math::v2(Player.Position.x, Player.Position.y);
                GameState->Camera.Center = math::v2(Player.Position.x, Player.Position.y);
                GameState->GameCamera.Zoom = GameState->ZoomBeforeGodMode;
            }
            else
            {
                GameState->ZoomBeforeGodMode = GameState->GameCamera.Zoom;
            }
        }
    }
    
    if(!GameState->EditorState.Loaded)
    {
        InitEditorFields(GameState);
        CreateEditorButtons(GameState);
    }
    
    
#if GLITCH_DEBUG
    if(KEY_DOWN(Key_F1))
    {
        GameState->RenderState.RenderColliders = !GameState->RenderState.RenderColliders;
    }
    
    if(KEY_DOWN(Key_F2))
    {
        GameState->RenderState.RenderFPS = !GameState->RenderState.RenderFPS;
    }
    
    if(KEY_DOWN(Key_F4))
    {
        GameState->RenderState.RenderPaths = !GameState->RenderState.RenderPaths;
    }
    
    if(KEY_DOWN(Key_F7))
    {
        GameState->RenderGame = !GameState->RenderGame;
    }
    
    if(KEY_DOWN(Key_F8))
    {
        GameState->AIDebugModeOn = !GameState->AIDebugModeOn;
    }
    
    if(KEY_DOWN(Key_F9))
    {
        GameState->StatGainModeOn = !GameState->StatGainModeOn;
    }
    
    if(GameState->GameMode == Mode_InGame && KEY(Key_LeftCtrl) && KEY_DOWN(Key_P))
    {
        GameState->Paused = !GameState->Paused;
    }
    
    if(GameState->GameMode == Mode_InGame && (KEY(Key_LeftCtrl) || KEY(Key_RightCtrl)))
    {
        if(KEY_DOWN(Key_F))
            SaveGame(GameState);
        if(KEY_DOWN(Key_V))
            LoadGame(GameState);
    }
    
#endif
    
    if (ACTION_DOWN(Action_Menu) && !GameState->Console.Open)
    {
        switch(GameState->GameMode)
        {
            case Mode_MainMenu:
            {
                //StopSoundEffect(GameState, &GameState->SoundManager.MainMenuTrack);
                GameState->GameMode = Mode_InGame;
                GameState->Paused = false;
            }
            break;
            case Mode_InGame:
            {
                //PlaySoundEffect(GameState, &GameState->SoundManager.MainMenuTrack);
                GameState->GameMode = Mode_MainMenu;
                GameState->Paused = true;
            }
            break;
        }
    }
    
    
    if(GameState->GameMode == Mode_MainMenu)
    {
        if(KEY_DOWN(Key_Up))
            GameState->MainMenu.SelectedIndex -= 1;
        else if(KEY_DOWN(Key_Down))
            GameState->MainMenu.SelectedIndex += 1;
        
        if(GameState->MainMenu.SelectedIndex < 0)
            GameState->MainMenu.SelectedIndex = GameState->MainMenu.OptionCount - 1;
        else if(GameState->MainMenu.SelectedIndex == (i32)GameState->MainMenu.OptionCount)
            GameState->MainMenu.SelectedIndex = 0;
        
        if(KEY_DOWN(Key_Enter))
        {
            char* Selection = GameState->MainMenu.Options[GameState->MainMenu.SelectedIndex];
            
            if(strcmp(Selection, "Exit") == 0) //TODO(Daniel) do an enumeration instead
            {
                GameState->GameMode = Mode_Exit;
            }
            else if(strcmp(Selection, "Continue") == 0)
            {
                GameState->GameMode = Mode_InGame;
            }
            GameState->MainMenu.SelectedIndex = 0;
        }
    }
    
    switch(GameState->GameCamera.FadingMode)
    {
        case Fading_In:
        {
            GameState->GameCamera.FadingAlpha -= GameState->GameCamera.FadingSpeed * (r32)DeltaTime;
            
            if(GameState->GameCamera.FadingAlpha <= 0.0f)
            {
                GameState->GameCamera.FadingAlpha = 0.0f;
                GameState->GameCamera.FadingMode = Fading_None;
            }
        }
        break;
        case Fading_Out:
        {
            GameState->GameCamera.FadingAlpha += GameState->GameCamera.FadingSpeed * (r32)DeltaTime;
            
            if(GameState->GameCamera.FadingAlpha >= 1.0f)
            {
                GameState->GameCamera.FadingAlpha = 1.0f;
                GameState->GameCamera.FadingMode = Fading_None;
            }
        }
        case Fading_OutIn:
        {
            if(GameState->GameCamera.FadingIn)
            {
                GameState->GameCamera.FadingAlpha -= GameState->GameCamera.FadingSpeed * (r32)DeltaTime;
                
                if(GameState->GameCamera.FadingAlpha <= 0.0f)
                {
                    GameState->GameCamera.FadingAlpha = 0.0f;
                    GameState->GameCamera.FadingMode = Fading_None;
                    GameState->GameCamera.FadingIn = false;
                }
            }
            else
            {
                GameState->GameCamera.FadingAlpha += GameState->GameCamera.FadingSpeed * (r32)DeltaTime;
                
                if(GameState->GameCamera.FadingAlpha >= GameState->GameCamera.EndAlpha)
                {
                    GameState->GameCamera.FadingAlpha = GameState->GameCamera.EndAlpha;
                    GameState->GameCamera.FadingIn = true;
                }
            }
        }
        break;
    }
    
    math::v2 Center = GameState->GameCamera.Center;
    
    if(GameState->GodModeOn)
    {
        r32 Zoom = GameState->Camera.Zoom;
        
        math::v2 Direction = math::v2(0, 0);
        
        if(KEY(Key_W) || KEY(Key_Up))
        {
            Direction.y = 1;
        }
        else if(KEY(Key_S) || KEY(Key_Down))
        {
            Direction.y = -1;
        }
        
        if(KEY(Key_A) || KEY(Key_Left))
        {
            Direction.x = -1;
        }
        else if(KEY(Key_D) || KEY(Key_Right))
        {
            Direction.x = 1;
        }
        
        r32 Factor = 72.0f / GameState->GameCamera.Zoom;
        
        if(KEY(Key_Add))
        {
            Zoom += (r32)(GameState->GodModeZoomSpeed / Factor * DeltaTime);
        }
        else if(KEY(Key_Subtract))
        {
            Zoom += (r32)(-GameState->GodModeZoomSpeed / Factor * DeltaTime);
        }
        
        Direction = math::Normalize(Direction);
        
        if(Abs(Direction.x) > 0.0 || Abs(Direction.y) > 0.0)
        {
            GameState->GameCamera.Center = Center + math::v2(Direction.x * GameState->GodModePanSpeed * Factor * DeltaTime, Direction.y * GameState->GodModePanSpeed * Factor * DeltaTime);
        }
        
        GameState->GameCamera.Zoom = Min(Max(Zoom, GameState->GodModeMinZoom), GameState->GodModeMaxZoom);
    }
    
    if(KEY_DOWN(Key_L) && KEY(Key_LeftCtrl))
    {
        GameState->RenderLight = !GameState->RenderLight;
    }
    
    switch(GameState->GameMode)
    {
        case Mode_InGame:
        case Mode_MainMenu:
        {
            if(!GameState->Paused && !GameState->StatGainModeOn)
            {
                UpdateEntities(GameState, InputController, SoundQueue, DeltaTime);
                UpdateObjects(GameState, DeltaTime);
                
                TickTimers(GameState, DeltaTime);
                
                /*if(!TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
                {
                r32 Radius = 0.05f;
                i32 RandomAngle = rand() % 360;
                math::v2 Offset = math::v2(sin(RandomAngle) * Radius, cos(RandomAngle) * Radius);
                Center.x += Offset.x / 1.5f;
                Center.y += Offset.y;
                }*/
                
                if(!GameState->GodModeOn)
                {
                    if(math::Distance(GameState->GameCamera.CenterTarget, Center) > 0.01f)
                    {
                        auto Direction = math::Normalize(GameState->GameCamera.CenterTarget - Center);
                        
                        if(GameState->CurrentLevel.Type == Level_Isometric)
                        {
                            auto PlayerPos = GameState->Entities[0].Position;
                            Center = ToIsometric(PlayerPos);
                        }
                        else
                        {
                            Center = GameState->Entities[0].Position;
                        }
                        // math::v2(Center.x + Direction.x * GameState->GameCamera.FollowSpeed * DeltaTime, Center.y + Direction.y  * GameState->GameCamera.FollowSpeed * DeltaTime);
                        
                        GameState->GameCamera.Center = Center;
                    }
                }
            }
            else if(GameState->StatGainModeOn)
            {
                b32 ControllerPresent = InputController->ControllerPresent;
                
                b32 UpPressed = ControllerPresent ? JOYSTICK_AXIS_Y_DOWN(true) : KEY_DOWN(Key_W) || KEY_DOWN(Key_Up);
                b32 DownPressed = ControllerPresent ? JOYSTICK_AXIS_Y_DOWN(false) : KEY_DOWN(Key_S) || KEY_DOWN(Key_Down);
                
                if(UpPressed)
                {
                    GameState->SelectedGainIndex--;
                    
                    if(GameState->SelectedGainIndex == -1)
                        GameState->SelectedGainIndex = 2;
                }
                else if(DownPressed)
                {
                    GameState->SelectedGainIndex++;
                    if(GameState->SelectedGainIndex == 3)
                        GameState->SelectedGainIndex = 0;
                }
                
                if(ACTION_DOWN(Action_Interact) || KEY_DOWN(Key_Enter))
                {
                    auto Player = GameState->Entities[0];
                    
                    GameState->LastCharacterData = GameState->CharacterData;
                    
                    switch((Player_Gain_Type)GameState->SelectedGainIndex)
                    {
                        case Gain_Health:
                        {
                            r32 Ratio = (r32)Player.Health / (r32)GameState->CharacterData.Health;
                            GameState->CharacterData.Health += 5;
                            Player.Health = (i16)(GameState->CharacterData.Health * Ratio);
                        }
                        break;
                        case Gain_Stamina:
                        {
                            r32 Ratio = (r32)Player.Player.Stamina / (r32)GameState->CharacterData.Stamina;
                            GameState->CharacterData.Stamina += 5;
                            Player.Player.Stamina = (i16)(GameState->CharacterData.Stamina * Ratio);
                        }
                        break;
                        case Gain_Strength:
                        {
                            GameState->CharacterData.Strength += 1;
                        }
                        break;
                    }
                    //@Incomplete: Play sound!
                    
                    Player.Player.Will -= GameState->StatData[GameState->CharacterData.Level].WillForLevel;
                    GameState->CharacterData.Level++;
                    Player.Weapon.Damage = GameState->CharacterData.Strength;
                    GameState->SelectedGainIndex = 0;
                    GameState->StatGainModeOn = false;
                    SaveGame(GameState);
                }
            }
            
            if(GameState->PlayerState == Player_Alive && GameState->Entities[0].Dead)
            {
                GameState->PlayerState = Player_Dead;
                StartTimer(GameState, GameState->DeathScreenTimer);
            }
            
            if(GameState->PlayerState == Player_Dead)
            {
                if(TimerDone(GameState, GameState->DeathScreenTimer) && InputController->AnyKeyPressed)
                {
                    GameState->PlayerState = Player_Alive;
                    ReloadCurrentLevel(GameState);
                }
            }
            
            GameState->Camera = GameState->GameCamera;
        }
        break;
        case Mode_Editor:
        {
            Center = GameState->EditorCamera.Center;
            EditorUpdateEntities(GameState,InputController, SoundQueue, DeltaTime);
            
            switch(GameState->EditorState.PlacementMode)
            {
                case Editor_Placement_PlaceEntity:
                case Editor_Placement_SelectEntity:
                {
                    if(KEY_DOWN(Key_Left))
                    {
                        GameState->EditorState.PlacementEntity = (Entity_Placement_Type)((i32)GameState->EditorState.PlacementEntity - 1);
                        if(GameState->EditorState.PlacementEntity == -1)
                            GameState->EditorState.PlacementEntity = (Entity_Placement_Type)((i32)Placement_Entity_Max - 1);
                    }
                    else if(KEY_DOWN(Key_Right))
                    {
                        GameState->EditorState.PlacementEntity = (Entity_Placement_Type)((i32)GameState->EditorState.PlacementEntity + 1);
                        if(GameState->EditorState.PlacementEntity == Placement_Entity_Max)
                            GameState->EditorState.PlacementEntity = (Entity_Placement_Type)0;
                    }
                }
                break;
                case Editor_Placement_Tile:
                {
                    if(KEY_DOWN(Key_0))
                    {
                        GameState->EditorState.RenderAllLayers = !GameState->EditorState.RenderAllLayers; 
                    }
                    else if(KEY_DOWN(Key_1))
                    {
                        GameState->EditorState.CurrentTilemapLayer = 0;
                    }
                    else if(KEY_DOWN(Key_2))
                    {
                        GameState->EditorState.CurrentTilemapLayer = 1; 
                    }
                    else if(KEY_DOWN(Key_3))
                    {
                        GameState->EditorState.CurrentTilemapLayer = 2; 
                    }
                    else if(KEY_DOWN(Key_4))
                    {
                        GameState->EditorState.CurrentTilemapLayer = 3; 
                    }
                    else if(KEY_DOWN(Key_5))
                    {
                        GameState->EditorState.CurrentTilemapLayer = 4; 
                    }
                }
                break;
            }
            
            GameState->Camera = GameState->EditorCamera;
            TickTimers(GameState, DeltaTime);
        }
        break;
    }
    
    GameState->Camera.ProjectionMatrix = math::Ortho(0.0f,
                                                     (GameState->Camera.ViewportWidth / GameState->Camera.Zoom),
                                                     0.0f,
                                                     (GameState->Camera.ViewportHeight / GameState->Camera.Zoom),
                                                     -1.0f,
                                                     1.0f);
    //GameState->Camera.ViewMatrix = math::Rotate(GameState->Camera.ViewMatrix, 45.0f, math::v3(0.0f, 0.0f, 0.0f));
    GameState->Camera.ViewMatrix = math::Translate(math::m4(1.0f),
                                                   math::v3(-Center.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2,
                                                            -Center.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2,
                                                            0));
    
    InputController->CurrentCharacter = 0;
    GameState->RenderState.DeltaTime = DeltaTime;
    GameState->ClearTilePositionFrame = !GameState->ClearTilePositionFrame;
    GetActionButtonsForQueue(InputController);
    
    GameUpdateStruct->EntityCount = GameState->EntityCount;
    memcpy(&GameUpdateStruct->EntityPositions,&GameState->EntityPositions,sizeof(math::v2) * NUM_ENTITIES);
    
    GameState->Renderer.Camera = GameState->Camera;
    PushTilemapRenderCommands(GameState);
    PushEntityRenderCommands(GameState);
}

 