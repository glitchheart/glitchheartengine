#include "windows.h"
#include "load_oal.h"

HINSTANCE g_hOpenALDLL = NULL;


ALboolean LoadOAL11Library(char* szOALFullPathName, LPOPENALFNTABLE lpOALFnTable)
{
    if(!lpOALFnTable)
        return AL_FALSE;
    
    if(szOALFullPathName)
        g_hOpenALDLL = LoadLibraryA(szOALFullPathName);
    else
        g_hOpenALDLL = LoadLibraryA("openal32.dll");
    
    if(!g_hOpenALDLL)
        return AL_FALSE;
    
    memset(lpOALFnTable, 0, sizeof(OPENALFNTABLE));
    
    lpOALFnTable->alEnable = (LPALENABLE)GetProcAddress(g_hOpenALDLL, "alEnable");
    if(lpOALFnTable->alEnable == NULL)
    {
        OutputDebugString("Failed to retrieve 'alenable' function address\n");
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


