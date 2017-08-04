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

static void EditorUpdateEntities(game_state* GameState, input_controller* InputController, sound_queue* SoundQueue, r64 DeltaTime)
{
    if(KEY_DOWN(Key_Escape))
    {
        if(GameState->EditorState.AnimationMode == Animation_Edit || GameState->EditorState.AnimationMode == Animation_Create ||GameState->EditorState.AnimationMode == Animation_SelectTexture)
        {
            GameState->EditorState.AnimationMode = Animation_SelectAnimation;
            ToggleAnimationFields(&GameState->EditorState, false);
            GameState->EditorState.CreateNewAnimationButton->Active = true;
        }
        else
        {
            GameState->EditorState.MenuOpen = !GameState->EditorState.MenuOpen;
        }
    }
    
    if(KEY_DOWN(Key_Enter) && GameState->EditorState.FocusedTextfield)
    {
        GameState->EditorState.Textfields[GameState->EditorState.FocusedTextfield].InFocus = false;
        GameState->EditorState.FocusedTextfield = 0;
    }
    
    if(GameState->EditorState.MenuOpen)
    {
        if(KEY_DOWN(Key_Up))
        {
            GameState->EditorState.SelectedMenuOption++;
            
            if(GameState->EditorState.SelectedMenuOption == 3)
                GameState->EditorState.SelectedMenuOption = 0;
        }
        else if(KEY_DOWN(Key_Down))
        {
            
            GameState->EditorState.SelectedMenuOption--;
            if(GameState->EditorState.SelectedMenuOption == -1)
                GameState->EditorState.SelectedMenuOption = 2;
        }
        
        if(KEY_DOWN(Key_Enter))
        {
            GameState->EditorState.MenuOpen = false;
            
            switch((Editor_Menu_Option)GameState->EditorState.SelectedMenuOption)
            {
                case Editor_Menu_Animations:
                {
                    GameState->EditorState.Mode = Editor_Animation;
                    
                    GameState->EditorState.CreateNewAnimationButton->Active = true;
                    GameState->EditorState.TileIsSolidCheckbox->Active = false;
                    GameState->EditorState.Mode = Editor_Animation;
                    GameState->EditorState.Editing = true;
                }
                break;
                case Editor_Menu_Level:
                {
                    ToggleAnimationFields(&GameState->EditorState, false);
                    ReloadCurrentLevel(GameState);
                    GameState->GameMode = Mode_Editor;
                    GameState->Paused = false;
                    GameState->EditorCamera.Center = GameState->GameCamera.Center;
                }
                break;
                case Editor_Menu_Game:
                {
                    ToggleAnimationFields(&GameState->EditorState, false);
                    if(GameState->EditorState.Mode == Editor_Level)
                    {
                        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                    }
                    
                    ReloadCurrentLevel(GameState);
                    GameState->GameMode = Mode_InGame;
                }
                break;
            }
        }
    }
    else
    {
        b32 InToolbar = GameState->RenderState.WindowHeight - InputController->MouseY >= GameState->RenderState.WindowHeight;
        
        for(u32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
        {
            button* Button = &GameState->EditorState.Buttons[ButtonIndex];
            Button->Clicked = false;
            
            if(MOUSE_DOWN(Mouse_Left))
            {
                if(Button->Active && InputController->MouseX >= Button->ScreenPosition.x && InputController->MouseX <= Button->ScreenPosition.x + Button->Size.x && 
                   GameState->RenderState.WindowHeight - InputController->MouseY >= Button->ScreenPosition.y && GameState->RenderState.WindowHeight - InputController->MouseY <= Button->ScreenPosition.y + Button->Size.y)
                {
                    Button->Clicked = true;
                    PLAY_SOUND(ButtonClick);
                    
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
                            GameState->EditorState.CreateNewAnimationButton->Active = true;
                            GameState->EditorState.TileIsSolidCheckbox->Active = false;
                            GameState->EditorState.Mode = Editor_Animation;
                            GameState->EditorState.SelectedTexture = 0;
                            GameState->EditorState.SelectedAnimation = 0;
                            GameState->EditorState.Editing = true;
                        }
                        break;
                        case Button_SwitchMode:
                        {
                            if(GameState->EditorState.PlacementMode == Editor_Placement_SelectEntity)
                                GameState->EditorState.PlacementMode = Editor_Placement_PlaceEntity;
                            else if(GameState->EditorState.PlacementMode == Editor_Placement_PlaceEntity)
                                GameState->EditorState.PlacementMode = Editor_Placement_Tile;
                            else
                                GameState->EditorState.PlacementMode = Editor_Placement_SelectEntity;
                        }
                        break;
                    }
                }
            }
            
            if(Button->Active)
            {
                if(!TimerDone(GameState, Button->ClickAnimationTimer))
                    Button->Color = math::v4(0.5, 0.5, 0.5, 1);
                else
                    Button->Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
            }
        }
        
        for(u32 Index = 0; Index < 10; Index++)
        {
            checkbox* Checkbox = &GameState->EditorState.Checkboxes[Index];
            Checkbox->JustChecked = false;
            
            if(MOUSE_DOWN(Mouse_Left))
            {
                if(Checkbox->Active && InputController->MouseX >= Checkbox->ScreenPosition.x && InputController->MouseX <= Checkbox->ScreenPosition.x + 25 && GameState->RenderState.WindowHeight - InputController->MouseY >= Checkbox->ScreenPosition.y && GameState->RenderState.WindowHeight - InputController->MouseY <= Checkbox->ScreenPosition.y + 25)
                {
                    Checkbox->Checked = !Checkbox->Checked;
                    Checkbox->JustChecked = true;
                }
            }
        }
        
        if(GameState->EditorState.TileIsSolidCheckbox->JustChecked)
        {
            u32 SelectedTile = GameState->EditorState.SelectedTileType;
            GameState->CurrentLevel.Tilemap.Tiles[SelectedTile].IsSolid = GameState->EditorState.TileIsSolidCheckbox->Checked;
            UpdateTileData(SelectedTile, GameState->CurrentLevel.Tilemap.Tiles[SelectedTile].IsSolid, &GameState->CurrentLevel.Tilemap);
            SaveTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", GameState->CurrentLevel.SheetName), ".tm"), &GameState->RenderState, GameState->CurrentLevel, false);
            
            // @Incomplete: Should call SaveTilesheetMetafile!!!!
        }
        
        i32 FocusedField = GameState->EditorState.FocusedTextfield;
        
        for(i32 Index = 0; Index < 20; Index++)
        {
            textfield* Textfield = &GameState->EditorState.Textfields[Index];
            Textfield->InFocus = false;
            
            if(MOUSE_DOWN(Mouse_Left))
            {
                if(Textfield->Active && InputController->MouseX >= Textfield->ScreenPosition.x && InputController->MouseX <= Textfield->ScreenPosition.x + Textfield->Size.x && 
                   GameState->RenderState.WindowHeight - InputController->MouseY >= Textfield->ScreenPosition.y && GameState->RenderState.WindowHeight - InputController->MouseY <= Textfield->ScreenPosition.y + Textfield->Size.y)
                {
                    GameState->EditorState.FocusedTextfield = Index;
                    Textfield->InFocus = true;
                }
            }
            
            if(Index == GameState->EditorState.FocusedTextfield)
            {
                if(InputController->DeleteCharacter && Textfield->TextIndex != 0)
                {
                    Textfield->Text[--Textfield->TextIndex] = 0;
                }
                
                if(InputController->CurrentCharacter && InputController->CurrentCharacter != ' ')
                {
                    b32 CanEnter = true;
                    
                    switch(Textfield->Type)
                    {
                        case Textfield_Normal:
                        {}
                        break;
                        case Textfield_Integer:
                        {
                            CanEnter = InputController->CurrentCharacter == '0' ||
                                InputController->CurrentCharacter == '1' ||
                                InputController->CurrentCharacter == '2' ||
                                InputController->CurrentCharacter == '3' ||
                                InputController->CurrentCharacter == '4' ||
                                InputController->CurrentCharacter == '5' ||
                                InputController->CurrentCharacter == '6' ||
                                InputController->CurrentCharacter == '7' ||
                                InputController->CurrentCharacter == '9';
                        }
                        break;
                        case Textfield_Decimal:
                        {
                            CanEnter = InputController->CurrentCharacter == '0' ||
                                InputController->CurrentCharacter == '1' ||
                                InputController->CurrentCharacter == '2' ||
                                InputController->CurrentCharacter == '3' ||
                                InputController->CurrentCharacter == '4' ||
                                InputController->CurrentCharacter == '5' ||
                                InputController->CurrentCharacter == '6' ||
                                InputController->CurrentCharacter == '7' ||
                                InputController->CurrentCharacter == '8' ||
                                InputController->CurrentCharacter == '9' ||
                                InputController->CurrentCharacter == '.';
                        }
                        break;
                    }
                    
                    if(CanEnter && Textfield->TextIndex != TEXTFIELD_LENGTH)
                    {
                        Textfield->Text[Textfield->TextIndex++] = InputController->CurrentCharacter;
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
            case Editor_Level:
            {
                if(KEY_DOWN(Key_S))
                {
                    SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                }
                
                if(GameState->EditorState.CreateNewLevelButton->Clicked)
                {
                    level Level;
                    Level.SheetName = "overworld1";
                    Level.Name = "test_level";
                    
                    CreateNewLevelWithSize("../assets/levels/level_new.plv", 200, 120, &Level, GameState, SoundQueue);
                    GameState->CurrentLevel = Level;
                }
                
                auto Pos = math::UnProject(math::v3(InputController->MouseX, GameState->RenderState.Viewport[3] - InputController->MouseY, 0),
                                           GameState->Camera.ViewMatrix,
                                           GameState->Camera.ProjectionMatrix,
                                           math::v4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
                
                DebugPrintVec2(math::v2(Pos.x,Pos.y));
                
                GameState->EditorState.CreateNewAnimationButton->Active = false;
                
                if(KEY_DOWN(Key_E))
                {
                    if(GameState->EditorState.PlacementMode == Editor_Placement_PlaceEntity)
                    {
                        GameState->EditorState.PlacementMode = Editor_Placement_SelectEntity;
                    }
                    else
                    {
                        GameState->EditorState.PlacementMode = Editor_Placement_PlaceEntity;
                    }
                }
                
                if(KEY_DOWN(Key_T))
                {
                    GameState->EditorState.PlacementMode = Editor_Placement_Tile;
                }
                
                GameState->EditorState.TileBrushWidthField->Active = false;
                GameState->EditorState.TileBrushHeightField->Active = false;
                
                if(!InToolbar)
                {
                    switch(GameState->EditorState.PlacementMode)
                    {
                        case Editor_Placement_SelectEntity:
                        {
                            if(GameState->EditorState.SelectedEntity 
                               && GameState->EditorState.SelectedEntity->Type == Entity_Enemy 
                               && KEY_DOWN(Key_P))
                            {
                                GameState->EditorState.IsInCreateWaypointMode = !GameState->EditorState.IsInCreateWaypointMode;
                                
                                if(GameState->EditorState.IsInCreateWaypointMode)
                                {
                                    GameState->EditorState.SelectedEntity->Enemy.WaypointCount = 0;
                                }
                            }
                            
                            if(GameState->EditorState.IsInCreateWaypointMode)
                            {
                                if(MOUSE_DOWN(Mouse_Left) && GameState->EditorState.SelectedEntity->Enemy.WaypointCount < 10)
                                {
                                    
                                    math::v2 NewPos = ToCartesian(math::v2(Pos.x, Pos.y));
                                    
                                    i32 X = (i32)math::Floor(NewPos.x / 0.5f);
                                    i32 Y = (i32)math::Floor(NewPos.y / 0.5f);
                                    
                                    if(!GameState->CurrentLevel.Tilemap.Data[1][X][Y].IsSolid)
                                    {
                                        GameState->EditorState.SelectedEntity->Enemy.Waypoints[GameState->EditorState.SelectedEntity->Enemy.WaypointCount++] = math::v2i(X,Y);
                                    }
                                }
                            }
                            else
                            {
                                math::v2 NewPos = ToCartesian(math::v2(Pos.x, Pos.y));
                                
                                NewPos.x /= 0.5f;
                                NewPos.y /= 0.5f;
                                
                                math::v2 ExactPos = NewPos;
                                
                                NewPos.x = (r32)math::Floor(NewPos.x);
                                NewPos.y = (r32)math::Floor(NewPos.y);
                                
                                if(MOUSE_DOWN(Mouse_Left))
                                {
                                    entity* Selected = 0;
                                    
                                    for(u32 EntityIndex = 0;
                                        EntityIndex < GameState->EntityCount;
                                        EntityIndex++)
                                    {
                                        entity* Entity = &GameState->Entities[EntityIndex];
                                        
                                        if(Entity->Type != Entity_Weapon && NewPos.x >= Entity->Position.x - Entity->Scale && NewPos.y < Entity->Position.y + Entity->Scale && NewPos.x < Entity->Position.x + Entity->Scale && NewPos.y > Entity->Position.y)
                                        {
                                            Selected = Entity;
                                            GameState->EditorState.CurrentSelectedEntityOffset = Entity->Position - ExactPos;
                                            break;
                                        }
                                    }
                                    
                                    GameState->EditorState.SelectedEntity = Selected;
                                }
                                
                                if(GameState->EditorState.SelectedEntity && MOUSE(Mouse_Left))
                                {
                                    GameState->EditorState.SelectedEntity->Position = math::v2(NewPos.x + GameState->EditorState.CurrentSelectedEntityOffset.x, NewPos.y - GameState->EditorState.SelectedEntity->Scale / 2 + GameState->EditorState.CurrentSelectedEntityOffset.y);
                                }
                            }
                        }
                        break;
                        case Editor_Placement_PlaceEntity:
                        {
                            if(MOUSE_DOWN(Mouse_Left))
                            {
                                math::v2 NewPos = ToCartesian(math::v2(Pos.x, Pos.y));
                                
                                NewPos.x = floor(NewPos.x / 0.5f);
                                NewPos.y = floor(NewPos.y / 0.5f);
                                
                                switch(GameState->EditorState.PlacementEntity)
                                {
                                    case Placement_Entity_Skeleton:
                                    {
                                        LoadSkeletonData(GameState, -1, math::v2(NewPos.x, NewPos.y - 0.5f));
                                    }
                                    break;
                                    case Placement_Entity_Blob:
                                    {
                                        LoadBlobData(GameState, -1, math::v2(NewPos.x, NewPos.y - 0.5f));
                                    }
                                    break;
                                    case Placement_Entity_Wraith:
                                    {
                                        LoadWraithData(GameState, -1, math::v2(NewPos.x, NewPos.y - 0.5));
                                    }
                                    break;
                                    case Placement_Entity_Minotaur:
                                    {
                                        LoadMinotaurData(GameState, -1, math::v2(NewPos.x, NewPos.y - 0.5f));
                                    }
                                    break;
                                    case Placement_Entity_Barrel:
                                    {}
                                    break;
                                    case Placement_Entity_Bonfire:
                                    {
                                        LoadBonfireData(GameState, SoundQueue, -1, math::v2(NewPos.x, NewPos.y - 0.5f));
                                    }
                                    break;
                                }
                                
                                SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                            }
                        }
                        break;
                        case Editor_Placement_Tile:
                        {
                            GameState->EditorState.TileBrushWidthField->Active = true;
                            GameState->EditorState.TileBrushHeightField->Active = true;
                            
                            sscanf(GameState->EditorState.TileBrushWidthField->Text, "%f", &GameState->EditorState.TileBrushSize.x);
                            sscanf(GameState->EditorState.TileBrushHeightField->Text, "%f", &GameState->EditorState.TileBrushSize.y);
                            
                            GameState->EditorState.TileIsSolidCheckbox->Active = true;
                            
                            math::v2 NewPos = ToCartesian(math::v2(Pos.x, Pos.y));
                            
                            i32 X = (i32)floor(NewPos.x / 0.5f);
                            i32 Y = (i32)floor(NewPos.y / 0.5f);
                            
                            GameState->EditorState.TileX = (r32)X;
                            GameState->EditorState.TileY = (r32)Y;
                            
                            if(MOUSE(Mouse_Left))
                            {
                                i32 TilesheetWidth = GameState->CurrentLevel.Tilemap.RenderEntity.Texture->Width / GameState->CurrentLevel.Tilemap.TileWidth;
                                i32 TilesheetHeight = GameState->CurrentLevel.Tilemap.RenderEntity.Texture->Height / GameState->CurrentLevel.Tilemap.TileHeight;
                                
                                r32 ToolbarX = GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x;
                                r32 ToolbarY = GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y;
                                
                                if(InputController->MouseX >= ToolbarX && InputController->MouseX <= ToolbarX + TilesheetWidth * GameState->EditorState.RenderedTileSize && GameState->RenderState.WindowHeight - InputController->MouseY > ToolbarY && GameState->RenderState.WindowHeight - InputController->MouseY <=  ToolbarY + TilesheetHeight * GameState->EditorState.RenderedTileSize)
                                {
                                    i32 X = (i32)((InputController->MouseX - (GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x)) / GameState->EditorState.RenderedTileSize);
                                    i32 Y = (i32)((GameState->RenderState.WindowHeight - InputController->MouseY - (GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y)) / GameState->EditorState.RenderedTileSize);
                                    
                                    GameState->EditorState.SelectedTilePosition = math::v2((r32)X, (r32)Y);
                                    i32 Selected = X + Y * TilesheetWidth;
                                    
                                    GameState->EditorState.SelectedTileType = Selected;
                                    GameState->EditorState.TileIsSolidCheckbox->Checked = GameState->CurrentLevel.Tilemap.Tiles[Selected].IsSolid;
                                }
                                else
                                {
                                    if(X >= 0 && X < (i32)GameState->CurrentLevel.Tilemap.Width 
                                       && Y >= 0 && Y < (i32)GameState->CurrentLevel.Tilemap.Height)
                                    {
                                        tilemap* Tilemap = &GameState->CurrentLevel.Tilemap;
                                        
                                        if(KEY(Key_F))
                                        {
                                            for(i32 IndexX = 0; IndexX < Tilemap->Width; IndexX++)
                                            {
                                                for(i32 IndexY = 0; IndexY < Tilemap->Height; IndexY++)
                                                {
                                                    for(i32 X = 0; X < (i32)GameState->EditorState.TileBrushSize.x && X + IndexX < Tilemap->Width; X++)
                                                    {
                                                        for(i32 Y = 0; Y < (i32)GameState->EditorState.TileBrushSize.y && Y + IndexY < Tilemap->Height; Y++)
                                                        {
                                                            Tilemap->Data[GameState->EditorState.CurrentTilemapLayer][IndexX + X][IndexY + Y] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                                        }
                                                    }
                                                    
                                                    Tilemap->RenderInfo.Dirty = true;
                                                }
                                            }
                                        }
                                        else
                                        {
                                            
                                            for(i32 IndexY = 0; IndexY < Tilemap->Height; IndexY++)
                                            {
                                                for(i32 RelativeX = 0; RelativeX < GameState->EditorState.TileBrushSize.x && RelativeX + X < Tilemap->Width; RelativeX++)
                                                {
                                                    for(i32 RelativeY = 0; RelativeY < GameState->EditorState.TileBrushSize.y && RelativeY + Y < Tilemap->Height; RelativeY++)
                                                    {
                                                        Tilemap->Data[GameState->EditorState.CurrentTilemapLayer][X + RelativeX][Y + RelativeY] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                                    }
                                                }
                                            }
                                            
                                            Tilemap->RenderInfo.DirtyLayer = GameState->EditorState.CurrentTilemapLayer;
                                            Tilemap->RenderInfo.Dirty = true;
                                        }
                                    }
                                }
                            }
                            if(MOUSE(Mouse_Left) && KEY(Key_LeftShift))
                            {
                                if(X >= 0 && X < (i32)GameState->CurrentLevel.Tilemap.Width && Y >= 0 && Y < (i32)GameState->CurrentLevel.Tilemap.Height)
                                {
                                    GameState->CurrentLevel.Tilemap.Data[GameState->EditorState.CurrentTilemapLayer][X][Y].TypeIndex = -1;
                                }
                            }
                        }
                        break;
                    }
                }
                
                // View translation
                if(InputController->MouseX >= GameState->EditorState.ToolbarX && InputController->MouseX < GameState->EditorState.ToolbarX + GameState->EditorState.ToolbarWidth)
                {
                    GameState->EditorState.ToolbarScrollOffsetY += (r32)InputController->ScrollY * GameState->EditorState.ToolbarScrollSpeed * (r32)DeltaTime;
                    
                    if(GameState->EditorState.ToolbarScrollOffsetY > 0)
                        GameState->EditorState.ToolbarScrollOffsetY = 0.0f;
                    else if(GameState->EditorState.ToolbarScrollOffsetY < -((r32)GameState->CurrentLevel.Tilemap.TileCount) * 60)
                        GameState->EditorState.ToolbarScrollOffsetY = -((r32)GameState->CurrentLevel.Tilemap.TileCount) * 60;
                }
                else
                {
                    GameState->EditorCamera.Zoom += (r32)InputController->ScrollY * GameState->EditorState.ZoomingSpeed * (r32)DeltaTime * GameState->EditorCamera.Zoom;
                    GameState->EditorCamera.Zoom = Max(Min(GameState->EditorCamera.Zoom, GameState->EditorState.MaxZoom), GameState->EditorState.MinZoom);
                }
                
                if(MOUSE(Mouse_Right))
                {
                    if(GameState->EditorState.LastKnownMouseX == 0 && GameState->EditorState.LastKnownMouseY == 0)
                    {
                        GameState->EditorState.LastKnownMouseX = (r32)InputController->MouseX;
                        GameState->EditorState.LastKnownMouseY = (r32)InputController->MouseY;
                    }
                    
                    math::v2 Direction = math::v2(InputController->MouseX - GameState->EditorState.LastKnownMouseX, InputController->MouseY - GameState->EditorState.LastKnownMouseY);
                    
                    GameState->EditorCamera.Center -= math::v2(Direction.x / GameState->EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime, Direction.y / GameState->EditorCamera.Zoom * -GameState->EditorState.PanningSpeed * DeltaTime);
                    
                    GameState->EditorState.LastKnownMouseX = (r32)InputController->MouseX;
                    GameState->EditorState.LastKnownMouseY = (r32)InputController->MouseY;
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
                GameState->EditorState.CreateNewAnimationButton->Active = true;
                
                switch(GameState->EditorState.AnimationMode)
                {
                    case Animation_SelectAnimation:
                    {
                        if(GameState->EditorState.CreateNewAnimationButton->Clicked)
                        {
                            GameState->EditorState.AnimationMode = Animation_SelectTexture;
                            GameState->EditorState.LoadedAnimation = 0;
                        }
                        
                        if(KEY_DOWN(Key_Enter))
                        {
                            GameState->EditorState.AnimationMode = Animation_Edit;
                            ToggleAnimationFields(&GameState->EditorState, true);
                        }
                        
                    }
                    break;
                    case Animation_Edit:
                    {
                    }
                    break;
                    case Animation_SelectTexture:
                    {
                        if(KEY_DOWN(Key_Enter))
                        {
                            GameState->EditorState.AnimationMode = Animation_Create;
                            
                            GameState->EditorState.LoadedAnimation = (animation*)malloc(sizeof(animation));
                            
                            GameState->EditorState.LoadedAnimation->Name = (char*) calloc(30, sizeof(char));
                            GameState->EditorState.LoadedAnimation->FrameCount = 0;
                            GameState->EditorState.LoadedAnimation->FrameSize = math::v2(0, 0);
                            GameState->EditorState.LoadedAnimation->FrameOffset = math::v2(0, 0);
                            GameState->EditorState.LoadedAnimation->TimePerFrame = 0.0f;
                            GameState->EditorState.LoadedAnimation->Loop = 1;
                            GameState->EditorState.LoadedAnimation->Texture = &GameState->RenderState.TextureArray[GameState->EditorState.SelectedTexture];
                            
                            for(i32 Index = 0; Index < TEXTFIELD_LENGTH; Index++)
                            {
                                GameState->EditorState.AnimationNameField->Text[Index] = 0;
                            }
                            
                            ToggleAnimationFields(&GameState->EditorState, true);
                        }
                    }
                    break;
                    case Animation_Create:
                    {
                        if(GameState->EditorState.SaveAnimationButton->Clicked && GameState->EditorState.LoadedAnimation)
                        {
                            ToggleAnimationFields(&GameState->EditorState, false);
                            GameState->EditorState.LoadedAnimation->Loop = GameState->EditorState.ShouldLoop;
                            SaveAnimationToFile(GameState, *GameState->EditorState.LoadedAnimation);
                            GameState->EditorState.LoadedAnimation = 0;
                            GameState->EditorState.HasLoadedAnimations = false;
                        }
                    }
                    break;
                }
                
                if(GameState->EditorState.LoadedAnimation)
                {
                    if(GameState->EditorState.AnimationMode == Animation_Edit || GameState->EditorState.AnimationMode == Animation_Create)
                        GameState->EditorState.CreateNewAnimationButton->Active = false;
                    
                    animation* LoadedAnimation = GameState->EditorState.LoadedAnimation;
                    
                    sscanf(GameState->EditorState.AnimationNameField->Text, "%s", LoadedAnimation->Name);
                    sscanf(GameState->EditorState.AnimationFrameCountField->Text, "%d", &LoadedAnimation->FrameCount);
                    sscanf(GameState->EditorState.AnimationFrameWidthField->Text, "%f", &LoadedAnimation->FrameSize.x);
                    sscanf(GameState->EditorState.AnimationFrameHeightField->Text, "%f", &LoadedAnimation->FrameSize.y);
                    sscanf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%f", &LoadedAnimation->FrameOffset.x);
                    sscanf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%f", &LoadedAnimation->FrameOffset.y);
                    sscanf(GameState->EditorState.AnimationFrameDurationField->Text, "%f", &LoadedAnimation->TimePerFrame);
                    GameState->EditorState.ShouldLoop =
                        GameState->EditorState.AnimationLoopCheckbox->Checked; 
                    
                    
                    if(LoadedAnimation->Frames)
                    {
                        //free(LoadedAnimation->Frames);
                    }
                    
                    if(LoadedAnimation->FrameCount > 0)
                    {
                        LoadedAnimation->Frames = (sprite_sheet_frame*)malloc(LoadedAnimation->FrameCount * sizeof(sprite_sheet_frame));
                        
                        i32 X = (i32)LoadedAnimation->FrameSize.x * (i32)LoadedAnimation->FrameOffset.x;
                        i32 Y = (i32)LoadedAnimation->FrameSize.y * (i32)LoadedAnimation->FrameOffset.y;
                        
                        i32 FrameIndex = 0;
                        
                        while(FrameIndex < (i32)LoadedAnimation->FrameCount)
                        {
                            if(FrameIndex > 0)
                            {
                                if(X + (i32)LoadedAnimation->FrameSize.x < LoadedAnimation->Texture->Width)
                                    X += (i32)LoadedAnimation->FrameSize.x;
                                else
                                {
                                    X = 0;
                                    Y += (i32)LoadedAnimation->FrameSize.y;
                                }
                            }
                            
                            LoadedAnimation->Frames[FrameIndex] = { (r32)X, (r32)Y };
                            FrameIndex++;
                        }
                    }
                    
                    TickAnimation(&GameState->EditorState.AnimationInfo, LoadedAnimation, DeltaTime);
                }
                else
                {
                    GameState->EditorState.CreateNewAnimationButton->Active = true;
                }
            }
            break;
        }
        
        SetFieldValues(GameState);
    }
}

extern "C" UPDATE(Update)
{
    game_state* GameState = (game_state*)GameMemory->PermanentStorage;
    Assert(GameState);
    
    if(!GameState->IsInitialized)
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
        
        if(GameState->ShouldReload)
        {
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
        GameState->GameCamera.ScreenShakeTimer.TimerHandle = -1;
        GameState->GameCamera.ScreenShakeTimer.TimerMax = 0.2f;
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
            LoadPlayerData(GameState, 0);
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
                GameState->EditorCamera.Center = GameState->GameCamera.Center;
            }
            else
            {
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
                
                if(!TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
                {
                    r32 Radius = 0.05f;
                    i32 RandomAngle = rand() % 360;
                    math::v2 Offset = math::v2(sin(RandomAngle) * Radius, cos(RandomAngle) * Radius);
                    Center.x += Offset.x / 1.5f;
                    Center.y += Offset.y;
                }
                
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
}

 