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
        
        real32 FrameWidth = 1.0 / Animation->Columns;
        real32 FrameHeight = 1.0 / Animation->Rows;
        
        for(uint32 i = 0; i < Animation->FrameCount; i++)
        {
            real32 OffsetX;
            real32 OffsetY;
            
            if(fgets(LineBuffer, 255, File))
            {
                sscanf(LineBuffer, "%f %f", &OffsetX, &OffsetY);
                Animation->Frames[i] = { OffsetX * FrameWidth, OffsetY * FrameHeight };
            }
        }
        
        uint32 TextureIndex;
        
        //texturepath
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "texture %d", &TextureIndex);
            Animation->TextureHandle = RenderState->Textures[TextureIndex];
        }
    }
    else
        printf("Animation-file not loaded: '%s'\n", FilePath);
}

static void LoadAnimations(game_state* GameState)
{
    LoadAnimationFromFile("../assets/animations/player_anim_idle_new.pownim", &GameState->EnemyIdleAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_walk_new.pownim", &GameState->EnemyWalkAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_walk_up.pownim", &GameState->EnemyWalkUpAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_walk_down.pownim", &GameState->EnemyWalkDownAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_attack_new.pownim", &GameState->EnemyAttackAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_hit.pownim", &GameState->EnemyHitAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_sword_top_right.pownim", &GameState->SwordTopRightAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_idle_new.pownim", &GameState->PlayerIdleAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_walk_new.pownim", &GameState->PlayerWalkAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_walk_up.pownim", &GameState->PlayerWalkUpAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_walk_down.pownim", &GameState->PlayerWalkDownAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_attack_new.pownim", &GameState->PlayerAttackAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_sword_up.pownim", &GameState->SwordUpAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_sword_down.pownim", &GameState->SwordDownAnimation, &GameState->RenderState);
    LoadAnimationFromFile("../assets/animations/player_anim_hit.pownim", &GameState->PlayerHitAnimation, &GameState->RenderState);
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