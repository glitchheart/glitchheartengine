static void LoadWavFile(const char *Filename, u32 *SoundBuffer)
{
    FILE *SoundFile = 0;
    wave_format WaveFormat;
    RIFF_header RiffHeader;
    wave_data WaveData;
    
    SoundFile = fopen(Filename, "rb");
    
    if (SoundFile)
    {
        fread(&RiffHeader, sizeof(RIFF_header), 1, SoundFile);
        
        if ((RiffHeader.ChunkID[0] != 'R' ||
             RiffHeader.ChunkID[1] != 'I' ||
             RiffHeader.ChunkID[2] != 'F' ||
             RiffHeader.ChunkID[3] != 'F' ||
             (RiffHeader.Format[0] != 'W' ||
              RiffHeader.Format[1] != 'A' ||
              RiffHeader.Format[2] != 'V' ||
              RiffHeader.Format[3] != 'E')))
        {
            ERR("Riff header malformed");
        }
        
        fread(&WaveFormat, sizeof(wave_format), 1, SoundFile);
        
        if (WaveFormat.SubChunkID[0] != 'f' ||
            WaveFormat.SubChunkID[1] != 'm' ||
            WaveFormat.SubChunkID[2] != 't' ||
            WaveFormat.SubChunkID[3] != ' ')
        {
            ERR("Wave info malformed");
        }
        
        if (WaveFormat.SubChunkSize > 16)
        {
            fseek(SoundFile, sizeof(short), SEEK_CUR);
        }
        
        fread(&WaveData, sizeof(wave_data), 1, SoundFile);
        
        if (WaveData.SubChunkID[0] != 'd' ||
            WaveData.SubChunkID[1] != 'a' ||
            WaveData.SubChunkID[2] != 't' ||
            WaveData.SubChunkID[3] != 'a')
        {
            ERR("Wave data malformed");
        }
        
        unsigned char *Data = PushTempSize(WaveData.SubChunk2Size, unsigned char);
        
        if (!fread(Data, WaveData.SubChunk2Size, 1, SoundFile))
        {
            ERR("Error loading Wave data");
        }
        
        ALsizei Size = WaveData.SubChunk2Size;
        ALsizei Frequency = WaveFormat.SampleRate;
        ALenum Format = 0;
        
        if (WaveFormat.NumChannels == 1)
        {
            if (WaveFormat.BitsPerSample == 8)
            {
                Format = AL_FORMAT_MONO8;
            }
            else if (WaveFormat.BitsPerSample == 16)
            {
                Format = AL_FORMAT_MONO16;
            }
        }
        else if (WaveFormat.NumChannels == 2)
        {
            if (WaveFormat.BitsPerSample == 8)
            {
                Format = AL_FORMAT_STEREO8;
            }
            else if (WaveFormat.BitsPerSample == 16)
            {
                Format = AL_FORMAT_STEREO16;
            }
        }
        
        alGenBuffers(1, SoundBuffer);
        alBufferData(*SoundBuffer, Format, (void*)Data, Size, Frequency);
        
        fclose(SoundFile);
    }
}

static inline void ResetCommands(sound_commands* Commands)
{
    Clear(&Commands->SoundArena);
    Commands->SoundCount = 0;
}

