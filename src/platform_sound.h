#ifndef PLATFORM_SOUND_H
#define PLATFORM_SOUND_H

#define SOURCES 64
#define SOUNDS 64

struct sound_device
{
    ALCdevice *Device;
    ALCcontext *Context;
    b32 IsInitialized;
    ALuint Sources[SOURCES];
    ALuint Buffers[SOUNDS];
    u32 BufferCount;
    b32 Muted;
    b32 Paused;
    b32 Stopped;
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