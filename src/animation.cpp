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
        
        for(uint32 i = 0; i < Animation->FrameCount; i++)
        {
            real32 OffsetX;
            real32 OffsetY;
            
            if(fgets(LineBuffer, 255, File))
            {
                sscanf(LineBuffer, "{%f,%f}", &OffsetX, &OffsetY);
                Animation->Frames[i] = { OffsetX, OffsetY };
            }
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

#endif

static void PlayAnimation(animation* Animation)
{
    Animation->Playing = true;
}


static void PlayAnimation(entity* Entity, char* AnimationName)
{
    if(!Entity->CurrentAnimation || strcmp(Entity->CurrentAnimation, AnimationName) != 0)
    {
        animation* Animation = &Entity->Animations[AnimationName];
        Animation->Playing = true;
        Animation->FrameIndex = 0;
        Animation->CurrentTime = 0.0;
        Entity->CurrentAnimation = AnimationName;
    }
}
static void StopAnimation(animation* Animation)
{
    Animation->Playing = false;
}

static void TickAnimation(animation* Animation, real64 DeltaTime)
{
    if(Animation->Playing)
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
                    StopAnimation(Animation);
                }
            }
        }
        Animation->CurrentTime += DeltaTime;
    }
}



