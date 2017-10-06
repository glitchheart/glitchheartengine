#include "windows.h"
#include "load_oal.h"

HINSTANCE g_hOpenALDLL = NULL;

enum dll_arch
{
    A_32,
    A_64
};

ALboolean LoadOAL11Library(char* FilePath, oal_api* OALFunctions)
{
    if(!OALFunctions)
        return AL_FALSE;
    
    dll_arch Arch = A_64;
    
    if(FilePath)
        g_hOpenALDLL = LoadLibrary(FilePath);
    else
    {
        
        
#if defined(_WIN64)
        CopyFile("../libs/soft_oal/dll/Win64/soft_oal.dll", "soft_oal.dll", FALSE);
        Arch = A_64;
#else
        CopyFile("../libs/soft_oal/dll/Win32/soft_oal.dll", "soft_oal.dll", FALSE);
        Arch = A_32;
#endif
        g_hOpenALDLL = LoadLibrary("soft_oal.dll");
    }
    
    if(!g_hOpenALDLL)
    {
        i32 Err = GetLastError();
        DEBUG_PRINT("Could not load OpenAL: %d\n", Err);
    }
    
    memset(OALFunctions, 0, sizeof(oal_api));
    
    OALFunctions->alEnable = (LPALENABLE)GetProcAddress(g_hOpenALDLL, "alEnable");
    if(OALFunctions->alEnable == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alEnable' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDisable = (LPALDISABLE)GetProcAddress(g_hOpenALDLL, "alDisable");
    if (OALFunctions->alDisable == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDisable' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsEnabled = (LPALISENABLED)GetProcAddress(g_hOpenALDLL, "alIsEnabled");
    if (OALFunctions->alIsEnabled == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsEnabled' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBoolean = (LPALGETBOOLEAN)GetProcAddress(g_hOpenALDLL, "alGetBoolean");
    if (OALFunctions->alGetBoolean == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBoolean' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetInteger = (LPALGETINTEGER)GetProcAddress(g_hOpenALDLL, "alGetInteger");
    if (OALFunctions->alGetInteger == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetInteger' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetFloat = (LPALGETFLOAT)GetProcAddress(g_hOpenALDLL, "alGetFloat");
    if (OALFunctions->alGetFloat == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetFloat' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetDouble = (LPALGETDOUBLE)GetProcAddress(g_hOpenALDLL, "alGetDouble");
    if (OALFunctions->alGetDouble == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetDouble' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBooleanv = (LPALGETBOOLEANV)GetProcAddress(g_hOpenALDLL, "alGetBooleanv");
    if (OALFunctions->alGetBooleanv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBooleanv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetIntegerv = (LPALGETINTEGERV)GetProcAddress(g_hOpenALDLL, "alGetIntegerv");
    if (OALFunctions->alGetIntegerv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetIntegerv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetFloatv = (LPALGETFLOATV)GetProcAddress(g_hOpenALDLL, "alGetFloatv");
    if (OALFunctions->alGetFloatv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetFloatv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetDoublev = (LPALGETDOUBLEV)GetProcAddress(g_hOpenALDLL, "alGetDoublev");
    if (OALFunctions->alGetDoublev == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetDoublev' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetString = (LPALGETSTRING)GetProcAddress(g_hOpenALDLL, "alGetString");
    if (OALFunctions->alGetString == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetString' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetError = (LPALGETERROR)GetProcAddress(g_hOpenALDLL, "alGetError");
    if (OALFunctions->alGetError == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetError' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsExtensionPresent = (LPALISEXTENSIONPRESENT)GetProcAddress(g_hOpenALDLL, "alIsExtensionPresent");
    if (OALFunctions->alIsExtensionPresent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsExtensionPresent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetProcAddress = (LPALGETPROCADDRESS)GetProcAddress(g_hOpenALDLL, "alGetProcAddress");
    if (OALFunctions->alGetProcAddress == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetProcAddress' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetEnumValue = (LPALGETENUMVALUE)GetProcAddress(g_hOpenALDLL, "alGetEnumValue");
    if (OALFunctions->alGetEnumValue == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetEnumValue' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListeneri = (LPALLISTENERI)GetProcAddress(g_hOpenALDLL, "alListeneri");
    if (OALFunctions->alListeneri == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListeneri' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListenerf = (LPALLISTENERF)GetProcAddress(g_hOpenALDLL, "alListenerf");
    if (OALFunctions->alListenerf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListenerf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListener3f = (LPALLISTENER3F)GetProcAddress(g_hOpenALDLL, "alListener3f");
    if (OALFunctions->alListener3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListener3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alListenerfv = (LPALLISTENERFV)GetProcAddress(g_hOpenALDLL, "alListenerfv");
    if (OALFunctions->alListenerfv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alListenerfv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListeneri = (LPALGETLISTENERI)GetProcAddress(g_hOpenALDLL, "alGetListeneri");
    if (OALFunctions->alGetListeneri == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListeneri' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListenerf =(LPALGETLISTENERF)GetProcAddress(g_hOpenALDLL, "alGetListenerf");
    if (OALFunctions->alGetListenerf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListenerf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListener3f = (LPALGETLISTENER3F)GetProcAddress(g_hOpenALDLL, "alGetListener3f");
    if (OALFunctions->alGetListener3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListener3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetListenerfv = (LPALGETLISTENERFV)GetProcAddress(g_hOpenALDLL, "alGetListenerfv");
    if (OALFunctions->alGetListenerfv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetListenerfv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGenSources = (LPALGENSOURCES)GetProcAddress(g_hOpenALDLL, "alGenSources");
    if (OALFunctions->alGenSources == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGenSources' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDeleteSources = (LPALDELETESOURCES)GetProcAddress(g_hOpenALDLL, "alDeleteSources");
    if (OALFunctions->alDeleteSources == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDeleteSources' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsSource = (LPALISSOURCE)GetProcAddress(g_hOpenALDLL, "alIsSource");
    if (OALFunctions->alIsSource == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsSource' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcei = (LPALSOURCEI)GetProcAddress(g_hOpenALDLL, "alSourcei");
    if (OALFunctions->alSourcei == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcei' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcef = (LPALSOURCEF)GetProcAddress(g_hOpenALDLL, "alSourcef");
    if (OALFunctions->alSourcef == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcef' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSource3f = (LPALSOURCE3F)GetProcAddress(g_hOpenALDLL, "alSource3f");
    if (OALFunctions->alSource3f == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSource3f' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcefv = (LPALSOURCEFV)GetProcAddress(g_hOpenALDLL, "alSourcefv");
    if (OALFunctions->alSourcefv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcefv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcei = (LPALGETSOURCEI)GetProcAddress(g_hOpenALDLL, "alGetSourcei");
    if (OALFunctions->alGetSourcei == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcei' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcef = (LPALGETSOURCEF)GetProcAddress(g_hOpenALDLL, "alGetSourcef");
    if (OALFunctions->alGetSourcef == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcef' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetSourcefv = (LPALGETSOURCEFV)GetProcAddress(g_hOpenALDLL, "alGetSourcefv");
    if (OALFunctions->alGetSourcefv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetSourcefv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePlayv = (LPALSOURCEPLAYV)GetProcAddress(g_hOpenALDLL, "alSourcePlayv");
    if (OALFunctions->alSourcePlayv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePlayv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceStopv = (LPALSOURCESTOPV)GetProcAddress(g_hOpenALDLL, "alSourceStopv");
    if (OALFunctions->alSourceStopv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceStopv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePlay = (LPALSOURCEPLAY)GetProcAddress(g_hOpenALDLL, "alSourcePlay");
    if (OALFunctions->alSourcePlay == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePlay' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourcePause = (LPALSOURCEPAUSE)GetProcAddress(g_hOpenALDLL, "alSourcePause");
    if (OALFunctions->alSourcePause == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourcePause' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceStop = (LPALSOURCESTOP)GetProcAddress(g_hOpenALDLL, "alSourceStop");
    if (OALFunctions->alSourceStop == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceStop' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGenBuffers = (LPALGENBUFFERS)GetProcAddress(g_hOpenALDLL, "alGenBuffers");
    if (OALFunctions->alGenBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGenBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDeleteBuffers = (LPALDELETEBUFFERS)GetProcAddress(g_hOpenALDLL, "alDeleteBuffers");
    if (OALFunctions->alDeleteBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDeleteBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alIsBuffer = (LPALISBUFFER)GetProcAddress(g_hOpenALDLL, "alIsBuffer");
    if (OALFunctions->alIsBuffer == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alIsBuffer' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alBufferData = (LPALBUFFERDATA)GetProcAddress(g_hOpenALDLL, "alBufferData");
    if (OALFunctions->alBufferData == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alBufferData' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBufferi = (LPALGETBUFFERI)GetProcAddress(g_hOpenALDLL, "alGetBufferi");
    if (OALFunctions->alGetBufferi == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBufferi' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alGetBufferf = (LPALGETBUFFERF)GetProcAddress(g_hOpenALDLL, "alGetBufferf");
    if (OALFunctions->alGetBufferf == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alGetBufferf' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceQueueBuffers = (LPALSOURCEQUEUEBUFFERS)GetProcAddress(g_hOpenALDLL, "alSourceQueueBuffers");
    if (OALFunctions->alSourceQueueBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceQueueBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alSourceUnqueueBuffers = (LPALSOURCEUNQUEUEBUFFERS)GetProcAddress(g_hOpenALDLL, "alSourceUnqueueBuffers");
    if (OALFunctions->alSourceUnqueueBuffers == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alSourceUnqueueBuffers' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDistanceModel = (LPALDISTANCEMODEL)GetProcAddress(g_hOpenALDLL, "alDistanceModel");
    if (OALFunctions->alDistanceModel == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDistanceModel' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDopplerFactor = (LPALDOPPLERFACTOR)GetProcAddress(g_hOpenALDLL, "alDopplerFactor");
    if (OALFunctions->alDopplerFactor == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDopplerFactor' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alDopplerVelocity = (LPALDOPPLERVELOCITY)GetProcAddress(g_hOpenALDLL, "alDopplerVelocity");
    if (OALFunctions->alDopplerVelocity == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alDopplerVelocity' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetString = (LPALCGETSTRING)GetProcAddress(g_hOpenALDLL, "alcGetString");
    if (OALFunctions->alcGetString == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetString' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetIntegerv = (LPALCGETINTEGERV)GetProcAddress(g_hOpenALDLL, "alcGetIntegerv");
    if (OALFunctions->alcGetIntegerv == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetIntegerv' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(g_hOpenALDLL, "alcOpenDevice");
    if (OALFunctions->alcOpenDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcOpenDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(g_hOpenALDLL, "alcCloseDevice");
    if (OALFunctions->alcCloseDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcCloseDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(g_hOpenALDLL, "alcCreateContext");
    if (OALFunctions->alcCreateContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcCreateContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(g_hOpenALDLL, "alcMakeContextCurrent");
    if (OALFunctions->alcMakeContextCurrent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcMakeContextCurrent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcProcessContext = (LPALCPROCESSCONTEXT)GetProcAddress(g_hOpenALDLL, "alcProcessContext");
    if (OALFunctions->alcProcessContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcProcessContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetCurrentContext = (LPALCGETCURRENTCONTEXT)GetProcAddress(g_hOpenALDLL, "alcGetCurrentContext");
    if (OALFunctions->alcGetCurrentContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetCurrentContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetContextsDevice = (LPALCGETCONTEXTSDEVICE)GetProcAddress(g_hOpenALDLL, "alcGetContextsDevice");
    if (OALFunctions->alcGetContextsDevice == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetContextsDevice' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcSuspendContext = (LPALCSUSPENDCONTEXT)GetProcAddress(g_hOpenALDLL, "alcSuspendContext");
    if (OALFunctions->alcSuspendContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcSuspendContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(g_hOpenALDLL, "alcDestroyContext");
    if (OALFunctions->alcDestroyContext == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcDestroyContext' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetError = (LPALCGETERROR)GetProcAddress(g_hOpenALDLL, "alcGetError");
    if (OALFunctions->alcGetError == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetError' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcIsExtensionPresent = (LPALCISEXTENSIONPRESENT)GetProcAddress(g_hOpenALDLL, "alcIsExtensionPresent");
    if (OALFunctions->alcIsExtensionPresent == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcIsExtensionPresent' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetProcAddress = (LPALCGETPROCADDRESS)GetProcAddress(g_hOpenALDLL, "alcGetProcAddress");
    if (OALFunctions->alcGetProcAddress == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetProcAddress' function address\n");
        return AL_FALSE;
    }
    OALFunctions->alcGetEnumValue = (LPALCGETENUMVALUE)GetProcAddress(g_hOpenALDLL, "alcGetEnumValue");
    if (OALFunctions->alcGetEnumValue == NULL)
    {
        DEBUG_PRINT("Failed to retrieve 'alcGetEnumValue' function address\n");
        return AL_FALSE;
    }
    
    return AL_TRUE;
}

ALvoid UnloadOAL11Library()
{
    if(g_hOpenALDLL)
    {
        FreeLibrary(g_hOpenALDLL);
        g_hOpenALDLL = NULL;
    }
}


