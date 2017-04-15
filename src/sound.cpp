
static void PlaySound(loaded_sound *LoadedSound)
{
    if (LoadedSound)
    {
        alSourcef(LoadedSound->Source, AL_PITCH, 1);
        alSourcef(LoadedSound->Source, AL_GAIN, 1);
        alSource3f(LoadedSound->Source, AL_POSITION, 0, 0, 0);
        alSource3f(LoadedSound->Source, AL_VELOCITY, 0, 0, 0);
        alSourcei(LoadedSound->Source, AL_LOOPING, AL_FALSE);
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