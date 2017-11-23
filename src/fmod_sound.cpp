static void FMODError(FMOD_RESULT Err)
{
    DEBUG_PRINT("FMOD Error! (%d) %s\n", Err, FMOD_ErrorString(Err));
}

static void LoadSound(const char* FilePath, sound_device* SoundDevice)
{
    //@Incomplete: Find out the exact parameters for a sound
    auto Result = FMOD_System_CreateSound(SoundDevice->System, FilePath, FMOD_DEFAULT, 0, &SoundDevice->Sounds[SoundDevice->SoundCount++]);
    if(Result != FMOD_OK)
    {
        FMODError(Result);
    }
}

static void LoadSounds(sound_device* SoundDevice, sound_commands* Commands)
{
    if(Commands->LoadSounds)
    {
        auto DirData = Commands->SoundsToLoad;
        
        for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
        {
            LoadSound(DirData.FilePaths[FileIndex], SoundDevice);
        }
        Commands->LoadSounds = false;
        Commands->SoundsToLoad.FilesLength = 0;
    }
}


static void CleanupSound(sound_device* SoundDevice)
{
    //@Incomplete: Cleanup FMOD
}

static void PlaySound(sound_effect* SoundEffect, sound_device* SoundDevice, sound_commands* Commands)
{
    if(SoundEffect)
    {
        auto Sound = SoundDevice->Sounds[SoundEffect->Buffer];
        FMOD_CHANNEL* Channel;
        auto Result = FMOD_System_PlaySound(SoundDevice->System, Sound, 0, Commands->Paused, &Channel);
        FMOD_Channel_SetChannelGroup(Channel, SoundDevice->MasterGroup);
        if(Result != FMOD_OK)
        {
            FMODError(Result);
        }
    }
}

static inline void ResetCommands(sound_commands* Commands)
{
    Clear(&Commands->SoundArena);
    Commands->SoundCount = 0;
}

static void PlaySounds(sound_device* SoundDevice, sound_commands* Commands)
{
    if(SoundDevice && Commands && SoundDevice->System)
    {
        
        LoadSounds(SoundDevice, Commands);
        
        FMOD_System_GetMasterChannelGroup(SoundDevice->System, &SoundDevice->MasterGroup);
        
        FMOD_ChannelGroup_SetVolume(SoundDevice->MasterGroup, Commands->SFXVolume);
        FMOD_ChannelGroup_SetMute(SoundDevice->MasterGroup, Commands->Muted);
        FMOD_ChannelGroup_SetPaused(SoundDevice->MasterGroup, Commands->Paused);
        
        if(Commands->Stopped)
        {
            FMOD_ChannelGroup_Stop(SoundDevice->MasterGroup);
            ResetCommands(Commands);
        }
        
        if(!Commands->Muted && !Commands->Paused && !Commands->Stopped)
        {
            for(i32 Sound = 0;
                Sound < Commands->SoundCount;
                Sound++)
            {
                auto SoundEffect = (sound_effect*)&Commands->SoundArena.CurrentBlock->Base[Sound];
                PlaySound(SoundEffect, SoundDevice, Commands);
            }
            ResetCommands(Commands);
        }
    }
}

static void InitAudio_FMOD(sound_device* SoundDevice)
{
    FMOD_RESULT Result;
    FMOD_SYSTEM* System;
    Result = FMOD_System_Create(&System);
    
    if(Result != FMOD_OK)
    {
        FMODError(Result);
        return;
    }
    
    Result = FMOD_System_Init(System, 512, FMOD_INIT_NORMAL, 0);
    if(Result != FMOD_OK)
    {
        FMODError(Result);
    }
    
    u32 Version;
    FMOD_System_GetVersion(System, &Version);
    if(Version < FMOD_VERSION)
    {
        DEBUG_PRINT("Version error\n");
    }
    
    SoundDevice->System = System;
    SoundDevice->IsInitialized = true;
    
    Result = FMOD_System_GetMasterChannelGroup(SoundDevice->System, &SoundDevice->MasterGroup);
    if(Result != FMOD_OK)
    {
        FMODError(Result);
        return;
    }
}


