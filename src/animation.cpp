static void LoadAnimationFromFile(const char* FilePath, game_state* GameState)
{
    animation Animation;
    
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    Animation.Name = (char*)malloc(sizeof(char) * 30);
    
    if(File)
    {
        //name
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "name %s", Animation.Name);
        }
        
        //type
        if(fgets(LineBuffer, 255, File))
        {
            //TODO(Daniel) write this when we have transformation animations
            //sscanf(LineBuffer, "name %s", Animation.Name);
        }
        
        //framecount
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "framecount %d", &Animation.FrameCount);
        }
        
        //framesize
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "framesize %f %f", &Animation.FrameSize.x, &Animation.FrameSize.y);
        }
        
        //loop
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "loop %d", &Animation.Loop);
        }
        
        //timeperframe
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "timeperframe %f", &Animation.TimePerFrame);
        }
        
        //frames
        fgets(LineBuffer, 255, File);
        
        Animation.Frames = (sprite_sheet_frame*)malloc(sizeof(sprite_sheet_frame) * Animation.FrameCount);
        
        for(uint32 i = 0; i < Animation.FrameCount; i++)
        {
            real32 OffsetX = 0.0f;
            real32 OffsetY = 0.0f;
            
            if(fgets(LineBuffer, 255, File))
            {
                sscanf(LineBuffer, "%f %f", &OffsetX, &OffsetY);
                Animation.Frames[i] = { OffsetX, OffsetY };
            }
        }
        
        uint32 TextureIndex;
        
        //texturepath
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "texture %d", &TextureIndex);
            
            Animation.Texture = &GameState->RenderState.Textures[TextureIndex];
        }
        GameState->Animations.insert(std::pair<char*, animation>(Animation.Name, Animation));
    }
    else
        printf("Animation-file not loaded: '%s'\n", FilePath);
}

static void LoadAnimations(game_state* GameState)
{
    FILE* File = fopen("../assets/animations/.animations", "r");
    char LineBuffer[255];
    
    while(fgets(LineBuffer, 255, File))
    {
        LoadAnimationFromFile(Concat(Concat("../assets/animations/", LineBuffer), ".pownim"), GameState);
    }
    fclose(File);
    /*
    LoadAnimationFromFile("../assets/animations/enemy_anim_idle.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_walk.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_walk_up.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_walk_down.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_attack.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_hit.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/player/idle.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/idle_up.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/idle_left.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/idle_right.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/player/run_up.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/run_down.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/run_left.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/run_right.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/player/attack_up.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/attack_down.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/attack_left.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/player/attack_right.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/sword_attack.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/skeleton/skeleton_attack.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/skeleton/skeleton_idle.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/skeleton/skeleton_hit.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/skeleton/skeleton_death.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/skeleton/skeleton_walk.pownim", GameState);
    LoadAnimationFromFile("../assets/animations/skeleton/skeleton_react.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/blob/blob_walk.pownim", GameState);
    
    LoadAnimationFromFile("../assets/animations/explosion.pownim", GameState);
*/
}

static void PlayAnimation(entity* Entity, char* AnimationName, game_state* GameState)
{
    if(!Entity->CurrentAnimation || !Entity->CurrentAnimation->Name || strcmp(Entity->CurrentAnimation->Name, AnimationName) != 0)
    {
        Entity->CurrentAnimation = &GameState->Animations[AnimationName];
        Entity->AnimationInfo.Playing = true;
        Entity->AnimationInfo.FrameIndex = 0;
        Entity->AnimationInfo.CurrentTime = 0.0;
    }
}

static void StopAnimation(animation_info* Info)
{
    Info->Playing = false;
}

static void TickAnimation(animation_info* Info, animation* Animation, real64 DeltaTime)
{
    if(Animation) 
    {
        if(Info->Playing)
        {
            Info->CurrentTime += DeltaTime;
            if(Info->CurrentTime >= Animation->TimePerFrame)
            {
                Info->FrameIndex++;
                Info->CurrentTime = 0.0;
                
                if(Info->FrameIndex == Animation->FrameCount)
                {
                    Info->FrameIndex = 0;
                    
                    if(!Animation->Loop)
                    {
                        StopAnimation(Info);
                    }
                }
            }
        }
    }
}