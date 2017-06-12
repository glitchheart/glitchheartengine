static void LoadAnimationFromFile(const char* FilePath, animation* Animation, render_state* RenderState)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    Animation->Name = (char*)malloc(sizeof(char) * 30);
    
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
        
        //framesize
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "framesize %f", &Animation->FrameSize);
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
        
        //frames
        fgets(LineBuffer, 255, File);
        
        Animation->Frames = (sprite_sheet_frame*)malloc(sizeof(sprite_sheet_frame) * Animation->FrameCount);
        
        for(uint32 i = 0; i < Animation->FrameCount; i++)
        {
            real32 OffsetX = 0.0f;
            real32 OffsetY = 0.0f;
            
            if(fgets(LineBuffer, 255, File))
            {
                sscanf(LineBuffer, "%f %f", &OffsetX, &OffsetY);
                Animation->Frames[i] = { OffsetX, OffsetY };
            }
        }
        
        uint32 TextureIndex;
        
        //texturepath
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "texture %d", &TextureIndex);
            
            Animation->Texture = &RenderState->Textures[TextureIndex];
        }
    }
    else
        printf("Animation-file not loaded: '%s'\n", FilePath);
}

static void LoadAnimations(game_state* GameState)
{
    LoadAnimationFromFile("../assets/animations/enemy_anim_idle.pownim", &GameState->EnemyIdleAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_walk.pownim", &GameState->EnemyWalkAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_walk_up.pownim", &GameState->EnemyWalkUpAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_walk_down.pownim", &GameState->EnemyWalkDownAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_attack.pownim", &GameState->EnemyAttackAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/enemy_anim_hit.pownim", &GameState->EnemyHitAnimation, &GameState->RenderState);
    
    LoadAnimationFromFile("../assets/animations/player/idle.pownim", &GameState->PlayerIdleAnimation, &GameState->RenderState);
    
    LoadAnimationFromFile("../assets/animations/player/run_up.pownim", &GameState->PlayerRunUpAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player/run_down.pownim", &GameState->PlayerRunDownAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player/run_left.pownim", &GameState->PlayerRunLeftAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player/run_right.pownim", &GameState->PlayerRunRightAnimation, &GameState->RenderState);
    
    LoadAnimationFromFile("../assets/animations/player/attack_up.pownim", &GameState->PlayerAttackUpAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player/attack_down.pownim", &GameState->PlayerAttackDownAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player/attack_left.pownim", &GameState->PlayerAttackLeftAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player/attack_right.pownim", &GameState->PlayerAttackRightAnimation, &GameState->RenderState);
    
    LoadAnimationFromFile("../assets/animations/sword_attack.pownim", &GameState->SwordAttackAnimation, &GameState->RenderState);
}

static void PlayAnimation(entity* Entity, animation* Animation)
{
    if(!Entity->CurrentAnimation || strcmp(Entity->CurrentAnimation->Name, Animation->Name) != 0)
    {
        Entity->CurrentAnimation = Animation;
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