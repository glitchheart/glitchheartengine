static void LoadWavFile(const char *Filename, sound_effect *LoadedSound)
{
    FILE *SoundFile = 0;
    wave_format WaveFormat;
    RIFF_header RiffHeader;
    wave_data WaveData;
    
    SoundFile = fopen(Filename, "rb");
    //printf("Buffer: %d\n", LoadedSound->Buffer);
    
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
            HandleError(__FILE__, __LINE__, "Riff header malformed");
        }
        
        fread(&WaveFormat, sizeof(wave_format), 1, SoundFile);
        
        if (WaveFormat.SubChunkID[0] != 'f' ||
            WaveFormat.SubChunkID[1] != 'm' ||
            WaveFormat.SubChunkID[2] != 't' ||
            WaveFormat.SubChunkID[3] != ' ')
        {
            HandleError(__FILE__, __LINE__, "Wave info malformed");
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
            HandleError(__FILE__, __LINE__, "Wave data malformed");
            //printf("ChunkID: %c %c %c %c \n", WaveData.SubChunkID[0], WaveData.SubChunkID[1], WaveData.SubChunkID[2], WaveData.SubChunkID[3]);
        }
        
        unsigned char *Data = (unsigned char *)malloc(WaveData.SubChunk2Size);
        
        if (!fread(Data, WaveData.SubChunk2Size, 1, SoundFile))
        {
            HandleError(__FILE__, __LINE__, "Error loading Wave data");
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
        
        //printf("Samplerate: %d, Size: %d\n", Frequency, Size);
    }
}

static void ResetSoundQueue(sound_manager *SoundManager)
{
    sound_queue SoundQueue = {};
    SoundQueue.SoundCount = 0;
    SoundManager->SoundQueue = SoundQueue;
}

static void InitAudio(sound_device *SoundDevice)
{
    SoundDevice->Device = alcOpenDevice(0);
    if (!SoundDevice->Device)
    {
        SoundDevice->Device = 0;
        HandleError(__FILE__, __LINE__, "Could not load Sound device");
        exit(EXIT_FAILURE);
    }
    
    ALboolean Enumeration;
    ALfloat ListenerOrientation[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    
    Enumeration = alcIsExtensionPresent(0, "ALC_ENUMERATION_EXT");
    if (!Enumeration)
    {
        HandleError(__FILE__, __LINE__, "Enumeration extension not supported");
        exit(EXIT_FAILURE);
    }
    
    SoundDevice->Context = alcCreateContext(SoundDevice->Device, 0);
    alcMakeContextCurrent(SoundDevice->Context);
    if (!SoundDevice->Context)
    {
        HandleError(__FILE__, __LINE__, "Could not set sound context");
        SoundDevice->Context = 0;
    }
    
    alListenerfv(AL_ORIENTATION, ListenerOrientation);
    
    alGenSources((ALuint)SOURCES,SoundDevice->Sources);
    
    SoundDevice->IsInitialized = SoundDevice->Device && SoundDevice->Context;
}

static void LoadSound(const char *filename,  sound_info SoundInfo, sound_effect *LoadedSound, sound_device* SoundDevice)
{
    LoadWavFile(filename, LoadedSound);
    LoadedSound->SoundInfo = SoundInfo;
    SoundDevice->Buffers[SoundDevice->BufferCount++] = LoadedSound->Buffer;
}

static void LoadSounds(sound_manager *SoundManager, sound_device* SoundDevice)
{
    sound_info DefaultSoundInfo = {};
    DefaultSoundInfo.Pitch = 1;
    DefaultSoundInfo.Gain = 1;
    r32 Position[3] = {0, 0, 0};
    memcpy(DefaultSoundInfo.Position, Position, 3);
    r32 Velocity[3] = {0, 0, 0};
    memcpy(DefaultSoundInfo.Velocity, Velocity, 3);
    DefaultSoundInfo.Loop = AL_FALSE;
    
    LoadSound("../assets/audio/Deadliners Track 1.wav", DefaultSoundInfo, &SoundManager->Track01,SoundDevice);
    LoadSound("../assets/audio/Countdown_1.wav", DefaultSoundInfo, &SoundManager->Effect01,SoundDevice);
    LoadSound("../assets/audio/mainmenu.wav", DefaultSoundInfo, &SoundManager->MainMenuTrack,SoundDevice);
    LoadSound("../assets/audio/sword_slash_01.wav", DefaultSoundInfo, &SoundManager->SwordSlash01,SoundDevice);
    LoadSound("../assets/audio/sword_hit_01.wav", DefaultSoundInfo, &SoundManager->SwordHit01,SoundDevice);
    LoadSound("../assets/audio/sword_hit_02.wav", DefaultSoundInfo, &SoundManager->SwordHit02, SoundDevice);
    LoadSound("../assets/audio/dash.wav", DefaultSoundInfo, &SoundManager->Dash,SoundDevice);
    LoadSound("../assets/audio/explosion.wav", DefaultSoundInfo, &SoundManager->Explosion, SoundDevice);
    LoadSound("../assets/audio/ui/button_click.wav", DefaultSoundInfo, &SoundManager->ButtonClick, SoundDevice);
    LoadSound("../assets/audio/barrel_break.wav", DefaultSoundInfo, &SoundManager->BarrelBreak, SoundDevice);
    LoadSound("../assets/audio/throw.wav", DefaultSoundInfo, &SoundManager->Throw, SoundDevice);
    LoadSound("../assets/audio/slide_1.wav", DefaultSoundInfo, &SoundManager->Slide01, SoundDevice);
    LoadSound("../assets/audio/use_health.wav", DefaultSoundInfo, &SoundManager->UseHealth, SoundDevice);
    
    LoadSound("../assets/audio/enemies/minotaur/minotaur_grunt01.wav", DefaultSoundInfo, &SoundManager->MinotaurGrunt01, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/minotaur_grunt02.wav", DefaultSoundInfo, &SoundManager->MinotaurGrunt02, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/minotaur_hit.wav", DefaultSoundInfo, &SoundManager->MinotaurHit, SoundDevice);
    LoadSound("../assets/audio/enemies/minotaur/minotaur_death.wav", DefaultSoundInfo, &SoundManager->MinotaurDeath, SoundDevice);
    
    DefaultSoundInfo.Gain = 0.5;
    LoadSound("../assets/audio/enemies/shield_impact.wav", DefaultSoundInfo, &SoundManager->ShieldImpact, SoundDevice);
    
    // // Add more sounds here if necessary
}

static void CleanupSound(sound_device* SoundDevice, sound_manager* SoundManager)
{
    alDeleteSources(SOURCES, SoundDevice->Sources);
    alDeleteBuffers(SoundDevice->BufferCount, SoundDevice->Buffers);
    
    SoundDevice->Device = alcGetContextsDevice(SoundDevice->Context);
    alcMakeContextCurrent(0);
    alcDestroyContext(SoundDevice->Context);
    alcCloseDevice(SoundDevice->Device);
}

static void PlaySound(sound_effect *SoundEffect,sound_device* Device)
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
                break;
            }
        }
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

