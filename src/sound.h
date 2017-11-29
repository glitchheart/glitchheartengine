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
    i32 LoopCount;
};

struct sound_effect
{
    u32 Buffer;
    u32 Source;
    i32 SourceState;
    sound_info SoundInfo;
};

struct sound_commands
{
    i32 SoundCount;
    memory_arena SoundArena;
    
    directory_data SoundsToLoad;
    b32 LoadSounds;
    
    i32 Sounds;
    
    r32 SFXVolume;
    r32 MusicVolume;
    b32 Muted;
    b32 Paused;
    b32 Stopped;
};

#endif
