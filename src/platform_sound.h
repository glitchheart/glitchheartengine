#ifndef PLATFORM_SOUND_H
#define PLATFORM_SOUND_H

struct sound_info
{
    real32 Pitch;
    real32 Gain;
    real32 Position[3];
    real32 Velocity[3];
    bool32 Loop;
};

struct sound_effect
{
    uint32 Buffer;
    uint32 Source;
    int32 SourceState;
    sound_info SoundInfo;
    bool32 PlayOnce;
};

struct loaded_sound
{
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

struct sound_queue
{
    uint32 SoundCount;
    sound_effect Sounds[32];

    uint32 StoppedSoundCount;
    sound_effect StoppedSounds[32];

    uint32 PausedSoundCount;
    sound_effect PausedSounds[32];
};

struct sound_manager
{
    ALCdevice *Device;
    ALCcontext *Context;

    union {
        loaded_sound SoundEffects[64];
        struct
        {
            loaded_sound Effect01;
        };
    };

    union {
        loaded_sound MusicTracks[32];
        struct
        {
            loaded_sound Track01;
        };
    };
    sound_queue SoundQueue;
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

#endif