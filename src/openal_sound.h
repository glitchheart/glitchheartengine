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

struct sound_device_info
{
    const char* DeviceName;
    i32 MajorVersion;
    i32 MinorVersion;
    u32 SourceCount;
    const char* Extensions[32];
    i32 ExtensionCount;
    b32 Selected;
};

struct oal_devices_list
{
    sound_device_info DeviceInfo[32];
    i32 DeviceCount;
};

struct sound_device
{
    ALCdevice *Device;
    sound_device_info DeviceInfo;
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
    sound_effect SourceToSound[SOURCES];
    
    i32 SoundIndex;
    u32 SoundArray[SOUNDS];
};

struct RIFF_header
{
    unsigned char ChunkID[4];
    unsigned int ChunkSize;
    unsigned char Format[4];
};

struct wave_format
{
    unsigned char SubChunkID[4];
    unsigned int SubChunkSize;
    short AudioFormat;
    short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    short BlockAlign;
    short BitsPerSample;
};

struct wave_data
{
    unsigned char SubChunkID[4];
    unsigned int SubChunk2Size;
};

#endif