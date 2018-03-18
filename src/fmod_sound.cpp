static void fmod_error(FMOD_RESULT Err)
{
    Debug("FMOD Error! (%d) %s\n", Err, FMOD_ErrorString(Err));
}

static void load_sound(const char* file_path, SoundDevice* sound_device)
{
    //@Incomplete: Find out the exact parameters for a sound
    auto result = FMOD_System_CreateSound(sound_device->System, file_path, FMOD_DEFAULT, 0, &sound_device->Sounds[sound_device->SoundCount++]);
    if(result != FMOD_OK)
    {
        fmod_error(result);
    }
}

static void load_sounds(SoundDevice* sound_device, SoundCommands* commands)
{
    if(commands->LoadSounds)
    {
        auto dir_data = commands->SoundsToLoad;
        
        for(i32 file_index = 0; file_index < dir_data.FilesLength; file_index++)
        {
            load_sound(dir_data.FilePaths[file_index], sound_device);
        }
        commands->LoadSounds = false;
        commands->SoundsToLoad.FilesLength = 0;
    }
}


static void cleanup_sound(SoundDevice* sound_device)
{
    FMOD_System_Release(sound_device->System);
}

FMOD_RESULT F_CALLBACK channel_control_callback(FMOD_CHANNELCONTROL *chan_control, FMOD_CHANNELCONTROL_TYPE control_type, FMOD_CHANNELCONTROL_CALLBACK_TYPE callback_type, void *command_data1, void *command_data2)
{
    switch(callback_type)
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

static void play_sound(const sound_effect& sound_effect, SoundDevice* sound_device, SoundCommands* commands)
{
    auto sound = SoundDevice->Sounds[SoundEffect.Buffer];
    FMOD_CHANNEL* new_channel;
    auto result = FMOD_System_PlaySound(SoundDevice->System, sound, SoundDevice->MasterGroup, true, &new_channel);
    FMOD_Channel_SetPitch(new_channel, SoundEffect.SoundInfo.Pitch);
    FMOD_Channel_SetVolume(new_channel, SoundEffect.SoundInfo.Gain * Commands->SFXVolume);
    
    r32 vol;
    FMOD_Channel_GetVolume(new_channel, &vol);
    
    Assert(SoundEffect.SoundInfo.LoopCount >= -1);
    
    auto fmod_mode = SoundEffect.SoundInfo.Loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    
    FMOD_Channel_SetMode(new_channel, (FMOD_MODE)(FMOD_DEFAULT | fmod_mode));
    FMOD_Channel_SetLoopCount(new_channel, SoundEffect.SoundInfo.LoopCount);
    
    if(result != FMOD_OK)
    {
        fmod_error(result);
    }
    FMOD_Channel_SetPaused(new_channel, commands->Paused);
}

static inline void reset_commands(SoundCommands* commands)
{
    commands->SoundCount = 0;
    clear(&commands->SoundArena);
}

static void play_sounds(SoundDevice* sound_device, SoundCommands* commands)
{
    if(sound_device && commands && sound_device->System)
    {
        load_sounds(sound_device, commands);
        
        FMOD_System_GetMasterChannelGroup(sound_device->System, &sound_device->MasterGroup);
        
        FMOD_ChannelGroup_SetVolume(sound_device->MasterGroup, commands->SFXVolume);
        FMOD_ChannelGroup_SetMute(sound_device->MasterGroup, commands->Muted);
        FMOD_ChannelGroup_SetPaused(sound_device->MasterGroup, commands->Paused);
        
        if(commands->Stopped)
        {
            FMOD_ChannelGroup_Stop(sound_device->MasterGroup);
            reset_commands(commands);
        }
        
        if(!commands->Muted && !commands->Paused && !commands->Stopped)
        {
            for(i32 sound = 0;
                sound < commands->SoundCount;
                sound++)
            {
                SoundEffect* sound_effect =(SoundEffect*)commands->SoundArena.CurrentBlock->Base + sound;
                
                play_sound(*sound_effect, SoundDevice, Commands);
            }
        }
        if(FMOD_System_Update(sound_device->System) != FMOD_OK)
        {
            Debug("FMOD Failed updating\n");
        }
        reset_commands(commands);
    }
}

static void init_audio_fmod(SoundDevice* sound_device)
{
    FMOD_RESULT result;
    FMOD_SYSTEM* system;
    result = FMOD_System_Create(&system);
    
    if(result != FMOD_OK)
    {
        fmod_error(result);
        return;
    }
    
    result = FMOD_System_Init(system, 1024, FMOD_INIT_NORMAL, 0);
    if(result != FMOD_OK)
    {
        fmod_error(result);
    }
    
    u32 version;
    FMOD_System_GetVersion(system, &version);
    if(version < FMOD_VERSION)
    {
        Debug("Version error\n");
    }
    
    sound_device->System = system;
    sound_device->IsInitialized = true;
    
    result = FMOD_System_GetMasterChannelGroup(sound_device->System, &sound_device->MasterGroup);
    if(result != FMOD_OK)
    {
        fmod_error(result);
        return;
    }
}


