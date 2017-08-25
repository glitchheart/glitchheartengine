static void PushMemoryDebug(renderer& Renderer, debug_memory_info& DebugMemoryInfo)
{
    r32 CurrentY = 780;
    r32 Offset = 30;
    
    char DebugBuffer[255];
    sprintf(DebugBuffer, "Temporary memory");
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tBlocks: %d", DebugMemoryInfo.TempBlockCount);
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tTotal allocated: %llu bytes", DebugMemoryInfo.TempSizeAllocated);
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer, "Permanent memory");
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tBlocks: %d", DebugMemoryInfo.PermanentBlocks);
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tTotal allocated: %llu bytes", DebugMemoryInfo.PermanentSizeAllocated);
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer, "Total memory");
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(0, 0, 1, 1));
    
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tBlocks: %d", DebugMemoryInfo.BlocksAllocated);
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
    CurrentY -= Offset;
    
    sprintf(DebugBuffer,"\tTotal allocated: %llu bytes", DebugMemoryInfo.SizeAllocated);
    PushText(Renderer, DebugBuffer, math::v3(50, CurrentY, 2), 0, math::rgba(1, 0, 1, 1));
}


static void PushDebugRender(renderer& Renderer, debug_state& DebugState)
{
#if GLITCH_DEBUG
    if(DebugState.DebugMemory)
    {
        PushMemoryDebug(Renderer, DebugState.DebugMemoryInfo);
    }
    
#endif
}


