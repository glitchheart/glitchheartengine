static void LoadSound(sound_commands* Commands, char* FilePath, i32* Handle)
{
    if(Commands->SoundsToLoad.FilesLength == 0)
    {
        Commands->SoundsToLoad.FileNames = PushTempArray(512, char*);
        Commands->SoundsToLoad.FilePaths = PushTempArray(512, char*);
    }
    
    char Path[255];
    strcpy(Path, FilePath);
    auto FileName = GetFileNameFromPath(Path, "wav");
    
    if(Handle)
        *Handle = Commands->Sounds++;
    
    Commands->LoadSounds = true;
    Commands->SoundsToLoad.FilePaths[Commands->SoundsToLoad.FilesLength] = PushTempString(FilePath);
    Commands->SoundsToLoad.FileNames[Commands->SoundsToLoad.FilesLength] = PushTempString(FileName);
    
    Commands->SoundsToLoad.FilesLength++;
}

static i32* LoadSounds(sound_commands* Commands, const char* FilePath, memory_arena* Arena)
{
    directory_data DirData = {};
    Platform.GetAllFilesWithExtension(FilePath, "wav", &DirData, true);
    
    auto Handles = PushArray(Arena, DirData.FilesLength, i32);
    
    for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
    {
        Handles[FileIndex]= Commands->Sounds++;
    }
    
    Commands->LoadSounds = true;
    Commands->SoundsToLoad = DirData;
    return Handles;
}

static inline void PlaySoundEffect(sound_commands* SoundCommands, i32 BufferHandle, b32 Loop = false, i32 LoopCount = -1, r32 Gain = -1.0f,  r32 Pitch = 1.0f, r32 RollOff = 0.0f, r32 X = 0.0f, r32 Y = 0.0f)
{
    sound_effect* SoundEffect = PushStruct(&SoundCommands->SoundArena, sound_effect);
    SoundCommands->SoundCount++;
    SoundEffect->Buffer = BufferHandle;
    SoundEffect->SoundInfo.Pitch = Pitch;
    SoundEffect->SoundInfo.Rolloff = RollOff;
    SoundEffect->SoundInfo.Loop = Loop;
    SoundEffect->SoundInfo.LoopCount = LoopCount;
    SoundEffect->SoundInfo.Gain = Gain > -1.0f ? Gain : SoundCommands->SFXVolume;
}

static inline void PlayMusicTrack(sound_commands* SoundCommands, i32 BufferHandle,b32 Loop = false, i32 LoopCount = -1, r32 Gain = -1.0f, r32 Pitch = 1.0f)
{
    sound_effect* SoundEffect = PushStruct(&SoundCommands->SoundArena, sound_effect);
    SoundCommands->SoundCount++;
    SoundEffect->Buffer = BufferHandle;
    SoundEffect->SoundInfo.Pitch = Pitch;
    SoundEffect->SoundInfo.Rolloff = 0.0f;
    SoundEffect->SoundInfo.Loop = Loop;
    SoundEffect->SoundInfo.Gain = Gain > -1.0f ? Gain : SoundCommands->MusicVolume;
}

#define PLAY_SOUND(SoundHandle,...) PlaySoundEffect(SoundCommands, SoundHandle, ##__VA_ARGS__)
#define PLAY_TRACK(TrackHandle,...) PlayMusicTrack(SoundCommands, TrackHandle, ##__VA_ARGS__)