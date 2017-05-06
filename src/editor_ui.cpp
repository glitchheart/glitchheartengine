static void CheckEditorUIInput(game_state* GameState, real64 DeltaTime)
{
    real32 SX = 2.0f / GameState->RenderState.WindowWidth;
    real32 SY = 2.0f / GameState->RenderState.WindowHeight;
    
    auto MouseX = GameState->InputController.MouseX;
    auto MouseY = GameState->InputController.MouseY;
    
    if(MouseX < GameState->RenderState.WindowWidth / 12)
    {
        auto Y = (MouseY / GameState->RenderState.WindowHeight) - 0.3 + 50 * SY;
        
        uint32 YAsIndex = (uint32) ((Y - 0.03) / 0.06);
        
        if(YAsIndex < GameState->EntityCount)
        {
            GameState->EditorUI.SelectedIndex = YAsIndex;
        }
    }
}