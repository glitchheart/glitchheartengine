 void PlaySoundEffect(game_state *GameState, sound_effect* LoadedSound, real32 Pitch = 1.0f)
 {
     sound_effect SoundEffect = {};
     SoundEffect.Buffer = LoadedSound->Buffer;
     LoadedSound->SoundInfo.Pitch = Pitch;
     SoundEffect.SoundInfo = LoadedSound->SoundInfo;
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