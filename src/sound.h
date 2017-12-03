#ifndef SOUND_H
#define SOUND_H

#define MAX_SOUND_EFFECTS 400

struct sound_info
{
    r32 Pitch;
    r32 Gain;
    r32 Rolloff;
    b32 Loop;
    i32 LoopCount;
};

struct sound_effect
{
    i32 Buffer;
    sound_info SoundInfo;
};

struct sound_commands
{
    memory_arena SoundArena;
    i32 SoundCount;
    
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
