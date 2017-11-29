
#define SOURCES 64
#define SOUNDS 64


struct sound_device
{
    b32 IsInitialized;
    
    FMOD_SYSTEM* System;
    FMOD_SOUND* Sounds[SOUNDS];
    i32 SoundCount;
    
    r32 SFXVolume;
    r32 MusicVolume;
    
    FMOD_CHANNELGROUP* ChannelGroups;
    FMOD_CHANNELGROUP* MasterGroup;
};