static void StopSound(game_state* GameState, sound_device* SoundDevice)
{
    
    for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
    {
        alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,0);
        alSourceStop(SoundDevice->Sources[SourceIndex]);
        alSourcei(SoundDevice->Sources[SourceIndex],AL_BUFFER,0);
        if(!SoundDevice->Muted)
        {
            alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,1);
        }
    }
    
    ResetSoundQueue(&GameState->SoundManager);
}

static void PauseSound(game_state* GameState, sound_device* SoundDevice)
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
            alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,1);
        }
    }
}


static void PlaySounds(game_state *GameState, sound_device* Device)
{
    if(GameState->SoundManager.Muted && !Device->Muted)
    {
        for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
        {
            alSourcef(Device->Sources[SourceIndex],AL_GAIN,0);
        }
        Device->Muted = true;
    }
    else if(!GameState->SoundManager.Muted)
    {
        for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
        {
            alSourcef(Device->Sources[SourceIndex],AL_GAIN,1);
        }
        Device->Muted = false;
    }
    
    if(GameState->SoundManager.Stopped)
    {
        StopSound(GameState,Device);
        GameState->SoundManager.Stopped = false;
    }
    else 
    {
        for (u32 Sound = 0;
             Sound < GameState->SoundManager.SoundQueue.SoundCount;
             Sound++)
        {
            PlaySound(&GameState->SoundManager.SoundQueue.Sounds[Sound],Device);
        }
        
        ResetSoundQueue(&GameState->SoundManager);
    }
    if(GameState->SoundManager.Paused)
    {
        PauseSound(GameState,Device);
        Device->Paused = true;
    }
    else if(!GameState->SoundManager.Paused && Device->Paused)
    {
        alSourcePlayv(SOURCES,Device->Sources);
        Device->Paused = false;
    }
}