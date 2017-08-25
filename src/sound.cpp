 static inline void PlaySoundEffect(sound_manager* SoundManager, sound_queue* SoundQueue, sound_effect* LoadedSound, r32 Pitch = 1.0f, r32 X = 0.0f, r32 Y = 0.0f, r32 RollOff = 0.0f, b32 Loop = false, i32 EntityHandle = -1)
 {
     sound_effect* SoundEffect = PushStruct(&SoundQueue->SoundArena, sound_effect);
     SoundQueue->SoundCount++;
     SoundEffect->Buffer = LoadedSound->Buffer;
     LoadedSound->SoundInfo.Pitch = Pitch;
     SoundEffect->SoundInfo = LoadedSound->SoundInfo;
     SoundEffect->SoundInfo.Position[0] = X; 
     SoundEffect->SoundInfo.Position[1] = Y;
     SoundEffect->SoundInfo.Position[2] = 1.0f;
     SoundEffect->SoundInfo.Rolloff = RollOff;
     SoundEffect->SoundInfo.Loop = Loop;
     SoundEffect->SoundInfo.EntityHandle = EntityHandle;
     SoundEffect->SoundInfo.Gain = SoundManager->SFXGain;
 }
 
 static inline void PlayMusicTrack(sound_manager* SoundManager, sound_queue* SoundQueue, sound_effect* LoadedSound, r32 Pitch = 1.0f, b32 Loop = true)
 {
     sound_effect* SoundEffect = PushStruct(&SoundQueue->SoundArena, sound_effect);
     SoundQueue->SoundCount++;
     SoundEffect->Buffer = LoadedSound->Buffer;
     SoundEffect->SoundInfo.Pitch = Pitch;
     SoundEffect->SoundInfo = LoadedSound->SoundInfo;
     SoundEffect->SoundInfo.Rolloff = 0.0f;
     SoundEffect->SoundInfo.Loop = Loop;
     SoundEffect->SoundInfo.Gain = SoundManager->MusicGain;
 }
 
#define PLAY_SOUND(Sound,...) PlaySoundEffect(&GameState->SoundManager, SoundQueue, &GameState->SoundManager.SoundEffects.## Sound, __VA_ARGS__)
 #define PLAY_TRACK(Track,...) PlayMusicTrack(&GameState->SoundManager, SoundQueue, &GameState->SoundManager.SoundEffects.## Track, __VA_ARGS__)