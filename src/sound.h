#ifndef SOUND_H
#define SOUND_H

struct sound_info
{
    r32 Pitch;
    r32 Gain;
    r32 Position[3];
    r32 Velocity[3];
    b32 Loop;
};

struct sound_effect
{
    u32 Buffer;
    u32 Source;
    i32 SourceState;
    sound_info SoundInfo;
};

struct sound_queue
{
    u32 SoundCount;
    sound_effect Sounds[32];
};


struct sound_manager
{
    b32 Muted;
    b32 Paused;
    b32 Stopped;
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
            sound_effect Slide01;
            sound_effect ButtonClick;
            sound_effect UseHealth;
            
            sound_effect MinotaurGrunt01;
            sound_effect MinotaurGrunt02;
            sound_effect MinotaurHit;
            sound_effect MinotaurDeath;
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