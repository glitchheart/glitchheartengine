static void SaveAnimationToFile(game_state* GameState, const animation& Animation, renderer& Renderer)
{
    FILE* File;
    
    File = fopen(Concat(Concat("../assets/animations/", Animation.Name), ".pownim"), "w");
    if (File)
    {
        texture_data* TextureData = Renderer.TextureMap[Animation.Texture];
        
        fprintf(File, "name %s\n", Animation.Name);
        fprintf(File, "type sprite\n");
        fprintf(File, "framecount %d\n", Animation.FrameCount);
        fprintf(File, "framesize %d %d\n", (i32)Animation.FrameSize.x, (i32)Animation.FrameSize.y);
        fprintf(File, "center %f %f\n", Animation.Center.x, Animation.Center.y);
        fprintf(File, "loop %d\n", Animation.Loop);
        fprintf(File, "timeperframe %f\n", Animation.TimePerFrame);
        fprintf(File, "frames\n");
        
        i32 X = (i32)(Animation.FrameOffset.y) * (i32)Animation.FrameSize.x;
        i32 Y = (i32)(Animation.FrameOffset.y) * (i32)Animation.FrameSize.y;
        
        i32 FrameIndex = 0;
        
        while (FrameIndex < (i32)Animation.FrameCount)
        {
            fprintf(File, "%d %d\n", X, Y);
            
            FrameIndex++;
            
            if (X + (i32)Animation.FrameSize.x <= TextureData->Width)
                X += (i32)Animation.FrameSize.x;
            else
            {
                X = 0;
                Y += (i32)Animation.FrameSize.y;
            }
        }
        
        fprintf(File, "texture %s\n", Animation.Texture);
        
        GameState->AnimationArray[GameState->AnimationIndex] = Animation;
        GameState->AnimationMap[GameState->AnimationArray[GameState->AnimationIndex].Name] = &GameState->AnimationArray[GameState->AnimationIndex];
        
        GameState->AnimationIndex++;
        
        fclose(File);
    }
}

static void LoadAnimationFromFile(const char* FilePath, game_state* GameState, renderer& Renderer)
{
    animation Animation;
    
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    Animation.Name = PushString(&GameState->TotalArena, 30);
    
    if (File)
    {
        //name
        if (fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "name %s", Animation.Name);
        }
        
        //type
        if (fgets(LineBuffer, 255, File))
        {
            //TODO(Daniel) write this when we have transformation animations
            //sscanf(LineBuffer, "name %s", Animation.Name);
        }
        
        //framecount
        if (fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "framecount %d", &Animation.FrameCount);
        }
        
        //framesize
        if (fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "framesize %f %f", &Animation.FrameSize.x, &Animation.FrameSize.y);
        }
        
        if (fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "center %f %f", &Animation.Center.x, &Animation.Center.y);
        }
        
        //loop
        if (fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "loop %d", &Animation.Loop);
        }
        
        //timeperframe
        if (fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "timeperframe %f", &Animation.TimePerFrame);
        }
        
        //frames
        fgets(LineBuffer, 255, File);
        
        Animation.Frames = PushArray(&GameState->TotalArena, Animation.FrameCount, sprite_sheet_frame);
        
        for (i32 i = 0; i < Animation.FrameCount; i++)
        {
            r32 OffsetX = 0.0f;
            r32 OffsetY = 0.0f;
            
            if (fgets(LineBuffer, 255, File))
            {
                sscanf(LineBuffer, "%f %f", &OffsetX, &OffsetY);
                Animation.Frames[i] = { OffsetX, OffsetY };
            }
        }
        
        //texturepath
        if (fgets(LineBuffer, 255, File))
        {
            char TextureNameBuffer[255];
            int Length;
            
            sscanf(LineBuffer, "texture %s%n", TextureNameBuffer, &Length);
            
            char* TextureName = PushString(&GameState->TotalArena, 70);
            strcpy(TextureName, TextureNameBuffer);
            
            if (strcmp(TextureName, "") != 0 && Renderer.TextureMap[TextureName])
            {
                Copy(&GameState->TotalArena, Animation.Texture, TextureName, 70 * sizeof(char), char);
            }
            else
            {
                DEBUG_PRINT("Texture: '%s' could not be found. Animation '%s' will not be loaded. Please add the missing texture.\n", TextureName, Animation.Name);
                return;
            }
        }
        
        GameState->AnimationArray[GameState->AnimationIndex] = Animation;
        
        GameState->AnimationMap[GameState->AnimationArray[GameState->AnimationIndex].Name] = &GameState->AnimationArray[GameState->AnimationIndex];
        
        GameState->AnimationIndex++;
        fclose(File);
    }
    else
        DEBUG_PRINT("Animation-file not loaded: '%s'\n", FilePath);
}

