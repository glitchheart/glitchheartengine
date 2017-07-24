 void PlaySoundEffect(game_state *GameState, sound_effect* LoadedSound, r32 Pitch = 1.0f, r32 X = 0.0f, r32 Y = 0.0f, r32 RollOff = 0.0f, b32 Loop = false, i32 EntityHandle = -1)
 {
     sound_effect SoundEffect = {};
     SoundEffect.Buffer = LoadedSound->Buffer;
     LoadedSound->SoundInfo.Pitch = Pitch;
     SoundEffect.SoundInfo = LoadedSound->SoundInfo;
     SoundEffect.SoundInfo.Position[0] = X; 
     SoundEffect.SoundInfo.Position[1] = Y;
     SoundEffect.SoundInfo.Position[2] = 1.0f;
     SoundEffect.SoundInfo.Rolloff = RollOff;
     SoundEffect.SoundInfo.Loop = Loop;
     SoundEffect.SoundInfo.EntityHandle = EntityHandle;
     SoundEffect.SoundInfo.Gain = GameState->SoundManager.SFXGain;
     GameState->SoundManager.SoundQueue.Sounds[GameState->SoundManager.SoundQueue.SoundCount++] = SoundEffect;
 }
 
 void PlayMusicTrack(game_state* GameState, sound_effect* LoadedSound, r32 Pitch = 1.0f, b32 Loop = true)
 {
     sound_effect SoundEffect = {};
     SoundEffect.Buffer = LoadedSound->Buffer;
     SoundEffect.SoundInfo.Pitch = Pitch;
     SoundEffect.SoundInfo = LoadedSound->SoundInfo;
     SoundEffect.SoundInfo.Rolloff = 0.0f;
     SoundEffect.SoundInfo.Loop = Loop;
     SoundEffect.SoundInfo.Gain = GameState->SoundManager.MusicGain;
     GameState->SoundManager.SoundQueue.Sounds[GameState->SoundManager.SoundQueue.SoundCount++] = SoundEffect;
 }
 
 void TogglePauseSound(game_state* GameState)
 {
     GameState->SoundManager.Paused = !GameState->SoundManager.Paused;
 }
 
 void ToggleMuteSound(game_state* GameState)
 {
     GameState->SoundManager.Muted = !GameState->SoundManager.Muted;
 }
 
 void StopSound(game_state* GameState)
 {
     GameState->SoundManager.Stopped = true;
 }