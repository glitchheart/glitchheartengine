#ifndef SOUND_H
#define SOUND_H

struct SoundDevice;

namespace sound
{

struct SoundHandle
{
    i32 handle;
};

struct AudioSourceHandle
{
    i32 handle;
};

enum ChannelType
{
    MUSIC,
    SFX
};

enum LoopType
{
    LOOP_OFF,
    LOOP_NORMAL,
    LOOP_BIDI
};

enum SoundCommandType
{
    SC_PLAY_AUDIO_SOURCE,
    SC_STOP_AUDIO_SOURCE,
    SC_PAUSE_AUDIO_SOURCE,
    SC_UNPAUSE_AUDIO_SOURCE,
    SC_LOAD_SOUND,
    SC_ONE_SHOT,
    SC_AUDIO_SOURCE_POSITION
};

enum RolloffMode
{
    RM_INVERSE,
    RM_LINEAR,
    RM_LINEARSQUARE,
    RM_CUSTOM
};

enum RelativeSpaceMode
{
    RSM_HEADRELATIVE,
    RSM_WORLDRELATIVE
};

enum ChannelSpace
{
    CS_2D,
    CS_3D
};

struct ChannelAttributes
{
    ChannelType type;
    r32 volume;
    u32 position_ms;
    
    r32 pan_level;
    r32 pitch;
    r32 frequency;
    r32 low_pass_gain;
    b32 ramp;
    
    ChannelSpace channel_space;
    
    struct
    {
        math::Vec3 pos;
        math::Vec3 vel;
        
        struct
        {
            math::Vec3 orientation;
            r32 inside_angle;
            r32 outside_angle;
            r32 outside_volume;
        } cone;
        struct
        {
            math::Vec3 roll_off_points[32];
            i32 roll_off_point_count;
        } custom_rolloff;
        struct
        {
            b32 custom;
            r32 custom_level;
            r32 center_freq;
        } distance_filter;
        r32 doppler_level;
        r32 level_3d;
        r32 min_distance;
        r32 max_distance;
        struct
        {
            r32 direct;
            r32 reverb;
        } occlusion;
        r32 spread_angle;
        RolloffMode rolloff_mode;
        RelativeSpaceMode relative_space_mode;
    } att_3d;
    
    struct
    {
        LoopType type;
        i32 count;
        struct
        {
            u32 start;
            u32 end;
        } loop_points;
    } loop;
    
    struct
    {
        struct
        {
            r32 levels[32];
            i32 level_count;
        } input;
        struct
        {
            r32 levels[32];
            i32 level_count;
        } output;
    } mix_levels;
    struct
    {
        i32 instance;
        r32 wet;
    } reverb;
    
};

struct AudioSource
{
    AudioSourceHandle handle;
    SoundHandle sound_handle;
    
    ChannelAttributes channel_attributes;
    
    b32 paused;
    b32 muted;
};

struct ChannelGroup
{
    // Empty for now. May need stuff later
};

struct SoundCommand
{
    SoundCommandType type;
    
    union
    {
        struct
        {
            AudioSourceHandle handle;
        } play_audio_source;
        struct
        {
            AudioSourceHandle handle;
        } stop_audio_source;
        struct 
        {
            AudioSourceHandle handle;
        } pause_audio_source;
        struct 
        {
            AudioSourceHandle handle;
        } unpause_audio_source;
        struct
        {
            char file_path[255];
        } load_sound;
        struct
        {
            SoundHandle handle;
            ChannelAttributes channel_attributes;
        } one_shot;
        struct
        {
            AudioSourceHandle handle;
            u32 new_position_ms;
        } set_position;
    };
};

typedef void (*LoadSound)(const char *file_path, SoundDevice *device);

struct SoundApi
{
    SoundDevice *device;
    LoadSound load_sound;
};

struct SoundSystem
{
    SoundApi api_functions;
    b32 update;
    
    MemoryArena arena;
    
    SoundHandle *sounds;
    i32 sound_count;
    
    AudioSource *audio_sources;
    i32 audio_source_count;
    
    // For later. Doesn't really matter much for now
    ChannelGroup *channel_groups;
    i32 channel_group_count;
    
    SoundCommand* commands;
    i32 command_count;
    
    b32 paused;
    b32 muted;
    
    r32 sfx_volume;
    r32 music_volume;
    r32 master_volume;
};

struct SoundSystemData
{
    SoundSystem *system;
    SoundDevice *device;
};

}

#endif
