#define ArrayCount(Array) (sizeof(Array) / sizeof(Array[0])) 

#ifdef POWDER_DEBUG
#define Assert(Expression) if(!(Expression)) {printf("Assertion failed in: %s on line %d\n",__FILE__,__LINE__); exit(EXIT_FAILURE);}
#else
#define Assert(Expression)
#endif

#define NUM_JOYSTICK_KEYS Joystick_Count
#define NUM_KEYS Key_Count
#define NUM_MOUSE_BUTTONS Mouse_Count

#define Min(A,B) ((A < B) ? (A) : (B))
#define Max(A,B) ((A > B) ? (A) : (B))
#define Abs(x) ((x) < 0 ? -(x) : (x))


#include "stdint.h"

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef float real32;
typedef double real64;

static void HandleError(char const *File, int32 LineNum, char const *msg)
{
    fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
    fprintf(stderr, "%s\n", msg);
}

struct CompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return strcmp(lhs, rhs) < 0;
    }
};

static char* CombineStrings(const char * str1, const char* str2)
{
    char * str3 = (char *) malloc(1 + strlen(str1) + strlen(str2));
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}


struct config_data
{
    char* Title;
    char* Version;
    uint32 ScreenWidth;
    uint32 ScreenHeight;
    bool32 Fullscreen;
};

static void LoadConfig(const char* FilePath, config_data* ConfigData)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    ConfigData->Title = (char*)malloc(sizeof(char) * 30);
    ConfigData->Version = (char*)malloc(sizeof(char) * 30);
    
    if(File)
    {
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "title %s", ConfigData->Title);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "version %s", ConfigData->Version);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "screen_width %d", &ConfigData->ScreenWidth);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
        }
        
        if(fgets(LineBuffer, 255, File))
        {
            sscanf(LineBuffer, "fullscreen %d", &ConfigData->Fullscreen);
        }
        fclose(File);
    }
    
}


