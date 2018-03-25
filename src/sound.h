#ifndef SOUND_H
#define SOUND_H

#define MAX_SOUND_EFFECTS 400
#define MAX_AUDIO_SOURCES 64

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

#endif
