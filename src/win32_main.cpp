//#define _CRTDBG_MAP_ALLOC  
//#define _DEBUG

//#include <stdlib.h>  
//#include <crtdbg.h>  

#define DEBUG
//#include "platform.h"
#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

#include <glad/glad.h>
#include "fmod.h"
#include "fmod_errors.h"

#include "Commdlg.h"
#include <windows.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <GLFW/glfw3.h>

#include "win32_main.h"

platform_api Platform;
log_state LogState;

#include "gmap.cpp"
#include "keycontroller.h"

#include "console.h"
#include "opengl_rendering.h"
#include "keycontroller.cpp"
#include "sound.h"
#include "fmod_sound.h"
#include "fmod_sound.cpp"
#include "filehandling.h"

input_controller InputController;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"


static FILETIME GetLastWriteTime(const char* FilePath)
{
    FILETIME LastWriteTime = {};
    
    WIN32_FIND_DATA FindData;
    HANDLE FindHandle = FindFirstFileA(FilePath, &FindData);
    
    if(FindHandle != INVALID_HANDLE_VALUE)
    {
        LastWriteTime = FindData.ftLastWriteTime;
        FindClose(FindHandle);
    }
    return LastWriteTime;
}

static game_code LoadGameCode(char* DllPath, char* TempDllPath)
{
    game_code Result = {};
    Result.DllPath = DllPath;
    Result.TempDllPath = TempDllPath;
    
    CopyFile(Result.DllPath, Result.TempDllPath, false);
    Result.Update = UpdateStub;
    Result.GameCodeDLL = LoadLibraryA(Result.TempDllPath);
    
    Result.LastDllWriteTime = GetLastWriteTime(Result.DllPath);
    
    if (Result.GameCodeDLL)
    {
        Result.Update = (update *)GetProcAddress(Result.GameCodeDLL, "Update");
        Result.IsValid = Result.Update != 0;
    }
    
    if (!Result.IsValid)
    {
        DEBUG_PRINT("Invalid game code\n");
        Result.Update = UpdateStub;
    }
    
    return Result;
}

