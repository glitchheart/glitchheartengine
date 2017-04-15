#ifndef SOUND_H
#define SOUND_H

enum sound_effects
{
    THEME_MUSIC = 0;
    SOUND_02 = 1;
    SOUND_03 = 2;
    SOUND_04 = 3;
    SOUND_05 = 4;
    SOUND_06 = 5;
};

struct sound_info
{
    real32 Pitch;
    real32 Gain;
    real32 Position[3];
    real32 Velocity[3];
    bool32 Loop;
};

struct loaded_sound
{
    uint32 AssetIndex;
    uint32 BufferSize;
    ALuint Buffer;
    ALuint Source;
    ALenum Format;
    ALsizei Size;
    ALsizei Frequency;
    ALvoid *Data;
    ALint SourceState;
    sound_info SoundInfo;
};

struct sound_manager
{
    ALCdevice *Device;
    ALCcontext *Context;
    loaded_sound LoadedSounds[32];
    uint32 LoadedSoundCount;

    bool32 IsInitialized;
};

struct RIFF_header
{
    char ChunkID[4];
    long ChunkSize;
    char Format[4];
};

struct wave_format
{
    char SubChunkID[4];
    long SubChunkSize;
    short AudioFormat;
    short NumChannels;
    long SampleRate;
    long ByteRate;
    short BlockAlign;
    short BitsPerSample;
};

struct wave_data
{
    char SubChunkID[4];
    long SubChunk2Size;
};

static void LoadWavFile(const char *Filename, loaded_sound *LoadedSound)
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
        ;

        if (!fread(Data, WaveData.SubChunk2Size, 1, SoundFile))
        {
            HandleError(__FILE__, __LINE__, "Error loading Wave data");
        }

        LoadedSound->Size = WaveData.SubChunk2Size;
        LoadedSound->Frequency = WaveFormat.SampleRate;

        if (WaveFormat.NumChannels == 1)
        {
            if (WaveFormat.BitsPerSample == 8)
            {
                LoadedSound->Format = AL_FORMAT_MONO8;
            }
            else if (WaveFormat.BitsPerSample == 16)
            {
                LoadedSound->Format = AL_FORMAT_MONO16;
            }
        }
        else if (WaveFormat.NumChannels == 2)
        {
            if (WaveFormat.BitsPerSample == 8)
            {
                LoadedSound->Format = AL_FORMAT_STEREO8;
            }
            else if (WaveFormat.BitsPerSample == 16)
            {
                LoadedSound->Format = AL_FORMAT_STEREO16;
            }
        }
        alGenBuffers(1, &LoadedSound->Buffer);
        alBufferData(LoadedSound->Buffer, LoadedSound->Format, (void *)Data, LoadedSound->Size, LoadedSound->Frequency);
        fclose(SoundFile);
        LoadedSound->Data = Data;

        printf("Samplerate: %d, Size: %d\n", LoadedSound->Frequency, LoadedSound->Size);
    }
}

static void list_audio_devices(const ALCchar *devices)
{
    const ALCchar *device = devices, *next = devices + 1;
    size_t len = 0;

    fprintf(stdout, "Devices list:\n");
    fprintf(stdout, "----------\n");
    while (device && *device != '\0' && next && *next != '\0')
    {
        fprintf(stdout, "%s\n", device);
        len = strlen(device);
        device += (len + 1);
        next += (len + 2);
    }
    fprintf(stdout, "----------\n");
}

static void InitAudio(sound_manager *SoundManager)
{
    SoundManager->Device = alcOpenDevice(0);
    if (!SoundManager->Device)
    {
        SoundManager->Device = 0;
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

    list_audio_devices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));

    SoundManager->Context = alcCreateContext(SoundManager->Device, 0);
    alcMakeContextCurrent(SoundManager->Context);
    if (!SoundManager->Context)
    {
        HandleError(__FILE__, __LINE__, "Could not set sound context");
        SoundManager->Context = 0;
    }

    alListenerfv(AL_ORIENTATION, ListenerOrientation);
    SoundManager->LoadedSoundCount = 0;
    SoundManager->IsInitialized = SoundManager->Device && SoundManager->Context;
}

static void LoadSound(const char *filename, sound_manager *SoundManager, sound_info SoundInfo)
{
    loaded_sound LoadedSound = {};
    alGenSources((ALuint)1, &LoadedSound.Source);
    LoadWavFile(filename, &LoadedSound);
    alSourcei(LoadedSound.Source, AL_BUFFER, LoadedSound.Buffer);
    LoadedSound.AssetIndex = SoundManager->LoadedSoundCount++;
    LoadedSound.SoundInfo = SoundInfo;
    SoundManager->LoadedSounds[LoadedSound.AssetIndex] = LoadedSound;
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

    LoadSound("./assets/audio/Deadliners Track 1.wav", SoundManager, DefaultSoundInfo);
    LoadSound("./assets/audio/Countdown_1.wav", SoundManager, DefaultSoundInfo);
    LoadSound("./assets/audio/Countdown_2.wav", SoundManager, DefaultSoundInfo);
    LoadSound("./assets/audio/Countdown_3.wav", SoundManager, DefaultSoundInfo);
    LoadSound("./assets/audio/Countdown_4.wav", SoundManager, DefaultSoundInfo);
    LoadSound("./assets/audio/Countdown_5.wav", SoundManager, DefaultSoundInfo);
    // Add more sounds here if necessary
}

#endif