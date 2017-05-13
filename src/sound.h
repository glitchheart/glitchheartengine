#ifndef SOUND_H
#define SOUND_H

struct sound_info
{
    real32 Pitch;
    real32 Gain;
    real32 Position[3];
    real32 Velocity[3];
    bool32 Loop;
};

struct sound_effect
{
    uint32 Buffer;
    uint32 Source;
    int32 SourceState;
    sound_info SoundInfo;
    bool32 PlayOnce;
};

struct sound_queue
{
    uint32 SoundCount;
    sound_effect Sounds[32];
    
    uint32 StoppedSoundCount;
    sound_effect StoppedSounds[32];
    
    uint32 PausedSoundCount;
    sound_effect PausedSounds[32];
};


struct sound_manager
{
    union 
    {
        sound_effect SoundEffects[64];
        struct
        {
            sound_effect Effect01;
            sound_effect SwordSlash01;
            sound_effect SwordHit01;
        };
    };
    
    union 
    {
        sound_effect MusicTracks[32];
        struct
        {
            sound_effect Track01;
            sound_effect MainMenuTrack;
        };
    };
    
    sound_queue SoundQueue;
};


#endif