static void UnloadGameCode(game_code *GameCode)
{
    if (GameCode->GameCodeDLL)
    {
        FreeLibrary(GameCode->GameCodeDLL);
        GameCode->GameCodeDLL = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->Update = UpdateStub;
}

static void ReloadGameCode(game_code *GameCode, char* DllPath, char* TempDllPath)
{
    UnloadGameCode(GameCode);
    *GameCode = LoadGameCode(DllPath, TempDllPath);
}

static void ReloadDlls(game_code *Game, char* DllPath, char* TempDllPath)
{
    FILETIME LastWriteTime = GetLastWriteTime(Game->DllPath);
    
    if (CompareFileTime(&Game->LastDllWriteTime, &LastWriteTime) != 0)
    {
        DEBUG_PRINT("RELOAD\n");
        ReloadGameCode(Game, DllPath, TempDllPath);
        Assert(Game);
    }
}

static void SetInvalidKeys()
{
    InputController.AnyKeyPressed = false;
    for(u32 KeyCode = 0; KeyCode < NUM_KEYS; KeyCode++)
    {
        if(InputController.KeysJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController.KeysJustPressed[KeyCode] = Key_Invalid;
        }
        InputController.KeysUp[KeyCode] = false;
    } 
}

static void SetControllerInvalidKeys()
{
    InputController.AnyKeyPressed = false;
    for(u32 KeyCode = 0; KeyCode < NUM_JOYSTICK_KEYS; KeyCode++)
    {
        if(InputController.JoystickKeysJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController.JoystickKeysJustPressed[KeyCode] = Key_Invalid;
        }
    }
}

static void SetMouseInvalidKeys()
{
    InputController.AnyKeyPressed = false;
    for(u32 KeyCode = 0; KeyCode < NUM_MOUSE_BUTTONS; KeyCode++)
    {
        if(InputController.MouseButtonJustPressed[KeyCode] == Key_JustPressed)
        {
            InputController.MouseButtonJustPressed[KeyCode] = Key_Invalid;
        }
        InputController.MouseButtonsUp[KeyCode] = false;
    }
    InputController.ScrollX = 0;
    InputController.ScrollY = 0;
}


inline void LoadConfig(const char* FilePath, config_data* ConfigData, memory_arena* PermArena)
{
    FILE* File;
    File = fopen(FilePath, "r");
    char LineBuffer[255];
    
    ConfigData->Title = PushString(PermArena, 40);
    ConfigData->Version = PushString(PermArena, 40);
    
    if(File)
    {
        while(fgets(LineBuffer, 255, File))
        {
            if(StartsWith(LineBuffer, "title"))
            {
                // @Speed: This can probably be done much better and efficient
                i32 Index = 0;
                char TitleBuffer[50];
                
                b32 AfterTitle = false;
                
                for(i32 BufferIndex = 0; BufferIndex < (i32)strlen(LineBuffer); BufferIndex++)
                {
                    if(AfterTitle)
                    {
                        char Character = LineBuffer[BufferIndex];
                        if(Character == '\n' || Character == '\r')
                        {
                            break;
                        }
                        else
                        {
                            TitleBuffer[Index++] = Character;
                        }
                    }
                    
                    if(LineBuffer[BufferIndex] == ' ')
                    {
                        AfterTitle = true;
                    }
                }
                
                sprintf(ConfigData->Title, "%s", TitleBuffer);
            }
            else if(StartsWith(LineBuffer, "version"))
            {
                sscanf(LineBuffer, "version %s", ConfigData->Version);
            }
            else if(StartsWith(LineBuffer, "screen_width"))
            {
                sscanf(LineBuffer, "screen_width %d", &ConfigData->ScreenWidth);
            }
            else if(StartsWith(LineBuffer, "screen_height"))
            {
                sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
            }
            else if(StartsWith(LineBuffer, "scale_from_width"))
            {
                sscanf(LineBuffer, "scale_from_width %d", &ConfigData->ScaleFromWidth);
            }
            else if(StartsWith(LineBuffer, "scale_from_height"))
            {
                sscanf(LineBuffer, "scale_from_height %d", &ConfigData->ScaleFromHeight);
            }
            else if(StartsWith(LineBuffer, "contrast"))
            {
                sscanf(LineBuffer, "contrast %f", &ConfigData->Contrast);
            }
            else if(StartsWith(LineBuffer, "brightness"))
            {
                sscanf(LineBuffer, "brightness %f", &ConfigData->Brightness);
            }
            else if(StartsWith(LineBuffer, "fullscreen"))
            {
                sscanf(LineBuffer, "fullscreen %d", &ConfigData->Fullscreen);
            } 
            else if(StartsWith(LineBuffer, "muted"))
            {
                sscanf(LineBuffer, "muted %d", &ConfigData->Muted);
            }
            else if(StartsWith(LineBuffer, "sfx_volume"))
            {
                sscanf(LineBuffer, "sfx_volume %f", &ConfigData->SFXVolume);
            }
            else if(StartsWith(LineBuffer, "music_volume"))
            {
                sscanf(LineBuffer, "music_volume %f", &ConfigData->MusicVolume);
            }
            else if(StartsWith(LineBuffer, "zoom"))
            {
                sscanf(LineBuffer, "zoom %f", &ConfigData->Zoom);
            }
        }
        
        fclose(File);
    }
}

// Global
win32_memory_state Win32MemoryState;

PLATFORM_ALLOCATE_MEMORY(Win32AllocateMemory)
{
    Assert(sizeof(win32_memory_block) == 64);
    
    umm PageSize = 4096; //TODO: Not really always correct?
    umm TotalSize = Size + sizeof(win32_memory_block);
    umm BaseOffset = sizeof(win32_memory_block);
    umm ProtectOffset = 0;
    
    if(Flags & PM_UnderflowCheck)
    {
        TotalSize = Size + 2 * PageSize;
        BaseOffset = 2 * PageSize;
        ProtectOffset = PageSize;
    }
    
    if(Flags & PM_OverflowCheck)
    {
        umm SizeRoundedUp = AlignPow2(Size, PageSize);
        TotalSize = SizeRoundedUp + 2 * PageSize;
        BaseOffset = PageSize + SizeRoundedUp - Size;
        ProtectOffset = PageSize + SizeRoundedUp;
    }
    
    win32_memory_block* Block  = (win32_memory_block*)VirtualAlloc(0, TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    Assert(Block);
    Block->Block.Base = (u8*)Block + BaseOffset;
    Assert(Block->Block.Used == 0);
    Assert(Block->Block.Prev == 0);
    
    if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    {
        DWORD OldProtect = 0;
        BOOL Protected = VirtualProtect((u8*)Block + ProtectOffset, PageSize, PAGE_NOACCESS, &OldProtect);
        Assert(Protected);
    }
    
    Block->Block.Size = Size;
    Block->Block.Flags = Flags;
    
    platform_memory_block* PlatBlock = &Block->Block;
    
    if(Flags & PM_Temporary)
    {
        Assert((Win32MemoryState.TempCount + 1) < MAX_TEMP_BLOCKS);
        Win32MemoryState.TempSizeAllocated += TotalSize;
        Win32MemoryState.Blocks[Win32MemoryState.TempCount++] = PlatBlock;
    }
    else
    {
        Win32MemoryState.PermanentBlocks++;
        Win32MemoryState.PermanentSizeAllocated += TotalSize;
    }
    
    return PlatBlock;
}

PLATFORM_DEALLOCATE_MEMORY(Win32DeallocateMemory)
{
    if(Block)
    {
        if((Block->Flags & PM_Temporary) == 0)
        {
            Win32MemoryState.PermanentBlocks--;
            Win32MemoryState.PermanentSizeAllocated -= (Block->Size + sizeof(win32_memory_block));
        }
        
        win32_memory_block *Win32Block =  ((win32_memory_block*)Block);
        VirtualFree(Win32Block, 0, MEM_RELEASE);
    }
}

static void ClearTempMemory()
{
    for(i32 Temp = 0; Temp < Win32MemoryState.TempCount; Temp++)
    {
        Win32DeallocateMemory(Win32MemoryState.Blocks[Temp]);
    }
    
    Win32MemoryState.TempCount = 0;
    Win32MemoryState.TempSizeAllocated = 0;
}


inline PLATFORM_GET_ALL_FILES_WITH_EXTENSION(Win32FindFilesWithExtensions)
{
    if(DirectoryData->FilesLength == 0)
    {
        DirectoryData->FileNames = PushTempArray(512, char*);
        DirectoryData->FilePaths = PushTempArray(512, char*);
    }
    
    WIN32_FIND_DATA FindFile;
    HANDLE hFind = NULL;
    
    char Path[2048];
    
    //Process directories
    sprintf(Path, "%s*", DirectoryPath);
    hFind = FindFirstFile(Path, &FindFile);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(FindFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if(strcmp(FindFile.cFileName, ".") != 0
                   && strcmp(FindFile.cFileName, "..") != 0)
                {
                    char SubPath[2048];
                    sprintf(SubPath, "%s%s/", DirectoryPath, FindFile.cFileName);
                    Win32FindFilesWithExtensions(SubPath, Extension, DirectoryData, WithSubDirectories);
                }
                
            }
        }
        
        while(FindNextFile(hFind, &FindFile));
        FindClose(hFind);
    }
    else
    {
        DEBUG_PRINT("No files with extension %s found in %s\n", Extension, DirectoryPath);
        return;
    }
    
    //Process files
    sprintf(Path, "%s*.%s", DirectoryPath, Extension);
    hFind = FindFirstFile(Path, &FindFile);
    if(hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!(FindFile.dwFileAttributes &FILE_ATTRIBUTE_DIRECTORY))
            {
                if(strcmp(FindFile.cFileName, ".") != 0
                   && strcmp(FindFile.cFileName, "..") != 0)
                {
                    char* ConcatStr = Concat(DirectoryPath, FindFile.cFileName);
                    char* FileName = strtok(FindFile.cFileName, ".");
                    
                    DirectoryData->FilePaths[DirectoryData->FilesLength] = PushTempString(ConcatStr);
                    DirectoryData->FileNames[DirectoryData->FilesLength] = PushTempString(FileName);
                    DirectoryData->FilesLength++;
                }
            }
        } while (FindNextFile(hFind, &FindFile));
        FindClose(hFind);
    }
    else
    {
        DEBUG_PRINT("No files with extension %s found in %s\n", Extension, DirectoryPath);
        return;
    }
    
}


