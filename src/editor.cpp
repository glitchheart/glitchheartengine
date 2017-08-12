static void InitEditorFields(game_state* GameState, renderer& Renderer)
{
    GameState->EditorState.AnimationInfo.Playing = true;
    GameState->EditorState.AnimationInfo.FrameIndex = 0;
    
    GameState->EditorState.AnimationNameField = &GameState->EditorState.Textfields[0];
    GameState->EditorState.AnimationFrameWidthField = &GameState->EditorState.Textfields[1];
    GameState->EditorState.AnimationFrameHeightField = &GameState->EditorState.Textfields[2];
    GameState->EditorState.AnimationFrameCountField = &GameState->EditorState.Textfields[3];
    GameState->EditorState.AnimationFrameOffsetXField = &GameState->EditorState.Textfields[4];
    GameState->EditorState.AnimationFrameOffsetYField = &GameState->EditorState.Textfields[5];
    GameState->EditorState.AnimationFrameDurationField = &GameState->EditorState.Textfields[6];
    GameState->EditorState.TileBrushWidthField = &GameState->EditorState.Textfields[7];
    GameState->EditorState.TileBrushHeightField = &GameState->EditorState.Textfields[8];
    
    GameState->EditorState.AnimationLoopCheckbox = &GameState->EditorState.Checkboxes[0];
    
    r32 FieldX = 10.0f;
    
    GameState->EditorState.AnimationNameField->Active = false;
    GameState->EditorState.AnimationNameField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationNameField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 220);
    GameState->EditorState.AnimationNameField->Label = "Name";
    
    GameState->EditorState.AnimationFrameWidthField->Active = false;
    GameState->EditorState.AnimationFrameWidthField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameWidthField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 280);
    GameState->EditorState.AnimationFrameWidthField->Label = "Frame width";
    GameState->EditorState.AnimationFrameWidthField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameHeightField->Active = false;
    GameState->EditorState.AnimationFrameHeightField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameHeightField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 340);
    GameState->EditorState.AnimationFrameHeightField->Label = "Frame height";
    GameState->EditorState.AnimationFrameHeightField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameCountField->Active = false;
    GameState->EditorState.AnimationFrameCountField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameCountField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 400);
    GameState->EditorState.AnimationFrameCountField->Label = "Frame count";
    GameState->EditorState.AnimationFrameCountField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameOffsetXField->Active = false;
    GameState->EditorState.AnimationFrameOffsetXField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameOffsetXField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 460);
    GameState->EditorState.AnimationFrameOffsetXField->Label = "Frame offset x";
    GameState->EditorState.AnimationFrameOffsetXField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameOffsetYField->Active = false;
    GameState->EditorState.AnimationFrameOffsetYField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameOffsetYField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 520);
    GameState->EditorState.AnimationFrameOffsetYField->Label = "Frame offset y";
    GameState->EditorState.AnimationFrameOffsetYField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameDurationField->Active = false;
    GameState->EditorState.AnimationFrameDurationField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameDurationField->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 580);
    GameState->EditorState.AnimationFrameDurationField->Label = "Frame duration";
    GameState->EditorState.AnimationFrameDurationField->Type = Textfield_Decimal;
    
    GameState->EditorState.AnimationLoopCheckbox->Active = false;
    GameState->EditorState.AnimationLoopCheckbox->Checked = false;
    GameState->EditorState.AnimationLoopCheckbox->ScreenPosition = math::v2(FieldX, Renderer.WindowHeight - 640);
    GameState->EditorState.AnimationLoopCheckbox->Label = "Loop";
    
    GameState->EditorState.TileBrushWidthField->Active = false;
    GameState->EditorState.TileBrushWidthField->Size = math::v2(300, 30);
    GameState->EditorState.TileBrushWidthField->ScreenPosition = math::v2(10, 100);
    GameState->EditorState.TileBrushWidthField->Label = "Brush width";
    GameState->EditorState.TileBrushWidthField->Type = Textfield_Integer;
    sprintf(GameState->EditorState.TileBrushWidthField->Text, "1"); 
    
    GameState->EditorState.TileBrushHeightField->Active = false;
    GameState->EditorState.TileBrushHeightField->Size = math::v2(300, 30);
    GameState->EditorState.TileBrushHeightField->ScreenPosition = math::v2(10, 150);
    GameState->EditorState.TileBrushHeightField->Label = "Brush height";
    GameState->EditorState.TileBrushHeightField->Type = Textfield_Integer;
    sprintf(GameState->EditorState.TileBrushHeightField->Text, "1");
}

