#include "game.h"
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

static void TickTimers(game_state* GameState, real64 DeltaTime)
{
    for(uint32 Index = 0; Index < NUM_TIMERS; Index++)
    {
        if(GameState->Timers[Index] > 0)
            GameState->Timers[Index] -= DeltaTime;
        else
            GameState->Timers[Index] = 0;
    }
}

static void EditorUpdateEntities(game_state* GameState, real64 DeltaTime)
{
    bool32 InToolbar = GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= GameState->RenderState.WindowHeight - 150;
    
    for(uint32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
    {
        button* Button = &GameState->EditorState.Buttons[ButtonIndex];
        Button->Clicked = false;
        
        if(GetMouseButtonDown(Mouse_Left, GameState))
        {
            if(Button->Active && GameState->InputController.MouseX >= Button->ScreenPosition.x && GameState->InputController.MouseX <= Button->ScreenPosition.x + Button->Size.x && 
               GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= Button->ScreenPosition.y && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <= Button->ScreenPosition.y + Button->Size.y)
            {
                Button->Clicked = true;
                
                PlaySoundEffect(GameState, &GameState->SoundManager.ButtonClick);
                StartTimer(GameState, Button->ClickAnimationTimer);
                
                switch(Button->EditorType)
                {
                    case Button_Exit:
                    {
                        GameState->GameMode = Mode_Exit;
                    }
                    break;
                    case Button_SaveAndExit:
                    {
                        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                        ReloadCurrentLevel(GameState);
                        GameState->GameMode = Mode_InGame;
                    }
                    break;
                    case Button_Tilesheet:
                    {
                    }
                    break;
                    case Button_Animation:
                    {
                        std::map<char*, animation>::iterator AnimationIterator;
                        
                        if(GameState->EditorState.Animations)
                            free(GameState->EditorState.Animations);
                        
                        GameState->EditorState.Animations = (char**)malloc(GameState->Animations.size() * sizeof(char*));
                        
                        int32 Index = 0;
                        for(AnimationIterator = GameState->Animations.begin(); AnimationIterator != GameState->Animations.end(); AnimationIterator++)
                        {
                            GameState->EditorState.Animations[Index++] = AnimationIterator->first;
                        }
                        
                        GameState->EditorState.AnimationsLength = (int32)GameState->Animations.size();
                        
                        GameState->EditorState.CreateNewAnimationButton->Active = true;
                        GameState->EditorState.TileIsSolidCheckbox->Active = false;
                        GameState->EditorState.Mode = Editor_Animation;
                        
                        std::map<char*, texture*>::iterator TextureIterator;
                        
                        if(GameState->EditorState.Textures)
                            free(GameState->EditorState.Textures);
                        
                        GameState->EditorState.Textures = (char**)malloc(GameState->RenderState.Textures.size() * sizeof(char*));
                        
                        Index = 0;
                        
                        for(TextureIterator = GameState->RenderState.Textures.begin(); TextureIterator != GameState->RenderState.Textures.end(); TextureIterator++)
                        {
                            GameState->EditorState.Textures[Index++] = TextureIterator->first;
                        }
                        
                        GameState->EditorState.TexturesLength = (int32)GameState->RenderState.Textures.size();
                        GameState->EditorState.Mode = Editor_Animation;
                        GameState->EditorState.Editing = true;
                    }
                    break;
                    case Button_SwitchMode:
                    {
                        if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                            GameState->EditorState.PlacementMode = Editor_Placement_Entity;
                        else
                            GameState->EditorState.PlacementMode = Editor_Placement_Tile;
                    }
                    break;
                }
            }
        }
        
        if(Button->Active)
        {
            if(!TimerDone(GameState, Button->ClickAnimationTimer))
                Button->Color = glm::vec4(0.5, 0.5, 0.5, 1);
            else
                Button->Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
        }
    }
    
    for(uint32 Index = 0; Index < 10; Index++)
    {
        checkbox* Checkbox = &GameState->EditorState.Checkboxes[Index];
        Checkbox->JustChecked = false;
        
        if(GetMouseButtonDown(Mouse_Left, GameState))
        {
            if(Checkbox->Active && GameState->InputController.MouseX >= Checkbox->ScreenPosition.x && GameState->InputController.MouseX <= Checkbox->ScreenPosition.x + 25 && GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= Checkbox->ScreenPosition.y && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <= Checkbox->ScreenPosition.y + 25)
            {
                Checkbox->Checked = !Checkbox->Checked;
                Checkbox->JustChecked = true;
            }
        }
    }
    
    if(GameState->EditorState.TileIsSolidCheckbox->JustChecked)
    {
        uint32 SelectedTile = GameState->EditorState.SelectedTileType;
        GameState->CurrentLevel.Tilemap.Tiles[SelectedTile].IsSolid = GameState->EditorState.TileIsSolidCheckbox->Checked;
        UpdateTileData(SelectedTile, GameState->CurrentLevel.Tilemap.Tiles[SelectedTile].IsSolid, &GameState->CurrentLevel.Tilemap);
        SaveTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", GameState->CurrentLevel.SheetName), ".tm"), &GameState->RenderState, GameState->CurrentLevel, false);
        
        // @Incomplete: Should call SaveTilesheetMetafile!!!!
    }
    
    int32 FocusedField = GameState->EditorState.FocusedTextfield;
    
    for(int32 Index = 0; Index < 20; Index++)
    {
        textfield* Textfield = &GameState->EditorState.Textfields[Index];
        Textfield->InFocus = false;
        
        if(GetMouseButtonDown(Mouse_Left, GameState))
        {
            if(Textfield->Active && GameState->InputController.MouseX >= Textfield->ScreenPosition.x && GameState->InputController.MouseX <= Textfield->ScreenPosition.x + Textfield->Size.x && 
               GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= Textfield->ScreenPosition.y && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <= Textfield->ScreenPosition.y + Textfield->Size.y)
            {
                GameState->EditorState.FocusedTextfield = Index;
                Textfield->InFocus = true;
                /*
                PlaySoundEffect(GameState, &GameState->SoundManager.ButtonClick);
                StartTimer(GameState, Button->ClickAnimationTimer);
                */
            }
        }
        
        if(Index == GameState->EditorState.FocusedTextfield)
        {
            if(GameState->InputController.DeleteCharacter && Textfield->TextIndex != 0)
            {
                Textfield->Text[--Textfield->TextIndex] = 0;
            }
            
            if(GameState->InputController.CurrentCharacter && GameState->InputController.CurrentCharacter != ' ')
            {
                bool32 CanEnter = true;
                
                switch(Textfield->Type)
                {
                    case Textfield_Normal:
                    {}
                    break;
                    case Textfield_Integer:
                    {
                        CanEnter = GameState->InputController.CurrentCharacter == '0' ||
                            GameState->InputController.CurrentCharacter == '1' ||
                            GameState->InputController.CurrentCharacter == '2' ||
                            GameState->InputController.CurrentCharacter == '3' ||
                            GameState->InputController.CurrentCharacter == '4' ||
                            GameState->InputController.CurrentCharacter == '5' ||
                            GameState->InputController.CurrentCharacter == '6' ||
                            GameState->InputController.CurrentCharacter == '7' ||
                            GameState->InputController.CurrentCharacter == '8' ||
                            GameState->InputController.CurrentCharacter == '9';
                    }
                    break;
                    case Textfield_Decimal:
                    {
                        CanEnter = GameState->InputController.CurrentCharacter == '0' ||
                            GameState->InputController.CurrentCharacter == '1' ||
                            GameState->InputController.CurrentCharacter == '2' ||
                            GameState->InputController.CurrentCharacter == '3' ||
                            GameState->InputController.CurrentCharacter == '4' ||
                            GameState->InputController.CurrentCharacter == '5' ||
                            GameState->InputController.CurrentCharacter == '6' ||
                            GameState->InputController.CurrentCharacter == '7' ||
                            GameState->InputController.CurrentCharacter == '8' ||
                            GameState->InputController.CurrentCharacter == '9' ||
                            GameState->InputController.CurrentCharacter == '.';
                    }
                    break;
                }
                
                if(CanEnter && Textfield->TextIndex != TEXTFIELD_LENGTH)
                {
                    Textfield->Text[Textfield->TextIndex++] = GameState->InputController.CurrentCharacter;
                }
            }
        }
    }
    
    if(GameState->EditorState.FocusedTextfield == FocusedField)
    {
        GameState->EditorState.Textfields[GameState->EditorState.FocusedTextfield].InFocus = true;
    }
    
    
    switch(GameState->EditorState.Mode)
    {
        case Editor_Normal:
        {
            if(GameState->EditorState.CreateNewLevelButton->Clicked)
            {
                level Level;
                Level.SheetName = "overworld1";
                Level.Name = "test_level";
                
                CreateNewLevelWithSize("../assets/levels/level_new.plv", 200, 120, &Level, GameState);
                GameState->CurrentLevel = Level;
            }
            
            auto Pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0),
                                      GameState->Camera.ViewMatrix,
                                      GameState->Camera.ProjectionMatrix,
                                      glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
            
            GameState->EditorState.CreateNewAnimationButton->Active = false;
            GameState->EditorState.SaveAnimationButton->Active = false;
            
            if(GetKeyDown(Key_M, GameState))
            {
                if(GameState->EditorState.PlacementMode == Editor_Placement_Tile)
                {
                    GameState->EditorState.PlacementMode = Editor_Placement_Entity;
                    SpawnSkeleton(GameState,Pos);
                    GameState->EditorState.PlacementEntity = &GameState->Entities[GameState->EntityCount - 2];
                }
                else
                {
                    GameState->EditorState.PlacementMode = Editor_Placement_Tile;
                    if(GameState->EditorState.PlacementEntity)
                    {
                        DeleteEntity(GameState,GameState->EditorState.PlacementEntity->EntityIndex);
                        GameState->EditorState.PlacementEntity = 0;
                    }
                }
            }
            
            
            if(!InToolbar)
            {
                switch(GameState->EditorState.PlacementMode)
                {
                    case Editor_Placement_Entity:
                    {
                        if(GetMouseButtonDown(Mouse_Left, GameState) && !GameState->EditorState.PlacementEntity)
                        {
                            entity* Selected = 0;
                            
                            for(uint32 EntityIndex = 0;
                                EntityIndex < GameState->EntityCount;
                                EntityIndex++)
                            {
                                entity* Entity = &GameState->Entities[EntityIndex];
                                
                                if(Entity->Type != Entity_Weapon && Pos.x >= Entity->Position.x - Entity->Scale.x/2 && Pos.y < Entity->Position.y + Entity->Scale.y && Pos.x < Entity->Position.x + Entity->Scale.x && Pos.y > Entity->Position.y)
                                {
                                    Selected = Entity;
                                    break;
                                }
                            }
                            
                            GameState->EditorState.SelectedEntity = Selected;
                        }
                        
                        if(GameState->EditorState.SelectedEntity && GetMouseButton(Mouse_Left, GameState) && !GameState->EditorState.PlacementEntity)
                        {
                            GameState->EditorState.SelectedEntity->Position = glm::vec2(Pos.x, Pos.y - GameState->EditorState.SelectedEntity->Scale.y / 2 );
                        }
                        
                        if(GameState->EditorState.PlacementEntity)
                        {
                            GameState->EditorState.PlacementEntity->Position = glm::vec2(Pos.x, Pos.y - GameState->EditorState.PlacementEntity->Scale.y / 2);
                            
                            if(GetMouseButtonDown(Mouse_Left, GameState))
                            {
                                GameState->EditorState.PlacementEntity = 0;
                                SpawnSkeleton(GameState,Pos);
                                GameState->EditorState.PlacementEntity = &GameState->Entities[GameState->EntityCount - 1];
                            }
                        }
                    }
                    break;
                    case Editor_Placement_Tile:
                    {
                        GameState->EditorState.TileIsSolidCheckbox->Active = true;
                        int32 X = (int32)glm::floor(Pos.x);
                        int32 Y = (int32)glm::floor(Pos.y);
                        GameState->EditorState.TileX = (real32)X;
                        GameState->EditorState.TileY = (real32)glm::ceil(Pos.y);
                        
                        if(GetMouseButton(Mouse_Left, GameState))
                        {
                            if(GameState->InputController.MouseX >= GameState->EditorState.ToolbarX)
                            {
                                uint32 Selected = (uint32)((GameState->RenderState.WindowHeight - GameState->InputController.MouseY + abs(GameState->EditorState.ToolbarScrollOffsetY)) / 60.0f);
                                GameState->EditorState.SelectedTileType = Selected;
                                GameState->EditorState.TileIsSolidCheckbox->Checked = GameState->CurrentLevel.Tilemap.Tiles[Selected].IsSolid;
                            }
                            else
                            {
                                if(X >= 0 && X < (int32)GameState->CurrentLevel.Tilemap.Width 
                                   && Y >= 0 && Y < (int32)GameState->CurrentLevel.Tilemap.Height)
                                {
                                    tilemap* Tilemap = &GameState->CurrentLevel.Tilemap;
                                    
                                    if(GetKey(Key_F, GameState))
                                    {
                                        for(uint32 IndexX = 0; IndexX < Tilemap->Width; IndexX++)
                                        {
                                            for(uint32 IndexY = 0; IndexY < Tilemap->Height; IndexY++)
                                            {
                                                collision_AABB CollisionAABB;
                                                CollisionAABB.Center = glm::vec2(IndexX + 0.5f, IndexY + 0.5f);
                                                CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                                                
                                                Tilemap->Data[IndexX][IndexY] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                                
                                                Tilemap->RenderInfo.Dirty = true;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        collision_AABB CollisionAABB;
                                        CollisionAABB.Center = glm::vec2(X + 0.5f, Y + 0.5f);
                                        CollisionAABB.Extents = glm::vec2(0.5, 0.5);
                                        
                                        Tilemap->Data[X][Y] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                        
                                        Tilemap->RenderInfo.Dirty = true;
                                    }
                                }
                            }
                        }
                        if(GetMouseButton(Mouse_Left,GameState) && GetKey(Key_LeftShift,GameState))
                        {
                            if(X >= 0 && X < (int32)GameState->CurrentLevel.Tilemap.Width && Y >= 0 && Y < (int32)GameState->CurrentLevel.Tilemap.Height)
                            {
                                GameState->CurrentLevel.Tilemap.Data[X][Y] = GameState->CurrentLevel.Tilemap.Tiles[0];
                            }
                        }
                    }
                    break;
                }
            }
            
            // View translation
            if(GameState->InputController.MouseX >= GameState->EditorState.ToolbarX)
            {
                GameState->EditorState.ToolbarScrollOffsetY += (real32)GameState->InputController.ScrollY * GameState->EditorState.ToolbarScrollSpeed * (real32)DeltaTime;
                
                if(GameState->EditorState.ToolbarScrollOffsetY > 0)
                    GameState->EditorState.ToolbarScrollOffsetY = 0.0f;
                else if(GameState->EditorState.ToolbarScrollOffsetY < -((real32)GameState->CurrentLevel.Tilemap.TileCount) * 60)
                    GameState->EditorState.ToolbarScrollOffsetY = -((real32)GameState->CurrentLevel.Tilemap.TileCount) * 60;
            }
            else
            {
                GameState->EditorCamera.Zoom += (real32)GameState->InputController.ScrollY * GameState->EditorState.ZoomingSpeed * (real32)DeltaTime * GameState->EditorCamera.Zoom;
                GameState->EditorCamera.Zoom = Max(Min(GameState->EditorCamera.Zoom, GameState->EditorState.MaxZoom), GameState->EditorState.MinZoom);
            }
            
            if(GetMouseButton(Mouse_Right, GameState))
            {
                if(GameState->EditorState.LastKnownMouseX == 0 && GameState->EditorState.LastKnownMouseY == 0)
                {
                    GameState->EditorState.LastKnownMouseX = (real32)GameState->InputController.MouseX;
                    GameState->EditorState.LastKnownMouseY = (real32)GameState->InputController.MouseY;
                }
                
                glm::vec2 Direction = glm::vec2(GameState->InputController.MouseX - GameState->EditorState.LastKnownMouseX, GameState->InputController.MouseY - GameState->EditorState.LastKnownMouseY);
                
                GameState->EditorCamera.Center -= glm::vec2(Direction.x / GameState->EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime, Direction.y / GameState->EditorCamera.Zoom * -GameState->EditorState.PanningSpeed * DeltaTime);
                
                GameState->EditorState.LastKnownMouseX = (real32)GameState->InputController.MouseX;
                GameState->EditorState.LastKnownMouseY = (real32)GameState->InputController.MouseY;
            }
            else
            {
                GameState->EditorState.LastKnownMouseX = 0;
                GameState->EditorState.LastKnownMouseY = 0;
            }
        }
        break;
        case Editor_Animation:
        {
            GameState->EditorState.CreateNewLevelButton->Active = false;
            GameState->EditorState.SaveAnimationButton->Active = true;
            GameState->EditorState.CreateNewAnimationButton->Active = true;
            
            if(GameState->EditorState.SaveAnimationButton->Clicked && GameState->EditorState.LoadedAnimation)
            {
                GameState->EditorState.LoadedAnimation->Loop = GameState->EditorState.ShouldLoop;
                SaveAnimationToFile(GameState, *GameState->EditorState.LoadedAnimation);
                GameState->EditorState.LoadedAnimation = 0;
            }
            
            if(GameState->EditorState.CreateNewAnimationButton->Clicked)
            {
                GameState->EditorState.Editing = false;
                GameState->EditorState.LoadedAnimation = (animation*)malloc(sizeof(animation));
                
                GameState->EditorState.LoadedAnimation->Name = (char*) calloc(30, sizeof(char));
                GameState->EditorState.LoadedAnimation->FrameCount = 0;
                GameState->EditorState.LoadedAnimation->FrameSize = glm::vec2(0, 0);
                GameState->EditorState.LoadedAnimation->FrameOffset = glm::vec2(0, 0);
                GameState->EditorState.LoadedAnimation->Loop = 1;
                GameState->EditorState.LoadedAnimation->Texture = GameState->RenderState.Textures.begin()->second;
            }
            
            if(GameState->EditorState.LoadedAnimation)
            {
                animation* LoadedAnimation = GameState->EditorState.LoadedAnimation;
                
                if(!GameState->EditorState.AnimationNameField ||!GameState->EditorState.AnimationNameField->Active)
                {
                    InitEditorFields(GameState);
                    sprintf(GameState->EditorState.AnimationNameField->Text, "%s", LoadedAnimation->Name);
                    sprintf(GameState->EditorState.AnimationFrameCountField->Text, "%d", LoadedAnimation->FrameCount);
                    sprintf(GameState->EditorState.AnimationFrameWidthField->Text, "%d", (int32)LoadedAnimation->FrameSize.x);
                    sprintf(GameState->EditorState.AnimationFrameHeightField->Text, "%d", (int32)LoadedAnimation->FrameSize.y);
                    sprintf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%d", (int32)LoadedAnimation->FrameOffset.x);
                    sprintf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%d", (int32)LoadedAnimation->FrameOffset.y);
                    sprintf(GameState->EditorState.AnimationFrameDurationField->Text, "%3.4f", LoadedAnimation->TimePerFrame);
                    GameState->EditorState.AnimationLoopCheckbox->Checked = GameState->EditorState.ShouldLoop;
                }
                
                sscanf(GameState->EditorState.AnimationNameField->Text, "%s", LoadedAnimation->Name);
                sscanf(GameState->EditorState.AnimationFrameCountField->Text, "%d", &LoadedAnimation->FrameCount);
                sscanf(GameState->EditorState.AnimationFrameWidthField->Text, "%f", &LoadedAnimation->FrameSize.x);
                sscanf(GameState->EditorState.AnimationFrameHeightField->Text, "%f", &LoadedAnimation->FrameSize.y);
                sscanf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%f", &LoadedAnimation->FrameOffset.x);
                sscanf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%f", &LoadedAnimation->FrameOffset.y);
                sscanf(GameState->EditorState.AnimationFrameDurationField->Text, "%f", &LoadedAnimation->TimePerFrame);
                GameState->EditorState.ShouldLoop =
                    GameState->EditorState.AnimationLoopCheckbox->Checked; 
                
                SetFieldValues(GameState);
                
                if(LoadedAnimation->Frames)
                {
                    //free(LoadedAnimation->Frames);
                }
                
                if(LoadedAnimation->FrameCount > 0)
                {
                    LoadedAnimation->Frames = (sprite_sheet_frame*)malloc(LoadedAnimation->FrameCount * sizeof(sprite_sheet_frame));
                    
                    int32 X = 0;
                    int32 Y = 0;
                    
                    int32 FrameIndex = 0;
                    
                    while(FrameIndex < (int32)LoadedAnimation->FrameCount)
                    {
                        if(FrameIndex > 0)
                        {
                            if(X + (int32)LoadedAnimation->FrameSize.x < LoadedAnimation->Texture->Width)
                                X += (int32)LoadedAnimation->FrameSize.x;
                            else
                            {
                                X = 0;
                                Y += (int32)LoadedAnimation->FrameSize.y;
                            }
                        }
                        
                        LoadedAnimation->Frames[FrameIndex] = { (real32)X, (real32)Y };
                        FrameIndex++;
                    }
                }
                
                TickAnimation(&GameState->EditorState.AnimationInfo, LoadedAnimation, DeltaTime);
            }
        }
        break;
    }
}

extern "C" UPDATE(Update)
{
    CheckConsoleInput(GameState, DeltaTime);
    
    if(GameState->GameMode == Mode_Editor)
        CheckEditorUIInput(GameState, DeltaTime);
    
    if((GetKey(Key_LeftCtrl, GameState) || GetKey(Key_RightCtrl, GameState)) && GetKeyDown(Key_E, GameState))
    {
        if(GameState->GameMode == Mode_InGame)
        {
            ReloadCurrentLevel(GameState);
            GameState->GameMode = Mode_Editor;
            GameState->Paused = false;
            GameState->EditorCamera.Center = GameState->GameCamera.Center;
        }
        else
        {
            SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
            ReloadCurrentLevel(GameState);
            GameState->GameMode = Mode_InGame;
            if(GameState->EditorState.PlacementEntity)
            {
                DeleteEntity(GameState,GameState->EditorState.PlacementEntity->EntityIndex);
                GameState->EditorState.PlacementEntity = 0;
            }
        }
    }
    
    if(!GameState->EditorState.Loaded)
        CreateEditorButtons(GameState);
    
    if(!GameState->IsInitialized)
    {
        if(!GameState->ShouldReload)
        {
            srand(time(NULL));
            
            LoadAnimations(GameState);
            InitCommands();
            
            GameState->EditorCamera.Zoom = 3.0f; // @Cleanup: We might not want to reset these values every time we load a level
            GameState->EditorCamera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
            GameState->EditorCamera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
            
            GameState->GameMode = Mode_InGame;
        }
        
        LoadLevelFromFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
        
        GameState->GameCamera.Zoom = 2.5f;
        GameState->GameCamera.ViewportWidth = GameState->RenderState.WindowWidth / 20;
        GameState->GameCamera.ViewportHeight = GameState->RenderState.WindowHeight / 20;
        GameState->GameCamera.ScreenShakeTimer = (timer*)malloc(sizeof(timer));
        GameState->GameCamera.ScreenShakeTimer->TimerHandle = -1;
        GameState->GameCamera.ScreenShakeTimer->TimerMax = 0.2f;
        GameState->GameCamera.FollowSpeed = 22.0f; 
        GameState->GameCamera.FadingSpeed = 0.6f;
        StartFade(GameState->GameCamera, Fading_In, 0.6f, glm::vec3(0, 0, 0), 1.0f, 0.0f);
        
        // @Incomplete: This is not the right value, it is only set so high to remove smooth following as of now, since it needs to be done a little differently
        
        GameState->GameCamera.Center = GameState->Entities[0].Position; // Set center to player's position!
        
        GameState->IsInitialized = true;
        GameState->ShouldReload = false;
    }
    
#ifdef DEBUG
    if(GetKeyDown(Key_F1, GameState))
    {
        GameState->RenderState.RenderColliders = !GameState->RenderState.RenderColliders;
    }
    
    if(GetKeyDown(Key_F2, GameState))
    {
        GameState->RenderState.RenderFPS = !GameState->RenderState.RenderFPS;
    }
    
    
    if(GetKeyDown(Key_F3, GameState))
    {
        ToggleMuteSound(GameState);
    }
    
    if(GetKeyDown(Key_F4, GameState))
    {
        GameState->RenderState.RenderPaths = !GameState->RenderState.RenderPaths;
    }
    
    if(GetKeyDown(Key_F5, GameState))
    {
        TogglePauseSound(GameState);
    }
    
    
    if(GetKeyDown(Key_F6, GameState))
    {
        StopSound(GameState);
    }
    
    
    if(GameState->GameMode == Mode_InGame && GetKey(Key_LeftCtrl, GameState) && GetKeyDown(Key_P, GameState))
    {
        GameState->Paused = !GameState->Paused;
    }
    
#endif
    
    if (GetKeyDown(Key_Escape, GameState) && !GameState->Console.Open)
    {
        switch(GameState->GameMode)
        {
            case Mode_MainMenu:
            {
                //StopSoundEffect(GameState, &GameState->SoundManager.MainMenuTrack);
                GameState->GameMode = Mode_InGame;
            }
            break;
            case Mode_InGame:
            {
                //PlaySoundEffect(GameState, &GameState->SoundManager.MainMenuTrack);
                GameState->GameMode = Mode_MainMenu;
            }
            break;
        }
    }
    
    if(GameState->GameMode == Mode_MainMenu)
    {
        if(GetKeyDown(Key_Up, GameState))
            GameState->MainMenu.SelectedIndex -= 1;
        else if(GetKeyDown(Key_Down, GameState))
            GameState->MainMenu.SelectedIndex += 1;
        
        if(GameState->MainMenu.SelectedIndex < 0)
            GameState->MainMenu.SelectedIndex = GameState->MainMenu.OptionCount - 1;
        else if(GameState->MainMenu.SelectedIndex == (int32)GameState->MainMenu.OptionCount)
            GameState->MainMenu.SelectedIndex = 0;
        
        if(GetKeyDown(Key_Enter, GameState))
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
            GameState->GameCamera.FadingAlpha -= GameState->GameCamera.FadingSpeed * (real32)DeltaTime;
            
            if(GameState->GameCamera.FadingAlpha <= 0.0f)
            {
                GameState->GameCamera.FadingAlpha = 0.0f;
                GameState->GameCamera.FadingMode = Fading_None;
            }
        }
        break;
        case Fading_Out:
        {
            GameState->GameCamera.FadingAlpha += GameState->GameCamera.FadingSpeed * (real32)DeltaTime;
            
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
                GameState->GameCamera.FadingAlpha -= GameState->GameCamera.FadingSpeed * (real32)DeltaTime;
                
                if(GameState->GameCamera.FadingAlpha <= 0.0f)
                {
                    GameState->GameCamera.FadingAlpha = 0.0f;
                    GameState->GameCamera.FadingMode = Fading_None;
                    GameState->GameCamera.FadingIn = false;
                }
            }
            else
            {
                GameState->GameCamera.FadingAlpha += GameState->GameCamera.FadingSpeed * (real32)DeltaTime;
                
                if(GameState->GameCamera.FadingAlpha >= GameState->GameCamera.EndAlpha)
                {
                    GameState->GameCamera.FadingAlpha = GameState->GameCamera.EndAlpha;
                    GameState->GameCamera.FadingIn = true;
                }
            }
        }
        break;
    }
    
    glm::vec2 Center = GameState->Camera.Center;
    
    switch(GameState->GameMode)
    {
        case Mode_InGame:
        case Mode_MainMenu:
        {
            if(!GameState->Paused)
            {
                UpdateEntities(GameState, DeltaTime);
                TickTimers(GameState, DeltaTime);
                
                if(!TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
                {
                    real32 Radius = 0.05f;
                    int32 RandomAngle = rand() % 360;
                    glm::vec2 Offset = glm::vec2(sin(RandomAngle) * Radius, cos(RandomAngle) * Radius);
                    Center.x += Offset.x / 1.5f;
                    Center.y += Offset.y;
                }
                
                if(glm::distance(GameState->GameCamera.CenterTarget, Center) > 0.01f)
                {
                    auto Direction = glm::normalize(GameState->GameCamera.CenterTarget - Center);
                    Center = glm::vec2(Center.x + Direction.x * GameState->GameCamera.FollowSpeed * DeltaTime, Center.y + Direction.y  * GameState->GameCamera.FollowSpeed * DeltaTime);
                    GameState->GameCamera.Center = Center;
                }
            }
            
            if(GameState->PlayerState == Player_Alive && GameState->Entities[0].Dead)
            {
                GameState->PlayerState = Player_Dead;
                StartTimer(GameState, GameState->DeathScreenTimer);
            }
            
            if(GameState->PlayerState == Player_Dead)
            {
                if(TimerDone(GameState, GameState->DeathScreenTimer) && GameState->InputController.AnyKeyPressed)
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
            EditorUpdateEntities(GameState, DeltaTime);
            GameState->Camera = GameState->EditorCamera;
            TickTimers(GameState, DeltaTime); // @Cleanup: I don't remember why this has to be called individually for each mode. It might be okay to call it at the end of the Update-function
        }
        break;
    }
    
    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f,
                                                    static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom),
                                                    0.0f,
                                                    static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom),
                                                    -1.0f,
                                                    1.0f);
    
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f),
                                                  glm::vec3(-Center.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2,
                                                            -Center.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2,
                                                            0));
    GameState->InputController.CurrentCharacter = 0;
}