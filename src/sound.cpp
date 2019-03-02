
namespace sound
{

static ChannelAttributes get_default_channel_attributes()
{
    ChannelAttributes attributes = {};
    attributes.type = ChannelType::SFX;
    attributes.volume = 1.0f;
    attributes.position_ms = 0;
    attributes.pan_level = 0.0f;
    attributes.pitch = 1.0f;
    
    // Maybe not set frequency. Just use sound default and mix outside of engine?
    //attributes.frequency = ;
    attributes.low_pass_gain = 1.0f;
    attributes.ramp = false;
    attributes.channel_space = CS_2D;
    
    attributes.att_3d.pos = math::Vec3(0.0f);
    attributes.att_3d.vel = math::Vec3(0.0f);
    attributes.att_3d.cone.orientation = math::Vec3(0.0f);
    attributes.att_3d.cone.inside_angle = 360.0f;
    attributes.att_3d.cone.outside_angle = 360.0f;
    attributes.att_3d.cone.outside_volume = 1.0f;
    attributes.att_3d.custom_rolloff.roll_off_point_count = 0;
    attributes.att_3d.distance_filter.custom = false;
    attributes.att_3d.distance_filter.custom_level = 1.0f;
    attributes.att_3d.distance_filter.center_freq = 1500.0f;
    attributes.att_3d.doppler_level = 1.0f;
    attributes.att_3d.level_3d = 1.0f;
    attributes.att_3d.min_distance = 1.0f;
    attributes.att_3d.max_distance = 10000.0f;
    attributes.att_3d.occlusion.direct = 0.0f;
    attributes.att_3d.occlusion.reverb = 0.0f;
    attributes.att_3d.spread_angle = 0.0f;
    attributes.att_3d.rolloff_mode = RM_INVERSE;
    attributes.att_3d.relative_space_mode = RSM_WORLDRELATIVE;
    
    attributes.loop.type = LOOP_OFF;
    attributes.loop.count = -1;
    attributes.loop.loop_points.start = 0;
    attributes.loop.loop_points.end = 0;
    attributes.mix_levels.input.level_count = 0;
    attributes.mix_levels.output.level_count = 0;
    attributes.reverb.instance = 0;
    attributes.reverb.wet = 1.0f;
    
    return attributes;
}

static SoundCommand *push_next_command(SoundSystem *system)
{
    assert(system->command_count + 1 < global_max_sound_commands);
    return &system->commands[system->command_count++];
}

static void create_audio_source(SoundSystem *system, AudioSourceHandle *as_handle, SoundHandle sound_handle, b32 paused = false, b32 muted = false)
{
    assert(system->audio_source_count + 1 < global_max_audio_sources);
    as_handle->handle = system->audio_source_count + 1;
    AudioSource &new_source = system->audio_sources[system->audio_source_count++];
    new_source.handle.handle = as_handle->handle;
    new_source.sound_handle = sound_handle;
    new_source.channel_attributes = get_default_channel_attributes();
    
    new_source.muted = muted;
    new_source.paused = paused;
}

// Figure something out with destroy... Do we actually need it?
// Could just manually reuse audio sources or something
static void destroy_audio_source(SoundSystem *system, AudioSourceHandle as_handle)
{
    
}

static void load_sound(SoundSystem *system, const char *file_path, SoundHandle *handle)
{
    assert(system->sound_count + 1 < global_max_sounds);
    handle->handle = system->sound_count++ + 1;
    SoundCommand *command = push_next_command(system);
    command->type = SC_LOAD_SOUND;
    strcpy(command->load_sound.file_path, file_path);
}

// @Note: play a one shot sound defaulted as sfx type
// If you want to play this one shot as a music type, you need
// to pass in different channel attributes.
static void play_one_shot_sound(SoundSystem *system, SoundHandle handle, ChannelAttributes channel_attributes = get_default_channel_attributes())
{
    assert(handle.handle != 0 && handle.handle - 1 < system->sound_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_ONE_SHOT;
    command->one_shot.handle = handle;
    command->one_shot.channel_attributes = channel_attributes;
}

static void play_audio_source(SoundSystem *system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system->audio_source_count);
    
    AudioSource &as = system->audio_sources[as_handle.handle - 1];
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_PLAY_AUDIO_SOURCE;
    command->play_audio_source.handle = as_handle;
}

static void stop_audio_source(SoundSystem *system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system->audio_source_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_STOP_AUDIO_SOURCE;
    command->stop_audio_source.handle = as_handle;
}

static void pause_audio_source(SoundSystem *system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system->audio_source_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_PAUSE_AUDIO_SOURCE;
    command->pause_audio_source.handle = as_handle;
}

static void unpause_audio_source(SoundSystem *system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system->audio_source_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_UNPAUSE_AUDIO_SOURCE;
    command->pause_audio_source.handle = as_handle;
}

static void set_position_audio_source(SoundSystem *system, AudioSourceHandle as_handle, u32 position_ms)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system->audio_source_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_AUDIO_SOURCE_POSITION;
    command->set_position.handle = as_handle;
    command->set_position.new_position_ms = position_ms;
}

static AudioSource *get_audio_source(SoundSystem *system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system->audio_source_count);
    
    return &system->audio_sources[as_handle.handle - 1];
}

}
