#ifndef OPENAL_SOUND_H
#define OPENAL_SOUND_H

#define SOURCES 64
#define SOUNDS 64


char* ToString(sound_effect Sound)
{
    char* Result = PushTempString(64);
    sprintf(Result, "{Buffer: %d, \n Source: %d, \n SourceState: %d}", Sound.Buffer, Sound.Source, Sound.SourceState);
    return Result;
}


GENERIC_MAP(source_to_sound, sound_effect, i32, CmpInt, -1, "%d", INT_ASSIGN, VAL_COPY);
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
    
    b32 PrevPaused;
    
    b32 PrevStopped;
    
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