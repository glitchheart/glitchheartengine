static void load_sound(SoundCommands* commands, char* file_path, i32* handle)
{
    if(commands->sounds_to_load.files_length == 0)
    {
        commands->sounds_to_load.file_names = push_temp_array(512, char*);
        commands->sounds_to_load.file_paths = push_temp_array(512, char*);
    }
    
    char path[255];
    strcpy(path, file_path);
    auto file_name = get_file_name_from_path(path, "wav");
    
    if(handle)
        *handle = commands->sounds++;
    
    commands->load_sounds = true;
    commands->sounds_to_load.file_paths[commands->sounds_to_load.files_length] = push_temp_string(file_path);
    commands->sounds_to_load.file_names[commands->sounds_to_load.files_length] = push_temp_string(file_name);
    
    commands->sounds_to_load.files_length++;
}

static i32* load_sounds(SoundCommands* commands, const char* file_path, MemoryArena* arena)
{
    DirectoryData dir_data = {};
    platform.get_all_files_with_extension(file_path, "wav", &dir_data, true);
    
    auto handles = push_array(arena, dir_data.files_length, i32);
    
    for(i32 file_index = 0; file_index < dir_data.files_length; file_index++)
    {
        handles[file_index]= commands->sounds++;
    }
    
    commands->load_sounds = true;
    commands->sounds_to_load = dir_data;
    return handles;
}

static inline void play_sound_effect(SoundCommands* sound_commands, i32 buffer_handle, b32 loop = false, i32 loop_count = -1, r32 gain = -1.0f,  r32 pitch = 1.0f, r32 roll_off = 0.0f)
{
    SoundEffect* sound_effect = push_struct(&sound_commands->sound_arena, SoundEffect);
    sound_commands->sound_count++;
    sound_effect->buffer = buffer_handle;
    sound_effect->sound_info.pitch = pitch;
    sound_effect->sound_info.roll_off = roll_off;
    sound_effect->sound_info.loop = loop;
    sound_effect->sound_info.loop_count = loop_count;
    sound_effect->sound_info.gain = gain > -1.0f ? gain : sound_commands->sfx_volume;
}

static inline void play_music_track(SoundCommands* sound_commands, i32 buffer_handle, b32 loop = false, i32 loop_count = -1, r32 gain = -1.0f, r32 pitch = 1.0f)
{
    SoundEffect* sound_effect = push_struct(&sound_commands->sound_arena, SoundEffect);
    sound_commands->sound_count++;
    sound_effect->buffer = buffer_handle;
    sound_effect->sound_info.pitch = pitch;
    sound_effect->sound_info.roll_off = 0.0f;
    sound_effect->sound_info.loop = loop;
    sound_effect->sound_info.loop_count = loop_count;
    sound_effect->sound_info.gain = gain > -1.0f ? gain : sound_commands->music_volume;
}

#define PLAY_SOUND(sound_handle,...) play_sound_effect(sound_commands, sound_handle, ##__VA_ARGS__)
#define PLAY_TRACK(track_handle,...) play_music_track(sound_commands, track_handle, ##__VA_ARGS__)