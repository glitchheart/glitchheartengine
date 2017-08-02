static void LoadWavFile(const char *Filename, sound_effect *LoadedSound)
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
        
        unsigned char *Data = (unsigned char *)malloc(WaveData.SubChunk2Size);
        
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
        
        alGenBuffers(1, &LoadedSound->Buffer);
        alBufferData(LoadedSound->Buffer, Format, (void*)Data, Size, Frequency);
        fclose(SoundFile);
    }
}

static inline void ResetSoundQueue(sound_queue* SoundQueue)
{
    for(i32 Sound = 0; Sound < SoundQueue->SoundCount; Sound++)
    {
        SoundQueue->Sounds[Sound].Buffer = 0;
    }
    SoundQueue->SoundCount = 0;
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
    
    SoundDevice->Stopped = false;
    SoundDevice->Paused = false;
    SoundDevice->Muted = false;
    SoundDevice->PrevStopped = false;
    SoundDevice->PrevPaused = false;
    SoundDevice->PrevMuted = false;
    
    SoundDevice->IsInitialized = SoundDevice->Device && SoundDevice->Context;
    source_to_sound_Map_Init(&SoundDevice->SourceToSound, HashInt, 32);
}

static inline void LoadSound(const char *filename, sound_effect *LoadedSound, sound_device* SoundDevice, char* Name = "")
{
    LoadWavFile(filename, LoadedSound);
    LoadedSound->SoundInfo.Pitch = 1.0f;
    LoadedSound->SoundInfo.Position[0] = 0.0f;
    LoadedSound->SoundInfo.Position[1] = 0.0f;
    LoadedSound->SoundInfo.Position[2] = 0.0f;
    LoadedSound->SoundInfo.Rolloff = 0.0;
    LoadedSound->SoundInfo.Loop = false;
    LoadedSound->SoundInfo.EntityHandle = -1;
    LoadedSound->SoundInfo.Gain = 0.0f;
    LoadedSound->SoundInfo.Name = Name;
    SoundDevice->Buffers[SoundDevice->BufferCount++] = LoadedSound->Buffer;
}

static inline void LoadSounds(sound_effects* SoundEffects, sound_device* SoundDevice)
{
    LoadSound("../assets/audio/Countdown_1.wav", &SoundEffects->Effect01,SoundDevice);
    LoadSound("../assets/audio/mainmenu.wav", &SoundEffects->MainMenuTrack,SoundDevice);
    LoadSound("../assets/audio/sword_slash_01.wav", &SoundEffects->SwordSlash01,SoundDevice);
    LoadSound("../assets/audio/sword_hit_01.wav",  &SoundEffects->SwordHit01,SoundDevice);
    LoadSound("../assets/audio/sword_hit_02.wav",  &SoundEffects->SwordHit02, SoundDevice);
    LoadSound("../assets/audio/dash.wav", &SoundEffects->Dash,SoundDevice);
    LoadSound("../assets/audio/explosion.wav", &SoundEffects->Explosion, SoundDevice);
    LoadSound("../assets/audio/ui/button_click.wav", &SoundEffects->ButtonClick, SoundDevice);
    LoadSound("../assets/audio/barrel_break.wav", &SoundEffects->BarrelBreak, SoundDevice);
    LoadSound("../assets/audio/throw.wav", &SoundEffects->Throw, SoundDevice);
    LoadSound("../assets/audio/slide_1.wav", &SoundEffects->Slide01, SoundDevice);
    LoadSound("../assets/audio/use_health.wav", &SoundEffects->UseHealth, SoundDevice);
    
    LoadSound("../assets/audio/enemies/minotaur/minotaur_grunt01.wav", &SoundEffects->MinotaurGrunt01, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/minotaur_grunt02.wav",  &SoundEffects->MinotaurGrunt02, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/minotaur_hit.wav", &SoundEffects->MinotaurHit, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/minotaur_death.wav", &SoundEffects->MinotaurDeath, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/stomp.wav", &SoundEffects->MinotaurStomp, SoundDevice);
    LoadSound("../assets/audio/enemies/shield_impact.wav", &SoundEffects->ShieldImpact, SoundDevice);
    LoadSound("../assets/audio/splash_01.wav", &SoundEffects->Splash01, SoundDevice);
    
    LoadSound("../assets/audio/bonfire.wav", &SoundEffects->Bonfire, SoundDevice);
    
    LoadSound("../assets/audio/music/Brugt.wav", &SoundEffects->Brugt, SoundDevice, "Brugt");
    
    // // Add more sounds here if necessary
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

static inline void PlaySound(sound_effect *SoundEffect,sound_device* Device)
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
        alSourcei(Source, AL_BUFFER, SoundEffect->Buffer);
        alSourcef(Source, AL_PITCH, SoundEffect->SoundInfo.Pitch);
        alSourcef(Source, AL_GAIN, Device->Muted ? 0 : SoundEffect->SoundInfo.Gain);
        alSource3f(Source, AL_POSITION, SoundEffect->SoundInfo.Position[0], SoundEffect->SoundInfo.Position[1], SoundEffect->SoundInfo.Position[2]);
        alSource3f(Source, AL_VELOCITY, SoundEffect->SoundInfo.Velocity[0], SoundEffect->SoundInfo.Velocity[1], SoundEffect->SoundInfo.Velocity[2]);
        alSourcei(Source, AL_LOOPING, SoundEffect->SoundInfo.Loop);
        alSourcePlay(Source);
        alGetSourcei(SoundEffect->Source, AL_SOURCE_STATE, &SoundEffect->SourceState);
    }
}

