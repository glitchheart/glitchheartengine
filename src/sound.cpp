 static void LoadSounds(sound_commands* Commands, const char* FilePath)
 {
     if(!Commands->SoundMap.Initialized)
     {
         sound_buffer_Map_Init(&Commands->SoundMap, HashStringJenkins, 64);
     }
     
     directory_data DirData = {};
     Platform.GetAllFilesWithExtension(FilePath, "wav", &DirData, true);
     
     for(i32 FileIndex = 0; FileIndex < DirData.FilesLength; FileIndex++)
     {
         Commands->SoundMap[DirData.FileNames[FileIndex]] = Commands->Sounds++;
     }
     
     Commands->LoadSounds = true;
     Commands->SoundsToLoad = DirData;
 }
 
 static inline void PlaySoundEffect(sound_commands* SoundCommands, char* Sound, r32 Pitch = 1.0f, r32 X = 0.0f, r32 Y = 0.0f, r32 RollOff = 0.0f, b32 Loop = false, i32 EntityHandle = -1)
 {
     sound_effect* SoundEffect = PushStruct(&SoundCommands->SoundArena, sound_effect);
     SoundCommands->SoundCount++;
     auto& Buffer = SoundCommands->SoundMap[Sound];
     SoundEffect->Buffer = Buffer;
     SoundEffect->SoundInfo.Pitch = Pitch;
     SoundEffect->SoundInfo.Position[0] = X; 
     SoundEffect->SoundInfo.Position[1] = Y;
     SoundEffect->SoundInfo.Position[2] = 1.0f;
     SoundEffect->SoundInfo.Rolloff = RollOff;
     SoundEffect->SoundInfo.Loop = Loop;
     SoundEffect->SoundInfo.EntityHandle = EntityHandle;
     SoundEffect->SoundInfo.Gain = SoundCommands->SFXVolume;
 }
 
 static inline void PlayMusicTrack(sound_commands* SoundCommands, char* Sound, r32 Pitch = 1.0f, b32 Loop = true)
 {
     sound_effect* SoundEffect = PushStruct(&SoundCommands->SoundArena, sound_effect);
     SoundCommands->SoundCount++;
     auto& Buffer = SoundCommands->SoundMap[Sound];
     SoundEffect->Buffer = Buffer;
     SoundEffect->SoundInfo.Pitch = Pitch;
     SoundEffect->SoundInfo.Rolloff = 0.0f;
     SoundEffect->SoundInfo.Loop = Loop;
     SoundEffect->SoundInfo.Gain = SoundCommands->MusicVolume;
 }
 
#define PLAY_SOUND(Sound,...) PlaySoundEffect(SoundCommands, Sound, __VA_ARGS__)
#define PLAY_TRACK(Track,...) PlayMusicTrack(SoundCommands, Track, __VA_ARGS__)