#include "game.h"
#include "gmap.cpp"
#include "rendering.cpp"
#include "skeletal_animation.cpp"
#include "keycontroller.cpp"
#include "ui.cpp"
#include "sound.cpp"
#include "animation.cpp"
#include "collision.cpp"
#include "console.cpp"
#include "ai.cpp"
#include "entity.cpp"
#include "level.cpp"
#include "editor.cpp"
#include "debug.cpp"

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

i32 CompareFunction(const void* a, const void* b)
{
    render_entity APtr = *(render_entity*)a;
    render_entity BPtr = *(render_entity*)b;
    
    math::v3 PositionA = APtr.RenderType == Render_Type_Entity ? APtr.Entity->Position : APtr.Object->Position;
    math::v3 PositionB = BPtr.RenderType == Render_Type_Entity ? BPtr.Entity->Position : BPtr.Object->Position;
    
    if(APtr.Background || PositionA.z > PositionB.z)
        return -1;
    if(BPtr.Background || PositionA.z < PositionB.z)
        return 1;
    return 0;
}

static void PushTilemapRenderCommands(renderer& Renderer, game_state& GameState)
{
    PushBuffer(Renderer, GameState.CurrentLevel.Tilemap.BufferHandle, GameState.CurrentLevel.Tilemap.TextureName, math::v3(90, 0, 0));
}

static void PushEntityRenderCommands(renderer& Renderer, game_state& GameState)
{
    qsort(GameState.RenderEntities, GameState.RenderEntityCount, sizeof(render_entity), CompareFunction);
    
    for(i32 Index = 0; Index < GameState.RenderEntityCount; Index++) 
    {
        render_entity* RenderEntity = &GameState.RenderEntities[Index];
        
        if(RenderEntity->RenderType == Render_Type_Entity)
            RenderEntity->Entity->RenderEntityHandle = Index;
        else if(RenderEntity->RenderType == Render_Type_Object)
            RenderEntity->Object->RenderEntityHandle = Index;
        
        math::v3 Position = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Position : RenderEntity->Object->Position;
        math::v3 Center = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Center : RenderEntity->Object->Center;
        r32 EntityScale = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->Scale : RenderEntity->Object->Scale;
        
        animation* CurrentAnimation =  RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->CurrentAnimation : RenderEntity->Object->CurrentAnimation;
        animation_info AnimationInfo = RenderEntity->RenderType == Render_Type_Entity ? RenderEntity->Entity->AnimationInfo : RenderEntity->Object->AnimationInfo;
        
        math::v3 CurrentPosition;
        math::v3 CurrentScale;
        math::v2 CurrentFrame;
        math::v2 CurrentTextureOffset;
        math::rgba CurrentColor;
        char* CurrentTexture;
        
        if(CurrentAnimation)
        {
            r32 WidthInUnits = (r32)CurrentAnimation->FrameSize.x / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = (r32)CurrentAnimation->FrameSize.y / (r32)PIXELS_PER_UNIT;
            
            math::v3 Scale = math::v3(WidthInUnits * EntityScale, HeightInUnits * EntityScale, 1.0f);
            
            auto CorrectPos = Position;
            
            CurrentPosition = CorrectPos;
            
            CurrentScale = Scale;
            
            animation* Animation = CurrentAnimation;
            
            CurrentTexture = Animation->Texture;
            Assert(strlen(CurrentTexture));
            
            CurrentTextureOffset = math::v2(Animation->Frames[AnimationInfo.FrameIndex].X, Animation->Frames[AnimationInfo.FrameIndex].Y);
            
            CurrentColor = RenderEntity->Color;
            CurrentFrame = Animation->FrameSize;
        } 
        else 
        {
            CurrentTexture = RenderEntity->TextureName;
            texture_data* Texture = Renderer.TextureMap[RenderEntity->TextureName];
            
            auto CorrectPos = math::v3(Position.x, Position.y, Position.z);
            
            CurrentPosition = CorrectPos;
            
            r32 WidthInUnits = Texture->Width / (r32)PIXELS_PER_UNIT;
            r32 HeightInUnits = Texture->Height / (r32)PIXELS_PER_UNIT;
            
            CurrentScale = math::v3(WidthInUnits * EntityScale, HeightInUnits * EntityScale, 1.0f);
            
            CurrentFrame = math::v2(Texture->Width, Texture->Height);
        }
        
        PushSprite(Renderer, CurrentPosition, CurrentScale, CurrentFrame, CurrentTextureOffset, CurrentTexture, CurrentColor);
    }
}