static void ToggleAnimationFields(editor_state* EditorState, b32 Active)
{
    EditorState->AnimationNameField->Active = Active;
    EditorState->AnimationFrameWidthField->Active = Active;
    EditorState->AnimationFrameHeightField->Active = Active;
    EditorState->AnimationFrameCountField->Active = Active;
    EditorState->AnimationFrameOffsetXField->Active = Active;
    EditorState->AnimationFrameOffsetYField->Active = Active;
    EditorState->AnimationFrameDurationField->Active = Active;
    EditorState->AnimationLoopCheckbox->Active = Active;
    EditorState->SaveAnimationButton->Active = Active;
}

static void SetFieldValues(game_state* GameState, b32 Reload = false)
{
    switch(GameState->EditorState.Mode)
    {
        case Editor_Level:
        {
            
        }
        break;
        case Editor_Animation:
        {
            animation* LoadedAnimation = GameState->EditorState.LoadedAnimation;
            if(LoadedAnimation)
            {
                if(!GameState->EditorState.HasLoadedAnimations || Reload)
                {
                    if(LoadedAnimation->Name)
                        sprintf(GameState->EditorState.AnimationNameField->Text, "%s", LoadedAnimation->Name);
                    
                    for(i32 Index = (i32)strlen(LoadedAnimation->Name); Index < TEXTFIELD_LENGTH; Index++)
                    {
                        GameState->EditorState.AnimationNameField->Text[Index] = 0;
                    }
                    
                    if(LoadedAnimation->FrameCount != 0)
                        sprintf(GameState->EditorState.AnimationFrameCountField->Text, "%d", LoadedAnimation->FrameCount);
                    
                    if(LoadedAnimation->FrameSize.x != 0.0f)
                        sprintf(GameState->EditorState.AnimationFrameWidthField->Text, "%d", (i32)LoadedAnimation->FrameSize.x);
                    
                    if(LoadedAnimation->FrameSize.y != 0.0f)
                        sprintf(GameState->EditorState.AnimationFrameHeightField->Text, "%d", (i32)LoadedAnimation->FrameSize.y);
                    
                    if(LoadedAnimation->FrameCount > 0)
                    {
                        sprintf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%d", (i32)LoadedAnimation->Frames[0].X / (i32)LoadedAnimation->FrameSize.x);
                        sprintf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%d", (i32)LoadedAnimation->Frames[0].Y / (i32)LoadedAnimation->FrameSize.y);
                    }
                    
                    if(LoadedAnimation->TimePerFrame != 0.0f)
                        sprintf(GameState->EditorState.AnimationFrameDurationField->Text, "%3.4f", LoadedAnimation->TimePerFrame);
                    
                    GameState->EditorState.AnimationLoopCheckbox->Checked =
                        GameState->EditorState.ShouldLoop;
                    
                    GameState->EditorState.HasLoadedAnimations = true;
                }
            }
        }
        break;
        case Editor_Tilesheet:
        {
            GameState->EditorState.TileIsSolidCheckbox->Checked = GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorState.SelectedTileType].IsSolid;
        }
        break;
    }
}

