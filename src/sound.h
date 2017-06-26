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
};

struct sound_queue
{
    uint32 SoundCount;
    sound_effect Sounds[32];
};


struct sound_manager
{
    bool32 Muted;
    bool32 Paused;
    bool32 Stopped;
    union 
    {
        sound_effect SoundEffects[64];
        struct
        {
            sound_effect Effect01;
            sound_effect SwordSlash01;
            sound_effect SwordHit01;
            sound_effect SwordHit02;
            sound_effect Dash;
            sound_effect Explosion;
            sound_effect Throw;
            sound_effect BarrelBreak;
            
            sound_effect ButtonClick;
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