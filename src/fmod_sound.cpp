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
            AudioSource *as;
            FMOD_Channel_GetUserData((FMOD_CHANNEL*)chan_control, (void**)&as);
            if(as)
            {
                as->channel_attributes.position_ms = 0;
            }
            
            SoundDevice *device;
            FMOD_Channel_GetUserData((FMOD_CHANNEL*)chan_control, (void**)&device);
            
            if(device)
            {
                device->one_shot_point_count--;
                if(device->one_shot_point_count == 0)
                {
                    clear(&device->one_shot_arena);
                }
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

static void set_channel_attributes(FMOD_CHANNEL *channel, ChannelAttributes attributes, SoundSystem *system, SoundDevice *device, i32 handle = 0)
{
    auto mode = FMOD_DEFAULT;
    
    // LOOP_OFF is default?
    switch(attributes.loop.type)
    {
        case LOOP_NORMAL:
        {
            mode |= FMOD_LOOP_NORMAL;
            debug("Loop count: %d, type: %d\n", attributes.loop.count, attributes.loop.type);
            // @Incomplete: -1 is default, add loop_count later!
            FMOD_Channel_SetLoopCount(channel, attributes.loop.count);
            
            if(attributes.loop.loop_points.start != 0 || attributes.loop.loop_points.end != 0)
            {
                
                FMOD_Channel_SetLoopPoints(channel, attributes.loop.loop_points.start, TIME_UNIT, attributes.loop.loop_points.end, TIME_UNIT);
            }
        }
        break;
        case LOOP_BIDI:
        {
            mode |= FMOD_LOOP_BIDI;
        }
        break;
        default:
        break;
    }
    
    if(attributes.channel_space == CS_3D)
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
        if(attributes.att_3d.rolloff_mode == RM_CUSTOM)
        {
            
            mode |= FMOD_3D_CUSTOMROLLOFF;
            
            if(attributes.att_3d.custom_rolloff.roll_off_point_count > 0)
            {
                FMOD_VECTOR *rolloff_points;
                if(handle != 0)
                {
                    if(!device->rolloff_points[handle - 1])
                    {
                        device->rolloff_points[handle - 1] = push_array(&system->arena, attributes.att_3d.custom_rolloff.roll_off_point_count, FMOD_VECTOR);
                    }
                    rolloff_points = device->rolloff_points[handle - 1];
                }
                else
                {
                    rolloff_points = push_array(&device->one_shot_arena, attributes.att_3d.custom_rolloff.roll_off_point_count, FMOD_VECTOR);
                }
                
                auto points = attributes.att_3d.custom_rolloff.roll_off_points;
                
                for(i32 i = 0; i < attributes.att_3d.custom_rolloff.roll_off_point_count; i++)
                {
                    rolloff_points[i].x = points[i].x;
                    rolloff_points[i].y = points[i].y;
                    rolloff_points[i].z = points[i].z;
                }
                
                FMOD_Channel_Set3DCustomRolloff(channel, rolloff_points, attributes.att_3d.custom_rolloff.roll_off_point_count);
            }
        }
        
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
    
    if(attributes.mix_levels.input.levels && attributes.mix_levels.output.levels)
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
    //FMOD_Channel_SetMode(channel, FMOD_LOOP_NORMAL);
    
    r32 vol;
    FMOD_Channel_GetVolume(channel, &vol);
    
    FMOD_Channel_SetVolume(channel, attributes.volume);
}

static void play_audio_source(AudioSource& audio_source, SoundDevice* sound_device, SoundSystem* system)
{
    auto sound = sound_device->sounds[audio_source.sound_handle.handle - 1];
    auto &channel_attributes = audio_source.channel_attributes;
    auto as_handle = audio_source.handle.handle;
    
    FMOD_RESULT result = FMOD_OK;
    
    b32 is_playing = 0;
    FMOD_Channel_IsPlaying(sound_device->channels[as_handle - 1], &is_playing);
    
    auto channel = sound_device->channels[as_handle - 1];
    
    if(!is_playing)
    {
        result = FMOD_System_PlaySound(sound_device->system, sound, sound_device->master_group, true, &channel);
    }
    
    sound_device->channels[as_handle - 1] = channel;
    
    FMOD_Channel_SetCallback(channel, channel_control_callback);
    FMOD_Channel_SetUserData(channel, &audio_source);
    
    set_channel_attributes(channel, channel_attributes, system, sound_device, as_handle);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    FMOD_Channel_SetPaused(channel, audio_source.paused);
}

static void play_sound(SoundCommand *command, SoundDevice *device, SoundSystem *system)
{
    auto sound = device->sounds[command->one_shot.handle.handle - 1];
    
    device->one_shot_point_count++;
    
    FMOD_RESULT result = FMOD_OK;
    
    FMOD_CHANNEL *channel;
    result = FMOD_System_PlaySound(device->system, sound, device->master_group, true, &channel);
    
    set_channel_attributes(channel, command->one_shot.channel_attributes, system, device);
    
    if(result != FMOD_OK)
    {
        FMOD_DEBUG(result);
    }
    
    FMOD_Channel_SetCallback(channel, channel_control_callback);
    FMOD_Channel_SetUserData(channel, (void**)&device);
    
    FMOD_Channel_SetPaused(channel, false);
}


static void stop_audio_source(AudioSource &audio_source, SoundDevice* sound_device, SoundSystem *system)
{
    auto channel = sound_device->channels[audio_source.handle.handle - 1];
    
    FMOD_Channel_SetVolume(channel, 0.0f);
    
    FMOD_Channel_Stop(channel);
}

static void pause_audio_source(AudioSource &audio_source, SoundDevice* sound_device, SoundSystem *system, b32 paused)
{
    auto channel = sound_device->channels[audio_source.handle.handle - 1];
    
    audio_source.paused = paused;
    sound_device->paused_channels[audio_source.handle.handle - 1] = paused;
    FMOD_Channel_SetPaused(channel, paused);
}

static void set_position_audio_source(SoundCommand *command, AudioSource &audio_source, SoundDevice *sound_device, SoundSystem *system)
{
    auto channel = sound_device->channels[audio_source.handle.handle - 1];
    
    audio_source.channel_attributes.position_ms = command->set_position.new_position_ms;
    sound_device->channel_positions[audio_source.handle.handle - 1] = command->set_position.new_position_ms;
    FMOD_Channel_SetPosition(channel, command->set_position.new_position_ms, TIME_UNIT);
}


static void update_sound_commands(SoundDevice *device, SoundSystem *system, r64 delta_time)
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
                case SC_PAUSE_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command->play_audio_source.handle.handle - 1];
                    pause_audio_source(audio_source, device, system, true);
                }
                break;
                case SC_UNPAUSE_AUDIO_SOURCE:
                {
                    auto &audio_source = system->audio_sources[command->play_audio_source.handle.handle - 1];
                    pause_audio_source(audio_source, device, system, false);
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
                case SC_AUDIO_SOURCE_POSITION:
                {
                    auto &audio_source = system->audio_sources[command->play_audio_source.handle.handle - 1];
                    set_position_audio_source(command, audio_source, device, system);
                }
                break;
            }
        }
        
        if(FMOD_System_Update(device->system) != FMOD_OK)
        {
            debug("FMOD failed updating\n");
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