inline PLATFORM_FILE_EXISTS(Win32FileExists)
{
    struct stat Buffer;
    return (stat(FilePath,&Buffer) == 0);
}

inline PLATFORM_OPEN_FILE_WITH_DIALOG(Win32OpenFileWithDialog)
{
    OPENFILENAME Ofn;
    char SzFile[260];
    platform_file Result;
    
    HANDLE Hf;
    
    ZeroMemory(&Ofn, sizeof(Ofn));
    Ofn.lStructSize = sizeof(Ofn);
    Ofn.hwndOwner = 0;
    Ofn.lpstrFile = SzFile;
    Ofn.lpstrFile[0] = '\0';
    Ofn.nMaxFile = sizeof(SzFile);
    Ofn.lpstrFilter = "All\0*.*\0";
    Ofn.nFilterIndex = 1;
    Ofn.lpstrFileTitle = NULL;
    Ofn.nMaxFileTitle = 0;
    Ofn.lpstrInitialDir = NULL;
    Ofn.Flags = 0;
    
    if(GetOpenFileName(&Ofn) == TRUE)
    {
        Hf = CreateFile(Ofn.lpstrFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
        
        Result.File = (FILE*)Hf;
        strcpy(Result.Path, Ofn.lpstrFile);
        char* P = PushTempString(Result.Path);
        auto Tok = StrSep(&P, ".");
        Tok = StrSep(&P, ".");
        strcpy(Result.Extension, Tok);
    }
    return Result;
}

int main(int Argc, char** Args)
{
    game_memory GameMemory = {};
    
    GameMemory.ShouldReload = true;
    
    GameMemory.ExitGame = false;
    
    GameMemory.PlatformAPI.GetAllFilesWithExtension = Win32FindFilesWithExtensions;
    GameMemory.PlatformAPI.FileExists = Win32FileExists;
    GameMemory.PlatformAPI.AllocateMemory = Win32AllocateMemory;
    GameMemory.PlatformAPI.DeallocateMemory = Win32DeallocateMemory;
    GameMemory.PlatformAPI.OpenFileWithDialog = Win32OpenFileWithDialog;
    Platform = GameMemory.PlatformAPI;
    
    win32_state* Win32State = BootstrapPushStruct(win32_state, PermArena);
    
    LogState = GameMemory.LogState;
    InitLog(LFlag_File | LFlag_Debug, Concat("../log_", "", &Win32State->PermArena));
    
    
    char* DllPath = "game.dll";
    char* TempDllPath = "game_temp.dll";
    
    memory_arena DebugArena = {};
    
    GameMemory.DebugState = PushStruct(&Win32State->PermArena, debug_state);
    
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectOrigin = math::v2(50, 780);
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectSize = math::v2(300,0);
    
    config_data ConfigData;
    LoadConfig("../.config", &ConfigData, &Win32State->PermArena);
    
    GameMemory.ConfigData = ConfigData;
    
    InitKeys();
    render_state RenderState = {};
    renderer Renderer = {};
    
    Renderer.Commands.MinimumBlockSize = sizeof(render_command) * MAX_RENDER_COMMANDS;
    Renderer.UICommands.MinimumBlockSize = sizeof(render_command) * MAX_UI_COMMANDS;
    Renderer.LightCommands.MinimumBlockSize = sizeof(render_command) * MAX_LIGHT_COMMANDS;
    Renderer.SpritesheetAnimationCount = 0;
    Renderer.SpritesheetAnimationInfoCount = 0;
    
    InitializeOpenGL(RenderState, Renderer, &ConfigData, &Win32State->PermArena);
    
    game_code Game = LoadGameCode(DllPath, TempDllPath);
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager, DllPath);
    
    u32 FrameCounterForAssetCheck = 0;
    
    sound_device SoundDevice = {};
    InitAudio_FMOD(&SoundDevice);
    
    sound_commands SoundCommands = {};
    
    if (SoundDevice.IsInitialized)
    {
        ResetCommands(&SoundCommands);
        SoundCommands.SFXVolume = ConfigData.SFXVolume;
        SoundCommands.MusicVolume = ConfigData.MusicVolume;
        SoundCommands.Muted = ConfigData.Muted;
    }
    
    r64 LastFrame = GetTime();
    r64 CurrentFrame = 0.0;
    r64 DeltaTime;
    
    while (!ShouldCloseWindow(RenderState) && !Renderer.ShouldClose)
    {
        Renderer.WindowWidth = RenderState.WindowWidth;
        Renderer.WindowHeight = RenderState.WindowHeight;
        
        memcpy(Renderer.Viewport, RenderState.Viewport, sizeof(i32) * 4);
        
        
        //calculate deltatime
        CurrentFrame = GetTime();
        DeltaTime = Min(CurrentFrame - LastFrame, 0.1);
        LastFrame = CurrentFrame;
        Renderer.FPS = 1.0/DeltaTime;
        Renderer.CurrentFrame++;
        Renderer.FPSSum += Renderer.FPS;
        if(Renderer.CurrentFrame == 60)
        {
            Renderer.CurrentFrame = 0;
            Renderer.AverageFPS = Renderer.FPSSum / 60.0;
            Renderer.FPSSum = 0.0;
        }
        
        if(GameMemory.ExitGame)
        {
            DEBUG_PRINT("Quit\n");
            glfwSetWindowShouldClose(RenderState.Window, GLFW_TRUE);
        }
        
        ShowMouseCursor(RenderState, Renderer.ShowMouseCursor);
        
        ReloadAssets(RenderState, &AssetManager, &Win32State->PermArena);
        
        ReloadDlls(&Game, DllPath, TempDllPath);
        
        Game.Update(DeltaTime, &GameMemory, Renderer, &InputController, &SoundCommands);
        
        Render(RenderState, Renderer, &Win32State->PermArena);
        PlaySounds(&SoundDevice, &SoundCommands);
        
        SetControllerInvalidKeys();
        SetInvalidKeys();
        SetMouseInvalidKeys();
        
        PollEvents();
        
        if(ControllerPresent())
        {
            ControllerKeys(GLFW_JOYSTICK_1);
        }
        
        FrameCounterForAssetCheck++;
        if(FrameCounterForAssetCheck == 10)
        {
            ListenToFileChanges(&AssetManager);
            FrameCounterForAssetCheck = 0;
        }
        
        UpdateLog();
        ClearTempMemory();
    }
    
    CloseLog();
    CleanupSound(&SoundDevice);
    CloseWindow(RenderState);
    //_CrtDumpMemoryLeaks();
    //_CrtMemState MemState;
    //_CrtMemDumpStatistics(&MemState);
}
