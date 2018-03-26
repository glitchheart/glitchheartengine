static void load_sound(SoundCommands* commands, char* file_path, i32* handle, MemoryArena* arena)
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