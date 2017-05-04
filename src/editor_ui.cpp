static void CheckEditorUIInput(game_state* GameState, real32 DeltaTime)
{
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    real32 SX = 2.0f / Mode->width;
    real32 SY = 2.0f / Mode->height;
    
    auto MouseX = GameState->InputController.MouseX;
    auto MouseY = GameState->InputController.MouseY;
    
    auto Y = (MouseY / GameState->RenderState.WindowHeight) - 0.3 + 50 * SY;
    
    uint32 YAsIndex = (uint32) ((Y - 0.03) / 0.06);
    
    if(YAsIndex < GameState->EntityCount)
    {
        GameState->EditorUI.SelectedIndex = YAsIndex;
    }
}