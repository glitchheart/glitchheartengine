static void fmod_error(FMOD_RESULT err)
{
    Debug("FMOD Error! (%d) %s\n", err, FMOD_ErrorString(err));
}

static void load_sound(const char* file_path, SoundDevice* sound_device)
{
    //@Incomplete: Find out the exact parameters for a sound
    auto result = FMOD_System_CreateSound(sound_device->system, file_path, FMOD_DEFAULT, 0, &sound_device->sounds[sound_device->sound_count++]);
    if(result != FMOD_OK)
    {
        fmod_error(result);
    }
}

static void load_sounds(SoundDevice* sound_device, SoundCommands* commands)
{
    if(commands->load_sounds)
    {
        auto dir_data = commands->sounds_to_load;
        
        for(i32 file_index = 0; file_index < dir_data.files_length; file_index++)
        {
            load_sound(dir_data.file_paths[file_index], sound_device);
        }
        commands->load_sounds = false;
        commands->sounds_to_load.files_length = 0;
    }
}


static void cleanup_sound(SoundDevice* sound_device)
{
    FMOD_System_Release(sound_device->system);
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

static void play_sound(const SoundEffect& sound_effect, SoundDevice* sound_device, SoundCommands* commands)
{
    auto sound = sound_device->sounds[sound_effect.buffer];
    FMOD_CHANNEL* new_channel;
    auto result = FMOD_System_PlaySound(sound_device->system, sound, sound_device->master_group, true, &new_channel);
    FMOD_Channel_SetPitch(new_channel, sound_effect.sound_info.pitch);
    FMOD_Channel_SetVolume(new_channel, sound_effect.sound_info.gain * commands->sfx_volume);
    
    r32 vol;
    FMOD_Channel_GetVolume(new_channel, &vol);
    
    Assert(sound_effect.sound_info.loop_count >= -1);
    
    auto fmod_mode = sound_effect.sound_info.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    
    FMOD_Channel_SetMode(new_channel, (FMOD_MODE)(FMOD_DEFAULT | fmod_mode));
    FMOD_Channel_SetLoopCount(new_channel, sound_effect.sound_info.loop_count);
    
    if(result != FMOD_OK)
    {
        fmod_error(result);
    }
    FMOD_Channel_SetPaused(new_channel, commands->paused);
}

static inline void reset_commands(SoundCommands* commands)
{
    commands->sound_count = 0;
    clear(&commands->sound_arena);
}

static void play_sounds(SoundDevice* sound_device, SoundCommands* commands)
{
    if(sound_device && commands && sound_device->system)
    {
        load_sounds(sound_device, commands);
        
        FMOD_System_GetMasterChannelGroup(sound_device->system, &sound_device->master_group);
        
        FMOD_ChannelGroup_SetVolume(sound_device->master_group, commands->sfx_volume);
        FMOD_ChannelGroup_SetMute(sound_device->master_group, commands->muted);
        FMOD_ChannelGroup_SetPaused(sound_device->master_group, commands->paused);
        
        if(commands->stopped)
        {
            FMOD_ChannelGroup_Stop(sound_device->master_group);
            reset_commands(commands);
        }
        
        if(!commands->muted && !commands->paused && !commands->stopped)
        {
            for(i32 sound = 0;
                sound < commands->sound_count;
                sound++)
            {
                SoundEffect* sound_effect =(SoundEffect*)commands->sound_arena.current_block->base + sound;
                
                play_sound(*sound_effect, sound_device, commands);
            }
        }
        if(FMOD_System_Update(sound_device->system) != FMOD_OK)
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
    
    sound_device->system = system;
    sound_device->is_initialized = true;
    
    result = FMOD_System_GetMasterChannelGroup(sound_device->system, &sound_device->master_group);
    if(result != FMOD_OK)
    {
        fmod_error(result);
        return;
    }
}