static void CreateEditorButtons(game_state* GameState, renderer& Renderer)
{
    // @Incomplete: These values need to be updated when the window size is changed
    GameState->EditorState.TilemapOffset = math::v2(0, 300);
    GameState->EditorState.ToolbarX = 0;
    GameState->EditorState.ToolbarY = 0;
    GameState->EditorState.ToolbarWidth = 500.0f;
    GameState->EditorState.ToolbarHeight = (r32)Renderer.WindowHeight;
    
    GameState->EditorState.Buttons[0].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[0].Text = "Create/Edit animation";
    GameState->EditorState.Buttons[0].ScreenPosition = math::v2(5, (r32)Renderer.WindowHeight - 150);
    GameState->EditorState.Buttons[0].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[0].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[0].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[0].Active = false;
    GameState->EditorState.Buttons[0].EditorType = Button_Animation;
    GameState->EditorState.Buttons[0].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[0].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[1].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[1].Text = "Create/Edit tilesheet";
    GameState->EditorState.Buttons[1].ScreenPosition = math::v2(330, (r32)Renderer.WindowHeight - 150);
    GameState->EditorState.Buttons[1].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[1].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[1].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[1].Active = false;
    GameState->EditorState.Buttons[1].EditorType = Button_Tilesheet;
    GameState->EditorState.Buttons[1].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[1].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[2].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[2].Text = "Switch mode";
    GameState->EditorState.Buttons[2].ScreenPosition = math::v2(655, (r32)Renderer.WindowHeight - 150);
    GameState->EditorState.Buttons[2].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[2].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[2].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[2].Active = false;
    GameState->EditorState.Buttons[2].EditorType = Button_SwitchMode;
    GameState->EditorState.Buttons[2].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[2].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[3].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[3].Text = "Save and exit";
    GameState->EditorState.Buttons[3].ScreenPosition = math::v2(980, (r32)Renderer.WindowHeight - 150);
    GameState->EditorState.Buttons[3].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[3].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[3].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[3].Active = false;
    GameState->EditorState.Buttons[3].EditorType = Button_SaveAndExit;
    GameState->EditorState.Buttons[3].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[3].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[4].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[4].Text = "Exit";
    GameState->EditorState.Buttons[4].ScreenPosition = math::v2(1305, (r32)Renderer.WindowHeight - 150);
    GameState->EditorState.Buttons[4].Size = math::v2(280, 60);
    GameState->EditorState.Buttons[4].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[4].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[4].Active = false;
    GameState->EditorState.Buttons[4].EditorType = Button_Exit;
    GameState->EditorState.Buttons[4].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[4].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[5].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[5].Text = "Create animation";
    GameState->EditorState.Buttons[5].ScreenPosition = math::v2(10, 20);
    GameState->EditorState.Buttons[5].Size = math::v2(300, 50);
    GameState->EditorState.Buttons[5].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[5].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[5].Active = true;
    GameState->EditorState.Buttons[5].EditorType = Button_Animation;
    GameState->EditorState.Buttons[5].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[5].ClickAnimationTimer.TimerHandle = -1;
    GameState->EditorState.CreateNewAnimationButton = &GameState->EditorState.Buttons[5];
    
    GameState->EditorState.Buttons[6].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[6].Text = "Save animation";
    GameState->EditorState.Buttons[6].ScreenPosition = math::v2(10, Renderer.WindowHeight - 700);
    GameState->EditorState.Buttons[6].Size = math::v2(300, 50);
    GameState->EditorState.Buttons[6].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[6].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[6].Active = false;
    GameState->EditorState.Buttons[6].EditorType = Button_Animation;
    GameState->EditorState.Buttons[6].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[6].ClickAnimationTimer.TimerHandle = -1;
    GameState->EditorState.SaveAnimationButton = &GameState->EditorState.Buttons[6];
    
    GameState->EditorState.Buttons[7].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[7].Text = "Create new level";
    GameState->EditorState.Buttons[7].ScreenPosition = math::v2(700, 20);
    GameState->EditorState.Buttons[7].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[7].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[7].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[7].Active = true;
    GameState->EditorState.Buttons[7].EditorType = Button_CreateLevel;
    GameState->EditorState.Buttons[7].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[7].ClickAnimationTimer.TimerHandle = -1;
    GameState->EditorState.CreateNewLevelButton = &GameState->EditorState.Buttons[7];
    
    // Tile editor
    GameState->EditorState.TileIsSolidCheckbox = &GameState->EditorState.Checkboxes[1];
    GameState->EditorState.TileIsSolidCheckbox->Active = false;
    GameState->EditorState.TileIsSolidCheckbox->Checked = false;
    GameState->EditorState.TileIsSolidCheckbox->ScreenPosition = math::v2(20, 225);
    GameState->EditorState.TileIsSolidCheckbox->Label = "Is solid";
    
    GameState->EditorState.Loaded = true;
}

