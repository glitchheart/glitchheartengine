#define FMOD_DEBUG(err) fmod_error(err, __LINE__, __FILE__)
static void fmod_error(FMOD_RESULT err, int line, const char* file)
{
    debug("FMOD Error! (%d) %s in file %s on line %d\n", err, FMOD_ErrorString(err), file, line);
}

void fmod_error_check(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        FMOD_DEBUG(result);
        exit(EXIT_FAILURE);
    }
}

static void load_sound(const char* file_path, SoundDevice* device, sound::SoundCreateFlag create_flag = sound::SoundCreateFlag::CREATE)
{
    assert(device->sound_count + 1 < global_max_sounds);

    FMOD_RESULT result = FMOD_OK;
    
    if(create_flag == sound::SoundCreateFlag::CREATE)
    {
        result = FMOD_System_CreateSound(device->system, file_path, 0, nullptr, &device->sounds[device->sound_count++]);
    }
    else if(create_flag == sound::SoundCreateFlag::STREAM)
    {
        result = FMOD_System_CreateStream(device->system, file_path, 0, nullptr, &device->sounds[device->sound_count++]);
    }
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
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
            sound::AudioSource *as;
            FMOD_Channel_GetUserData((FMOD_CHANNEL*)chan_control, (void**)&as);
            if(as)
            {
                as->channel_attributes.position_ms = 0;
            }
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

static void set_channel_attributes(FMOD_CHANNEL *channel, sound::ChannelAttributes attributes, sound::SoundSystem *system, SoundDevice *device, i32 handle = 0)
{
    auto mode = FMOD_DEFAULT;
    
    // LOOP_OFF is default?
    switch(attributes.loop.type)
    {
        case sound::LoopType::LOOP_NORMAL:
        {
            mode |= FMOD_LOOP_NORMAL;
            // @Incomplete: -1 is default, add loop_count later!
            FMOD_Channel_SetLoopCount(channel, attributes.loop.count);
            
            if(attributes.loop.loop_points.start != 0 || attributes.loop.loop_points.end != 0)
            {
                
                FMOD_Channel_SetLoopPoints(channel, attributes.loop.loop_points.start, TIME_UNIT, attributes.loop.loop_points.end, TIME_UNIT);
            }
        }
        break;
        case sound::LoopType::LOOP_BIDI:
        {
            mode |= FMOD_LOOP_BIDI;
        }
        break;
        default:
	{
	    FMOD_Channel_SetLoopCount(channel, 0);
	    FMOD_Channel_SetMode(channel, FMOD_DEFAULT | FMOD_LOOP_OFF);
	}
        break;
    }
    
    if(attributes.channel_space == sound::ChannelSpace::CS_3D)
    {
        mode |= FMOD_3D;
        
        FMOD_VECTOR *pos = nullptr;
        FMOD_VECTOR *vel = nullptr;
        
        FMOD_Channel_Get3DAttributes(channel, pos, vel, nullptr);
        
        pos->x = attributes.att_3d.pos.x;
        pos->y = attributes.att_3d.pos.y;
        pos->z = attributes.att_3d.pos.z;
        
        vel->x = attributes.att_3d.vel.x;
        vel->y = attributes.att_3d.vel.y;
        vel->z = attributes.att_3d.vel.z;
        
        FMOD_Channel_Set3DAttributes(channel, pos, vel, nullptr);
        
        FMOD_VECTOR *orientation = nullptr;
        
        FMOD_Channel_Get3DConeOrientation(channel, orientation);
        
        orientation->x = attributes.att_3d.cone.orientation.x;
        orientation->y = attributes.att_3d.cone.orientation.y;
        orientation->z = attributes.att_3d.cone.orientation.z;
        
        FMOD_Channel_Set3DConeOrientation(channel, orientation);
        FMOD_Channel_Set3DConeSettings(channel, attributes.att_3d.cone.inside_angle, attributes.att_3d.cone.outside_angle, attributes.att_3d.cone.outside_volume);
        
        FMOD_Channel_Set3DDistanceFilter(channel, attributes.att_3d.distance_filter.custom, attributes.att_3d.distance_filter.custom_level, attributes.att_3d.distance_filter.center_freq);
        FMOD_Channel_Set3DDopplerLevel(channel, attributes.att_3d.doppler_level);
        FMOD_Channel_Set3DLevel(channel, attributes.att_3d.level_3d = 1.0f);
        
        FMOD_Channel_Set3DMinMaxDistance(channel, attributes.att_3d.min_distance, attributes.att_3d.max_distance);
        FMOD_Channel_Set3DOcclusion(channel, attributes.att_3d.occlusion.direct, attributes.att_3d.occlusion.reverb);
        FMOD_Channel_Set3DSpread(channel, attributes.att_3d.spread_angle);
    }
    
    FMOD_Channel_SetPan(channel, attributes.pan_level);
    FMOD_Channel_SetPitch(channel, attributes.pitch);
    FMOD_Channel_SetLowPassGain(channel, attributes.low_pass_gain);
    FMOD_Channel_SetVolumeRamp(channel, attributes.ramp);
    
    if(attributes.mix_levels.input.level_count > 0 && attributes.mix_levels.output.level_count > 0)
    {
        FMOD_Channel_SetMixLevelsInput(channel, attributes.mix_levels.input.levels, attributes.mix_levels.input.level_count);
        
        auto output_levels = attributes.mix_levels.output.levels;
        
        if(attributes.mix_levels.output.level_count == 8)
        {
            FMOD_Channel_SetMixLevelsOutput(channel, output_levels[0], output_levels[1], output_levels[2], output_levels[3], output_levels[4], output_levels[5], output_levels[6], output_levels[7]);
        }
    }
    
    FMOD_Channel_SetReverbProperties(channel, attributes.reverb.instance, attributes.reverb.wet);
    FMOD_Channel_SetMode(channel, (FMOD_MODE)mode);
    
    r32 vol;
    FMOD_Channel_GetVolume(channel, &vol);
    
    FMOD_Channel_SetVolume(channel, attributes.volume);
}

static void play_audio_source(sound::AudioSource& audio_source, SoundDevice* device, sound::SoundSystem* system)
{
    assert(device->sounds && audio_source.sound_handle.handle - 1 < device->sound_count);
    auto sound = device->sounds[audio_source.sound_handle.handle - 1];
    auto &channel_attributes = audio_source.channel_attributes;
    auto as_handle = audio_source.handle.handle;
    
    FMOD_RESULT result = FMOD_OK;
    
    b32 is_playing = 0;
    FMOD_Channel_IsPlaying(device->channels[as_handle - 1], &is_playing);
    
    auto channel = device->channels[as_handle - 1];
    
    if(!is_playing)
    {
        if(channel_attributes.type == sound::ChannelType::SFX)
        {
            result = FMOD_System_PlaySound(device->system, sound, device->sfx_channel_group, true, &channel);
        }
        else if(channel_attributes.type == sound::ChannelType::MUSIC)
        {
            result = FMOD_System_PlaySound(device->system, sound, device->music_channel_group, true, &channel);
        }
    }
    
    device->channels[as_handle - 1] = channel;
    
    // FMOD_Channel_SetCallback(channel, channel_control_callback);
    // FMOD_Channel_SetUserData(channel, &audio_source);
    
    set_channel_attributes(channel, channel_attributes, system, device, as_handle);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    FMOD_Channel_SetPaused(channel, audio_source.paused);
}

static void play_sound(sound::SoundCommand &command, SoundDevice *device, sound::SoundSystem *system)
{
    auto sound = device->sounds[command.one_shot.handle.handle - 1];
    
    FMOD_RESULT result = FMOD_OK;
    
    FMOD_CHANNEL *channel = nullptr;

    if(command.one_shot.channel_attributes.type == sound::ChannelType::SFX)
    {
        result = FMOD_System_PlaySound(device->system, sound, device->sfx_channel_group, true, &channel);
    }
    else
    {
	result = FMOD_System_PlaySound(device->system, sound, device->music_channel_group, true, &channel);

	// @Note: We set the priority to 0, to make sure that music channels do not get stolen by FMOD
	//        If we still experience stolen channels, look at global_max_channels in init_globals.h.
	FMOD_Channel_SetPriority(channel, 0);
    }
    
    set_channel_attributes(channel, command.one_shot.channel_attributes, system, device);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    // FMOD_Channel_SetCallback(channel, channel_control_callback);
    // FMOD_Channel_SetUserData(channel, (void**)&device);
    
    FMOD_Channel_SetPaused(channel, false);
}


static void stop_audio_source(sound::AudioSource &audio_source, SoundDevice* device, sound::SoundSystem *system)
{
    auto channel = device->channels[audio_source.handle.handle - 1];
    
    FMOD_Channel_SetVolume(channel, 0.0f);
    
    FMOD_Channel_Stop(channel);
}

static void pause_audio_source(sound::AudioSource &audio_source, SoundDevice* device, sound::SoundSystem *system, b32 paused)
{
    auto channel = device->channels[audio_source.handle.handle - 1];
    
    audio_source.paused = paused;
    device->paused_channels[audio_source.handle.handle - 1] = paused;
    FMOD_Channel_SetPaused(channel, paused);
}

static void set_position_audio_source(sound::SoundCommand &command, sound::AudioSource &audio_source, SoundDevice *device, sound::SoundSystem *system)
{
    auto channel = device->channels[audio_source.handle.handle - 1];
    
    audio_source.channel_attributes.position_ms = command.set_position.new_position_ms;
    device->channel_positions[audio_source.handle.handle - 1] = command.set_position.new_position_ms;
    FMOD_Channel_SetPosition(channel, command.set_position.new_position_ms, TIME_UNIT);
}


static void update_sound_commands(SoundDevice *device, sound::SoundSystem *system, r64 delta_time, b32 *save_config)
{
    if(device->system)
    {
        FMOD_System_GetMasterChannelGroup(device->system, &device->master_group);

        FMOD_ChannelGroup_SetVolume(device->master_group, system->master_volume);
        FMOD_ChannelGroup_SetMute(device->master_group, system->muted);
        FMOD_ChannelGroup_SetPaused(device->master_group, system->paused);
        FMOD_ChannelGroup_SetVolume(device->music_channel_group, system->music_volume);
        FMOD_ChannelGroup_SetVolume(device->sfx_channel_group, system->sfx_volume);
        
        if(system->sfx_volume != device->sfx_volume || system->music_volume != device->music_volume || system->master_volume != device->master_volume)
        {
            device->sfx_volume = system->sfx_volume;
            device->music_volume = system->music_volume;
            device->master_volume = system->master_volume;
            *save_config = true;
        }

        for(i32 i = 0; i < system->command_count; i++)
        {
            sound::SoundCommand& command = system->commands[i];
            
            switch(command.type)
            {
                case sound::SoundCommandType::SC_PLAY_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command.play_audio_source.handle.handle - 1];
                    play_audio_source(audio_source, device, system);
                }
                break;
                case sound::SoundCommandType::SC_STOP_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command.play_audio_source.handle.handle - 1];
                    stop_audio_source(audio_source, device, system);
                }
                break;
                case sound::SoundCommandType::SC_PAUSE_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command.play_audio_source.handle.handle - 1];
                    pause_audio_source(audio_source, device, system, true);
                }
                break;
                case sound::SoundCommandType::SC_UNPAUSE_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command.play_audio_source.handle.handle - 1];
                    pause_audio_source(audio_source, device, system, false);
                }
                break;
                case sound::SoundCommandType::SC_LOAD_SOUND:
                {
                    load_sound(command.load_sound.file_path, device);
                }
                break;
                case sound::SoundCommandType::SC_ONE_SHOT:
                {
                    play_sound(command, device, system);
                }
                break;
                case sound::SoundCommandType::SC_AUDIO_SOURCE_POSITION:
                {
                    auto &audio_source = system->audio_sources[command.play_audio_source.handle.handle - 1];
                    set_position_audio_source(command, audio_source, device, system);
                }
                break;
            }
        }

        for(i32 i = 0; i < system->audio_source_count; i++)
        {
            sound::AudioSource& source = system->audio_sources[i];

            FMOD_CHANNEL* channel = device->channels[source.handle.handle - 1];
            b32 is_playing;
            FMOD_Channel_IsPlaying(channel, &is_playing);
            if(is_playing)
            {
                FMOD_Channel_SetVolume(channel, source.channel_attributes.volume);
                // set_channel_attributes(channel, source.channel_attributes, system, device, source.handle.handle);
            }

        }
        
        if(FMOD_System_Update(device->system) != FMOD_OK)
        {
            debug("FMOD failed updating\n");
        }

        system->command_count = 0;
    }
}


