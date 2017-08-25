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
        DebugRect->RectOrigin.x += X - CurrentX - (Width / 2.0f);
        DebugRect->RectOrigin.y += Y - CurrentY + (Height / 2.0f);
    }
    
}

static void PushMemoryDebug(renderer& Renderer, debug_memory_info* DebugMemoryInfo, input_controller* InputController)
{
    debug_rect* DebugRect = &DebugMemoryInfo->DebugRect;
    
    MoveRect(Renderer, DebugRect, InputController);
    
    r32 CurrentX = DebugRect->RectOrigin.x;
    r32 CurrentY = DebugRect->RectOrigin.y;
    r32 Width = DebugRect->RectSize.x;
    r32 Height = DebugRect->RectSize.y;
    
    r32 Offset = 30;
    
    PushFilledRect(Renderer, math::v3(CurrentX, CurrentY - Height, 2), math::v3(Width,Height, 0.0f), math::rgba(0,1,0,0.2));
    
    CurrentY -= Offset;
    
    CurrentX += 5.0f;
    
    char DebugBuffer[255];
    sprintf(DebugBuffer, "Temporary memory");
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tBlocks: %d", DebugMemoryInfo->TempBlockCount);
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tTotal allocated: %llu bytes", DebugMemoryInfo->TempSizeAllocated);
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer, "Permanent memory");
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tBlocks: %d", DebugMemoryInfo->PermanentBlocks);
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tTotal allocated: %llu bytes", DebugMemoryInfo->PermanentSizeAllocated);
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer, "Total memory");
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tBlocks: %d", DebugMemoryInfo->BlocksAllocated);
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tTotal allocated: %llu bytes", DebugMemoryInfo->SizeAllocated);
    PushText(Renderer, DebugBuffer, math::v3(CurrentX, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
}


static void PushDebugRender(renderer& Renderer, debug_state* DebugState, input_controller* InputController)
{
#if GLITCH_DEBUG
    if(DebugState->DebugMemory)
    {
        PushMemoryDebug(Renderer, &DebugState->DebugMemoryInfo, InputController);
    }
    
#endif
}


