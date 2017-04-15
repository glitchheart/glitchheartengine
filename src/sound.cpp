
static void PlaySound(loaded_sound *LoadedSound)
{
    if (LoadedSound)
    {
        alSourcef(LoadedSound->Source, AL_PITCH, LoadedSound->SoundInfo.Pitch);
        alSourcef(LoadedSound->Source, AL_GAIN, LoadedSound->SoundInfo.Gain);
        alSource3f(LoadedSound->Source, AL_POSITION, LoadedSound->SoundInfo.Position[0], LoadedSound->SoundInfo.Position[1], LoadedSound->SoundInfo.Position[2]);
        alSource3f(LoadedSound->Source, AL_VELOCITY, LoadedSound->SoundInfo.Velocity[0], LoadedSound->SoundInfo.Velocity[1], LoadedSound->SoundInfo.Velocity[2]);
        alSourcei(LoadedSound->Source, AL_LOOPING, LoadedSound->SoundInfo.Loop);
        alSourcePlay(LoadedSound->Source);
        alGetSourcei(LoadedSound->Source, AL_SOURCE_STATE, &LoadedSound->SourceState);
    }
}

static void PlaySoundOnce(loaded_sound *LoadedSound)
{
    if (LoadedSound)
    {
        alGetSourcei(LoadedSound->Source, AL_SOURCE_STATE, &LoadedSound->SourceState);
        if (LoadedSound->SourceState != AL_PLAYING)
        {
            PlaySound(LoadedSound);
        }
    }
}

static void PauseSound(loaded_sound *LoadedSound)
{
    if (LoadedSound)
    {
        alSourcePause(LoadedSound->Source);
    }
}

static void StopSound(loaded_sound *LoadedSound)
{
    if (LoadedSound)
    {
        alSourceStop(LoadedSound->Source);
    }
}