static void CheckEditorUIInput(game_state* GameState, renderer& Renderer, input_controller* InputController, r64 DeltaTime)
{
    switch(GameState->EditorState.Mode)
    {
        case Editor_Animation:
        {
            if(GameState->EditorState.AnimationMode == Animation_SelectTexture)
            {
                if(KEY_DOWN(Key_Down))
                {
                    GameState->EditorState.SelectedTexture++;
                }
                else if(KEY_DOWN(Key_Up))
                {
                    GameState->EditorState.SelectedTexture--;
                }
                
                // @Incomplete: Get the f**k out
                /*if(GameState->EditorState.SelectedTexture == GameState->RenderState.TextureIndex)
                    GameState->EditorState.SelectedTexture = 0;
                else if(GameState->EditorState.SelectedTexture < 0)
                    GameState->EditorState.SelectedTexture = (i32)GameState->RenderState.TextureIndex - 1;*/
            }
            else if(GameState->EditorState.AnimationMode == Animation_SelectAnimation)
            {
                b32 Changed = false;
                
                if(GetKeyDown(Key_Down, InputController))
                {
                    DEBUG_PRINT("Down\n");
                    GameState->EditorState.SelectedAnimation++;
                    Changed = true;
                }
                else if(GetKeyDown(Key_Up, InputController))
                {
                    DEBUG_PRINT("Up\n");
                    GameState->EditorState.SelectedAnimation--;
                    Changed = true;
                }
                
                if(GameState->EditorState.SelectedAnimation == GameState->AnimationIndex)
                    GameState->EditorState.SelectedAnimation = 0;
                else if(GameState->EditorState.SelectedAnimation < 0)
                    GameState->EditorState.SelectedAnimation = (i32)GameState->AnimationIndex - 1;
                
                if(Changed)
                {
                    GameState->EditorState.LoadedAnimation = &GameState->AnimationArray[GameState->EditorState.SelectedAnimation];
                    
                    GameState->EditorState.ShouldLoop = GameState->EditorState.LoadedAnimation->Loop;
                    
                    animation* LoadedAnimation = GameState->EditorState.LoadedAnimation;
                    GameState->EditorState.LoadedAnimation->Loop = 1;
                    //InitEditorFields(GameState);
                    SetFieldValues(GameState, true);
                }
            }
        }
        break;
    }
    
    InputController->DeleteCharacter = KEY_DOWN(Key_Backspace);
    
    auto MouseX = InputController->MouseX;
    auto MouseY = InputController->MouseY;
    
    int ListRectWidth = 120;
    
    if(GameState->EditorState.SelectedEntity && KEY_DOWN(Key_Delete))
    {
        DeleteEntity(GameState,GameState->EditorState.SelectedEntity->EntityIndex);
        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, Renderer);
    }
}


