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
    
    r32 sfx_volume;
    r32 music_volume;
    
    FMOD_CHANNELGROUP* channel_groups;
    FMOD_CHANNELGROUP* master_group;
    
    FMOD_VECTOR **rolloff_points;
    
    b32 *paused_channels;
    u32 *channel_positions;
    
    MemoryArena one_shot_arena;
    i32 one_shot_point_count;
    
    MemoryArena total_arena;
};

#endif