static void Pick(game_state* GameState, input_controller* InputController, renderer& Renderer)
{
    
    if(MOUSE_DOWN(Mouse_Left))
    {
        GameState->SelectedModel = -1;
    }
    
    auto Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    
    auto RayCast = math::CastPickingRay((r32)InputController->MouseX, InputController->MouseY, Camera.ProjectionMatrix, Camera.ViewMatrix, (r32)Renderer.ViewportWidth, (r32)Renderer.ViewportHeight);
    
    auto Ray = RayCast.Ray;
    auto Origin = RayCast.Origin;
    auto Target = RayCast.Target;
    
    auto Radius = 30.0f;
    
    for(i32 Index = 0; Index < GameState->ModelCount; Index++)
    {
        auto Cen = GameState->Models[Index].Position + math::v3(0.0f, Radius / 2.0f, 0.0f);
        
        auto B = Dot(Ray, Origin - Cen);
        auto C = Dot(Origin - Cen, Origin - Cen) - pow(Radius, 2);
        auto PreCalc = pow(B, 2) - C;
        
        if(PreCalc >= 0.0f)
        {
            auto T1 = -B + sqrt(PreCalc);
            auto T2 = -B - sqrt(PreCalc);
            
            auto Pick1 = Origin + Ray * Min(T1, T2);
            auto Pick2 = Origin + Ray * Max(T1, T2);
            auto Dist = Min(Distance(Cen, Pick1), Distance(Cen, Pick2));
            
            if(MOUSE_DOWN(Mouse_Left) && (Dist < Radius || T1 == T2))
            {
                GameState->SelectedModel = Index;
            }
        }
    }
    
    if(GameState->SelectedModel != -1)
    {
        auto& Selected = GameState->Models[GameState->SelectedModel];
        auto MPos = Selected.Position;
        
        auto Or = ToMatrix(Selected.Orientation);
        
        auto MForward = math::Forward(Or);
        auto MUp = math::Up(Or);
        auto MRight = math::Right(Or);
        r32 LineLength = 20.0f;
        
        PushLine(Renderer, MPos, MPos + MForward * LineLength, 1.0f, math::rgba(1.0f, 0.0f, 0.0f, 1.0f));
        
        PushLine(Renderer, MPos, MPos + MUp * LineLength, 1.0f, math::rgba(0.0f, 0.0f, 1.0f, 1.0f));
        
        PushLine(Renderer, MPos, MPos + MRight * LineLength, 1.0f, math::rgba(0.0f, 1.0f, 0.0f, 1.0f));
        
        math::v3 CubeScale = math::v3(5.0f, 10.0f, 5.0f);
        math::v3 CubePos = MPos + math::v3(0.0f, 10.0f, 0.0f);
        
        PushWireframeCube(Renderer, CubePos, CubeScale, Selected.Orientation, math::rgba(1.0f, 0.0f, 1.0f, 1.0f), 1.0f);
        
        math::v3 LineP1 = CubePos;
        math::v3 LineP2 = LineP1 + MRight * LineLength;
        
        PushLine(Renderer, LineP1, LineP2, 2.0f, math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
        
        if(MOUSE(Mouse_Left))
        {
            auto NewPos = Origin + Ray * -10.0f;
            Selected.Position = NewPos;
        }
    }
}

platform_api Platform;
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
    }
    Assert(GameState);
    
    //@Incomplete: Hmmmm
    GameState->ReloadData = GameMemory->ReloadData;
    
    if(!GameState->IsInitialized || !GameMemory->IsInitialized)
    {
        model PlayerModel;
        LoadGLIMModel(Renderer, "../assets/models/hatman.glim", &PlayerModel);
        PlayerModel.Position = math::v3(0, 0, 0);
        PlayerModel.Scale = math::v3(2, 2, 2);
        PlayerModel.Orientation = math::quat(1.0f, 0.0f, 0.0f, 180.0f * DEGREE_IN_RADIANS);
        
        GameState->Models[GameState->ModelCount++] = PlayerModel;
        GameState->PlayerModel = GameState->ModelCount - 1;
        
        PlayAnimation(0, GameState->Models[GameState->PlayerModel], true);
        
        model Cube;
        
        //LoadGLIMModel(Renderer, "../assets/models/cube.glim", &Cube);
        
        //GameState->Models[GameState->ModelCount++] = Cube;
        //GameState->Models[GameState->ModelCount - 1].Position = math::v3(-10, -1.5f, 0);
        //GameState->Models[GameState->ModelCount - 1].Scale = math::v3(100, 1.0f, 100);
        
        //GameState->Models[GameState->ModelCount++] = Cube;
        //GameState->Models[GameState->ModelCount - 1].Position = math::v3(-9.0f, 0.0f, -2.0f);
        //GameState->Models[GameState->ModelCount - 1].Scale = math::v3(1.0f);
        
        LoadTextures(Renderer, &GameState->TotalArena);
        GameState->SelectedModel = -1;
        
        collision_volume C1;
        C1.Center = math::v3(0.0f, 0.0f, 0.0f);
        C1.Extents = math::v3(5.0f, 3.0f, 7.0f);
        C1.Orientation = math::quat();
        C1.Static = true;
        
        collision_volume C2;
        C2.Center = math::v3(10.0f, 0.0f, 0.0f);
        C2.Extents = math::v3(3.0f, 3.0f, 1.0f);
        C2.Orientation = math::quat();
        C2.Static = false;
        
        GameState->CollisionVolumes[GameState->CollisionVolumeCount++] = C1;
        GameState->CollisionVolumes[GameState->CollisionVolumeCount++] = C2;
        
        if(GameState->ShouldReload || GameMemory->ShouldReload)
        {
            GameState->GameCameraHandle = 0;
            GameState->EditorCameraHandle = 1;
            Renderer.CurrentCameraHandle = GameState->GameCameraHandle;
            Renderer.Cameras[GameState->GameCameraHandle].Zoom = GameMemory->ConfigData.Zoom;
            GameState->InitialZoom = GameMemory->ConfigData.Zoom;
            GameState->LevelPath = PushString(&GameState->TotalArena, GameMemory->ConfigData.StartingLevelFilePath);
            
            GameState->ShouldReload = GameMemory->ShouldReload;
            GameState->Console = {};
            GameState->RenderGame = true;
            GameState->RenderLight = true;
            GameState->ClearTilePositionFrame = false;
            GameState->StatGainModeOn = false;
            GameState->SelectedGainIndex = 0;
            GameState->AIDebugModeOn = false;
            GameState->GodModeOn = false;
            GameState->GodModePanSpeed = 10.0f;
            GameState->GodModeZoomSpeed = 45.0f;
            GameState->GodModeMinZoom = 2.0f;
            GameState->GodModeMaxZoom = 200.0f;
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
            GameState->EditorState.MaxZoom = 150;
            GameState->EditorState.RenderedTileSize = 30.0f;
            GameState->EditorState.TileBrushSize = math::v2(1, 1);
            GameState->EditorState.ToolbarScrollSpeed = 30000;
            GameState->EditorState.ToolbarScrollOffsetY = 0.0f;
            GameState->EditorState.IsInCreateWaypointMode = false;
            GameState->EditorState.SelectedAnimation = 0;
            
            sounds Sounds = {};
            //@Incomplete: Get actual sizes, this is retarded
            memcpy(&GameState->Sounds.SoundEffects, SoundEffects, sizeof(sound_effect) * (64 + 32));
            
            LoadGameDataFile(GameState);
            srand((u32)time(NULL));
            
            LoadAnimations(GameState, Renderer);
            InitCommands();
            
            auto& EditorCamera = Renderer.Cameras[GameState->EditorCameraHandle];
            
            EditorCamera.Zoom = GameState->InitialZoom; 
            EditorCamera.ViewportWidth = Renderer.WindowWidth;
            EditorCamera.ViewportHeight = Renderer.WindowHeight;
            
            GameState->GameMode = Mode_InGame;
            GameState->ShouldReload = false;
            GameMemory->ShouldReload = false;
            PLAY_TRACK(Brugt);
            
            Renderer.ClearColor = math::rgba(0.2f, 0.2f, 0.2f, 1.0f);
            Renderer.LineWidth = 1.0f;
        }
        
        LoadLevelFromFile(GameState->LevelPath, &GameState->CurrentLevel, GameState,  Renderer, SoundCommands);
        
        auto& GameCamera = Renderer.Cameras[GameState->GameCameraHandle];
        
        GameCamera.Zoom = GameState->InitialZoom;
        GameCamera.ViewportWidth = Renderer.WindowWidth;
        GameCamera.ViewportHeight = Renderer.WindowHeight;
        GameCamera.FollowSpeed = 3.5f; 
        GameCamera.FadingSpeed = 0.6f;
        
        StartFade(GameCamera, Fading_In, 0.6f, math::v3(0, 0, 0), 1.0f, 0.0f);
        
        // @Incomplete: This is not the right value, it is only set so high to remove smooth following as of now, since it needs to be done a little differently
        
        // Set center to player's position!
        GameCamera.Target = GameCamera.Center;
        
        GameState->IsInitialized = true;
        GameMemory->IsInitialized = true;
    }
    
    if(KEY_DOWN(Key_Q) && KEY(Key_LeftCtrl))
    {
        GameMemory->ExitGame = true;
        GameState->GameMode = Mode_Exit;
    }
    
    if(GameState->ReloadData)
    {
        if(GameState->ReloadData->ReloadPlayerFile)
        {
            LoadPlayerData(GameState, SoundCommands, 0);
            GameState->ReloadData->ReloadPlayerFile = false;
        }
        
        if(GameState->ReloadData->ReloadSkeletonFile)
        {
            for(u32 EntityIndex = 0; EntityIndex < GameState->EntityCount; EntityIndex++)
            {
                auto Entity = &GameState->Entities[EntityIndex];
                if(Entity->Type == Entity_Enemy && Entity->Enemy.EnemyType == Enemy_Skeleton && !IsSet(Entity, EFlag_Dead))
                {
                    LoadSkeletonData(GameState, EntityIndex);
                }
            }
            GameState->ReloadData->ReloadSkeletonFile = false;
        }
        
        if(GameState->ReloadData->ReloadBonfireFile)
        {
            for(u32 EntityIndex = 0; EntityIndex < GameState->EntityCount; EntityIndex++)
            {
                auto& Entity = GameState->Entities[EntityIndex];
                if(Entity.Type == Entity_Bonfire)
                {
                    LoadBonfireData(GameState, SoundCommands, EntityIndex);
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
        CheckEditorUIInput(GameState, Renderer, InputController, DeltaTime);
    
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
                Renderer.Cameras[GameState->EditorCameraHandle].Center = Renderer.Cameras[GameState->GameCameraHandle].Center;
            }
            else
            {
                GameState->RenderLight = true;
                GameState->GodModeOn = false;
                SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, Renderer);
                ReloadCurrentLevel(GameState);
                GameState->GameMode = Mode_InGame;
            }
        }
        else if(KEY_DOWN(Key_G)) // God-mode on/off
        {
            GameState->GodModeOn = !GameState->GodModeOn;
            
            if(!GameState->GodModeOn)
            {
                auto& Player = GameState->Entities[0];
                auto Pos = Player.Position;
                Renderer.Cameras[GameState->GameCameraHandle].Center = math::v3(Pos.x, Pos.y, Pos.z);
                Renderer.Cameras[GameState->GameCameraHandle].Zoom = GameState->ZoomBeforeGodMode;
            }
            else
            {
                GameState->ZoomBeforeGodMode = Renderer.Cameras[GameState->GameCameraHandle].Zoom;
            }
        }
    }
    
    if(!GameState->EditorState.Loaded)
    {
        InitEditorFields(GameState, Renderer);
        CreateEditorButtons(GameState, Renderer);
    }
    