static void EditorUpdateEntities(game_state* GameState, renderer& Renderer, input_controller* InputController, sound_queue* SoundQueue, r64 DeltaTime)
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
                    Renderer.Cameras[GameState->EditorCameraHandle].Center = Renderer.Cameras[GameState->GameCameraHandle].Center;
                }
                break;
                case Editor_Menu_Game:
                {
                    ToggleAnimationFields(&GameState->EditorState, false);
                    if(GameState->EditorState.Mode == Editor_Level)
                    {
                        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, Renderer);
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
        b32 InToolbar = Renderer.WindowHeight - InputController->MouseY >= Renderer.WindowHeight;
        
        for(u32 ButtonIndex = 0; ButtonIndex < 10; ButtonIndex++)
        {
            button* Button = &GameState->EditorState.Buttons[ButtonIndex];
            Button->Clicked = false;
            
            if(MOUSE_DOWN(Mouse_Left))
            {
                if(Button->Active && InputController->MouseX >= Button->ScreenPosition.x && InputController->MouseX <= Button->ScreenPosition.x + Button->Size.x && 
                   Renderer.WindowHeight - InputController->MouseY >= Button->ScreenPosition.y && Renderer.WindowHeight - InputController->MouseY <= Button->ScreenPosition.y + Button->Size.y)
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
                            SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, Renderer);
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
                if(Checkbox->Active && InputController->MouseX >= Checkbox->ScreenPosition.x && InputController->MouseX <= Checkbox->ScreenPosition.x + 25 && Renderer.WindowHeight - InputController->MouseY >= Checkbox->ScreenPosition.y && Renderer.WindowHeight - InputController->MouseY <= Checkbox->ScreenPosition.y + 25)
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
            SaveTilesheetMetaFile(Concat(Concat("../assets/textures/tilesheets/", GameState->CurrentLevel.SheetName), ".tm"), Renderer, GameState->CurrentLevel, false);
            
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
                   Renderer.WindowHeight - InputController->MouseY >= Textfield->ScreenPosition.y && Renderer.WindowHeight - InputController->MouseY <= Textfield->ScreenPosition.y + Textfield->Size.y)
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
                    SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, Renderer);
                }
                
                if(GameState->EditorState.CreateNewLevelButton->Clicked)
                {
                    level Level;
                    Level.SheetName = "overworld1";
                    Level.Name = "test_level";
                    
                    CreateNewLevelWithSize("../assets/levels/level_new.plv", 200, 120, &Level,  Renderer, GameState, SoundQueue);
                    GameState->CurrentLevel = Level;
                }
                
                auto Pos = math::UnProject(math::v3(InputController->MouseX, Renderer.Viewport[3] - InputController->MouseY, 0),
                                           Renderer.Cameras[GameState->GameCameraHandle].ViewMatrix,
                                           Renderer.Cameras[GameState->GameCameraHandle].ProjectionMatrix,
                                           math::v4(0, 0, Renderer.Viewport[2], Renderer.Viewport[3]));
                
                i32 X = (i32)Pos.x;
                i32 Z = (i32)Pos.y;
                
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
                                    i32 X = (i32)math::Floor(Pos.x / 0.5f);
                                    i32 Z = (i32)math::Floor(Pos.z / 0.5f);
                                    
                                    if(!GameState->CurrentLevel.Tilemap.Data[1][X][Z].IsSolid)
                                    {
                                        GameState->EditorState.SelectedEntity->Enemy.Waypoints[GameState->EditorState.SelectedEntity->Enemy.WaypointCount++] = math::v3i(X, 0, Z);
                                    }
                                }
                            }
                            else
                            {
                                if(MOUSE_DOWN(Mouse_Left))
                                {
                                    entity* Selected = 0;
                                    
                                    for(u32 EntityIndex = 0;
                                        EntityIndex < GameState->EntityCount;
                                        EntityIndex++)
                                    {
                                        entity* Entity = &GameState->Entities[EntityIndex];
                                        
                                        if(X == (i32)Entity->Position.x && Z == (i32)Entity->Position.z)
                                        {
                                            if(GameState->EditorState.SelectedEntity)
                                            {
                                                render_entity* RenderEntity = &GameState->RenderEntities[GameState->EditorState.SelectedEntity->RenderEntityHandle];
                                                RenderEntity->Color = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
                                            }
                                            
                                            Selected = Entity;
                                            
                                            render_entity* RenderEntity = &GameState->RenderEntities[Selected->RenderEntityHandle];
                                            RenderEntity->Color = math::rgba(0.8f, 0.5f, 0.0f, 1.0f);
                                            
                                            break;
                                        }
                                    }
                                    
                                    if(!Selected && GameState->EditorState.SelectedEntity)
                                    {
                                        render_entity* RenderEntity = &GameState->RenderEntities[GameState->EditorState.SelectedEntity->RenderEntityHandle];
                                        RenderEntity->Color = math::rgba(1.0f, 1.0f, 1.0f, 1.0f);
                                    }
                                    
                                    GameState->EditorState.SelectedEntity = Selected;
                                }
                                
                                if(GameState->EditorState.SelectedEntity && MOUSE(Mouse_Left))
                                {
                                    GameState->EditorState.SelectedEntity->Position = math::v3(X, 0.0f, Z);
                                }
                            }
                        }
                        break;
                        case Editor_Placement_PlaceEntity:
                        {
                            if(MOUSE_DOWN(Mouse_Left))
                            {
                                switch(GameState->EditorState.PlacementEntity)
                                {
                                    case Placement_Entity_Skeleton:
                                    {
                                        LoadSkeletonData(GameState, -1, math::v3(X, 0.0f, Z));
                                    }
                                    break;
                                    case Placement_Entity_Bonfire:
                                    {
                                        LoadBonfireData(GameState, SoundQueue, -1, math::v3(X, 0.0f,  Z));
                                    }
                                    break;
                                }
                                
                                SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState, Renderer);
                            }
                        }
                        break;
                        case Editor_Placement_Tile:
                        {
                            math::m4 IsoTransform(1.0f);
                            IsoTransform = math::Translate(IsoTransform, math::v3(0.0f, 0.25f, 0.0f));
                            IsoTransform = math::Scale(IsoTransform, math::v3((r32)(sqrt(2.0) / 2.0), (r32)(sqrt(2.0) / 4.0), 1.0f));
                            IsoTransform = math::Rotate(IsoTransform, -45, math::v3(0.0f, 0.0f, 0.0f));
                            IsoTransform = math::Inverse(IsoTransform);
                            
                            math::v4 NewPos = IsoTransform * math::v4(Pos.x, Pos.y, Pos.z, 1.0f);
                            
                            printf("New pos %d %d\n", (i32)NewPos.x, (i32)NewPos.y);
                            
                            GameState->EditorState.TileBrushWidthField->Active = true;
                            GameState->EditorState.TileBrushHeightField->Active = true;
                            
                            sscanf(GameState->EditorState.TileBrushWidthField->Text, "%f", &GameState->EditorState.TileBrushSize.x);
                            sscanf(GameState->EditorState.TileBrushHeightField->Text, "%f", &GameState->EditorState.TileBrushSize.y);
                            
                            GameState->EditorState.TileIsSolidCheckbox->Active = true;
                            
                            GameState->EditorState.TileX = (r32)X;
                            GameState->EditorState.TileZ = (r32)Z;
                            
                            if(MOUSE(Mouse_Left))
                            {
                                texture_data* Texture = Renderer.TextureMap[GameState->CurrentLevel.Tilemap.TextureName];
                                
                                i32 TilesheetWidth = Texture->Width / GameState->CurrentLevel.Tilemap.TileWidth;
                                i32 TilesheetHeight = Texture->Height / GameState->CurrentLevel.Tilemap.TileHeight;
                                
                                r32 ToolbarX = GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x;
                                r32 ToolbarY = GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y;
                                
                                if(InputController->MouseX >= ToolbarX && InputController->MouseX <= ToolbarX + TilesheetWidth * GameState->EditorState.RenderedTileSize && Renderer.WindowHeight - InputController->MouseY > ToolbarY && Renderer.WindowHeight - InputController->MouseY <=  ToolbarY + TilesheetHeight * GameState->EditorState.RenderedTileSize)
                                {
                                    i32 X = (i32)((InputController->MouseX - (GameState->EditorState.ToolbarX + GameState->EditorState.TilemapOffset.x)) / GameState->EditorState.RenderedTileSize);
                                    i32 Y = (i32)((Renderer.WindowHeight - InputController->MouseY - (GameState->EditorState.ToolbarY + GameState->EditorState.TilemapOffset.y)) / GameState->EditorState.RenderedTileSize);
                                    
                                    GameState->EditorState.SelectedTilePosition = math::v2((r32)X, (r32)Y);
                                    i32 Selected = X + Y * TilesheetWidth;
                                    
                                    GameState->EditorState.SelectedTileType = Selected;
                                    GameState->EditorState.TileIsSolidCheckbox->Checked = GameState->CurrentLevel.Tilemap.Tiles[Selected].IsSolid;
                                }
                                else
                                {
                                    if(X >= 0 && X < (i32)GameState->CurrentLevel.Tilemap.Width 
                                       && Z >= 0 && Z < (i32)GameState->CurrentLevel.Tilemap.Height)
                                    {
                                        tilemap* Tilemap = &GameState->CurrentLevel.Tilemap;
                                        
                                        if(KEY(Key_F))
                                        {
                                            for(i32 IndexX = 0; IndexX < Tilemap->Width; IndexX++)
                                            {
                                                for(i32 IndexZ = 0; IndexZ < Tilemap->Height; IndexZ++)
                                                {
                                                    for(i32 X = 0; X < (i32)GameState->EditorState.TileBrushSize.x && X + IndexX < Tilemap->Width; X++)
                                                    {
                                                        for(i32 Z = 0; Z < (i32)GameState->EditorState.TileBrushSize.y && Z + IndexZ < Tilemap->Height; Z++)
                                                        {
                                                            Tilemap->Data[GameState->EditorState.CurrentTilemapLayer][IndexX + X][IndexZ + Z] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
                                                        }
                                                    }
                                                    
                                                    Tilemap->RenderInfo.Dirty = true;
                                                    LoadTilemapBuffer(Renderer, *Tilemap);
                                                }
                                            }
                                        }
                                        else
                                        {
                                            
                                            for(i32 IndexZ = 0; IndexZ < Tilemap->Height; IndexZ++)
                                            {
                                                for(i32 RelativeX = 0; RelativeX < GameState->EditorState.TileBrushSize.x && RelativeX + X < Tilemap->Width; RelativeX++)
                                                {
                                                    for(i32 RelativeZ = 0; RelativeZ < GameState->EditorState.TileBrushSize.y && RelativeZ + Z < Tilemap->Height; RelativeZ++)
                                                    {
                                                        Tilemap->Data[GameState->EditorState.CurrentTilemapLayer][X + RelativeX][Z + RelativeZ] = Tilemap->Tiles[GameState->EditorState.SelectedTileType + 1];
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
                                if(X >= 0 && X < (i32)GameState->CurrentLevel.Tilemap.Width && Z >= 0 && Z < (i32)GameState->CurrentLevel.Tilemap.Height)
                                {
                                    GameState->CurrentLevel.Tilemap.Data[GameState->EditorState.CurrentTilemapLayer][X][Z].TypeIndex = -1;
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
                    Renderer.Cameras[GameState->EditorCameraHandle].Zoom += (r32)InputController->ScrollY * GameState->EditorState.ZoomingSpeed * (r32)DeltaTime * Renderer.Cameras[GameState->EditorCameraHandle].Zoom;
                    Renderer.Cameras[GameState->EditorCameraHandle].Zoom = Max(Min(Renderer.Cameras[GameState->EditorCameraHandle].Zoom, GameState->EditorState.MaxZoom), GameState->EditorState.MinZoom);
                }
                
                if(MOUSE(Mouse_Right))
                {
                    if(GameState->EditorState.LastKnownMouseX == 0 && GameState->EditorState.LastKnownMouseY == 0)
                    {
                        GameState->EditorState.LastKnownMouseX = (r32)InputController->MouseX;
                        GameState->EditorState.LastKnownMouseY = (r32)InputController->MouseY;
                    }
                    
                    math::v2 Direction = math::v2(InputController->MouseX - GameState->EditorState.LastKnownMouseX, InputController->MouseY - GameState->EditorState.LastKnownMouseY);
                    
                    auto& EditorCamera = Renderer.Cameras[GameState->EditorCameraHandle];
                    EditorCamera.Center -= math::v3(Direction.x / EditorCamera.Zoom * GameState->EditorState.PanningSpeed * DeltaTime, Direction.y / EditorCamera.Zoom * -GameState->EditorState.PanningSpeed * DeltaTime, 0);
                    
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
                            
                            // @Incomplete
                            //GameState->EditorState.LoadedAnimation->Texture = GameState->EditorState.SelectedTexture];
                            
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
                            
                            SaveAnimationToFile(GameState, *GameState->EditorState.LoadedAnimation, Renderer);
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
                        
                        texture_data* TextureData = Renderer.TextureMap[LoadedAnimation->Texture];
                        
                        while(FrameIndex < (i32)LoadedAnimation->FrameCount)
                        {
                            if(FrameIndex > 0)
                            {
                                if(X + (i32)LoadedAnimation->FrameSize.x < TextureData->Width)
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