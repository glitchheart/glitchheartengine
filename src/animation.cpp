#ifdef ANIMATION_LOADING
static void LoadAnimationFromFile(const char* FilePath, animation* Animation, render_state* RenderState)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    if(File)
    {
        //name
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "name %s", Animation->Name);
        }
        
        //type
        if(fgets(LineBuffer, 255, File))
        {
            //TODO(Daniel) write this when we have transformation animations
            //sscanf(LineBuffer, "name %s", Animation->Name);
        }
        
        //framecount
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "framecount %d", &Animation->FrameCount);
        }
        
        //loop
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "loop %d", &Animation->Loop);
        }
        
        //timeperframe
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "timeperframe %f", &Animation->TimePerFrame);
        }
        
        //rows
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "rows %d", &Animation->Rows);
        }
        
        //columns
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "columns %d", &Animation->Columns);
        }
        
        //frames
        fgets(LineBuffer, 255, File);
        
        Animation->Frames = (sprite_sheet_frame*)malloc(sizeof(sprite_sheet_frame) * Animation->FrameCount);
        
        for(int i = 0; i < Animation->FrameCount; i++)
        {
            real32 OffsetX;
            real32 OffsetY;
            
            if(fgets(LineBuffer, 255, File))
            {
                sscanf(LineBuffer, "{%f,%f}", &OffsetX, &OffsetY);
            }
            
            Animation->Frames[i] = { OffsetX, OffsetY };
        }
        
        char TexturePathBuffer[255];
        
        //texturepath
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "texturepath %s", TexturePathBuffer);
            
            if(RenderState->LoadedTextureHandles.find(&TexturePathBuffer[0]) != RenderState->LoadedTextureHandles.end())
            {
                Animation->TextureHandle = RenderState->LoadedTextureHandles[TexturePathBuffer];
            }
            else
            {
                Animation->TextureHandle = LoadTexture(&TexturePathBuffer[0]);
            }
        }
    }
}

static void LoadAnimations(animation* Animation, game_state* GameState)
{
    // Player animations
    uint32 FrameCount = 5;
    Animation->Loop = false;
    Animation->TimePerFrame = 0.1;
    Animation->Rows = 10;
    Animation->Columns = 1;
    
    Animation->Frames = (sprite_sheet_frame*)malloc(sizeof(sprite_sheet_frame) * FrameCount);
    Animation->Frames[0] = {0.0f,0.0f};
    Animation->Frames[1] = {0.1f,0.0f};
    Animation->Frames[2] = {0.2f,0.0f};
    Animation->Frames[3] = {0.3f,0.0f};
    Animation->Frames[4] = {0.4f,0.0f};
    
    Animation->FrameCount = FrameCount;
    Animation->FrameIndex = 0;
    Animation->CurrentTime = 0.0;
    
    Animation->Playing = false;
}
#endif

static void PlayAnimation(animation* Animation)
{
    Animation->Playing = true;
}


static void PlayAnimation(entity* Entity, char* AnimationName)
{
    animation* Animation = &Entity->Animations[AnimationName];
    Animation->Playing = true;
    Animation->FrameIndex = 0;
    Animation->CurrentTime = 0.0;
    Entity->CurrentAnimation = AnimationName;
    
}

static void StopAnimation(animation* Animation)
{
    Animation->Playing = false;
}

static void TickAnimation(animation* Animation, real32 DeltaTime)
{
    if(Animation->CurrentTime >= Animation->TimePerFrame)
    {
        Animation->FrameIndex++;
        Animation->CurrentTime = 0.0;
        if(Animation->FrameIndex == Animation->FrameCount)
        {
            Animation->FrameIndex = 0;
            if(Animation->Loop == 0)
            {
                printf("STOP\n");
                StopAnimation(Animation);
            }
        }
    }
    Animation->CurrentTime += DeltaTime;
}




