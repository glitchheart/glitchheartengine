static void SetEditorFields(game_state* GameState)
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
    GameState->EditorState.AnimationLoopField = &GameState->EditorState.Textfields[7];
    
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
    
    GameState->EditorState.AnimationFrameDurationField->Active = true;
    GameState->EditorState.AnimationFrameDurationField->Size = glm::vec2(300, 30);
    GameState->EditorState.AnimationFrameDurationField->ScreenPosition = glm::vec2(GameState->RenderState.WindowWidth - 305, 440);
    GameState->EditorState.AnimationFrameDurationField->Label = "Loop";
    GameState->EditorState.AnimationFrameDurationField->Type = Textfield_Integer;
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
        
        GameState->EditorState.LoadedAnimation->Texture = &GameState->RenderState.Textures[GameState->EditorState.Textures[GameState->EditorState.SelectedTexture]];
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
            SetEditorFields(GameState);
            
            sprintf(GameState->EditorState.AnimationNameField->Text, "%s", LoadedAnimation->Name);
            sprintf(GameState->EditorState.AnimationFrameCountField->Text, "%d", LoadedAnimation->FrameCount);
            sprintf(GameState->EditorState.AnimationFrameWidthField->Text, "%f", LoadedAnimation->FrameSize.x);
            sprintf(GameState->EditorState.AnimationFrameHeightField->Text, "%f", LoadedAnimation->FrameSize.y);
            sprintf(GameState->EditorState.AnimationFrameOffsetXField->Text, "%f", LoadedAnimation->FrameOffset.x);
            sprintf(GameState->EditorState.AnimationFrameOffsetYField->Text, "%f", LoadedAnimation->FrameOffset.y);
            sprintf(GameState->EditorState.AnimationFrameDurationField->Text, "%f", LoadedAnimation->TimePerFrame);
            sprintf(GameState->EditorState.AnimationLoopField->Text, "%f", GameState->EditorState.ShouldLoop);
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