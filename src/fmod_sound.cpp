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
    FMOD_System_Release(SoundDevice->System);
}

FMOD_RESULT F_CALLBACK ChannelControlCallback(FMOD_CHANNELCONTROL *ChanControl, FMOD_CHANNELCONTROL_TYPE ControlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE CallbackType, void *CommandData1, void *CommandData2)
{
    switch(CallbackType)
    {
        case FMOD_CHANNELCONTROL_CALLBACK_END:
        {
        }
        break;
        case FMOD_CHANNELCONTROL_CALLBACK_VIRTUALVOICE:
        {
        }
        break;
        case FMOD_CHANNELCONTROL_CALLBACK_SYNCPOINT:
        {
        }
        break;
        case FMOD_CHANNELCONTROL_CALLBACK_OCCLUSION:
        {
        }
        break;
        default:
        break;
    }
    
    return FMOD_OK;
}

static void PlaySound(const sound_effect& SoundEffect, sound_device* SoundDevice, sound_commands* Commands)
{
    auto Sound = SoundDevice->Sounds[SoundEffect.Buffer];
    FMOD_CHANNEL* NewChannel;
    auto Result = FMOD_System_PlaySound(SoundDevice->System, Sound, SoundDevice->MasterGroup, true, &NewChannel);
    FMOD_Channel_SetPitch(NewChannel, SoundEffect.SoundInfo.Pitch);
    FMOD_Channel_SetVolume(NewChannel, SoundEffect.SoundInfo.Gain * Commands->SFXVolume);
    
    r32 Vol;
    FMOD_Channel_GetVolume(NewChannel, &Vol);
    
    Assert(SoundEffect.SoundInfo.LoopCount >= -1);
    
    auto FMODMode = SoundEffect.SoundInfo.Loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    
    FMOD_Channel_SetMode(NewChannel, (FMOD_MODE)(FMOD_DEFAULT | FMODMode));
    FMOD_Channel_SetLoopCount(NewChannel, SoundEffect.SoundInfo.LoopCount);
    
    if(Result != FMOD_OK)
    {
        FMODError(Result);
    }
    FMOD_Channel_SetPaused(NewChannel, Commands->Paused);
}

static inline void ResetCommands(sound_commands* Commands)
{
    Commands->SoundCount = 0;
    Clear(&Commands->SoundArena);
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
                sound_effect* SoundEffect =(sound_effect*)Commands->SoundArena.CurrentBlock->Base + Sound;
                
                PlaySound(*SoundEffect, SoundDevice, Commands);
            }
        }
        if(FMOD_System_Update(SoundDevice->System) != FMOD_OK)
        {
            DEBUG_PRINT("FMOD Failed updating\n");
        }
        ResetCommands(Commands);
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
    
    Result = FMOD_System_Init(System, 1024, FMOD_INIT_NORMAL, 0);
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


