#define FMOD_DEBUG(err) fmod_error(err, __LINE__, __FILE__)
static void fmod_error(FMOD_RESULT err, int line, const char* file)
{
    debug("FMOD Error! (%d) %s in file %s on line %d\n", err, FMOD_ErrorString(err), file, line);
}

static void load_sound(const char* file_path, SoundDevice* sound_device)
{
    //@Incomplete: Find out the exact parameters for a sound
    auto result = FMOD_System_CreateSound(sound_device->system, file_path, FMOD_DEFAULT, 0, &sound_device->sounds[sound_device->sound_count++]);
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
}

/*static void load_sounds(SoundDevice* sound_device, SoundCommands* commands)
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
}*/

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

static void play_sound(SoundCommand *command, SoundDevice *device, SoundSystem *system)
{
    auto sound = device->sounds[command->one_shot.handle.handle - 1];
    
    FMOD_RESULT result = FMOD_OK;
    
    FMOD_CHANNEL *channel;
    result = FMOD_System_PlaySound(device->system, sound, device->master_group, true, &channel);
    
    FMOD_Channel_SetVolume(channel, command->one_shot.volume);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    FMOD_Channel_SetPaused(channel, false);
}

static void play_audio_source(AudioSource& audio_source, SoundDevice* sound_device, SoundSystem* system)
{
    auto sound = sound_device->sounds[audio_source.sound_handle.handle - 1];
    FMOD_RESULT result = FMOD_OK;
    
    b32 is_playing = 0;
    FMOD_Channel_IsPlaying(sound_device->channels[audio_source.handle.handle], &is_playing);
    
    debug("Playing %d\n", is_playing);
    
    auto channel = sound_device->channels[audio_source.handle.handle - 1];
    
    if(!is_playing)
    {
        result = FMOD_System_PlaySound(sound_device->system, sound, sound_device->master_group, true, &channel);
    }
    sound_device->channels[audio_source.handle.handle - 1] = channel;
    
    switch(audio_source.loop_type)
    {
        case LOOP_OFF:
        {
            FMOD_Channel_SetMode(channel, FMOD_LOOP_OFF);
        }
        break;
        case LOOP_NORMAL:
        {
            FMOD_Channel_SetMode(channel, FMOD_LOOP_NORMAL);
            
            // @Incomplete: -1 is default, add loop_count later!
            FMOD_Channel_SetLoopCount(channel, -1);
        }
        break;
        case LOOP_BIDI:
        {
            FMOD_Channel_SetMode(channel, FMOD_LOOP_BIDI);
        }
        break;
    }
    
    r32 vol;
    FMOD_Channel_GetVolume(channel, &vol);
    
    FMOD_Channel_SetVolume(channel, audio_source.volume);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    FMOD_Channel_SetPaused(channel, audio_source.paused);
}

static void stop_audio_source(AudioSource &audio_source, SoundDevice* sound_device, SoundSystem *system)
{
    auto channel = sound_device->channels[audio_source.handle.handle];
    
    FMOD_Channel_SetVolume(channel, 0.0f);
    
    FMOD_Channel_Stop(channel);
}

static void play_sound(SoundHandle handle, SoundDevice* sound_device, SoundSystem* system)
{
    auto sound = sound_device->sounds[handle.handle - 1];
    FMOD_CHANNEL* new_channel;
    auto result = FMOD_System_PlaySound(sound_device->system, sound, sound_device->master_group, true, &new_channel);
    //FMOD_Channel_SetPitch(new_channel, sound_effect.sound_info.pitch);
    //FMOD_Channel_SetVolume(new_channel, sound_effect.sound_info.gain * commands->sfx_volume);
    
    r32 vol;
    FMOD_Channel_GetVolume(new_channel, &vol);
    
    //assert(sound_effect.sound_info.loop_count >= -1);
    
    //auto fmod_mode = sound_effect.sound_info.loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
    
    //FMOD_Channel_SetMode(new_channel, (FMOD_MODE)(FMOD_DEFAULT | fmod_mode));
    //FMOD_Channel_SetLoopCount(new_channel, sound_effect.sound_info.loop_count);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    FMOD_Channel_SetPaused(new_channel, system->paused);
}

static void update_sound_commands(SoundDevice *device, SoundSystem *system)
{
    if(device->system)
    {
        FMOD_System_GetMasterChannelGroup(device->system, &device->master_group);
        
        FMOD_ChannelGroup_SetVolume(device->master_group, system->sfx_volume);
        FMOD_ChannelGroup_SetMute(device->master_group, system->muted);
        FMOD_ChannelGroup_SetPaused(device->master_group, system->paused);
        
        
        for(i32 i = 0; i < system->command_count; i++)
        {
            SoundCommand *command = (SoundCommand*)system->sound_commands.current_block->base + i;
            
            switch(command->type)
            {
                case SC_PLAY_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command->play_audio_source.handle.handle - 1];
                    play_audio_source(audio_source, device, system);
                }
                break;
                case SC_STOP_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command->play_audio_source.handle.handle - 1];
                    stop_audio_source(audio_source, device, system);
                }
                break;
                case SC_LOAD_SOUND:
                {
                    load_sound(command->load_sound.file_path, device);
                }
                break;
                case SC_ONE_SHOT:
                {
                    play_sound(command, device, system);
                }
                break;
            }
        }
        
        if(FMOD_System_Update(device->system) != FMOD_OK)
        {
            debug("FMOA failed updating\n");
        }
        
        clear(&system->sound_commands);
        system->command_count = 0;
    }
}

static void init_audio_fmod(SoundDevice* sound_device)
{
    FMOD_RESULT result;
    FMOD_SYSTEM* system;
    result = FMOD_System_Create(&system);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
        return;
    }
    
    result = FMOD_System_Init(system, 1024, FMOD_INIT_NORMAL, 0);
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    u32 version;
    FMOD_System_GetVersion(system, &version);
    if(version < FMOD_VERSION)
    {
        debug("Version error\n");
    }
    
    sound_device->system = system;
    sound_device->is_initialized = true;
    
    result = FMOD_System_GetMasterChannelGroup(sound_device->system, &sound_device->master_group);
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
        return;
    }
}

