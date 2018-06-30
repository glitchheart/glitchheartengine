
struct SoundHandle
{
    i32 handle;
};

struct AudioSourceHandle
{
    i32 handle;
};

struct ChannelHandle
{
    i32 handle;
};

struct AudioSource
{
    SoundHandle sound_handle;
    b32 loop;
    unsigned i32 position_ms;
    
    // Add all the stuff that FMOD at least supports
};

struct ChannelGroup
{
    // Empty for now. May need stuff later
};

enum SoundCommandType
{
    SC_PLAY_AUDIO_SOURCE,
    SC_STOP_AUDIO_SOURCE,
    SC_LOAD_SOUND,
    SC_ONE_SHOT
};

struct SoundCommand
{
    SoundCommandType type;
    
    union
    {
        struct
        {
            AudioSourceHandle handle;
        } play_audio_source;
        struct
        {
            AudioSourceHandle handle;
        } stop_audio_source;
        struct
        {
            char file_path[255];
        } load_sound;
        struct
        {
            SoundHandle handle;
        } one_shot;
    };
};

struct SoundSystem
{
    SoundHandle sounds[MAX_SOUNDS];
    i32 sound_count;
    
    AudioSource audio_sources[MAX_AUDIO_SOURCES];
    i32 audio_source_count;
    
    // For later. Doesn't really matter much for now
    ChannelGroup channel_groups[MAX_CHANNEL_GROUPS];
    i32 channel_group_count;
    
    MemoryArena sound_commands;
    i32 command_count;
    
    
};

static SoundCommand *push_next_command(SoundSystem &system)
{
    system.command_count++;
    return push_struct(&system.sound_commands, SoundCommand);
}

static void create_audio_source(SoundSystem &system, AudioSourceHandle *as_handle, SoundHandle sound_handle = 0, b32 loop = false, unsigned int position_ms = 0)
{
    as_handle->handle = system.audio_source_count + 1;
    AudioSource &newSource = system.audio_sources[system.audio_source_count++];
    newSource.sound_handle = sound_handle;
    newSource.loop = loop;
    newSource.position_ms = position_ms;
}

// Figure something out with destroy... Do we actually need it?
// Could just manually reuse audio sources or something
static void destroy_audio_source(SoundSystem &system, AudioSourceHandle as_handle);
{
    
}

static void load_sound(SoundSystem &system, const char *file_path, SoundHandle &handle)
{
    handle.handle = system.sound_count++ + 1;
    SoundCommand *command = push_next_command(system);
    command->type = SC_LOAD_SOUND;
    strcpy(command->file_path, file_path);
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
    SoundSystem system;
    system.sound_commands.minimum_block_size = sizeof(SoundCommand) * MAX_SOUND_COMMANDS;
    
    SoundHandle sound_handle;
    // Loads the wav file into memory and returns a buffer handle
    load_sound("sound.wav", &sound_handle);
    
    AudioSourceHandle as_handle;
    // Creates an empty audio source (maybe pass in a sound handle
    create_audio_source(&as_handle);
    
    AudioSource *as = get_audio_source(as_handle);
    as->sound_handle = sound_handle;
    as->loop = false;
    
    play_audio_source(as_handle);
    //play_audio_source(as);
    
    stop_audio_source(as_handle);
    //stop_audio_source(as);
    
    play_one_shot_sound(sound_handle);
}

