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
    GameState->EditorState.AnimationLoopCheckbox = &GameState->EditorState.Checkboxes[0];
    
    GameState->EditorState.AnimationNameField->Active = true;
    GameState->EditorState.AnimationNameField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationNameField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 860);
    GameState->EditorState.AnimationNameField->Label = "Name";
    
    GameState->EditorState.AnimationFrameWidthField->Active = true;
    GameState->EditorState.AnimationFrameWidthField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameWidthField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 800);
    GameState->EditorState.AnimationFrameWidthField->Label = "Frame width";
    GameState->EditorState.AnimationFrameWidthField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameHeightField->Active = true;
    GameState->EditorState.AnimationFrameHeightField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameHeightField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 740);
    GameState->EditorState.AnimationFrameHeightField->Label = "Frame height";
    GameState->EditorState.AnimationFrameHeightField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameCountField->Active = true;
    GameState->EditorState.AnimationFrameCountField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameCountField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 680);
    GameState->EditorState.AnimationFrameCountField->Label = "Frame count";
    GameState->EditorState.AnimationFrameCountField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameOffsetXField->Active = true;
    GameState->EditorState.AnimationFrameOffsetXField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameOffsetXField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 620);
    GameState->EditorState.AnimationFrameOffsetXField->Label = "Frame offset x";
    GameState->EditorState.AnimationFrameOffsetXField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameOffsetYField->Active = true;
    GameState->EditorState.AnimationFrameOffsetYField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameOffsetYField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 560);
    GameState->EditorState.AnimationFrameOffsetYField->Label = "Frame offset y";
    GameState->EditorState.AnimationFrameOffsetYField->Type = Textfield_Integer;
    
    GameState->EditorState.AnimationFrameDurationField->Active = true;
    GameState->EditorState.AnimationFrameDurationField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameDurationField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 500);
    GameState->EditorState.AnimationFrameDurationField->Label = "Frame  duration";
    GameState->EditorState.AnimationFrameDurationField->Type = Textfield_Decimal;
    
    GameState->EditorState.AnimationLoopCheckbox->Active = true;
    GameState->EditorState.AnimationLoopCheckbox->Checked = false;
    GameState->EditorState.AnimationLoopCheckbox->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 440);
    GameState->EditorState.AnimationLoopCheckbox->Label = "Loop";
}

