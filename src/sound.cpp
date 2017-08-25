 static inline void PlaySoundEffect(sound_commands* SoundCommands, sound_effect* LoadedSound, r32 Pitch = 1.0f, r32 X = 0.0f, r32 Y = 0.0f, r32 RollOff = 0.0f, b32 Loop = false, i32 EntityHandle = -1)
 {
     sound_effect* SoundEffect = PushStruct(&SoundCommands->SoundArena, sound_effect);
     SoundCommands->SoundCount++;
     SoundEffect->Buffer = LoadedSound->Buffer;
     LoadedSound->SoundInfo.Pitch = Pitch;
     SoundEffect->SoundInfo = LoadedSound->SoundInfo;
     SoundEffect->SoundInfo.Position[0] = X; 
     SoundEffect->SoundInfo.Position[1] = Y;
     SoundEffect->SoundInfo.Position[2] = 1.0f;
     SoundEffect->SoundInfo.Rolloff = RollOff;
     SoundEffect->SoundInfo.Loop = Loop;
     SoundEffect->SoundInfo.EntityHandle = EntityHandle;
     SoundEffect->SoundInfo.Gain = SoundCommands->SFXVolume;
 }
 
 static inline void PlayMusicTrack(sound_commands* SoundCommands, sound_effect* LoadedSound, r32 Pitch = 1.0f, b32 Loop = true)
 {
     sound_effect* SoundEffect = PushStruct(&SoundCommands->SoundArena, sound_effect);
     SoundCommands->SoundCount++;
     SoundEffect->Buffer = LoadedSound->Buffer;
     SoundEffect->SoundInfo.Pitch = Pitch;
     SoundEffect->SoundInfo = LoadedSound->SoundInfo;
     SoundEffect->SoundInfo.Rolloff = 0.0f;
     SoundEffect->SoundInfo.Loop = Loop;
     SoundEffect->SoundInfo.Gain = SoundCommands->MusicVolume;
 }
 
#define PLAY_SOUND(Sound,...) PlaySoundEffect(SoundCommands, &GameState->Sounds.## Sound, __VA_ARGS__)
 #define PLAY_TRACK(Track,...) PlayMusicTrack(SoundCommands, &GameState->Sounds.## Track, __VA_ARGS__)