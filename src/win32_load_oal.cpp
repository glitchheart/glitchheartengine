
ALboolean LoadOAL11Library(char* FilePath, oal_api* OALFunctions, HMODULE OpenALDLL)
{
    if(!OALFunctions)
        return AL_FALSE;
    
    if(FilePath)
        OpenALDLL = LoadLibrary(FilePath);
    else
    {
        OpenALDLL = LoadLibrary("OpenAL32.dll");
    }
    
    if(!OpenALDLL)
    {
        i32 Err = GetLastError();
        DEBUG_PRINT("Could not load OpenAL: %d\n", Err);
    }
    
    memset(OALFunctions, 0, sizeof(oal_api));
    
    OALFunctions->alEnable = (LPALENABLE)GetProcAddress(OpenALDLL, "alEnable");
    if(OALFunctions->alEnable == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alEnable' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDisable = (LPALDISABLE)GetProcAddress(OpenALDLL, "alDisable");
    if (OALFunctions->alDisable == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDisable' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsEnabled = (LPALISENABLED)GetProcAddress(OpenALDLL, "alIsEnabled");
    if (OALFunctions->alIsEnabled == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsEnabled' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBoolean = (LPALGETBOOLEAN)GetProcAddress(OpenALDLL, "alGetBoolean");
    if (OALFunctions->alGetBoolean == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBoolean' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetInteger = (LPALGETINTEGER)GetProcAddress(OpenALDLL, "alGetInteger");
    if (OALFunctions->alGetInteger == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetInteger' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetFloat = (LPALGETFLOAT)GetProcAddress(OpenALDLL, "alGetFloat");
    if (OALFunctions->alGetFloat == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetFloat' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetDouble = (LPALGETDOUBLE)GetProcAddress(OpenALDLL, "alGetDouble");
    if (OALFunctions->alGetDouble == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetDouble' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBooleanv = (LPALGETBOOLEANV)GetProcAddress(OpenALDLL, "alGetBooleanv");
    if (OALFunctions->alGetBooleanv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBooleanv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetIntegerv = (LPALGETINTEGERV)GetProcAddress(OpenALDLL, "alGetIntegerv");
    if (OALFunctions->alGetIntegerv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetIntegerv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetFloatv = (LPALGETFLOATV)GetProcAddress(OpenALDLL, "alGetFloatv");
    if (OALFunctions->alGetFloatv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetFloatv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetDoublev = (LPALGETDOUBLEV)GetProcAddress(OpenALDLL, "alGetDoublev");
    if (OALFunctions->alGetDoublev == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetDoublev' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetString = (LPALGETSTRING)GetProcAddress(OpenALDLL, "alGetString");
    if (OALFunctions->alGetString == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetString' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetError = (LPALGETERROR)GetProcAddress(OpenALDLL, "alGetError");
    if (OALFunctions->alGetError == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetError' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsExtensionPresent = (LPALISEXTENSIONPRESENT)GetProcAddress(OpenALDLL, "alIsExtensionPresent");
    if (OALFunctions->alIsExtensionPresent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsExtensionPresent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetProcAddress = (LPALGETPROCADDRESS)GetProcAddress(OpenALDLL, "alGetProcAddress");
    if (OALFunctions->alGetProcAddress == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetProcAddress' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetEnumValue = (LPALGETENUMVALUE)GetProcAddress(OpenALDLL, "alGetEnumValue");
    if (OALFunctions->alGetEnumValue == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetEnumValue' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListeneri = (LPALLISTENERI)GetProcAddress(OpenALDLL, "alListeneri");
    if (OALFunctions->alListeneri == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListeneri' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListenerf = (LPALLISTENERF)GetProcAddress(OpenALDLL, "alListenerf");
    if (OALFunctions->alListenerf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListenerf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListener3f = (LPALLISTENER3F)GetProcAddress(OpenALDLL, "alListener3f");
    if (OALFunctions->alListener3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListener3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListenerfv = (LPALLISTENERFV)GetProcAddress(OpenALDLL, "alListenerfv");
    if (OALFunctions->alListenerfv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListenerfv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListeneri = (LPALGETLISTENERI)GetProcAddress(OpenALDLL, "alGetListeneri");
    if (OALFunctions->alGetListeneri == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListeneri' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListenerf =(LPALGETLISTENERF)GetProcAddress(OpenALDLL, "alGetListenerf");
    if (OALFunctions->alGetListenerf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListenerf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListener3f = (LPALGETLISTENER3F)GetProcAddress(OpenALDLL, "alGetListener3f");
    if (OALFunctions->alGetListener3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListener3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListenerfv = (LPALGETLISTENERFV)GetProcAddress(OpenALDLL, "alGetListenerfv");
    if (OALFunctions->alGetListenerfv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListenerfv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGenSources = (LPALGENSOURCES)GetProcAddress(OpenALDLL, "alGenSources");
    if (OALFunctions->alGenSources == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGenSources' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDeleteSources = (LPALDELETESOURCES)GetProcAddress(OpenALDLL, "alDeleteSources");
    if (OALFunctions->alDeleteSources == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDeleteSources' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsSource = (LPALISSOURCE)GetProcAddress(OpenALDLL, "alIsSource");
    if (OALFunctions->alIsSource == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsSource' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcei = (LPALSOURCEI)GetProcAddress(OpenALDLL, "alSourcei");
    if (OALFunctions->alSourcei == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcei' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcef = (LPALSOURCEF)GetProcAddress(OpenALDLL, "alSourcef");
    if (OALFunctions->alSourcef == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcef' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSource3f = (LPALSOURCE3F)GetProcAddress(OpenALDLL, "alSource3f");
    if (OALFunctions->alSource3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSource3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcefv = (LPALSOURCEFV)GetProcAddress(OpenALDLL, "alSourcefv");
    if (OALFunctions->alSourcefv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcefv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcei = (LPALGETSOURCEI)GetProcAddress(OpenALDLL, "alGetSourcei");
    if (OALFunctions->alGetSourcei == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcei' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcef = (LPALGETSOURCEF)GetProcAddress(OpenALDLL, "alGetSourcef");
    if (OALFunctions->alGetSourcef == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcef' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcefv = (LPALGETSOURCEFV)GetProcAddress(OpenALDLL, "alGetSourcefv");
    if (OALFunctions->alGetSourcefv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcefv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePlayv = (LPALSOURCEPLAYV)GetProcAddress(OpenALDLL, "alSourcePlayv");
    if (OALFunctions->alSourcePlayv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePlayv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceStopv = (LPALSOURCESTOPV)GetProcAddress(OpenALDLL, "alSourceStopv");
    if (OALFunctions->alSourceStopv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceStopv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePlay = (LPALSOURCEPLAY)GetProcAddress(OpenALDLL, "alSourcePlay");
    if (OALFunctions->alSourcePlay == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePlay' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePause = (LPALSOURCEPAUSE)GetProcAddress(OpenALDLL, "alSourcePause");
    if (OALFunctions->alSourcePause == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePause' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceStop = (LPALSOURCESTOP)GetProcAddress(OpenALDLL, "alSourceStop");
    if (OALFunctions->alSourceStop == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceStop' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGenBuffers = (LPALGENBUFFERS)GetProcAddress(OpenALDLL, "alGenBuffers");
    if (OALFunctions->alGenBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGenBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(OpenALDLL, "alDeleteBuffers");
    if (OALFunctions->alDeleteBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDeleteBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsBuffer = (LPALISBUFFER)GetProcAddress(OpenALDLL, "alIsBuffer");
    if (OALFunctions->alIsBuffer == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsBuffer' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alBufferData = (LPALBUFFERDATA)GetProcAddress(OpenALDLL, "alBufferData");
    if (OALFunctions->alBufferData == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alBufferData' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBufferi = (LPALGETBUFFERI)GetProcAddress(OpenALDLL, "alGetBufferi");
    if (OALFunctions->alGetBufferi == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBufferi' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBufferf = (LPALGETBUFFERF)GetProcAddress(OpenALDLL, "alGetBufferf");
    if (OALFunctions->alGetBufferf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBufferf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)GetProcAddress(OpenALDLL, "alSourceQueueBuffers");
    if (OALFunctions->alSourceQueueBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceQueueBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)GetProcAddress(OpenALDLL, "alSourceUnqueueBuffers");
    if (OALFunctions->alSourceUnqueueBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceUnqueueBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDistanceModel = (LPALDISTANCEMODEL)GetProcAddress(OpenALDLL, "alDistanceModel");
    if (OALFunctions->alDistanceModel == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDistanceModel' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDopplerFactor = (LPALDOPPLERFACTOR)GetProcAddress(OpenALDLL, "alDopplerFactor");
    if (OALFunctions->alDopplerFactor == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDopplerFactor' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDopplerVelocity = (LPALDOPPLERVELOCITY)GetProcAddress(OpenALDLL, "alDopplerVelocity");
    if (OALFunctions->alDopplerVelocity == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDopplerVelocity' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetString = (LPALCGETSTRING)GetProcAddress(OpenALDLL, "alcGetString");
    if (OALFunctions->alcGetString == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetString' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetIntegerv = (LPALCGETINTEGERV)GetProcAddress(OpenALDLL, "alcGetIntegerv");
    if (OALFunctions->alcGetIntegerv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetIntegerv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(OpenALDLL, "alcOpenDevice");
    if (OALFunctions->alcOpenDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcOpenDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(OpenALDLL, "alcCloseDevice");
    if (OALFunctions->alcCloseDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcCloseDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(OpenALDLL, "alcCreateContext");
    if (OALFunctions->alcCreateContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcCreateContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(OpenALDLL, "alcMakeContextCurrent");
    if (OALFunctions->alcMakeContextCurrent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcMakeContextCurrent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcProcessContext = (LPALCPROCESSCONTEXT)GetProcAddress(OpenALDLL, "alcProcessContext");
    if (OALFunctions->alcProcessContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcProcessContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetCurrentContext = (LPALCGETCURRENTCONTEXT)GetProcAddress(OpenALDLL, "alcGetCurrentContext");
    if (OALFunctions->alcGetCurrentContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetCurrentContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)GetProcAddress(OpenALDLL, "alcGetContextsDevice");
    if (OALFunctions->alcGetContextsDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetContextsDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcSuspendContext = (LPALCSUSPENDCONTEXT)GetProcAddress(OpenALDLL, "alcSuspendContext");
    if (OALFunctions->alcSuspendContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcSuspendContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(OpenALDLL, "alcDestroyContext");
    if (OALFunctions->alcDestroyContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcDestroyContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetError = (LPALCGETERROR)GetProcAddress(OpenALDLL, "alcGetError");
    if (OALFunctions->alcGetError == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetError' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT)GetProcAddress(OpenALDLL, "alcIsExtensionPresent");
    if (OALFunctions->alcIsExtensionPresent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcIsExtensionPresent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetProcAddress = (LPALCGETPROCADDRESS)GetProcAddress(OpenALDLL, "alcGetProcAddress");
    if (OALFunctions->alcGetProcAddress == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetProcAddress' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetEnumValue = (LPALCGETENUMVALUE)GetProcAddress(OpenALDLL, "alcGetEnumValue");
    if (OALFunctions->alcGetEnumValue == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetEnumValue' function address\n");
        return AL_FALSE;
    }
    
    return AL_TRUE;
}

ALvoid UnloadOAL11Library(HMODULE OpenALDLL)
{
    if(OpenALDLL)
    {
        FreeLibrary(OpenALDLL);
        OpenALDLL= NULL;
    }
}


