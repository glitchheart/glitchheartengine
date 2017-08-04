#ifndef PLATFORM_SOUND_H
#define PLATFORM_SOUND_H

#define SOURCES 64
#define SOUNDS 64

GENERIC_MAP(source_to_sound,sound_effect, i32, CmpInt, -1);
struct sound_effect;

struct sound_device
{
    ALCdevice *Device;
    ALCcontext *Context;
    b32 IsInitialized;
    ALuint Sources[SOURCES];
    ALuint Buffers[SOUNDS];
    u32 BufferCount;
    b32 PrevMuted;
    b32 Muted;
    b32 PrevPaused;
    b32 Paused;
    b32 PrevStopped;
    b32 Stopped;
    r32 SourceGain[SOURCES];
    r32 SFXVolume;
    r32 MusicVolume;
    source_to_sound_map SourceToSound;
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