#if GLITCH_DEBUG
    if(KEY_DOWN(Key_F1))
    {
        //RenderState.RenderColliders = !RenderState.RenderColliders;
    }
    
    if(KEY_DOWN(Key_F2))
    {
        //RenderState.RenderFPS = !RenderState.RenderFPS;
    }
    
    if(KEY_DOWN(Key_F3))
    {
        SoundCommands->Muted = !SoundCommands->Muted;
    }
    
    if(KEY_DOWN(Key_F4))
    {
        //RenderState.RenderPaths = !RenderState.RenderPaths;
    }
    
    if(KEY_DOWN(Key_F5))
    {
        SoundCommands->Paused = !SoundCommands->Paused;
    }
    
    if(KEY_DOWN(Key_F6))
    {
        SoundCommands->Stopped = !SoundCommands->Stopped;
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
    
    auto& GameCamera = Renderer.Cameras[GameState->GameCameraHandle];
    
    math::v3 Center = GameCamera.Center;
    
    if(GameState->GodModeOn)
    {
        r32 Zoom = GameCamera.Zoom;
        
        math::v2 Direction = math::v2(0, 0);
        
        if(KEY(Key_W) || KEY(Key_Up))
        {
            Direction.y = -1;
        }
        else if(KEY(Key_S) || KEY(Key_Down))
        {
            Direction.y = 1;
        }
        
        if(KEY(Key_A) || KEY(Key_Left))
        {
            Direction.x = -1;
            Direction.y = -0.5;
        }
        else if(KEY(Key_D) || KEY(Key_Right))
        {
            Direction.x = 1;
            Direction.y = 0.5;
        }
        
        r32 Factor = 72.0f / GameCamera.Zoom;
        
        if(KEY(Key_Add))
        {
            Zoom += (r32)(GameState->GodModeZoomSpeed / Factor * DeltaTime);
        }
        else if(KEY(Key_Subtract))
        {
            Zoom += (r32)(-GameState->GodModeZoomSpeed / Factor * DeltaTime);
        }
        
        r32 ScrollFactor = 1.0f / GameCamera.Zoom;
        
        if(InputController->ScrollY > 0.0)
        {
            Zoom += (r32)(GameState->GodModeZoomSpeed / ScrollFactor * DeltaTime);
        }
        else if(InputController->ScrollY < 0.0)
        {
            Zoom += (r32)(-GameState->GodModeZoomSpeed / ScrollFactor * DeltaTime);
        }
        
        Direction = math::Normalize(Direction);
        
        if(Abs(Direction.x) > 0.0 || Abs(Direction.y) > 0.0)
        {
            GameCamera.Center += math::v3(Direction.x * GameState->GodModePanSpeed * Factor * DeltaTime, Direction.y * GameState->GodModePanSpeed * Factor * DeltaTime, 0);
        }
        
        GameCamera.Zoom = Min(Max(Zoom, GameState->GodModeMinZoom), GameState->GodModeMaxZoom);
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
                UpdateEntities(GameState, Renderer, InputController, SoundCommands, DeltaTime);
                
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
                    /*if(math::Distance(GameCamera.CenterTarget, math::v3(Center.x, Center.y, Center.z)) > 0.01f)
                    {
                    auto Direction = math::Normalize(GameCamera.CenterTarget - math::v3(Center.x, Center.y, Center.z));
                    
                    Center = math::v3(Center.x + Direction.x * GameCamera.FollowSpeed * DeltaTime, Center.y + Direction.y  * GameCamera.FollowSpeed * DeltaTime,Center.z + Direction.z * GameCamera.FollowSpeed * DeltaTime);
                    
                    GameCamera.Center = Center;
                    }*/
                }
                else
                {
                    r32 XInput = GetInputX(InputController);
                    r32 YInput = GetInputY(InputController);
                    
                    math::v3 Velocity;
                    Velocity.x = XInput * 10.0f;
                    Velocity.z = YInput * 10.0f;
                    
                    GameCamera.Center += math::v3(Velocity.x * DeltaTime, Velocity.y * DeltaTime, -Velocity.z * DeltaTime);
                }
            }
            
            if(GameState->PlayerState == Player_Alive && IsSet(&GameState->Entities[0], EFlag_Dead))
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
        }
        break;
        case Mode_Editor:
        {
            Center = Renderer.Cameras[GameState->EditorCameraHandle].Center;
            EditorUpdateEntities(GameState, Renderer, InputController, SoundCommands, DeltaTime);
            
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
            
            GameCamera = Renderer.Cameras[GameState->EditorCameraHandle];
            TickTimers(GameState, DeltaTime);
        }
        break;
    }
    auto& PlayerModel = GameState->Models[GameState->PlayerModel];
    r32 Near = -100.0f;
    r32 Far = 1000.0f;
    
    GameCamera.Center = PlayerModel.Position;
    
    if(GameState->GodModeOn)
    {
        auto Target = math::v3();
        auto DeltaX = InputController->MouseX - GameState->PrevMouseX;
        auto DeltaY = InputController->MouseY - GameState->PrevMouseY;
        math::quat Orientation = GameCamera.Orientation;
        if(MOUSE(Mouse_Right))
        {
            if(DeltaY > 0.0)
            {
                Orientation = math::Rotate(GameCamera.Orientation, 1.0f, math::v3(1.0f, 0.0f, 0.0f));
            }
            else if(DeltaY < 0.0)
            {
                Orientation = math::Rotate(GameCamera.Orientation, -1.0f, math::v3(1.0f, 0.0f, 0.0f));
            }
            
            if(DeltaX > 0.0)
            {
                Orientation = math::Rotate(GameCamera.Orientation, 1.0f, math::v3(0.0f, 1.0f, 0.0f));
            }
            else if(DeltaX < 0.0)
            {
                Orientation = math::Rotate(GameCamera.Orientation, -1.0f, math::v3(0.0f, 1.0f, 0.0f));
            }
            
        }
        
        math::v3 Position = GameCamera.Position;
        if(MOUSE(Mouse_Middle))
        {
            
            if(DeltaY > 0.0)
            {
                Position.y += 1.0f;
            }
            else if(DeltaY < 0.0)
            {
                Position.y += -1.0f;
            }
            
            if(DeltaX > 0.0)
            {
                Position.x += -1.0f;
            }
            else if(DeltaX < 0.0)
            {
                Position.x += 1.0f;
            }
            
        }
        
        CameraTransform(Renderer, GameCamera, Position, Orientation, Target, GameCamera.Zoom, Near, Far, CFlag_Orthographic);
    }
    else
    {
        CameraTransform(Renderer, GameCamera, GameCamera.Center, math::quat(), math::v3(), GameCamera.Zoom, Near, Far, CFlag_Orthographic);
    }
    
    auto Forward = math::Forward(GameCamera.ViewMatrix);
    auto Up = math::Up(GameCamera.ViewMatrix);
    auto Right = math::Right(GameCamera.ViewMatrix);
    
    
    if(GameState->GodModeOn)
    {
        r32 Vertices[32] =
        {
            -0.5, -0.5, -0.5, 1.0,
            0.5, -0.5, -0.5, 1.0,
            0.5,  0.5, -0.5, 1.0,
            -0.5,  0.5, -0.5, 1.0,
            -0.5, -0.5,  0.5, 1.0,
            0.5, -0.5,  0.5, 1.0,
            0.5,  0.5,  0.5, 1.0,
            -0.5,  0.5,  0.5, 1.0,
        };
        
        Pick(GameState, InputController, Renderer);
        
        if(GameState->SelectedModel != -1)
        {
            auto N1 = math::v3(1.0f, 0.0f, 0.0f);
            auto N2 = math::v3(0.0f, 1.0f, 0.0f);
            auto N3 = math::v3(0.0f, 0.0f, 1.0f);
            auto N4 = math::v3(-1.0f, 0.0f, 0.0f);
            auto N5 = math::v3(0.0f, -1.0f, 0.0f);
            auto N6 = math::v3(0.0f, 0.0f, -1.0f);
            
            auto P11 = math::v3(5.0f, 2.5f, 2.5f);
            auto P12 = P11 + N1 * 5.0f;
            
            auto P21 = math::v3(2.5f, 5.0f, 2.5f);
            auto P22 = P21 + N2 * 5.0f;
            
            auto P31 = math::v3(2.5f, 2.5f, 5.0f);
            auto P32 = P31 + N3 * 5.0f;
            
            auto P41 = math::v3(-5.0f, 2.5f, 2.5f);
            auto P42 = P41 + N4 * 5.0f;
            
            auto P51 = math::v3(2.5f, -5.0f, 2.5f);
            auto P52 = P51 + N5 * 5.0f;
            
            auto P61 = math::v3(2.5f, 2.5f, -5.0f);
            auto P62 = P61 + N6 * 5.0f;
            
            PushLine(Renderer, P11, P12, 1.0f, math::rgba(1.0f, 1.0f, 0.0f, 1.0f));
            PushLine(Renderer, P21, P22, 1.0f, math::rgba(1.0f, 1.0f, 0.0f, 1.0f));
            PushLine(Renderer, P31, P32, 1.0f, math::rgba(1.0f, 1.0f, 0.0f, 1.0f));
            PushLine(Renderer, P41, P42, 1.0f, math::rgba(1.0f, 1.0f, 0.0f, 1.0f));
            PushLine(Renderer, P51, P52, 1.0f, math::rgba(1.0f, 1.0f, 0.0f, 1.0f));
            PushLine(Renderer, P61, P62, 1.0f, math::rgba(1.0f, 1.0f, 0.0f, 1.0f));
            
            
        }
    }
    
    InputController->CurrentCharacter = 0;
    GameState->ClearTilePositionFrame = !GameState->ClearTilePositionFrame;
    GetActionButtonsForQueue(InputController);
    
    GameUpdateStruct->EntityCount = GameState->EntityCount;
    memcpy(&GameUpdateStruct->EntityPositions, &GameState->EntityPositions, sizeof(math::v2) * NUM_ENTITIES);
    
    PushDirectionalLight(Renderer, math::v3(-0.2, -1.0, -0.3), 
                         math::v3(0.1f, 0.1f, 0.1f), math::v3(0.2, 0.2, 0.2), math::v3(0.1, 0.1, 0.1));
    
    char FPSBuffer[64];
    sprintf(FPSBuffer, "FPS: %.2f - AVG FPS: %.2f - dt: %.10lf", Renderer.FPS, Renderer.AverageFPS, DeltaTime);
    TickAnimation(PlayerModel, DeltaTime);
    
    PushEntityRenderCommands(Renderer, *GameState);
    
    if(KEY_DOWN(Key_Home))
    {
        DebugState->DebugMemory = !DebugState->DebugMemory;
    }
    
    PushDebugRender(Renderer, DebugState, InputController);
    
    //PushTilemapRenderCommands(Renderer, *GameState);
    
    PushText(Renderer, FPSBuffer, math::v3(50, 850, 2), Font_Inconsolata, math::rgba(1, 0, 0, 1));
    
    for(i32 Index = 0; Index < GameState->ModelCount; Index++)
    {
        PushModel(Renderer, GameState->Models[Index]);
        PushSpotlight(Renderer, GameState->Models[Index].Position, math::v3(1.0f, 0.0f, 0.0f), DEGREE_IN_RADIANS * 12.5f, DEGREE_IN_RADIANS * 17.5f, math::v3(0.1f, 0.1f, 0.1f), math::v3(1.0f, 1.0f, 1.0), math::v3(1.0, 1.0, 1.0), 1.0f, 0.09f, 0.032f);
    }
    
    if(KEY(Key_X))
    {
        auto Angle = 20.0f;
        if(KEY(Key_LeftShift))
        {
            Angle = -Angle;
        }
        
        GameState->CollisionVolumes[1].Orientation = math::Rotate(GameState->CollisionVolumes[1].Orientation, Angle * DeltaTime, math::v3(1.0f, 0.0f, 0.0f));
    }
    
    if(KEY(Key_Y))
    {
        auto Translate = 1.0f;
        if(KEY(Key_LeftShift))
        {
            Translate = -Translate;
        }
        
        GameState->CollisionVolumes[1].Center += math::v3(Translate * DeltaTime, 0.0f, 0.0f);
        
    }
    
    for(i32 Index = 0; Index < GameState->CollisionVolumeCount; Index++)
    {
        auto& C = GameState->CollisionVolumes[Index];
        for(i32 J = 0; J < GameState->CollisionVolumeCount; J++)
        {
            if(J == Index)
                continue;
            sat_collision_info CollisionInfo;
            CheckSATCollision(C, GameState->CollisionVolumes[J], &CollisionInfo);
            C.Colliding = CollisionInfo.Colliding;
            
            if(!C.Static)
            {
                GameState->CollisionVolumes[Index].Center -= CollisionInfo.PV * CollisionInfo.Overlap;
            }
            
            PushLine(Renderer, C.Center, GameState->CollisionVolumes[J].Center, 3.0f, math::rgba(0.5f, 0.7f, 0.3f, 1.0f));
        }
        
        PushCollisionVolume(Renderer, C, true, true);
    }
    //PushFilledQuad(Renderer, math::v3(0, 0, 0), math::v3(1000, 1000, 0), math::v3(0, 0, 0),  math::rgba(1, 1, 1, 1), "play_button");
    
    if(PushButton(Renderer, "Reset player", rect(5, 5, 200, 50), math::rgba(1, 0, 0, 1), math::rgba(1, 1, 1, 1), InputController))
    {
        PlayerModel.Position = math::v3();
    }
    
    GameState->PrevMouseX = (r32)InputController->MouseX;
    GameState->PrevMouseY = (r32)InputController->MouseY;
}