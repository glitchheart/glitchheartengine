                                static void SaveAnimationToFile(game_state* GameState, const animation& Animation)
                                {
                                    FILE* File;
                                    File = fopen(Concat(Concat("../assets/animations/", Animation.Name),".pownim"), "w");
                                    if(File)
                                    {
                                        fprintf(File, "name %s\n", Animation.Name);
                                        fprintf(File, "type sprite\n");
                                        fprintf(File, "framecount %d\n", Animation.FrameCount);
                                        fprintf(File, "framesize %d %d\n", (int32)Animation.FrameSize.x, (int32)Animation.FrameSize.y);
                                        fprintf(File, "loop %d\n", Animation.Loop);
                                        fprintf(File, "timeperframe %f\n", Animation.TimePerFrame);
                                        fprintf(File, "frames\n");
                                        
                                        int32 X = 0;
                                        int32 Y = 0;
                                        
                                        int32 FrameIndex = 0;
                                        
                                        while(FrameIndex < (int32)Animation.FrameCount)
                                        {
                                            fprintf(File, "%d %d\n", X, Y);
                                            
                                            FrameIndex++;
                                            
                                            if(X + Animation.FrameSize.x <= Animation.Texture->Width)
                                                X += (int32)Animation.FrameSize.x;
                                            else
                                            {
                                                X = 0;
                                                Y += (int32)Animation.FrameSize.y;
                                            }
                                        }
                                        
                                        fprintf(File, "texture 7\n");
                                        
                                        GameState->Animations.insert(std::pair<char*, animation>(Animation.Name, Animation));
                                        
                                        fclose(File);
                                    }
                                }
                                
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
                                        
                                        char* TextureName = (char*) malloc(200 * sizeof(char));
                                        
                                        //texturepath
                                        if(fgets(LineBuffer, 255, File))
                                        {
                                            sscanf(LineBuffer, "texture %s", TextureName);
                                            Animation.Texture = &GameState->RenderState.Textures[TextureName];
                                            free(TextureName);
                                        }
                                        GameState->Animations.insert(std::pair<char*, animation>(Animation.Name, Animation));
                                    }
                                    else
                                        printf("Animation-file not loaded: '%s'\n", FilePath);
                                }
                                
                                static void LoadAnimations(game_state* GameState)
                                {
                                    directory_data DirData;
                                    FindFilesWithExtensions("../assets/animations/", "pownim", &DirData);
                                    
                                    for(int32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
                                    {
                                        LoadAnimationFromFile(DirData.FilePaths[FileIndex], GameState);
                                    }
                                    
                                    free(DirData.FilePaths);
                                    free(DirData.FileNames);
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