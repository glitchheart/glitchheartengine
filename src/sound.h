#ifndef SOUND_H
#define SOUND_H

struct sound_info
{
    char* Name;
    r32 Pitch;
    r32 Gain;
    r32 Position[3];
    r32 Velocity[3];
    r32 Rolloff;
    b32 Loop;
    i32 EntityHandle;
};

struct sound_effect
{
    u32 Buffer;
    u32 Source;
    i32 SourceState;
    sound_info SoundInfo;
};

GENERIC_MAP(sound_buffer, i32, char*, StrCmp,  NULL, "%s", STR_ASSIGN, VAL_COPY);

struct sound_commands
{
    i32 SoundCount;
    memory_arena SoundArena;
    
    directory_data SoundsToLoad;
    b32 LoadSounds;
    
    i32 Sounds;
    sound_buffer_map SoundMap;
    
    r32 SFXVolume;
    r32 MusicVolume;
    b32 Muted;
    b32 Paused;
    b32 Stopped;
};

#endif