static void InitAudio(sound_device *SoundDevice)
{
    SoundDevice->Device = alcOpenDevice(0);
    if (!SoundDevice->Device)
    {
        SoundDevice->Device = 0;
        ERR("Could not load Sound device");
        exit(EXIT_FAILURE);
    }
    
    ALboolean Enumeration;
    ALfloat ListenerOrientation[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    ALfloat ListenerPosition[] = {5.0f, 5.0f, 5.0f};
    
    
    Enumeration = alcIsExtensionPresent(0, "ALC_ENUMERATION_EXT");
    if (!Enumeration)
    {
        ERR("Enumeration extension not supported");
        exit(EXIT_FAILURE);
    }
    
    SoundDevice->Context = alcCreateContext(SoundDevice->Device, 0);
    alcMakeContextCurrent(SoundDevice->Context);
    if (!SoundDevice->Context)
    {
        ERR("Could not set sound context");
        SoundDevice->Context = 0;
    }
    
    alListenerfv(AL_ORIENTATION, ListenerOrientation);
    alListenerfv(AL_POSITION, ListenerPosition);
    alListenerf(AL_GAIN,0.95f);
    
    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    
    alGenSources((ALuint)SOURCES,SoundDevice->Sources);
    
    SoundDevice->PrevStopped = false;
    SoundDevice->PrevPaused = false;
    SoundDevice->PrevMuted = false;
    
    SoundDevice->IsInitialized = SoundDevice->Device && SoundDevice->Context;
    source_to_sound_Map_Init(&SoundDevice->SourceToSound, HashInt, 32);
}

static void LoadSound(const char *FilePath, sound_device* SoundDevice, u32* BufferHandle, char* Name = "")
{
    LoadWavFile(FilePath, BufferHandle);
    SoundDevice->Buffers[SoundDevice->BufferCount++] = *BufferHandle;
}

static inline void LoadSounds(sound_device* SoundDevice, sound_commands* Commands)
{
    if(Commands->LoadSounds)
    {
        auto DirData = Commands->SoundsToLoad;
        
        for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
        {
            LoadSound(DirData.FilePaths[FileIndex], SoundDevice, &SoundDevice->SoundArray[Commands->SoundMap[DirData.FileNames[FileIndex]]], DirData.FileNames[FileIndex]);
        }
        
        Commands->LoadSounds = false;
    }
}

static inline void CleanupSound(sound_device* SoundDevice)
{
    alDeleteSources(SOURCES, SoundDevice->Sources);
    alDeleteBuffers(SoundDevice->BufferCount, SoundDevice->Buffers);
    
    SoundDevice->Device = alcGetContextsDevice(SoundDevice->Context);
    alcMakeContextCurrent(0);
    alcDestroyContext(SoundDevice->Context);
    alcCloseDevice(SoundDevice->Device);
}

static inline void PlaySound(sound_effect *SoundEffect, sound_device* Device, sound_commands* Commands)
{
    if (SoundEffect)
    {
        ALuint Source = 0;
        
        i32 SourceState;
        for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
        {
            alGetSourcei(Device->Sources[SourceIndex],AL_SOURCE_STATE,&SourceState);
            if(SourceState != AL_PLAYING)
            {
                Source = Device->Sources[SourceIndex];
                Device->SourceGain[SourceIndex] = SoundEffect->SoundInfo.Gain;
                sound_effect SoundToStore;
                SoundToStore.Buffer = SoundEffect->Buffer;
                SoundToStore.Source = SoundEffect->Source;
                SoundToStore.SourceState = SoundEffect->SourceState;
                SoundToStore.SoundInfo = SoundEffect->SoundInfo;
                Device->SourceToSound[Device->Sources[SourceIndex]] = SoundToStore;
                break;
            }
        }
        alSourcef(Source,AL_MAX_DISTANCE,1.0);
        alSourcef(Source,AL_REFERENCE_DISTANCE,1.0);
        alSourcef(Source, AL_ROLLOFF_FACTOR, 1.0f);
        alSourcei(Source, AL_BUFFER, Device->SoundArray[SoundEffect->Buffer]);
        alSourcef(Source, AL_PITCH, SoundEffect->SoundInfo.Pitch);
        alSourcef(Source, AL_GAIN, Commands->Muted ? 0 : SoundEffect->SoundInfo.Gain);
        alSource3f(Source, AL_POSITION, SoundEffect->SoundInfo.Position[0], SoundEffect->SoundInfo.Position[1], SoundEffect->SoundInfo.Position[2]);
        alSource3f(Source, AL_VELOCITY, SoundEffect->SoundInfo.Velocity[0], SoundEffect->SoundInfo.Velocity[1], SoundEffect->SoundInfo.Velocity[2]);
        alSourcei(Source, AL_LOOPING, SoundEffect->SoundInfo.Loop);
        alSourcePlay(Source);
        alGetSourcei(SoundEffect->Source, AL_SOURCE_STATE, &SoundEffect->SourceState);
    }
}

static inline void StopSound(sound_device* SoundDevice, sound_commands* Commands)
{
    for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
    {
        alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,0);
        alSourceStop(SoundDevice->Sources[SourceIndex]);
        alSourcei(SoundDevice->Sources[SourceIndex],AL_BUFFER,0);
        if(!Commands->Muted)
        {
            alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
        }
    }
}

