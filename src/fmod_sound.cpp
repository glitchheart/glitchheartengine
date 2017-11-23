
static void FMODError(FMOD_RESULT Err)
{
    DEBUG_PRINT("FMOD Error! (%d) %s\n", Err, FMOD_ErrorString(Err));
}

static void InitAudio_FMOD()
{
    FMOD_RESULT Result;
    FMOD_SYSTEM* System;
    Result = FMOD_System_Create(&System);
    
    if(Result != FMOD_OK)
    {
        FMODError(Result);
        return;
    }
    
    Result = FMOD_System_Init(System, 512, FMOD_INIT_NORMAL, 0);
    if(Result != FMOD_OK)
    {
        FMODError(Result);
    }
    
    u32 Version;
    FMOD_System_GetVersion(System, &Version);
    if(Version < FMOD_VERSION)
    {
        DEBUG_PRINT("Version error\n");
    }
    
    FMOD_SOUND* Sound;
    FMOD_System_CreateSound(System, "/home/bross/code/glitchheartgame2017/assets/audio/explosion.wav", FMOD_DEFAULT, 0, &Sound);
    
    if(!Sound)
    {
        DEBUG_PRINT("No sound!\n");
    }
    
    FMOD_CHANNEL* Channel;
    FMOD_System_PlaySound(System, Sound, 0, false, &Channel);
}


