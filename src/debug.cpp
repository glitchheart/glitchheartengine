static void MoveRect(renderer& Renderer, debug_rect* DebugRect, input_controller* InputController)
{
    r32 CurrentY = DebugRect->RectOrigin.y;
    r32 CurrentX = DebugRect->RectOrigin.x;
    
    r32 Width = DebugRect->RectSize.x;
    r32 Height = DebugRect->RectSize.y;
    
    auto X = InputController->MouseX;
    auto Y = Renderer.WindowHeight - InputController->MouseY;
    
    if(MOUSE_UP(Mouse_Left))
    {
        DebugRect->Selected = false;
    }
    
    if(MOUSE_DOWN(Mouse_Left))
    {
        if(X > CurrentX && X < CurrentX + Width && Y > CurrentY - Height && Y < CurrentY)
        {
            DebugRect->Selected = true;
        }
    }
    
    if(MOUSE(Mouse_Left) && DebugRect->Selected)
    {
        //@Incomplete: Assuming middle drag. Not perfect
        DebugRect->RectOrigin.x += (r32)X - CurrentX - (Width / 2.0f);
        DebugRect->RectOrigin.y += (r32)Y - CurrentY + (Height / 2.0f);
    }
    
}

static void PushMemoryDebug(renderer& Renderer, debug_memory_info* DebugMemoryInfo, input_controller* InputController)
{
    debug_rect* DebugRect = &DebugMemoryInfo->DebugRect;
    
    auto TotalHeight = 0.0f;
    
    for(i32 I = 0; I < DebugMemoryInfo->DebugInfoCount; I++)
    {
        TotalHeight += 32.0f;
        TotalHeight += 32.0f * DebugMemoryInfo->DebugInfo[I].DebugValueCount;
    }
    
    r32 Height = Max(DebugRect->RectSize.y, TotalHeight);
    DebugRect->RectSize.y = Height;
    
    MoveRect(Renderer, DebugRect, InputController);
    
    r32 CurrentX = DebugRect->RectOrigin.x;
    r32 CurrentY = DebugRect->RectOrigin.y;
    r32 Width = DebugRect->RectSize.x;
    
    r32 Offset = 30;
    
    auto OriginalY = DebugRect->RectOrigin.y;
    
    CurrentY -= Offset;
    
    CurrentX += 5.0f;
    
    PushFilledRect(Renderer, math::v3(CurrentX, OriginalY - Height, 2), math::v3(Width, Height, 0.0f), math::rgba(0,1,0,0.2));
    
    for(i32 InfoIndex = 0; InfoIndex < DebugMemoryInfo->DebugInfoCount; InfoIndex++)
    {
        auto Info = DebugMemoryInfo->DebugInfo[InfoIndex];
        char DebugBuffer[255];
        sprintf(DebugBuffer, Info.Header);
        PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
        
        for(i32 ValueIndex = 0; ValueIndex < Info.DebugValueCount; ValueIndex++)
        {
            CurrentY -= Offset;
            debug_value Next = Info.DebugValues[ValueIndex];
            switch(Next.Type)
            {
                case DB_Float:
                {
                    sprintf(DebugBuffer, Next.Format, Next.Float.Value);
                } break;
                case DB_Int:
                {
                    sprintf(DebugBuffer, Next.Format, Next.Int.Value);
                } break;
                case DB_U64:
                {
                    sprintf(DebugBuffer, Next.Format, Next.U64.Value);
                } break;
            }
            
            PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
            
        }
        CurrentY -= Offset;
    }
}


static void PushDebugRender(renderer& Renderer, debug_state* DebugState, input_controller* InputController)
{
    
    if(DebugState->DebugMemory)
    {
        PushMemoryDebug(Renderer, &DebugState->DebugMemoryInfo, InputController);
    }
    
}


