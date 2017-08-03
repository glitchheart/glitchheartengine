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

struct sound_queue
{
    i32 SoundCount;
    sound_effect Sounds[32];
};


struct sound_effects
{
    
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
            
            sound_effect ShieldImpact;
            
            sound_effect MinotaurGrunt01;
            sound_effect MinotaurGrunt02;
            sound_effect MinotaurHit;
            sound_effect MinotaurDeath;
            sound_effect MinotaurStomp;
            sound_effect Splash01;
            sound_effect Bonfire;
        };
    };
    
    union 
    {
        sound_effect MusicTracks[32];
        struct
        {
            sound_effect Track01;
            sound_effect MainMenuTrack;
            sound_effect Brugt;
        };
    };
};


struct sound_manager
{
    r32 SFXGain;
    r32 MusicGain;
    b32 Muted;
    b32 Paused;
    b32 Stopped;
    sound_effects SoundEffects;
};


#endif