static void SetFieldValues(game_state* GameState)
{
    switch(GameState->EditorState.Mode)
    {
        case Editor_Animation:
        {
            animation* LoadedAnimation = GameState->EditorState.LoadedAnimation;
            
            sprintf(GameState->EditorState.AnimationNameField->Text, "%s", LoadedAnimation->Name);
            sprintf(GameState->EditorState.AnimationFrameCountField->Text, "%d", LoadedAnimation->FrameCount);
            sprintf(GameState->EditorState.AnimationFrameWidthField->Text, "%d", (int32)LoadedAnimation->FrameSize.x);
            sprintf(GameState->EditorState.AnimationFrameHeightField->Text, "%d", (int32)LoadedAnimation->FrameSize.y);
            sprintf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%d", (int32)LoadedAnimation->FrameOffset.x);
            sprintf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%d", (int32)LoadedAnimation->FrameOffset.y);
            sprintf(GameState->EditorState.AnimationFrameDurationField->Text, "%3.4f", LoadedAnimation->TimePerFrame);
            GameState->EditorState.AnimationLoopCheckbox->Checked =
                GameState->EditorState.ShouldLoop;
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
    GameState->EditorState.ToolbarX = (real32)RenderState->WindowWidth - 100;
    GameState->EditorState.ToolbarY = 0;
    GameState->EditorState.ToolbarWidth = 100.0f;
    GameState->EditorState.ToolbarHeight = (real32)RenderState->WindowHeight;
    
    GameState->EditorState.Buttons[0].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[0].Text = "Create/Edit animation";
    GameState->EditorState.Buttons[0].ScreenPosition = glm::vec2(5, (real32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[0].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[0].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[0].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[0].Active = true;
    GameState->EditorState.Buttons[0].EditorType = Button_Animation;
    GameState->EditorState.Buttons[0].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[0].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[0].ClickAnimationTimer->TimerHandle = -1;
    
    GameState->EditorState.Buttons[1].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[1].Text = "Create/Edit tilesheet";
    GameState->EditorState.Buttons[1].ScreenPosition = glm::vec2(330, (real32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[1].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[1].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[1].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[1].Active = true;
    GameState->EditorState.Buttons[1].EditorType = Button_Tilesheet;
    GameState->EditorState.Buttons[1].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[1].ClickAnimationTimer->TimerMax = 0.2f;GameState->EditorState.Buttons[1].ClickAnimationTimer->TimerHandle = -1;
    
    GameState->EditorState.Buttons[2].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[2].Text = "Switch mode";
    GameState->EditorState.Buttons[2].ScreenPosition = glm::vec2(655, (real32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[2].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[2].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[2].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[2].Active = true;
    GameState->EditorState.Buttons[2].EditorType = Button_SwitchMode;
    GameState->EditorState.Buttons[2].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[2].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[2].ClickAnimationTimer->TimerHandle = -1;
    
    GameState->EditorState.Buttons[3].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[3].Text = "Save and exit";
    GameState->EditorState.Buttons[3].ScreenPosition = glm::vec2(980, (real32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[3].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[3].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[3].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[3].Active = true;
    GameState->EditorState.Buttons[3].EditorType = Button_SaveAndExit;
    GameState->EditorState.Buttons[3].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[3].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[3].ClickAnimationTimer->TimerHandle = -1;
    
    GameState->EditorState.Buttons[4].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[4].Text = "Exit";
    GameState->EditorState.Buttons[4].ScreenPosition = glm::vec2(1305, (real32)RenderState->WindowHeight - 150);
    GameState->EditorState.Buttons[4].Size = glm::vec2(280, 60);
    GameState->EditorState.Buttons[4].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[4].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[4].Active = true;
    GameState->EditorState.Buttons[4].EditorType = Button_Exit;
    GameState->EditorState.Buttons[4].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[4].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[4].ClickAnimationTimer->TimerHandle = -1;
    
    GameState->EditorState.Buttons[5].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[5].Text = "Create new";
    GameState->EditorState.Buttons[5].ScreenPosition = glm::vec2(20, 20);
    GameState->EditorState.Buttons[5].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[5].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[5].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[5].Active = true;
    GameState->EditorState.Buttons[5].EditorType = Button_Animation;
    GameState->EditorState.Buttons[5].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[5].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[5].ClickAnimationTimer->TimerHandle = -1;
    GameState->EditorState.CreateNewAnimationButton = &GameState->EditorState.Buttons[5];
    
    GameState->EditorState.Buttons[6].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[6].Text = "Save animation";
    GameState->EditorState.Buttons[6].ScreenPosition = glm::vec2(700, 20);
    GameState->EditorState.Buttons[6].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[6].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[6].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[6].Active = false;
    GameState->EditorState.Buttons[6].EditorType = Button_Animation;
    GameState->EditorState.Buttons[6].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[6].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[6].ClickAnimationTimer->TimerHandle = -1;
    GameState->EditorState.SaveAnimationButton = &GameState->EditorState.Buttons[6];
    
    GameState->EditorState.Buttons[7].Text = (char*)malloc(sizeof(char) * 20);
    GameState->EditorState.Buttons[7].Text = "Create new level";
    GameState->EditorState.Buttons[7].ScreenPosition = glm::vec2(700, 20);
    GameState->EditorState.Buttons[7].Size = glm::vec2(320, 60);
    GameState->EditorState.Buttons[7].Color = glm::vec4(1.0f / 255.0f * 154.0f, 1.0f / 255.0f * 51.0f, 1.0f / 255.0f * 52.0f, 1);
    GameState->EditorState.Buttons[7].TextColor = glm::vec4(1, 1, 1, 1);
    GameState->EditorState.Buttons[7].Active = true;
    GameState->EditorState.Buttons[7].EditorType = Button_CreateLevel;
    GameState->EditorState.Buttons[7].ClickAnimationTimer = (timer*)malloc(sizeof(timer));
    GameState->EditorState.Buttons[7].ClickAnimationTimer->TimerMax = 0.2f;
    GameState->EditorState.Buttons[7].ClickAnimationTimer->TimerHandle = -1;
    GameState->EditorState.CreateNewLevelButton = &GameState->EditorState.Buttons[7];
    
    // Tile editor
    GameState->EditorState.TileIsSolidCheckbox = &GameState->EditorState.Checkboxes[1];
    GameState->EditorState.TileIsSolidCheckbox->Active = false;
    GameState->EditorState.TileIsSolidCheckbox->Checked = false;
    GameState->EditorState.TileIsSolidCheckbox->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 200, 200);
    GameState->EditorState.TileIsSolidCheckbox->Label = "Is solid";
    
    GameState->EditorState.Loaded = true;
}

static void CheckEditorUIInput(game_state* GameState, real64 DeltaTime)
{
    if(GameState->EditorState.LoadedAnimation && !GameState->EditorState.Editing)
    {
        if(GetKeyDown(Key_Down, GameState))
        {
            GameState->EditorState.SelectedTexture++;
        }
        else if(GetKeyDown(Key_Up, GameState))
        {
            GameState->EditorState.SelectedTexture--;
        }
        
        if(GameState->EditorState.SelectedTexture == GameState->EditorState.TexturesLength)
            GameState->EditorState.SelectedTexture = 0;
        else if(GameState->EditorState.SelectedTexture < 0)
            GameState->EditorState.SelectedTexture = (int32)GameState->EditorState.TexturesLength - 1;
        
        GameState->EditorState.LoadedAnimation->Texture = GameState->RenderState.Textures[GameState->EditorState.Textures[GameState->EditorState.SelectedTexture]];
    }
    else
    {
        bool32 Changed = false;
        if(GetKeyDown(Key_Down, GameState))
        {
            GameState->EditorState.SelectedAnimation++;
            Changed = true;
        }
        else if(GetKeyDown(Key_Up, GameState))
        {
            GameState->EditorState.SelectedAnimation--;
            Changed = true;
        }
        
        if(GameState->EditorState.SelectedAnimation == GameState->Animations.size())
            GameState->EditorState.SelectedAnimation = 0;
        else if(GameState->EditorState.SelectedAnimation < 0)
            GameState->EditorState.SelectedAnimation = (int32)GameState->Animations.size() - 1;
        
        if(Changed)
        {
            GameState->EditorState.LoadedAnimation = &GameState->Animations[GameState->EditorState.Animations[GameState->EditorState.SelectedAnimation]];
            GameState->EditorState.ShouldLoop = GameState->EditorState.LoadedAnimation->Loop;
            
            animation* LoadedAnimation = GameState->EditorState.LoadedAnimation;
            GameState->EditorState.LoadedAnimation->Loop = 1;
            InitEditorFields(GameState);
            SetFieldValues(GameState);
        }
    }
    
    GameState->InputController.DeleteCharacter = GetKeyDown(Key_Backspace, GameState);
    
    real32 SX = 2.0f / GameState->RenderState.WindowWidth;
    real32 SY = 2.0f / GameState->RenderState.WindowHeight;
    
    auto MouseX = GameState->InputController.MouseX;
    auto MouseY = GameState->InputController.MouseY;
    
    int ListRectWidth = 120;
    
    
    if(GameState->EditorState.SelectedEntity && GetKeyDown(Key_Delete,GameState))
    {
        DeleteEntity(GameState,GameState->EditorState.SelectedEntity->EntityIndex);
    }
    
    if(MouseX <= ListRectWidth)
    {
        auto Y = GameState->RenderState.WindowHeight - MouseY;
        
        auto Index = (int32)floor(((real32)GameState->RenderState.WindowHeight / 2.0f - (real32)Y) / 20.0f + 1.0f);
        
        if(Index >= 0 && Index < GameState->EntityCount)
            GameState->EditorUI.SelectedIndex = Index;
    }
    
    if((GetKey(Key_LeftCtrl, GameState) || GetKey(Key_RightCtrl, GameState)) && GetKeyDown(Key_I, GameState))
    {
        
        GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorUI.SelectedIndex].IsSolid = !GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorUI.SelectedIndex].IsSolid; 
    }
}