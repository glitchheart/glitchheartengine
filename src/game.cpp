 #include "game.h"
#include "gmap.cpp"
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
 
 static void TickTimers(game_state* GameState, r64 DeltaTime)
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
 
 static void EditorUpdateEntities(game_state* GameState, r64 DeltaTime)
 {
     if(GetKeyDown(Key_Escape, GameState))
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
     
     if(GetKeyDown(Key_Enter, GameState) && GameState->EditorState.FocusedTextfield)
     {
         GameState->EditorState.Textfields[GameState->EditorState.FocusedTextfield].InFocus = false;
         GameState->EditorState.FocusedTextfield = 0;
     }
     
     if(GameState->EditorState.MenuOpen)
     {
         if(GetKeyDown(Key_Up, GameState))
         {
             GameState->EditorState.SelectedMenuOption++;
             
             if(GameState->EditorState.SelectedMenuOption == 3)
                 GameState->EditorState.SelectedMenuOption = 0;
         }
         else if(GetKeyDown(Key_Down, GameState))
         {
             
             GameState->EditorState.SelectedMenuOption--;
             if(GameState->EditorState.SelectedMenuOption == -1)
                 GameState->EditorState.SelectedMenuOption = 2;
         }
         
         if(GetKeyDown(Key_Enter, GameState))
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
                     
                     std::map<char const*, texture*>::iterator TextureIterator;
                     
                     if(GameState->EditorState.Textures)
                         free(GameState->EditorState.Textures);
                     
                     GameState->EditorState.Textures = (char const**)malloc(GameState->RenderState.Textures.size() * sizeof(char*));
                     
                     i32 Index = 0;
                     
                     for(TextureIterator = GameState->RenderState.Textures.begin(); TextureIterator != GameState->RenderState.Textures.end(); TextureIterator++)
                     {
                         GameState->EditorState.Textures[Index++] = &*TextureIterator->first;
                     }
                     
                     GameState->EditorState.TexturesLength = (i32)GameState->RenderState.Textures.size();
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
         b32 InToolbar = GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= GameState->RenderState.WindowHeight;
         
         for(u32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
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
                             GameState->EditorState.CreateNewAnimationButton->Active = true;
                             GameState->EditorState.TileIsSolidCheckbox->Active = false;
                             GameState->EditorState.Mode = Editor_Animation;
                             
                             std::map<char const*, texture*>::iterator TextureIterator;
                             
                             if(GameState->EditorState.Textures)
                                 free(GameState->EditorState.Textures);
                             
                             GameState->EditorState.Textures = (char const**)malloc(GameState->RenderState.Textures.size() * sizeof(char*));
                             
                             i32 Index = 0;
                             
                             for(TextureIterator = GameState->RenderState.Textures.begin(); TextureIterator != GameState->RenderState.Textures.end(); TextureIterator++)
                             {
                                 GameState->EditorState.Textures[Index++] = &*TextureIterator->first;
                             }
                             
                             GameState->EditorState.TexturesLength = (i32)GameState->RenderState.Textures.size();
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
                     Button->Color = glm::vec4(0.5, 0.5, 0.5, 1);
                 else
                     Button->Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
             }
         }
         
         for(u32 Index = 0; Index < 10; Index++)
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
             
             if(GetMouseButtonDown(Mouse_Left, GameState))
             {
                 if(Textfield->Active && GameState->InputController.MouseX >= Textfield->ScreenPosition.x && GameState->InputController.MouseX <= Textfield->ScreenPosition.x + Textfield->Size.x && 
                    GameState->RenderState.WindowHeight - GameState->InputController.MouseY >= Textfield->ScreenPosition.y && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <= Textfield->ScreenPosition.y + Textfield->Size.y)
                 {
                     GameState->EditorState.FocusedTextfield = Index;
                     Textfield->InFocus = true;
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
                     b32 CanEnter = true;
                     
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
             case Editor_Level:
             {
                 if(GetKeyDown(Key_S, GameState))
                 {
                     SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
                 }
                 
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
                 
                 if(GetKeyDown(Key_E, GameState))
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
                 
                 if(GetKeyDown(Key_T, GameState))
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
                                && GetKeyDown(Key_P, GameState))
                             {
                                 GameState->EditorState.IsInCreateWaypointMode = !GameState->EditorState.IsInCreateWaypointMode;
                                 
                                 if(GameState->EditorState.IsInCreateWaypointMode)
                                 {
                                     GameState->EditorState.SelectedEntity->Enemy.WaypointCount = 0;
                                 }
                             }
                             
                             if(GameState->EditorState.IsInCreateWaypointMode)
                             {
                                 if(GetMouseButtonDown(Mouse_Left, GameState) && GameState->EditorState.SelectedEntity->Enemy.WaypointCount < 10)
                                 {
                                     auto X = (i32)glm::floor(Pos.x);
                                     auto Y = (i32)glm::floor(Pos.y);
                                     
                                     if(!GameState->CurrentLevel.Tilemap.Data[1][X][Y].IsSolid)
                                     {
                                         GameState->EditorState.SelectedEntity->Enemy.Waypoints[GameState->EditorState.SelectedEntity->Enemy.WaypointCount++] = 
                                         { 
                                             X,
                                             Y
                                         };
                                     }
                                 }
                             }
                             else
                             {
                                 if(GetMouseButtonDown(Mouse_Left, GameState))
                                 {
                                     entity* Selected = 0;
                                     
                                     for(u32 EntityIndex = 0;
                                         EntityIndex < GameState->EntityCount;
                                         EntityIndex++)
                                     {
                                         entity* Entity = &GameState->Entities[EntityIndex];
                                         
                                         if(Entity->Type != Entity_Weapon && Pos.x >= Entity->Position.x - Entity->Scale / 2 && Pos.y < Entity->Position.y + Entity->Scale && Pos.x < Entity->Position.x + Entity->Scale && Pos.y > Entity->Position.y)
                                         {
                                             Selected = Entity;
                                             break;
                                         }
                                     }
                                     
                                     GameState->EditorState.SelectedEntity = Selected;
                                 }
                                 
                                 if(GameState->EditorState.SelectedEntity && GetMouseButton(Mouse_Left, GameState))
                                 {
                                     GameState->EditorState.SelectedEntity->Position = glm::vec2(Pos.x, Pos.y - GameState->EditorState.SelectedEntity->Scale / 2 );
                                 }
                             }
                         }
                         break;
                         case Editor_Placement_PlaceEntity:
                         {
                             if(GetMouseButtonDown(Mouse_Left, GameState))
                             {
                                 switch(GameState->EditorState.PlacementEntity)
                                 {
                                     case Placement_Entity_Skeleton:
                                     {
                                         LoadSkeletonData(GameState, -1, glm::vec2(Pos.x, Pos.y - 0.5f));
                                     }
                                     break;
                                     case Placement_Entity_Blob:
                                     {
                                         LoadBlobData(GameState, -1, glm::vec2(Pos.x, Pos.y - 0.5f));
                                     }
                                     break;
                                     case Placement_Entity_Wraith:
                                     {
                                         LoadWraithData(GameState, -1, glm::vec2(Pos.x, Pos.y - 0.5));
                                     }
                                     break;
                                     case Placement_Entity_Minotaur:
                                     {
                                         LoadMinotaurData(GameState, -1, glm::vec2(Pos.x, Pos.y - 0.5f));
                                     }
                                     break;
                                     case Placement_Entity_Barrel:
                                     {}
                                     break;
                                     case Placement_Entity_Bonfire:
                                     {
                                         LoadBonfireData(GameState, -1, glm::vec2(Pos.x, Pos.y - 0.5f));
                                     }
                                     break;
                                 }
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
                             i32 X = (i32)glm::floor(Pos.x);
                             i32 Y = (i32)glm::floor(Pos.y);
                             GameState->EditorState.TileX = (r32)X;
                             GameState->EditorState.TileY = (r32)glm::ceil(Pos.y);
                             
                             if(GetMouseButton(Mouse_Left, GameState))
                             {
                                 i32 TilesheetWidth = GameState->CurrentLevel.Tilemap.RenderEntity.Texture->Width / GameState->CurrentLevel.Tilemap.TileSize;
                                 i32 TilesheetHeight = GameState->CurrentLevel.Tilemap.RenderEntity.Texture->Height / GameState->CurrentLevel.Tilemap.TileSize;
                                 
                                 r32 ToolbarX = GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x;
                                 r32 ToolbarY = GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y;
                                 
                                 printf("Mouse position: (%f,%f)\n", GameState->InputController.MouseX, GameState->RenderState.WindowHeight - GameState->InputController.MouseY);
                                 
                                 if(GameState->InputController.MouseX >= ToolbarX && GameState->InputController.MouseX <= ToolbarX + TilesheetWidth * GameState->EditorState.RenderedTileSize && GameState->RenderState.WindowHeight - GameState->InputController.MouseY > ToolbarY && GameState->RenderState.WindowHeight - GameState->InputController.MouseY <=  ToolbarY + TilesheetHeight * GameState->EditorState.RenderedTileSize)
                                 {
                                     i32 X = (i32)((GameState->InputController.MouseX - (GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x)) / GameState->EditorState.RenderedTileSize);
                                     i32 Y = (i32)((GameState->RenderState.WindowHeight - GameState->InputController.MouseY - (GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y)) / GameState->EditorState.RenderedTileSize);
                                     
                                     GameState->EditorState.SelectedTilePosition = glm::vec2((r32)X, (r32)Y);
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
                                         
                                         if(GetKey(Key_F, GameState))
                                         {
                                             for(u32 IndexX = 0; IndexX < Tilemap->Width; IndexX++)
                                             {
                                                 for(u32 IndexY = 0; IndexY < Tilemap->Height; IndexY++)
                                                 {
                                                     for(i32 X = 0; X < (i32)GameState->EditorState.TileBrushSize.x && X + IndexX < Tilemap->Width; X++)
                                                     {
                                                         for(i32 Y = 0; Y < (i32)GameState->EditorState.TileBrushSize.y && Y + IndexY < Tilemap->Height; Y++)
                                                         {
                                                             Tilemap->Data[1][IndexX + X][IndexY + Y] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                                         }
                                                     }
                                                     Tilemap->RenderInfo.Dirty = true;
                                                 }
                                             }
                                         }
                                         else
                                         {
                                             for(u32 IndexY = 0; IndexY < Tilemap->Height; IndexY++)
                                             {
                                                 for(u32 RelativeX = 0; RelativeX < GameState->EditorState.TileBrushSize.x && RelativeX + X < Tilemap->Width; RelativeX++)
                                                 {
                                                     for(u32 RelativeY = 0; RelativeY < GameState->EditorState.TileBrushSize.y && RelativeY + Y < Tilemap->Height; RelativeY++)
                                                     {
                                                         Tilemap->Data[GameState->EditorState.CurrentTilemapLayer][X + RelativeX][Y + RelativeY] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                                     }
                                                 }
                                             }
                                             
                                             Tilemap->RenderInfo.Dirty = true;
                                         }
                                     }
                                 }
                             }
                             if(GetMouseButton(Mouse_Left,GameState) && GetKey(Key_LeftShift,GameState))
                             {
                                 if(X >= 0 && X < (i32)GameState->CurrentLevel.Tilemap.Width && Y >= 0 && Y < (i32)GameState->CurrentLevel.Tilemap.Height)
                                 {
                                     GameState->CurrentLevel.Tilemap.Data[GameState->EditorState.CurrentTilemapLayer][X][Y] = GameState->CurrentLevel.Tilemap.Tiles[0];
                                 }
                             }
                         }
                         break;
                     }
                 }
                 
                 // View translation
                 if(GameState->InputController.MouseX >= GameState->EditorState.ToolbarX && GameState->InputController.MouseX < GameState->EditorState.ToolbarX + GameState->EditorState.ToolbarWidth)
                 {
                     GameState->EditorState.ToolbarScrollOffsetY += (r32)GameState->InputController.ScrollY * GameState->EditorState.ToolbarScrollSpeed * (r32)DeltaTime;
                     
                     if(GameState->EditorState.ToolbarScrollOffsetY > 0)
                         GameState->EditorState.ToolbarScrollOffsetY = 0.0f;
                     else if(GameState->EditorState.ToolbarScrollOffsetY < -((r32)GameState->CurrentLevel.Tilemap.TileCount) * 60)
                         GameState->EditorState.ToolbarScrollOffsetY = -((r32)GameState->CurrentLevel.Tilemap.TileCount) * 60;
                 }
                 else
                 {
                     GameState->EditorCamera.Zoom += (r32)GameState->InputController.ScrollY * GameState->EditorState.ZoomingSpeed * (r32)DeltaTime * GameState->EditorCamera.Zoom;
                     GameState->EditorCamera.Zoom = Max(Min(GameState->EditorCamera.Zoom, GameState->EditorState.MaxZoom), GameState->EditorState.MinZoom);
                 }
                 
                 if(GetMouseButton(Mouse_Right, GameState))
                 {
                     if(GameState->EditorState.LastKnownMouseX == 0 && GameState->EditorState.LastKnownMouseY == 0)
                     {
                         GameState->EditorState.LastKnownMouseX = (r32)GameState->InputController.MouseX;
                         GameState->EditorState.LastKnownMouseY = (r32)GameState->InputController.MouseY;
                     }
                     
                     glm::vec2 Direction = glm::vec2(GameState->InputController.MouseX - GameState->EditorState.LastKnownMouseX, GameState->InputController.MouseY - GameState->EditorState.LastKnownMouseY);
                     
                     GameState->EditorCamera.Center -= glm::vec2(Direction.x / GameState->EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime, Direction.y / GameState->EditorCamera.Zoom * -GameState->EditorState.PanningSpeed * DeltaTime);
                     
                     GameState->EditorState.LastKnownMouseX = (r32)GameState->InputController.MouseX;
                     GameState->EditorState.LastKnownMouseY = (r32)GameState->InputController.MouseY;
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
                         
                         if(GetKeyDown(Key_Enter, GameState))
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
                         if(GetKeyDown(Key_Enter, GameState))
                         {
                             GameState->EditorState.AnimationMode = Animation_Create;
                             
                             GameState->EditorState.LoadedAnimation = (animation*)malloc(sizeof(animation));
                             
                             GameState->EditorState.LoadedAnimation->Name = (char*) calloc(30, sizeof(char));
                             GameState->EditorState.LoadedAnimation->FrameCount = 0;
                             GameState->EditorState.LoadedAnimation->FrameSize = glm::vec2(0, 0);
                             GameState->EditorState.LoadedAnimation->FrameOffset = glm::vec2(0, 0);
                             GameState->EditorState.LoadedAnimation->TimePerFrame = 0.0f;
                             GameState->EditorState.LoadedAnimation->Loop = 1;
                             GameState->EditorState.LoadedAnimation->Texture = GameState->RenderState.Textures[GameState->EditorState.Textures[GameState->EditorState.SelectedTexture]];
                             
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
                 LoadBonfireData(GameState, EntityIndex);
             }
         }
         GameState->ReloadData->ReloadBonfireFile = false;
     }
     
     CheckConsoleInput(GameState, DeltaTime);
     
     if(GameState->GameMode == Mode_Editor)
         CheckEditorUIInput(GameState, DeltaTime);
     
     if((GetKey(Key_LeftCtrl, GameState) || GetKey(Key_RightCtrl, GameState)))
     {
         if(GetKeyDown(Key_E, GameState)) // Editor-mode on/off
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
         else if(GetKeyDown(Key_G, GameState)) // God-mode on/off
         {
             GameState->GodModeOn = !GameState->GodModeOn;
             
             if(!GameState->GodModeOn)
             {
                 auto Player = GameState->Entities[0];
                 GameState->GameCamera.Center = glm::vec2(Player.Position.x, Player.Position.y);
                 GameState->Camera.Center = glm::vec2(Player.Position.x, Player.Position.y);
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
     
     if(!GameState->IsInitialized)
     {
         if(GameState->ShouldReload)
         {
             LoadGameDataFile(GameState);
             srand((u32)time(NULL));
             
             LoadAnimations(GameState);
             InitCommands();
             
             light_source AmbientLight;
             AmbientLight.Type = Light_Ambient;
             AmbientLight.Active = true;
             AmbientLight.Color = glm::vec4(0.1,0,0,1);
             AmbientLight.Ambient.Intensity = 0.5f;
             GameState->LightSources[GameState->LightSourceCount++] = AmbientLight;
             
             GameState->EditorCamera.Zoom = GameState->InitialZoom; 
             GameState->EditorCamera.ViewportWidth = GameState->RenderState.WindowWidth;
             GameState->EditorCamera.ViewportHeight = GameState->RenderState.WindowHeight;
             
             GameState->GameMode = Mode_InGame;
             GameState->ShouldReload = false;
         }
         
         LoadLevelFromFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
         
         GameState->GameCamera.Zoom = GameState->InitialZoom;
         GameState->GameCamera.ViewportWidth = GameState->RenderState.WindowWidth;
         GameState->GameCamera.ViewportHeight = GameState->RenderState.WindowHeight;
         GameState->GameCamera.ScreenShakeTimer.TimerHandle = -1;
         GameState->GameCamera.ScreenShakeTimer.TimerMax = 0.2f;
         GameState->GameCamera.FollowSpeed = 12.0f; 
         GameState->GameCamera.FadingSpeed = 0.6f;
         
         StartFade(GameState->GameCamera, Fading_In, 0.6f, glm::vec3(0, 0, 0), 1.0f, 0.0f);
         
         // @Incomplete: This is not the right value, it is only set so high to remove smooth following as of now, since it needs to be done a little differently
         
         GameState->GameCamera.Center = GameState->Entities[0].Position; // Set center to player's position!
         GameState->GameCamera.CenterTarget = GameState->Entities[0].Position;
         GameState->IsInitialized = true;
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
     
     if(GetKeyDown(Key_F7, GameState))
     {
         GameState->RenderGame = !GameState->RenderGame;
     }
     
     if(GetKeyDown(Key_F8, GameState))
     {
         GameState->AIDebugModeOn = !GameState->AIDebugModeOn;
     }
     
     if(GetKeyDown(Key_F9, GameState))
     {
         GameState->StatGainModeOn = !GameState->StatGainModeOn;
     }
     
     if(GameState->GameMode == Mode_InGame && GetKey(Key_LeftCtrl, GameState) && GetKeyDown(Key_P, GameState))
     {
         GameState->Paused = !GameState->Paused;
     }
     
     if(GameState->GameMode == Mode_InGame && (GetKey(Key_LeftCtrl, GameState) || GetKey(Key_RightCtrl, GameState)))
     {
         if(GetKeyDown(Key_F, GameState))
             SaveGame(GameState);
         if(GetKeyDown(Key_V, GameState))
             LoadGame(GameState);
     }
     
#endif
     
     if (GetActionButtonDown(Action_Menu, GameState) && !GameState->Console.Open)
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
         if(GetKeyDown(Key_Up, GameState))
             GameState->MainMenu.SelectedIndex -= 1;
         else if(GetKeyDown(Key_Down, GameState))
             GameState->MainMenu.SelectedIndex += 1;
         
         if(GameState->MainMenu.SelectedIndex < 0)
             GameState->MainMenu.SelectedIndex = GameState->MainMenu.OptionCount - 1;
         else if(GameState->MainMenu.SelectedIndex == (i32)GameState->MainMenu.OptionCount)
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
     
     glm::vec2 Center = GameState->GameCamera.Center;
     
     if(GameState->GodModeOn)
     {
         r32 Zoom = GameState->Camera.Zoom;
         
         glm::vec2 Direction = glm::vec2(0, 0);
         
         if(GetKey(Key_W, GameState) || GetKey(Key_Up, GameState))
         {
             Direction.y = 1;
         }
         else if(GetKey(Key_S, GameState) || GetKey(Key_Down, GameState))
         {
             Direction.y = -1;
         }
         
         if(GetKey(Key_A, GameState) || GetKey(Key_Left, GameState))
         {
             Direction.x = -1;
         }
         else if(GetKey(Key_D, GameState) || GetKey(Key_Right, GameState))
         {
             Direction.x = 1;
         }
         
         r32 Factor = 72.0f / GameState->GameCamera.Zoom;
         
         if(GetKey(Key_Add, GameState))
         {
             Zoom += (r32)(GameState->GodModeZoomSpeed / Factor * DeltaTime);
         }
         else if(GetKey(Key_Subtract, GameState))
         {
             Zoom += (r32)(-GameState->GodModeZoomSpeed / Factor * DeltaTime);
         }
         
         Direction = glm::normalize(Direction);
         
         if(Abs(Direction.x) > 0.0 || Abs(Direction.y) > 0.0)
         {
             GameState->GameCamera.Center = Center + glm::vec2(Direction.x * GameState->GodModePanSpeed * Factor * DeltaTime, Direction.y * GameState->GodModePanSpeed * Factor * DeltaTime);
         }
         
         GameState->GameCamera.Zoom = Min(Max(Zoom, GameState->GodModeMinZoom), GameState->GodModeMaxZoom);
     }
     
     if(GetKeyDown(Key_L, GameState) && GetKey(Key_LeftCtrl, GameState))
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
                 UpdateEntities(GameState, DeltaTime);
                 UpdateObjects(GameState, DeltaTime);
                 
                 TickTimers(GameState, DeltaTime);
                 
                 if(!TimerDone(GameState, GameState->GameCamera.ScreenShakeTimer))
                 {
                     r32 Radius = 0.05f;
                     i32 RandomAngle = rand() % 360;
                     glm::vec2 Offset = glm::vec2(sin(RandomAngle) * Radius, cos(RandomAngle) * Radius);
                     Center.x += Offset.x / 1.5f;
                     Center.y += Offset.y;
                 }
                 
                 if(!GameState->GodModeOn)
                 {
                     if(glm::distance(GameState->GameCamera.CenterTarget, Center) > 0.01f)
                     {
                         auto Direction = glm::normalize(GameState->GameCamera.CenterTarget - Center);
                         Center = glm::vec2(Center.x + Direction.x * GameState->GameCamera.FollowSpeed * DeltaTime, Center.y + Direction.y  * GameState->GameCamera.FollowSpeed * DeltaTime);
                         GameState->GameCamera.Center = Center;
                     }
                 }
             }
             else if(GameState->StatGainModeOn)
             {
                 b32 ControllerPresent = GameState->InputController.ControllerPresent;
                 
                 b32 UpPressed = ControllerPresent ? GetJoystickAxisYDown(GameState, true) : GetKeyDown(Key_W, GameState) || GetKeyDown(Key_Up, GameState);
                 b32 DownPressed = ControllerPresent ? GetJoystickAxisYDown(GameState, false) : GetKeyDown(Key_S, GameState) || GetKeyDown(Key_Down, GameState);
                 
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
                 
                 if(GetActionButtonDown(Action_Interact,GameState) || GetKeyDown(Key_Enter, GameState))
                 {
                     auto& Player = GameState->Entities[0];
                     
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
             Center = GameState->EditorCamera.Center;
             EditorUpdateEntities(GameState, DeltaTime);
             
             switch(GameState->EditorState.PlacementMode)
             {
                 case Editor_Placement_PlaceEntity:
                 case Editor_Placement_SelectEntity:
                 {
                     if(GetKeyDown(Key_Left, GameState))
                     {
                         GameState->EditorState.PlacementEntity = (Entity_Placement_Type)((i32)GameState->EditorState.PlacementEntity - 1);
                         if(GameState->EditorState.PlacementEntity == -1)
                             GameState->EditorState.PlacementEntity = (Entity_Placement_Type)((i32)Placement_Entity_Max - 1);
                     }
                     else if(GetKeyDown(Key_Right, GameState))
                     {
                         GameState->EditorState.PlacementEntity = (Entity_Placement_Type)((i32)GameState->EditorState.PlacementEntity + 1);
                         if(GameState->EditorState.PlacementEntity == Placement_Entity_Max)
                             GameState->EditorState.PlacementEntity = (Entity_Placement_Type)0;
                     }
                 }
                 break;
                 case Editor_Placement_Tile:
                 {
                     if(GetKeyDown(Key_1, GameState))
                     {
                         GameState->EditorState.CurrentTilemapLayer = 0; 
                     }
                     else if(GetKeyDown(Key_2, GameState))
                     {
                         GameState->EditorState.CurrentTilemapLayer = 1; 
                     }
                 }
                 break;
             }
             // @Cleanup: I don't remember why this has to be called individually for each mode. It might be okay to call it at the end of the Update-function
             GameState->Camera = GameState->EditorCamera;
             TickTimers(GameState, DeltaTime);
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
     GameState->RenderState.DeltaTime = DeltaTime;
 }
 
 