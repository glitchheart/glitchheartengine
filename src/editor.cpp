static void DeleteEntity(game_state* GameState, uint32 EntityIndex)
{
    uint32 RenderEntityHandle = GameState->EditorState.SelectedEntity->RenderEntityHandle;
    
    for(uint32 RenderIndex = RenderEntityHandle; RenderIndex < GameState->RenderState.RenderEntityCount - 1; RenderIndex++)
    {
        GameState->RenderState.RenderEntities[RenderIndex] = GameState->RenderState.RenderEntities[RenderIndex + 1];
        GameState->RenderState.RenderEntities[RenderIndex].Entity->RenderEntityHandle = RenderIndex;
    }
    
    GameState->RenderState.RenderEntityCount--;
    
    GameState->EditorState.SelectedEntity = 0;
    
    for(uint32 Index = EntityIndex; Index < GameState->EntityCount - 1; Index++)
    {
        GameState->Entities[Index] = GameState->Entities[Index + 1];
        GameState->Entities[Index].EntityIndex = Index;
    }
    GameState->EntityCount--;
}

static void CheckEditorUIInput(game_state* GameState, real64 DeltaTime)
{
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
        printf("Bibba\n");
        GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorUI.SelectedIndex].IsSolid = !GameState->CurrentLevel.Tilemap.Tiles[GameState->EditorUI.SelectedIndex].IsSolid; 
    }
}