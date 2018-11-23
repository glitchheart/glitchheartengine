#ifndef FMOD_SOUND_H
#define FMOD_SOUND_H

#define TIME_UNIT FMOD_TIMEUNIT_MS

struct SoundDevice
{
    b32 is_initialized;
    
    FMOD_SYSTEM* system;
    FMOD_SOUND** sounds;
    i32 sound_count;
    
    FMOD_CHANNEL** channels;
    i32 channel_count;

    r32 master_volume;
    r32 sfx_volume;
    r32 music_volume;
    b32 muted;
    
    FMOD_CHANNELGROUP* channel_groups;

    FMOD_CHANNELGROUP* sfx_channel_group;
    FMOD_CHANNELGROUP* music_channel_group;
    FMOD_CHANNELGROUP* master_group;
    
    FMOD_VECTOR **rolloff_points;
    
    b32 *paused_channels;
    u32 *channel_positions;
    
    
    MemoryArena total_arena;
};

#endif
