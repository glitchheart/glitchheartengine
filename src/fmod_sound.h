#ifndef FMOD_SOUND_H
#define FMOD_SOUND_H

#define SOUNDS 64
#define CHANNELS 64

struct SoundDevice
{
    b32 is_initialized;
    
    FMOD_SYSTEM* system;
    FMOD_SOUND* sounds[SOUNDS];
    i32 sound_count;
    
    FMOD_CHANNEL* channels[CHANNELS];
    i32 channel_count;
    
    r32 sfx_volume;
    r32 music_volume;
    
    FMOD_CHANNELGROUP* channel_groups;
    FMOD_CHANNELGROUP* master_group;
};

#endif