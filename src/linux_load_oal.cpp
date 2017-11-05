
ALboolean LoadOAL11Library(char* FilePath, oal_api* OALFunctions, HMODULE OpenALLib)
{
    if(!OALFunctions)
        return AL_FALSE;
    
    if(FilePath)
        OpenALLib = dlopen(FilePath, RTLD_LAZY);
    else
    {
        OpenALLib = dlopen("/home/bross/code/glitchheartgame2017/build/libopenal.so", RTLD_LAZY);
    }
    
    if(!OpenALLib)
    {
        char* Err = dlerror();
        DEBUG_PRINT("Could not load OpenAL: %s\n", Err);
    }
    
    memset(OALFunctions, 0, sizeof(oal_api));
    
    OALFunctions->alEnable = (LPALENABLE)dlsym(OpenALLib, "alEnable");
    if(OALFunctions->alEnable == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alEnable' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDisable = (LPALDISABLE)dlsym(OpenALLib, "alDisable");
    if (OALFunctions->alDisable == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDisable' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsEnabled = (LPALISENABLED)dlsym(OpenALLib, "alIsEnabled");
    if (OALFunctions->alIsEnabled == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsEnabled' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBoolean = (LPALGETBOOLEAN)dlsym(OpenALLib, "alGetBoolean");
    if (OALFunctions->alGetBoolean == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBoolean' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetInteger = (LPALGETINTEGER)dlsym(OpenALLib, "alGetInteger");
    if (OALFunctions->alGetInteger == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetInteger' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetFloat = (LPALGETFLOAT)dlsym(OpenALLib, "alGetFloat");
    if (OALFunctions->alGetFloat == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetFloat' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetDouble = (LPALGETDOUBLE)dlsym(OpenALLib, "alGetDouble");
    if (OALFunctions->alGetDouble == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetDouble' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBooleanv = (LPALGETBOOLEANV)dlsym(OpenALLib, "alGetBooleanv");
    if (OALFunctions->alGetBooleanv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBooleanv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetIntegerv = (LPALGETINTEGERV)dlsym(OpenALLib, "alGetIntegerv");
    if (OALFunctions->alGetIntegerv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetIntegerv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetFloatv = (LPALGETFLOATV)dlsym(OpenALLib, "alGetFloatv");
    if (OALFunctions->alGetFloatv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetFloatv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetDoublev = (LPALGETDOUBLEV)dlsym(OpenALLib, "alGetDoublev");
    if (OALFunctions->alGetDoublev == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetDoublev' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetString = (LPALGETSTRING)dlsym(OpenALLib, "alGetString");
    if (OALFunctions->alGetString == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetString' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetError = (LPALGETERROR)dlsym(OpenALLib, "alGetError");
    if (OALFunctions->alGetError == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetError' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsExtensionPresent = (LPALISEXTENSIONPRESENT)dlsym(OpenALLib, "alIsExtensionPresent");
    if (OALFunctions->alIsExtensionPresent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsExtensionPresent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetProcAddress = (LPALGETPROCADDRESS)dlsym(OpenALLib, "alGetProcAddress");
    if (OALFunctions->alGetProcAddress == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetProcAddress' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetEnumValue = (LPALGETENUMVALUE)dlsym(OpenALLib, "alGetEnumValue");
    if (OALFunctions->alGetEnumValue == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetEnumValue' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListeneri = (LPALLISTENERI)dlsym(OpenALLib, "alListeneri");
    if (OALFunctions->alListeneri == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListeneri' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListenerf = (LPALLISTENERF)dlsym(OpenALLib, "alListenerf");
    if (OALFunctions->alListenerf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListenerf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListener3f = (LPALLISTENER3F)dlsym(OpenALLib, "alListener3f");
    if (OALFunctions->alListener3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListener3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListenerfv = (LPALLISTENERFV)dlsym(OpenALLib, "alListenerfv");
    if (OALFunctions->alListenerfv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListenerfv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListeneri = (LPALGETLISTENERI)dlsym(OpenALLib, "alGetListeneri");
    if (OALFunctions->alGetListeneri == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListeneri' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListenerf =(LPALGETLISTENERF)dlsym(OpenALLib, "alGetListenerf");
    if (OALFunctions->alGetListenerf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListenerf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListener3f = (LPALGETLISTENER3F)dlsym(OpenALLib, "alGetListener3f");
    if (OALFunctions->alGetListener3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListener3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListenerfv = (LPALGETLISTENERFV)dlsym(OpenALLib, "alGetListenerfv");
    if (OALFunctions->alGetListenerfv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListenerfv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGenSources = (LPALGENSOURCES)dlsym(OpenALLib, "alGenSources");
    if (OALFunctions->alGenSources == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGenSources' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDeleteSources = (LPALDELETESOURCES)dlsym(OpenALLib, "alDeleteSources");
    if (OALFunctions->alDeleteSources == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDeleteSources' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsSource = (LPALISSOURCE)dlsym(OpenALLib, "alIsSource");
    if (OALFunctions->alIsSource == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsSource' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcei = (LPALSOURCEI)dlsym(OpenALLib, "alSourcei");
    if (OALFunctions->alSourcei == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcei' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcef = (LPALSOURCEF)dlsym(OpenALLib, "alSourcef");
    if (OALFunctions->alSourcef == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcef' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSource3f = (LPALSOURCE3F)dlsym(OpenALLib, "alSource3f");
    if (OALFunctions->alSource3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSource3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcefv = (LPALSOURCEFV)dlsym(OpenALLib, "alSourcefv");
    if (OALFunctions->alSourcefv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcefv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcei = (LPALGETSOURCEI)dlsym(OpenALLib, "alGetSourcei");
    if (OALFunctions->alGetSourcei == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcei' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcef = (LPALGETSOURCEF)dlsym(OpenALLib, "alGetSourcef");
    if (OALFunctions->alGetSourcef == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcef' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcefv = (LPALGETSOURCEFV)dlsym(OpenALLib, "alGetSourcefv");
    if (OALFunctions->alGetSourcefv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcefv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePlayv = (LPALSOURCEPLAYV)dlsym(OpenALLib, "alSourcePlayv");
    if (OALFunctions->alSourcePlayv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePlayv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceStopv = (LPALSOURCESTOPV)dlsym(OpenALLib, "alSourceStopv");
    if (OALFunctions->alSourceStopv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceStopv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePlay = (LPALSOURCEPLAY)dlsym(OpenALLib, "alSourcePlay");
    if (OALFunctions->alSourcePlay == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePlay' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePause = (LPALSOURCEPAUSE)dlsym(OpenALLib, "alSourcePause");
    if (OALFunctions->alSourcePause == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePause' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceStop = (LPALSOURCESTOP)dlsym(OpenALLib, "alSourceStop");
    if (OALFunctions->alSourceStop == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceStop' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGenBuffers = (LPALGENBUFFERS)dlsym(OpenALLib, "alGenBuffers");
    if (OALFunctions->alGenBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGenBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDeleteBuffers = (LPALDELETEBUFFERS)dlsym(OpenALLib, "alDeleteBuffers");
    if (OALFunctions->alDeleteBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDeleteBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsBuffer = (LPALISBUFFER)dlsym(OpenALLib, "alIsBuffer");
    if (OALFunctions->alIsBuffer == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsBuffer' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alBufferData = (LPALBUFFERDATA)dlsym(OpenALLib, "alBufferData");
    if (OALFunctions->alBufferData == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alBufferData' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBufferi = (LPALGETBUFFERI)dlsym(OpenALLib, "alGetBufferi");
    if (OALFunctions->alGetBufferi == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBufferi' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBufferf = (LPALGETBUFFERF)dlsym(OpenALLib, "alGetBufferf");
    if (OALFunctions->alGetBufferf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBufferf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)dlsym(OpenALLib, "alSourceQueueBuffers");
    if (OALFunctions->alSourceQueueBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceQueueBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)dlsym(OpenALLib, "alSourceUnqueueBuffers");
    if (OALFunctions->alSourceUnqueueBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceUnqueueBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDistanceModel = (LPALDISTANCEMODEL)dlsym(OpenALLib, "alDistanceModel");
    if (OALFunctions->alDistanceModel == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDistanceModel' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDopplerFactor = (LPALDOPPLERFACTOR)dlsym(OpenALLib, "alDopplerFactor");
    if (OALFunctions->alDopplerFactor == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDopplerFactor' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDopplerVelocity = (LPALDOPPLERVELOCITY)dlsym(OpenALLib, "alDopplerVelocity");
    if (OALFunctions->alDopplerVelocity == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDopplerVelocity' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetString = (LPALCGETSTRING)dlsym(OpenALLib, "alcGetString");
    if (OALFunctions->alcGetString == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetString' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetIntegerv = (LPALCGETINTEGERV)dlsym(OpenALLib, "alcGetIntegerv");
    if (OALFunctions->alcGetIntegerv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetIntegerv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcOpenDevice = (LPALCOPENDEVICE)dlsym(OpenALLib, "alcOpenDevice");
    if (OALFunctions->alcOpenDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcOpenDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcCloseDevice = (LPALCCLOSEDEVICE)dlsym(OpenALLib, "alcCloseDevice");
    if (OALFunctions->alcCloseDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcCloseDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcCreateContext = (LPALCCREATECONTEXT)dlsym(OpenALLib, "alcCreateContext");
    if (OALFunctions->alcCreateContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcCreateContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)dlsym(OpenALLib, "alcMakeContextCurrent");
    if (OALFunctions->alcMakeContextCurrent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcMakeContextCurrent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcProcessContext = (LPALCPROCESSCONTEXT)dlsym(OpenALLib, "alcProcessContext");
    if (OALFunctions->alcProcessContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcProcessContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetCurrentContext = (LPALCGETCURRENTCONTEXT)dlsym(OpenALLib, "alcGetCurrentContext");
    if (OALFunctions->alcGetCurrentContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetCurrentContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)dlsym(OpenALLib, "alcGetContextsDevice");
    if (OALFunctions->alcGetContextsDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetContextsDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcSuspendContext = (LPALCSUSPENDCONTEXT)dlsym(OpenALLib, "alcSuspendContext");
    if (OALFunctions->alcSuspendContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcSuspendContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcDestroyContext = (LPALCDESTROYCONTEXT)dlsym(OpenALLib, "alcDestroyContext");
    if (OALFunctions->alcDestroyContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcDestroyContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetError = (LPALCGETERROR)dlsym(OpenALLib, "alcGetError");
    if (OALFunctions->alcGetError == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetError' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT)dlsym(OpenALLib, "alcIsExtensionPresent");
    if (OALFunctions->alcIsExtensionPresent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcIsExtensionPresent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetProcAddress = (LPALCGETPROCADDRESS)dlsym(OpenALLib, "alcGetProcAddress");
    if (OALFunctions->alcGetProcAddress == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetProcAddress' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetEnumValue = (LPALCGETENUMVALUE)dlsym(OpenALLib, "alcGetEnumValue");
    if (OALFunctions->alcGetEnumValue == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetEnumValue' function address\n");
        return AL_FALSE;
    }
    
    return AL_TRUE;
}

ALvoid UnloadOAL11Library(HMODULE OpenALLib)
{
    if(OpenALLib)
    {
        dlclose(OpenALLib);
        OpenALLib= NULL;
    }
}


