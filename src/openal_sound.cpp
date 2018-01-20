static void LoadWavFile(sound_device* SoundDevice, const char *Filename, u32 *SoundBuffer)
{
    FILE *SoundFile = 0;
    wave_format WaveFormat;
    RIFF_header RiffHeader;
    wave_data WaveData;
    
    SoundFile = fopen(Filename, "rb");
    
    if (SoundFile)
    {
        DEBUG_PRINT("File: %s\n", Filename);
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
            printf("Riff: %s\n", RiffHeader.ChunkID);
            printf("Fmt: %s\n", RiffHeader.Format);
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
        
        OpenALApi.alGenBuffers(1, SoundBuffer);
        OpenALApi.alBufferData(*SoundBuffer, Format, (void*)Data, Size, Frequency);
        
        fclose(SoundFile);
    }
}

static inline void ResetCommands(sound_commands* Commands)
{
    //Clear(&Commands->SoundArena);
    Commands->SoundCount = 0;
}

static void InitAudio_OpenAL(sound_device *SoundDevice, oal_devices_list* DevicesList)
{
    HMODULE OpenALDLL = {};
    auto Result = LoadOAL11Library(0, &OpenALApi, OpenALDLL);
    
    if(!Result)
    {
        ERR("Could not load OpenAL library");
    }
    
    if(OpenALApi.
       alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT"))
    {
        char* Devices = (char*)OpenALApi.alcGetString(NULL, ALC_DEVICE_SPECIFIER);
        char* DefaultDeviceName = (char*)OpenALApi.alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
        const char* ActualDeviceName;
        
        if(!strlen(Devices))
        {
            ERR("No sound devices found\n");
        }
        
        sound_device_info ALDeviceInfo;
        
        i32 DefaultDeviceIndex = 0;
        i32 Index = 0;
        
        while(*Devices != NULL)
        {
            if(strcmp(DefaultDeviceName, Devices) == 0)
            {
                DefaultDeviceIndex = Index;
            }
            ALCdevice* Device = OpenALApi.alcOpenDevice(Devices);
            if(Device)
            {
                ALCcontext* Context = OpenALApi.alcCreateContext(Device, NULL);
                if(Context)
                {
                    ActualDeviceName = OpenALApi.alcGetString(Device, ALC_DEVICE_SPECIFIER);
                    b32 NewName = true;
                    for(i32 I = 0; I < DevicesList->DeviceCount; I++)
                    {
                        if(strcmp(DevicesList->DeviceInfo[I].DeviceName, ActualDeviceName) == 0)
                        {
                            NewName = false;
                        }
                    }
                    if((NewName) && (ActualDeviceName != NULL) && (strlen(ActualDeviceName) > 0))
                    {
                        memset(&ALDeviceInfo, 0, sizeof(sound_device_info));
                        ALDeviceInfo.Selected = true;
                        ALDeviceInfo.DeviceName = ActualDeviceName;
                        OpenALApi.alcGetIntegerv(Device, ALC_MAJOR_VERSION, sizeof(i32), &ALDeviceInfo.MajorVersion);
                        OpenALApi.alcGetIntegerv(Device, ALC_MINOR_VERSION, sizeof(i32), &ALDeviceInfo.MinorVersion);
                        
                        // Check for ALC Extensions
                        if (OpenALApi.alcIsExtensionPresent(Device, "ALC_EXT_CAPTURE") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = ("ALC_EXT_CAPTURE");
                        if (OpenALApi.alcIsExtensionPresent(Device, "ALC_EXT_EFX") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "ALC_EXT_EFX";
                        
                        // Check for AL Extensions
                        if (OpenALApi.alIsExtensionPresent("AL_EXT_OFFSET") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "AL_EXT_OFFSET";
                        
                        if (OpenALApi.alIsExtensionPresent("AL_EXT_LINEAR_DISTANCE") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "AL_EXT_LINEAR_DISTANCE";
                        if (OpenALApi.alIsExtensionPresent("AL_EXT_EXPONENT_DISTANCE") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "AL_EXT_EXPONENT_DISTANCE";
                        
                        if (OpenALApi.alIsExtensionPresent("EAX2.0") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "EAX2.0";
                        if (OpenALApi.alIsExtensionPresent("EAX3.0") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "EAX3.0";
                        if (OpenALApi.alIsExtensionPresent("EAX4.0") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "EAX4.0";
                        if (OpenALApi.alIsExtensionPresent("EAX5.0") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "EAX5.0";
                        
                        if (OpenALApi.alIsExtensionPresent("EAX-RAM") == AL_TRUE)
                            ALDeviceInfo.Extensions[ALDeviceInfo.ExtensionCount++] = "EAX-RAM";
                        DevicesList->DeviceInfo[DevicesList->DeviceCount++] = ALDeviceInfo;
                    }
                    OpenALApi.alcMakeContextCurrent(NULL);
                    OpenALApi.alcDestroyContext(Context);
                }
                OpenALApi.alcCloseDevice(Device);
            }
            Devices += strlen(Devices) + 1;
            Index += 1;
        }
    }
    
    if(DevicesList->DeviceCount > 0)
    {
        auto& Device = DevicesList->DeviceInfo[0];
        SoundDevice->Device = OpenALApi.alcOpenDevice(0);
        Device.Selected = true;
        SoundDevice->DeviceInfo = Device;
    }
    
    if (!SoundDevice->Device)
    {
        SoundDevice->Device = 0;
        ERR("Could not load Sound device");
        return;
        //exit(EXIT_FAILURE);
    }
    
    ALboolean Enumeration;
    ALfloat ListenerOrientation[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    ALfloat ListenerPosition[] = {5.0f, 5.0f, 5.0f};
    
    
    Enumeration = OpenALApi.alcIsExtensionPresent(0, "ALC_ENUMERATION_EXT");
    if (!Enumeration)
    {
        ERR("Enumeration extension not supported");
        exit(EXIT_FAILURE);
    }
    
    SoundDevice->Context = OpenALApi.alcCreateContext(SoundDevice->Device, 0);
    OpenALApi.alcMakeContextCurrent(SoundDevice->Context);
    if (!SoundDevice->Context)
    {
        ERR("Could not set sound context");
        SoundDevice->Context = 0;
    }
    
    OpenALApi.alListenerfv(AL_ORIENTATION, ListenerOrientation);
    OpenALApi.alListenerfv(AL_POSITION, ListenerPosition);
    OpenALApi.alListenerf(AL_GAIN,0.95f);
    
    OpenALApi.alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    
    OpenALApi.alGenSources((ALuint)SOURCES,SoundDevice->Sources);
    
    SoundDevice->PrevStopped = false;
    SoundDevice->PrevPaused = false;
    SoundDevice->PrevMuted = false;
    
    SoundDevice->IsInitialized = SoundDevice->Device && SoundDevice->Context;
}

static void LoadSound(const char *FilePath, sound_device* SoundDevice, u32* BufferHandle, char* Name = "")
{
    LoadWavFile(SoundDevice, FilePath, BufferHandle);
}

static inline void LoadSounds(sound_device* SoundDevice, sound_commands* Commands)
{
    if(Commands->LoadSounds)
    {
        auto DirData = Commands->SoundsToLoad;
        
        for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
        {
            LoadSound(DirData.FilePaths[FileIndex], SoundDevice, &SoundDevice->SoundArray[SoundDevice->SoundIndex++], DirData.FileNames[FileIndex]);
        }
        Commands->LoadSounds = false;
        Commands->SoundsToLoad.FilesLength = 0;
    }
}

static inline void CleanupSound(sound_device* SoundDevice)
{
    OpenALApi.alDeleteSources(SOURCES, SoundDevice->Sources);
    OpenALApi.alDeleteBuffers(SoundDevice->BufferCount, SoundDevice->Buffers);
    
    SoundDevice->Device = OpenALApi.alcGetContextsDevice(SoundDevice->Context);
    OpenALApi.alcMakeContextCurrent(0);
    OpenALApi.alcDestroyContext(SoundDevice->Context);
    OpenALApi.alcCloseDevice(SoundDevice->Device);
}

static inline void PlaySound(sound_effect *SoundEffect, sound_device* SoundDevice, sound_commands* Commands)
{
    if (SoundEffect)
    {
        ALuint Source = 0;
        
        i32 SourceState;
        for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
        {
            OpenALApi.alGetSourcei(SoundDevice->Sources[SourceIndex],AL_SOURCE_STATE,&SourceState);
            if(SourceState != AL_PLAYING)
            {
                Source = SoundDevice->Sources[SourceIndex];
                SoundDevice->SourceGain[SourceIndex] = SoundEffect->SoundInfo.Gain;
                sound_effect SoundToStore;
                SoundToStore.Buffer = SoundEffect->Buffer;
                SoundToStore.Source = SoundEffect->Source;
                SoundToStore.SourceState = SoundEffect->SourceState;
                SoundToStore.SoundInfo = SoundEffect->SoundInfo;
                SoundDevice->SourceToSound[SourceIndex] = SoundToStore;
                break;
            }
        }
        OpenALApi.alSourcef(Source,AL_MAX_DISTANCE,1.0);
        OpenALApi.alSourcef(Source,AL_REFERENCE_DISTANCE,1.0);
        OpenALApi.alSourcef(Source, AL_ROLLOFF_FACTOR, 1.0f);
        OpenALApi.alSourcei(Source, AL_BUFFER, SoundDevice->SoundArray[SoundEffect->Buffer]);
        OpenALApi.alSourcef(Source, AL_PITCH, SoundEffect->SoundInfo.Pitch);
        OpenALApi.alSourcef(Source, AL_GAIN, Commands->Muted ? 0 : SoundEffect->SoundInfo.Gain);
        OpenALApi.alSource3f(Source, AL_POSITION, SoundEffect->SoundInfo.Position[0], SoundEffect->SoundInfo.Position[1], SoundEffect->SoundInfo.Position[2]);
        OpenALApi.alSource3f(Source, AL_VELOCITY, SoundEffect->SoundInfo.Velocity[0], SoundEffect->SoundInfo.Velocity[1], SoundEffect->SoundInfo.Velocity[2]);
        OpenALApi.alSourcei(Source, AL_LOOPING, SoundEffect->SoundInfo.Loop);
        OpenALApi.alSourcePlay(Source);
        OpenALApi.alGetSourcei(SoundEffect->Source, AL_SOURCE_STATE, &SoundEffect->SourceState);
    }
}

static inline void StopSound(sound_device* SoundDevice, sound_commands* Commands)
{
    for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
    {
        OpenALApi.alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,0);
        OpenALApi.alSourceStop(SoundDevice->Sources[SourceIndex]);
        OpenALApi.alSourcei(SoundDevice->Sources[SourceIndex],AL_BUFFER,0);
        if(!Commands->Muted)
        {
            OpenALApi.alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
        }
    }
}

static inline void PauseSound(sound_device* SoundDevice, sound_commands* Commands)
{
    for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
    {
        OpenALApi.alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,0);
        ALint SourceState;
        OpenALApi.alGetSourcei(SoundDevice->Sources[SourceIndex],AL_SOURCE_STATE,&SourceState);
        if(SourceState == AL_PLAYING)
        {
            OpenALApi.alSourcePause(SoundDevice->Sources[SourceIndex]);
        }
        else
        {
            OpenALApi.alSourcei(SoundDevice->Sources[SourceIndex], AL_BUFFER,0);
        }
        if(!Commands->Muted)
        {
            OpenALApi.alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
        }
    }
}

static void PlaySounds(sound_device* SoundDevice, sound_commands* Commands)
{
    if(SoundDevice && Commands && SoundDevice->Device)
    {
        LoadSounds(SoundDevice, Commands);
        
        if(Commands->Muted && !SoundDevice->PrevMuted)
        {
            SoundDevice->PrevMuted = true;
            for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
            {
                OpenALApi.alSourcef(SoundDevice->Sources[SourceIndex], AL_GAIN, 0);
            }
        }
        else if(!Commands->Muted && SoundDevice->PrevMuted)
        {
            SoundDevice->PrevMuted = false;
            for(u32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
            {
                OpenALApi.alSourcef(SoundDevice->Sources[SourceIndex],AL_GAIN,SoundDevice->SourceGain[SourceIndex]);
            }
        }
        
        if(Commands->Stopped && !SoundDevice->PrevStopped)
        {
            SoundDevice->PrevStopped = true;
            StopSound(SoundDevice, Commands);
            ResetCommands(Commands);
        }
        else
        {
            SoundDevice->PrevStopped = false;
            for (i32 Sound = 0;
                 Sound < Commands->SoundCount;
                 Sound++)
            {
                auto SoundEffect = (sound_effect*)&Commands->SoundArena.CurrentBlock->Base[Sound];
                PlaySound(SoundEffect, SoundDevice, Commands);
            }
            ResetCommands(Commands);
        }
        
        if(Commands->Paused && !SoundDevice->PrevPaused)
        {
            SoundDevice->PrevPaused = true;
            PauseSound(SoundDevice, Commands);
        }
        else if(!Commands->Paused && SoundDevice->PrevPaused)
        {
            SoundDevice->PrevPaused = false;
            OpenALApi.alSourcePlayv(SOURCES,SoundDevice->Sources);
        }
        
        if(!Commands->Muted && !Commands->Paused && !Commands->Stopped)
        {
            for(i32 SourceIndex = 0; SourceIndex < SOURCES; SourceIndex++)
            {
                auto Source = SoundDevice->Sources[SourceIndex];
                ALint SourceState;
                OpenALApi.alGetSourcei(Source,AL_SOURCE_STATE,&SourceState);
                auto Sound = SoundDevice->SourceToSound[Source];
                if(SourceState == AL_PLAYING)
                {
                    if(Sound.SoundInfo.Rolloff > 0)
                    {
                        //@Incomplete: Implement rolloff in game code or at least differently
                    }
                }
            }
        }
    }
}