static void init_audio_fmod(sound::SoundSystemData *data)
{
    SoundDevice *device = data->device;
    sound::SoundSystem *sound_system = data->system;
    
    FMOD_RESULT result;
    FMOD_SYSTEM* system;
    result = FMOD_System_Create(&system);
    
    fmod_error_check(result);
    
    i32 num_drivers = 0;
    FMOD_System_GetNumDrivers(system, &num_drivers);
    
    if(num_drivers == 0)
    {
        result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_NOSOUND);
        fmod_error_check(result);
    }

    // @Note: Global max channels should be high enough, so that no channels will ever get stolen during gameplay.
    result = FMOD_System_Init(system, global_max_channels, FMOD_INIT_NORMAL, nullptr);
    fmod_error_check(result);
    
    u32 version;
    FMOD_System_GetVersion(system, &version);
    if(version < FMOD_VERSION)
    {
        debug("Version error\n");
    }
    
    device->system = system;
    device->is_initialized = true;
    
    result = FMOD_System_GetMasterChannelGroup(device->system, &device->master_group);
    fmod_error_check(result);
    
    device->paused_channels = push_array(&device->total_arena, global_max_channels, b32);
    device->channel_positions = push_array(&device->total_arena, global_max_channels, u32);
    device->channels = push_array(&device->total_arena, global_max_channels, FMOD_CHANNEL*);
    device->sounds = push_array(&device->total_arena, global_max_sounds, FMOD_SOUND*);

    FMOD_System_CreateChannelGroup(device->system, "Music", &device->music_channel_group);
    FMOD_System_CreateChannelGroup(device->system, "SFX", &device->sfx_channel_group);

    sound_system->api_functions.device = device;
    sound_system->api_functions.load_sound = load_sound;
}


static void init_audio_fmod_thread(WorkQueue *queue, void* data_ptr)
{
    init_audio_fmod((sound::SoundSystemData*)data_ptr);
}