static inline void StopSound(sound_device* SoundDevice)
{
    for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
    {
        alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,0);
        alSourceStop(SoundDevice->Sources[SourceIndex]);
        alSourcei(SoundDevice->Sources[SourceIndex],AL_BUFFER,0);
        if(!SoundDevice->Muted)
        {
            alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
        }
    }
}

static inline void PauseSound(sound_device* SoundDevice)
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
            alSourcei(SoundDevice->Sources[SourceIndex],AL_BUFFER,0);
        }
        if(!SoundDevice->Muted)
        {
            alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
        }
    }
}

static void PlaySounds(sound_device* Device, sound_queue* SoundQueue, glm::vec2* EntityPositions, i32 EntityCount)
{
    
    if(Device->Muted && !Device->PrevMuted)
    {
        for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
        {
            alSourcef(Device->Sources[SourceIndex],AL_GAIN,0);
        }
    }
    else
    {
        for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
        {
            alSourcef(Device->Sources[SourceIndex],AL_GAIN,Device->SourceGain[SourceIndex]);
        }
    }
    
    if(Device->Stopped && !Device->PrevStopped)
    {
        StopSound(Device);
        ResetSoundQueue(SoundQueue);
    }
    else
    {
        for (u32 Sound = 0;
             Sound < SoundQueue->SoundCount;
             Sound++)
        {
            PlaySound(&SoundQueue->Sounds[Sound],Device);
        }
        ResetSoundQueue(SoundQueue);
    }
    
    if(Device->Paused && !Device->PrevPaused)
    {
        PauseSound(Device);
    }
    else if(!Device->Paused && Device->PrevPaused)
    {
        Device->PrevPaused = false;
        alSourcePlayv(SOURCES,Device->Sources);
    }
    
    if(!Device->Paused && !Device->Stopped && !Device->Muted)
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
                        r32 Distance = glm::distance(glm::vec2(Sound.SoundInfo.Position[0], Sound.SoundInfo.Position[1]), EntityPositions[0]);
                        r32 VolFactor = 1.0f - (Distance/Sound.SoundInfo.Rolloff);
                        alSourcef(Device->Sources[SourceIndex],AL_GAIN,Max(0.0f,VolFactor) * Device->SFXVolume);
                        Device->SourceGain[SourceIndex] = Max(0.0f,VolFactor) * Device->SFXVolume;
                    }
                }
            }
        }
    }
}