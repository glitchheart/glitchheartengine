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

static void PlayAnimation(animation* Animation)
{
    Animation->Playing = true;
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
            if(!Animation->Loop)
            {
                StopAnimation(Animation);
            }
        }
    }
    Animation->CurrentTime += DeltaTime;
}




