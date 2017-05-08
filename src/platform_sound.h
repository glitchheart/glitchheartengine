#ifndef PLATFORM_SOUND_H
#define PLATFORM_SOUND_H

struct sound_device
{
    ALCdevice *Device;
    ALCcontext *Context;
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