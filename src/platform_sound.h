#ifndef PLATFORM_SOUND_H
#define PLATFORM_SOUND_H

#define SOURCES 64
#define SOUNDS 64

struct sound_device
{
    ALCdevice *Device;
    ALCcontext *Context;
    bool32 IsInitialized;
    ALuint Sources[SOURCES];
    ALuint Buffers[SOUNDS];
    uint32 BufferCount;
    bool32 Muted;
    bool32 Paused;
    bool32 Stopped;
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