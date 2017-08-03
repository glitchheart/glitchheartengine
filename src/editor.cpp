static void InitEditorFields(game_state* GameState)
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
    GameState->EditorState.AnimationNameField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 220);
    GameState->EditorState.AnimationNameField->Label = "Name";
    
    GameState->EditorState.AnimationFrameWidthField->Active = false;
    GameState->EditorState.AnimationFrameWidthField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameWidthField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 280);
    GameState->EditorState.AnimationFrameWidthField->Label = "Frame width";
    GameState->EditorState.AnimationFrameWidthField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameHeightField->Active = false;
    GameState->EditorState.AnimationFrameHeightField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameHeightField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 340);
    GameState->EditorState.AnimationFrameHeightField->Label = "Frame height";
    GameState->EditorState.AnimationFrameHeightField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameCountField->Active = false;
    GameState->EditorState.AnimationFrameCountField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameCountField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 400);
    GameState->EditorState.AnimationFrameCountField->Label = "Frame count";
    GameState->EditorState.AnimationFrameCountField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameOffsetXField->Active = false;
    GameState->EditorState.AnimationFrameOffsetXField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameOffsetXField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 460);
    GameState->EditorState.AnimationFrameOffsetXField->Label = "Frame offset x";
    GameState->EditorState.AnimationFrameOffsetXField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameOffsetYField->Active = false;
    GameState->EditorState.AnimationFrameOffsetYField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameOffsetYField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 520);
    GameState->EditorState.AnimationFrameOffsetYField->Label = "Frame offset y";
    GameState->EditorState.AnimationFrameOffsetYField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameDurationField->Active = false;
    GameState->EditorState.AnimationFrameDurationField->Size = math::v2(300, 30);
    GameState->EditorState.AnimationFrameDurationField->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 580);
    GameState->EditorState.AnimationFrameDurationField->Label = "Frame duration";
    GameState->EditorState.AnimationFrameDurationField->Type = Textfield_Decimal;
    
    GameState->EditorState.AnimationLoopCheckbox->Active = false;
    GameState->EditorState.AnimationLoopCheckbox->Checked = false;
    GameState->EditorState.AnimationLoopCheckbox->ScreenPosition = math::v2(FieldX, GameState->RenderState.WindowHeight - 640);
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

static void CreateEditorButtons(game_state* GameState)
{
    render_state* RenderState = &GameState->RenderState;
    // @Incomplete: These values need to be updated when the window size is changed
    GameState->EditorState.TilemapOffset = math::v2(0, 300);
    GameState->EditorState.ToolbarX = 0;
    GameState->EditorState.ToolbarY = 0;
    GameState->EditorState.ToolbarWidth = 500.0f;
    GameState->EditorState.ToolbarHeight = (r32)RenderState->WindowHeight;
    
    GameState->EditorState.Buttons[0].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[0].Text = "Create/Edit animation";
    GameState->EditorState.Buttons[0].ScreenPosition = math::v2(5, (r32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[0].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[0].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[0].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[0].Active = false;
    GameState->EditorState.Buttons[0].EditorType = Button_Animation;
    GameState->EditorState.Buttons[0].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[0].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[1].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[1].Text = "Create/Edit tilesheet";
    GameState->EditorState.Buttons[1].ScreenPosition = math::v2(330, (r32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[1].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[1].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[1].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[1].Active = false;
    GameState->EditorState.Buttons[1].EditorType = Button_Tilesheet;
    GameState->EditorState.Buttons[1].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[1].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[2].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[2].Text = "Switch mode";
    GameState->EditorState.Buttons[2].ScreenPosition = math::v2(655, (r32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[2].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[2].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[2].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[2].Active = false;
    GameState->EditorState.Buttons[2].EditorType = Button_SwitchMode;
    GameState->EditorState.Buttons[2].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[2].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[3].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[3].Text = "Save and exit";
    GameState->EditorState.Buttons[3].ScreenPosition = math::v2(980, (r32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[3].Size = math::v2(320, 60);
    GameState->EditorState.Buttons[3].Color = math::v4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[3].TextColor = math::v4(1, 1, 1, 1);
    GameState->EditorState.Buttons[3].Active = false;
    GameState->EditorState.Buttons[3].EditorType = Button_SaveAndExit;
    GameState->EditorState.Buttons[3].ClickAnimationTimer.TimerMax = 0.2f;
    GameState->EditorState.Buttons[3].ClickAnimationTimer.TimerHandle = -1;
    
    GameState->EditorState.Buttons[4].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[4].Text = "Exit";
    GameState->EditorState.Buttons[4].ScreenPosition = math::v2(1305, (r32)RenderState->WindowHeight - 150);
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
    GameState->EditorState.Buttons[6].ScreenPosition = math::v2(10, GameState->RenderState.WindowHeight - 700);
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

static void CheckEditorUIInput(game_state* GameState, input_controller* InputController,  r64 DeltaTime)
{
    switch(GameState->EditorState.Mode)
    {
        case Editor_Animation:
        {
            if(GameState->EditorState.AnimationMode == Animation_SelectTexture)
            {
                if(KEY_DOWN(Key_Down))
                {
                    DEBUG_PRINT("Down\n");
                    GameState->EditorState.SelectedTexture++;
                }
                else if(KEY_DOWN(Key_Up))
                {
                    DEBUG_PRINT("Up\n");
                    GameState->EditorState.SelectedTexture--;
                }
                
                if(GameState->EditorState.SelectedTexture == GameState->RenderState.TextureIndex)
                    GameState->EditorState.SelectedTexture = 0;
                else if(GameState->EditorState.SelectedTexture < 0)
                    GameState->EditorState.SelectedTexture = (i32)GameState->RenderState.TextureIndex - 1;
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
    
    r32 SX = 2.0f / GameState->RenderState.WindowWidth;
    r32 SY = 2.0f / GameState->RenderState.WindowHeight;
    
    auto MouseX = InputController->MouseX;
    auto MouseY = InputController->MouseY;
    
    int ListRectWidth = 120;
    
    if(GameState->EditorState.SelectedEntity && KEY_DOWN(Key_Delete))
    {
        DeleteEntity(GameState,GameState->EditorState.SelectedEntity->EntityIndex);
        SaveLevelToFile(GameState->LevelPath, &GameState->CurrentLevel, GameState);
    }
}