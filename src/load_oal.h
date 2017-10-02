#include "al.h"
#include "alc.h"

#ifndef LOAD_OPENAL
#define LOAD_OPENAL

#ifndef ALchar
#define ALchar char
#endif

#ifndef ALCchar
#define ALCchar char
#endif

typedef void (ALAPIENTRY *LPALENABLE)( ALenum capability);

typedef struct
{
    LPALENABLE alEnable;
} OPENALFNTABLE, *LPOPENALFNTABLE;


#endif


ALboolean LoadOAL11Library(char* szOALFullPathName, LPOPENALFNTABLE lpOALFnTable);
ALvoid UnloadOAL11Library();


