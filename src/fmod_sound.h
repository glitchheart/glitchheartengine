#ifndef FMOD_SOUND_H
#define FMOD_SOUND_H

#define TIME_UNIT FMOD_TIMEUNIT_MS

struct SoundDevice;

struct SoundWorkData
{
    SoundDevice *device;
    char path[256];
    FMOD_SOUND* sound;
};

struct SoundDevice
{
    struct
    {
        WorkQueue *work_queue;
        SoundWorkData* work_data;
        i32 thread_info_count;
        ThreadInfo *thread_infos;
        i32 sounds_being_loaded;
    } threading;

    
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
    
    b32 *paused_channels;
    u32 *channel_positions;
    
    
    MemoryArena total_arena;
};

#endif
