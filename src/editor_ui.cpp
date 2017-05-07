static void CheckEditorUIInput(game_state* GameState, real64 DeltaTime)
{
    real32 SX = 2.0f / GameState->RenderState.WindowWidth;
    real32 SY = 2.0f / GameState->RenderState.WindowHeight;
    
    auto MouseX = GameState->InputController.MouseX;
    auto MouseY = GameState->InputController.MouseY;
    
    int ListRectWidth = 120;
    
    if(MouseX <= ListRectWidth)
    {
        auto Y = GameState->RenderState.WindowHeight - MouseY;
        
        auto Index = (int32)floor(((real32)GameState->RenderState.WindowHeight / 2.0f - (real32)Y) / 20.0f + 1.0f);
        
        if(Index >= 0 && Index < GameState->EntityCount)
            GameState->EditorUI.SelectedIndex = Index;
    }
}