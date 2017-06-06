static void LoadWavFile(const char *Filename, sound_effect *LoadedSound)
{
    FILE *SoundFile = 0;
    wave_format WaveFormat;
    RIFF_header RiffHeader;
    wave_data WaveData;
    
    SoundFile = fopen(Filename, "rb");
    printf("Buffer: %d\n", LoadedSound->Buffer);
    
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
            printf("ChunkID: %c %c %c %c \n", WaveData.SubChunkID[0], WaveData.SubChunkID[1], WaveData.SubChunkID[2], WaveData.SubChunkID[3]);
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
        
        printf("Samplerate: %d, Size: %d\n", Frequency, Size);
    }
}

static void ResetSoundQueue(sound_manager *SoundManager)
{
    sound_queue SoundQueue = {};
    SoundQueue.SoundCount = 0;
    SoundQueue.StoppedSoundCount = 0;
    SoundQueue.PausedSoundCount = 0;
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
    
    SoundDevice->IsInitialized = SoundDevice->Device && SoundDevice->Context;
}

static void LoadSound(const char *filename,  sound_info SoundInfo, sound_effect *LoadedSound)
{
    alGenSources((ALuint)1, &LoadedSound->Source);
    LoadWavFile(filename, LoadedSound);
    alSourcei(LoadedSound->Source, AL_BUFFER, LoadedSound->Buffer);
    LoadedSound->SoundInfo = SoundInfo;
}

static void LoadSounds(sound_manager *SoundManager)
{
    sound_info DefaultSoundInfo = {};
    DefaultSoundInfo.Pitch = 1;
    DefaultSoundInfo.Gain = 1;
    real32 Position[3] = {0, 0, 0};
    memcpy(DefaultSoundInfo.Position, Position, 3);
    real32 Velocity[3] = {0, 0, 0};
    memcpy(DefaultSoundInfo.Velocity, Velocity, 3);
    DefaultSoundInfo.Loop = AL_FALSE;
    
    LoadSound("../assets/audio/Deadliners Track 1.wav", DefaultSoundInfo, &SoundManager->Track01);
    LoadSound("../assets/audio/Countdown_1.wav", DefaultSoundInfo, &SoundManager->Effect01);
    LoadSound("../assets/audio/mainmenu.wav", DefaultSoundInfo, &SoundManager->MainMenuTrack);
    LoadSound("../assets/audio/sword_slash_01.wav", DefaultSoundInfo, &SoundManager->SwordSlash01);
    LoadSound("../assets/audio/sword_hit_01.wav", DefaultSoundInfo, &SoundManager->SwordHit01);
    LoadSound("../assets/audio/dash.wav", DefaultSoundInfo, &SoundManager->Dash);
    // // Add more sounds here if necessary
}

static void CleanupSound(sound_device* SoundDevice, sound_manager* SoundManager)
{
    alDeleteSources(1, &SoundManager->Track01.Source);
    alDeleteBuffers(1, &SoundManager->Track01.Buffer);
    
    SoundDevice->Device = alcGetContextsDevice(SoundDevice->Context);
    alcMakeContextCurrent(0);
    alcDestroyContext(SoundDevice->Context);
    alcCloseDevice(SoundDevice->Device);
}

static void PlaySound(sound_effect *SoundEffect)
{
    if (SoundEffect)
    {
        if (!SoundEffect->PlayOnce && SoundEffect->SourceState != AL_PLAYING)
        {
            alGenSources((ALuint)1, &SoundEffect->Source);
        }
        alSourcei(SoundEffect->Source, AL_BUFFER, SoundEffect->Buffer);
        alSourcef(SoundEffect->Source, AL_PITCH, SoundEffect->SoundInfo.Pitch);
        alSourcef(SoundEffect->Source, AL_GAIN, SoundEffect->SoundInfo.Gain);
        alSource3f(SoundEffect->Source, AL_POSITION, SoundEffect->SoundInfo.Position[0], SoundEffect->SoundInfo.Position[1], SoundEffect->SoundInfo.Position[2]);
        alSource3f(SoundEffect->Source, AL_VELOCITY, SoundEffect->SoundInfo.Velocity[0], SoundEffect->SoundInfo.Velocity[1], SoundEffect->SoundInfo.Velocity[2]);
        alSourcei(SoundEffect->Source, AL_LOOPING, SoundEffect->SoundInfo.Loop);
        alSourcePlay(SoundEffect->Source);
        alGetSourcei(SoundEffect->Source, AL_SOURCE_STATE, &SoundEffect->SourceState);
        //printf("Source: %d\n", SoundEffect->Source);
        //printf("Buffer: %d\n", SoundEffect->Buffer);
    }
}

static void PlaySoundOnce(sound_effect *SoundEffect)
{
    if (SoundEffect)
    {
        alGetSourcei(SoundEffect->Source, AL_SOURCE_STATE, &SoundEffect->SourceState);
        if (SoundEffect->SourceState != AL_PLAYING)
        {
            PlaySound(SoundEffect);
        }
    }
}

static void PauseSound(sound_effect *SoundEffect)
{
    if (SoundEffect)
    {
        alSourcePause(SoundEffect->Source);
    }
}

static void StopSound(sound_effect *SoundEffect)
{
    if (SoundEffect)
    {
        alSourceStop(SoundEffect->Source);
    }
}

static void PlaySounds(game_state *GameState)
{
    for (uint32 Sound = 0;
         Sound < GameState->SoundManager.SoundQueue.SoundCount;
         Sound++)
    {
        if (GameState->SoundManager.SoundQueue.Sounds[Sound].PlayOnce)
        {
            PlaySoundOnce(&GameState->SoundManager.SoundQueue.Sounds[Sound]);
        }
        else
        {
            PlaySound(&GameState->SoundManager.SoundQueue.Sounds[Sound]);
        }
    }
    
    for (uint32 Sound = 0;
         Sound < GameState->SoundManager.SoundQueue.StoppedSoundCount;
         Sound++)
    {
        
        StopSound(&GameState->SoundManager.SoundQueue.StoppedSounds[Sound]);
    }
    
    for (uint32 Sound = 0;
         Sound < GameState->SoundManager.SoundQueue.PausedSoundCount;
         Sound++)
    {
        
        PauseSound(&GameState->SoundManager.SoundQueue.PausedSounds[Sound]);
    }
    ResetSoundQueue(&GameState->SoundManager);
}