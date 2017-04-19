
void PlaySoundEffect(game_state *GameState, loaded_sound *LoadedSound)
{
    sound_effect SoundEffect = {};
    SoundEffect.Buffer = LoadedSound->Buffer;
    SoundEffect.Source = LoadedSound->Source;
    SoundEffect.SoundInfo = LoadedSound->SoundInfo;
    SoundEffect.SourceState = LoadedSound->SourceState;
    GameState->SoundManager.SoundQueue.Sounds[GameState->SoundManager.SoundQueue.SoundCount++] = SoundEffect;
}

void PlaySoundEffectOnce(game_state *GameState, loaded_sound *LoadedSound)
{
    sound_effect SoundEffect = {};
    SoundEffect.PlayOnce = true;
    SoundEffect.Buffer = LoadedSound->Buffer;
    SoundEffect.Source = LoadedSound->Source;
    SoundEffect.SoundInfo = LoadedSound->SoundInfo;
    SoundEffect.SourceState = LoadedSound->SourceState;
    GameState->SoundManager.SoundQueue.Sounds[GameState->SoundManager.SoundQueue.SoundCount++] = SoundEffect;
}

void StopSoundEffect(game_state *GameState, loaded_sound *LoadedSound)
{
    sound_effect SoundEffect = {};
    SoundEffect.PlayOnce = true;
    SoundEffect.Buffer = LoadedSound->Buffer;
    SoundEffect.Source = LoadedSound->Source;
    SoundEffect.SoundInfo = LoadedSound->SoundInfo;
    SoundEffect.SourceState = LoadedSound->SourceState;
    GameState->SoundManager.SoundQueue.StoppedSounds[GameState->SoundManager.SoundQueue.StoppedSoundCount++] = SoundEffect;
}

void PauseSoundEffect(game_state *GameState, loaded_sound *LoadedSound)
{
    sound_effect SoundEffect = {};
    SoundEffect.PlayOnce = true;
    SoundEffect.Buffer = LoadedSound->Buffer;
    SoundEffect.Source = LoadedSound->Source;
    SoundEffect.SoundInfo = LoadedSound->SoundInfo;
    SoundEffect.SourceState = LoadedSound->SourceState;
    GameState->SoundManager.SoundQueue.PausedSounds[GameState->SoundManager.SoundQueue.PausedSoundCount++] = SoundEffect;
    
}