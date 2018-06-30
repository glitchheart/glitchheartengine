/*static void load_sound(SoundCommands* commands, char* file_path, i32* handle, MemoryArena* arena)
{
    char path[255];
    strcpy(path, file_path);
    auto file_name = get_file_name_from_path(path, arena, "wav");
    
    if(handle)
        *handle = commands->sounds++;
        
    commands->load_sounds = true;
    strcpy(commands->sounds_to_load.file_paths[commands->sounds_to_load.files_length], file_path);
    strcpy(commands->sounds_to_load.file_names[commands->sounds_to_load.files_length], file_name);
    
    commands->sounds_to_load.files_length++;
}

// Call this function once to register audio source at init time
static void register_audio_source(SoundCommands* sound_commands, AudioSource& audio_source)
{
    audio_source.handle = sound_commands->audio_source_count++ + 1;
    sound_commands->audio_sources[audio_source.handle].sound_info = audio_source.sound_info;
    sound_commands->audio_sources[audio_source.handle].buffer_handle = audio_source.buffer_handle;
}

static inline void push_audio_source(SoundCommands* sound_commands, AudioSource& audio_source)
{
    if(audio_source.handle - 1 == -1)
    {
        register_audio_source(sound_commands, audio_source);
    }
    
    auto& source = sound_commands->audio_sources[audio_source.handle - 1];
    source.sound_info = audio_source.sound_info;
    source.buffer_handle = audio_source.buffer_handle;
    source.muted = audio_source.muted;
    source.paused = audio_source.paused;
    source.play = true;
}

static void update_audio_source(SoundCommands* sound_commands, AudioSource& audio_source)
{
    if(audio_source.handle - 1 == -1)
    {
        register_audio_source(sound_commands, audio_source);
    }
    sound_commands->audio_sources[audio_source.handle - 1] = audio_source;
    auto& source = sound_commands->audio_sources[audio_source.handle - 1];
    source.sound_info = audio_source.sound_info;
    source.buffer_handle = audio_source.buffer_handle;
    source.muted = audio_source.muted;
    source.paused = audio_source.paused;
    source.play = true;
}

static inline void play_sound_effect(SoundCommands* sound_commands, i32 buffer_handle, r32 gain = -1.0f,  r32 pitch = 1.0f, r32 roll_off = 0.0f)
{
    SoundEffect* sound_effect = push_struct(&sound_commands->sound_arena, SoundEffect);
    sound_commands->sound_count++;
    sound_effect->buffer = buffer_handle;
    sound_effect->sound_info.pitch = pitch;
    sound_effect->sound_info.roll_off = roll_off;
    sound_effect->sound_info.gain = gain > -1.0f ? gain : sound_commands->sfx_volume;
}

#define PLAY_SOUND(sound_handle,...) play_sound_effect(sound_commands, sound_handle, ##__VA_ARGS__)
*/


static SoundCommand *push_next_command(SoundSystem &system)
{
    assert(system.command_count + 1 < MAX_SOUND_COMMANDS);
    system.command_count++;
    return push_struct(&system.sound_commands, SoundCommand);
}

static void create_audio_source(SoundSystem &system, AudioSourceHandle *as_handle, SoundHandle sound_handle, b32 loop = false, unsigned int position_ms = 0)
{
    assert(system.audio_source_count + 1 < MAX_AUDIO_SOURCES);
    as_handle->handle = system.audio_source_count + 1;
    AudioSource &newSource = system.audio_sources[system.audio_source_count++];
    newSource.handle.handle = as_handle->handle;
    newSource.sound_handle = sound_handle;
    newSource.loop = loop;
    newSource.position_ms = position_ms;
}

// Figure something out with destroy... Do we actually need it?
// Could just manually reuse audio sources or something
static void destroy_audio_source(SoundSystem &system, AudioSourceHandle as_handle)
{
    
}

static void load_sound(SoundSystem &system, const char *file_path, SoundHandle *handle)
{
    assert(system.sound_count + 1 < MAX_SOUNDS);
    handle->handle = system.sound_count++ + 1;
    SoundCommand *command = push_next_command(system);
    command->type = SC_LOAD_SOUND;
    strcpy(command->load_sound.file_path, file_path);
}

static void play_one_shot_sound(SoundSystem &system, SoundHandle handle)
{
    assert(handle.handle != 0 && handle.handle - 1 < system.sound_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_ONE_SHOT;
    command->one_shot.handle = handle;
}

static void play_audio_source(SoundSystem &system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system.audio_source_count);
    
    AudioSource &as = system.audio_sources[as_handle.handle - 1];
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_PLAY_AUDIO_SOURCE;
    command->play_audio_source.handle = as_handle;
}

static void stop_audio_source(SoundSystem &system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system.audio_source_count);
    
    SoundCommand *command = push_next_command(system);
    command->type = SC_STOP_AUDIO_SOURCE;
    command->play_audio_source.handle = as_handle;
}

static AudioSource *get_audio_source(SoundSystem &system, AudioSourceHandle as_handle)
{
    assert(as_handle.handle != 0 && as_handle.handle - 1 < system.audio_source_count);
    
    return &system.audio_sources[as_handle.handle - 1];
}

static void sound_test()
{
    SoundSystem system = {};
    system.sound_commands.minimum_block_size = sizeof(SoundCommand) * MAX_SOUND_COMMANDS;
    
    SoundHandle sound_handle;
    // Loads the wav file into memory and returns a buffer handle
    load_sound(system, "sound.wav", &sound_handle);
    
    AudioSourceHandle as_handle;
    // Creates an empty audio source (maybe pass in a sound handle
    create_audio_source(system, &as_handle, sound_handle);
    
    AudioSource *as = get_audio_source(system, as_handle);
    as->sound_handle = sound_handle;
    as->loop = false;
    
    play_audio_source(system, as_handle);
    //play_audio_source(as);
    
    stop_audio_source(system, as_handle);
    //stop_audio_source(as);
    
    play_one_shot_sound(system, sound_handle);
}
