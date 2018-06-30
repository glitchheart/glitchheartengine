#ifndef SOUND_H
#define SOUND_H

#define MAX_SOUNDS 400
#define MAX_AUDIO_SOURCES 64
#define MAX_CHANNEL_GROUPS 64
#define MAX_SOUND_COMMANDS 1024

/*
struct SoundInfo
{
    r32 pitch;
    r32 gain;
    r32 roll_off;
    b32 loop;
    i32 loop_count;
};

struct SoundEffect
{
    i32 buffer;
    SoundInfo sound_info;
};

struct AudioSource
{
    i32 handle;
    SoundInfo sound_info;
    i32 buffer_handle;
    
    b32 muted;
    b32 paused;
    b32 play;
};

struct SoundCommands
{
    MemoryArena sound_arena;
    i32 sound_count;
    
    AudioSource audio_sources[MAX_AUDIO_SOURCES];
    i32 audio_source_count;
    
    DirectoryData sounds_to_load;
    b32 load_sounds;
    
    i32 sounds;
    
    r32 sfx_volume;
    r32 music_volume;
    b32 muted;
    b32 paused;
    b32 stopped;
};
*/


struct SoundHandle
{
    i32 handle;
};

struct AudioSourceHandle
{
    i32 handle;
};

enum LoopType
{
    LOOP_OFF,
    LOOP_NORMAL,
    LOOP_BIDI
};

struct AudioSource
{
    AudioSourceHandle handle;
    SoundHandle sound_handle;
    
    // Add all the stuff that FMOD at least supports
    
    // Assume endless loop for now (loop count later)
    LoopType loop_type;
    u32 position_ms;
    
    b32 paused;
    r32 volume;
};

struct ChannelGroup
{
    // Empty for now. May need stuff later
};

enum SoundCommandType
{
    SC_PLAY_AUDIO_SOURCE,
    SC_STOP_AUDIO_SOURCE,
    SC_LOAD_SOUND,
    SC_ONE_SHOT
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
            char file_path[255];
        } load_sound;
        struct
        {
            SoundHandle handle;
            r32 volume;
        } one_shot;
    };
};

struct SoundSystem
{
    SoundHandle sounds[MAX_SOUNDS];
    i32 sound_count;
    
    AudioSource audio_sources[MAX_AUDIO_SOURCES];
    i32 audio_source_count;
    
    // For later. Doesn't really matter much for now
    ChannelGroup channel_groups[MAX_CHANNEL_GROUPS];
    i32 channel_group_count;
    
    MemoryArena sound_commands;
    i32 command_count;
    
    b32 paused;
    b32 muted;
    
    r32 sfx_volume;
    r32 music_volume;
};


#endif