static void LoadAnimations(game_state* GameState, renderer& Renderer)
{
    animation_Map_Init(&GameState->AnimationMap, HashStringJenkins, 2048);
    directory_data DirData;
    Platform.GetAllFilesWithExtension("../assets/animations/", "pownim", &DirData, false);
    
    for (i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        LoadAnimationFromFile(DirData.FilePaths[FileIndex], GameState, Renderer);
    }
}

static inline void PlayAnimation(entity* Entity, char* AnimationName, game_state* GameState)
{
    if (!Entity->CurrentAnimation || !Entity->CurrentAnimation->Name || strcmp(Entity->CurrentAnimation->Name, AnimationName) != 0 || !Entity->AnimationInfo.Playing)
    {
        if (GameState->AnimationMap[AnimationName])
        {
            Entity->CurrentAnimation = GameState->AnimationMap[AnimationName];
            Entity->AnimationInfo.Playing = true;
            Entity->AnimationInfo.FrameIndex = 0;
            Entity->AnimationInfo.CurrentTime = 0.0;
        }
        else
        {
            Entity->CurrentAnimation = 0;
            Entity->AnimationInfo.Playing = false;
            Entity->AnimationInfo.FrameIndex = 0;
            Entity->AnimationInfo.CurrentTime = 0.0;
        }
    }
}

static inline void PlayAnimation(object_entity* Object, char* AnimationName, game_state* GameState)
{
    if (!Object->CurrentAnimation || !Object->CurrentAnimation->Name || strcmp(Object->CurrentAnimation->Name, AnimationName) != 0 || !Object->AnimationInfo.Playing)
    {
        if (GameState->AnimationMap[AnimationName])
        {
            Object->CurrentAnimation = GameState->AnimationMap[AnimationName];
            Object->AnimationInfo.Playing = true;
            Object->AnimationInfo.FrameIndex = 0;
            Object->AnimationInfo.CurrentTime = 0.0;
        }
        else
        {
            Object->CurrentAnimation = 0;
            Object->AnimationInfo.Playing = false;
            Object->AnimationInfo.FrameIndex = 0;
            Object->AnimationInfo.CurrentTime = 0.0;
        }
    }
}

static inline void StopAnimation(animation_info* Info)
{
    Info->Playing = false;
}

static void TickAnimation(animation_info* Info, animation* Animation, r64 DeltaTime)
{
    if (Animation)
    {
        if (Info->Playing)
        {
            Info->CurrentTime += DeltaTime;
            if (Info->CurrentTime >= Animation->TimePerFrame)
            {
                Info->FrameIndex++;
                Info->CurrentTime = 0.0;
                
                if (Info->FrameIndex >= Animation->FrameCount)
                {
                    if (!Info->FreezeFrame)
                        Info->FrameIndex = 0;
                    else
                        Info->FrameIndex = Animation->FrameCount - 1;
                    
                    if (!Animation->Loop)
                    {
                        StopAnimation(Info);
                    }
                }
            }
        }
    }
}