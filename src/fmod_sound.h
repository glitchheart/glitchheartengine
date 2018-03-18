
#define SOURCES 64
#define SOUNDS 64


struct SoundDevice
{
    b32 is_initialized;
    
    FMOD_SYSTEM* system;
    FMOD_SOUND* sounds[SOUNDS];
    i32 sound_count;
    
    r32 sfx_volume;
    r32 music_volume;
    
    FMOD_CHANNELGROUP* channel_groups;
    FMOD_CHANNELGROUP* master_group;
};