static inline void PauseSound(sound_device* SoundDevice, sound_commands* Commands)
{
    for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
    {
        alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,0);
        ALint SourceState;
        alGetSourcei(SoundDevice->Sources[SourceIndex],AL_SOURCE_STATE,&SourceState);
        if(SourceState == AL_PLAYING)
        {
            alSourcePause(SoundDevice->Sources[SourceIndex]);
        }
        else
        {
            alSourcei(SoundDevice->Sources[SourceIndex], AL_BUFFER,0);
        }
        if(!Commands->Muted)
        {
            alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
        }
    }
}

static void PlaySounds(sound_device* Device, sound_commands* Commands, math::v3* EntityPositions, i32 EntityCount)
{
    if(Device && Commands)
    {
        LoadSounds(Device, Commands);
        
        if(Commands->Muted && !Device->PrevMuted)
        {
            Device->PrevMuted = true;
            for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
            {
                alSourcef(Device->Sources[SourceIndex], AL_GAIN, 0);
            }
        }
        else if(!Commands->Muted && Device->PrevMuted)
        {
            Device->PrevMuted = false;
            for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
            {
                alSourcef(Device->Sources[SourceIndex],AL_GAIN,Device->SourceGain[SourceIndex]);
            }
        }
        
        
        if(Commands->Stopped && !Device->PrevStopped)
        {
            Device->PrevStopped = true;
            StopSound(Device, Commands);
            ResetCommands(Commands);
        }
        else
        {
            Device->PrevStopped = false;
            for (i32 Sound = 0;
                 Sound < Commands->SoundCount;
                 Sound++)
            {
                auto SoundEffect = (sound_effect*)&Commands->SoundArena.CurrentBlock->Base[Sound];
                PlaySound(SoundEffect, Device, Commands);
            }
            ResetCommands(Commands);
        }
        
        if(Commands->Paused && !Device->PrevPaused)
        {
            Device->PrevPaused = true;
            PauseSound(Device, Commands);
        }
        else if(!Commands->Paused && Device->PrevPaused)
        {
            Device->PrevPaused = false;
            alSourcePlayv(SOURCES,Device->Sources);
        }
        
        if(!Commands->Muted && !Commands->Paused && !Commands->Stopped)
        {
            for(i32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
            {
                auto Source = Device->Sources[SourceIndex];
                ALint SourceState;
                alGetSourcei(Source,AL_SOURCE_STATE,&SourceState);
                auto Sound = Device->SourceToSound[Source];
                if(SourceState == AL_PLAYING)
                {
                    if(Sound.SoundInfo.Rolloff > 0)
                    {
                        //@Incomplete: Should do rolloff in game code...
                        if(Sound.SoundInfo.EntityHandle != -1)
                        {
                            if(EntityPositions)
                            {
                                auto Position = EntityPositions[Sound.SoundInfo.EntityHandle];
                                Device->SourceToSound[Source].SoundInfo.Position[0] = Position.x;
                                Device->SourceToSound[Source].SoundInfo.Position[1] = Position.y;
                            }
                        }
                        
                        if(EntityPositions)
                        {
                            r32 DistanceToEntity = math::Distance(math::v3(Sound.SoundInfo.Position[0], Sound.SoundInfo.Position[1],
                                                                           Sound.SoundInfo.Position[2]), math::v3(0,0,0));
                            r32 VolFactor = 1.0f - (DistanceToEntity/Sound.SoundInfo.Rolloff);
                            alSourcef(Device->Sources[SourceIndex],AL_GAIN,Max(0.0f,VolFactor) * Commands->SFXVolume);
                            Device->SourceGain[SourceIndex] = Max(0.0f,VolFactor) * Commands->SFXVolume;
                        }
                    }
                }
            }
        }
    }
}