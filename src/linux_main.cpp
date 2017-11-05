//#define _CRTDBG_MAP_ALLOC  
//#define _DEBUG

//#include <stdlib.h>  
//#include <crtdbg.h>  

//#include "windows.h"

#include "platform.h"
#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

#include <glad/glad.h>
#include "al.h"
#include "alc.h"
//#include <windows.h>
#include <dlfcn.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <GLFW/glfw3.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#include "unistd.h"
#include "linux_main.h"

#include "linux_load_oal.h"
#include "linux_load_oal.cpp"

platform_api Platform;

#include "gmap.cpp"
#include "keycontroller.h"

#include "console.h"
#include "opengl_rendering.h"
#include "keycontroller.cpp"
#include "sound.h"
#include "openal_sound.h"
#include "openal_sound.cpp"
#include "filehandling.h"

input_controller InputController;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"

static b32 CopyFile(const char* Src, const char* Dst, b32 OverwriteExisting)
{
    if(!OverwriteExisting && (access(Src, F_OK) != -1))
    {
        return false;
    }
    
    i32 ReadFD;
    i32 WriteFD;
    struct stat StatBuf;
    off_t Offset = 0;
    
    if((ReadFD = open(Src, O_RDONLY) == -1))
        return false;
    
    if(fstat(ReadFD, &StatBuf) == -1) 
    {
        return false;
    }
    
    WriteFD = open(Dst, O_WRONLY | O_CREAT | O_TRUNC, StatBuf.st_mode);
    if(WriteFD == -1)
    {
        close(ReadFD);
        return false;
    }
    
    i32 Result = sendfile(WriteFD, ReadFD, &Offset, StatBuf.st_size);
    close(ReadFD);
    close(WriteFD);
    return Result > 0;
}


static time_t GetLastWriteTime(const char* FilePath)
{
    struct stat Result;
    if(stat(FilePath, &Result) == 0)
    {
        auto ModTime = Result.st_mtime;
        return ModTime;
    }
    return 0;
}


static game_code LoadGameCode(char* LibPath, char* TempLibPath)
{
    game_code Result = {};
    Result.LibPath = LibPath;
    Result.TempLibPath = TempLibPath;
    
    //TODO: Find Linux equivalent of CopyFile
    CopyFile(Result.LibPath, Result.TempLibPath, false);
    Result.Update = UpdateStub;
    
    //TODO: Find lib load function for linux
    Result.GameCodeLib = dlopen(Result.TempLibPath, RTLD_LAZY);
    
    //TODO: Find last write time function for linux
    Result.LastLibWriteTime = GetLastWriteTime(Result.LibPath);
    
    if (Result.GameCodeLib)
    {
        //TODO: Get Proc address equivalent for linux
        Result.Update = (update *)dlsym(Result.GameCodeLib, "Update");
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
    if (GameCode->GameCodeLib)
    {
        //TODO: Linux freelibrary
        dlclose(GameCode->GameCodeLib);
        GameCode->GameCodeLib = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->Update = UpdateStub;
}

static void ReloadGameCode(game_code *GameCode, char* LibPath, char* TempLibPath)
{
    UnloadGameCode(GameCode);
    *GameCode = LoadGameCode(LibPath, TempLibPath);
}

static void ReloadLibs(game_code *Game, char* LibPath, char* TempLibPath)
{
    //TODO: Replace with Linux
    FILETIME LastWriteTime = GetLastWriteTime(Game->LibPath);
    
    //TODO: Replace with linux
    if (difftime(Game->LastLibWriteTime, LastWriteTime) != 0)
    {
        DEBUG_PRINT("RELOAD\n");
        ReloadGameCode(Game, LibPath, TempLibPath);
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
                sscanf(LineBuffer, "title %s", ConfigData->Title);
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
            else if(StartsWith(LineBuffer, "screen_height"))
            {
                sscanf(LineBuffer, "screen_height %d", &ConfigData->ScreenHeight);
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
linux_memory_state LinuxMemoryState;

PLATFORM_ALLOCATE_MEMORY(LinuxAllocateMemory)
{
    Assert(sizeof(linux_memory_block) == 64);
    
    umm PageSize = 4096; //TODO: Not really always correct?
    umm TotalSize = Size + sizeof(linux_memory_block);
    umm BaseOffset = sizeof(linux_memory_block);
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
    
    //TODO: Linux
    //linux_memory_block* Block = (linux_memory_block*)mmap(0, TotalSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, 0, 0);
    linux_memory_block* Block = (linux_memory_block*)calloc(1, TotalSize);
    
    Assert(Block);
    Block->Block.Base = (u8*)Block + BaseOffset;
    Assert(Block->Block.Used == 0);
    Assert(Block->Block.Prev == 0);
    
    if(Flags & (PM_UnderflowCheck | PM_OverflowCheck))
    {
        //TODO Linux types and function
        // i32 Protected = mprotect((u8*)Block + ProtectOffset, PageSize, PROT_NONE);
        // Assert(Protected);
    }
    
    Block->Block.Size = Size;
    Block->Block.Flags = Flags;
    
    platform_memory_block* PlatBlock = &Block->Block;
    
    if(Flags & PM_Temporary)
    {
        Assert((LinuxMemoryState.TempCount + 1) < MAX_TEMP_BLOCKS);
        LinuxMemoryState.TempSizeAllocated += TotalSize;
        LinuxMemoryState.Blocks[LinuxMemoryState.TempCount++] = PlatBlock;
    }
    else
    {
        LinuxMemoryState.PermanentBlocks++;
        LinuxMemoryState.PermanentSizeAllocated += TotalSize;
    }
    
    return PlatBlock;
}

PLATFORM_DEALLOCATE_MEMORY(LinuxDeallocateMemory)
{
    if(Block)
    {
        if((Block->Flags & PM_Temporary) == 0)
        {
            LinuxMemoryState.PermanentBlocks--;
            LinuxMemoryState.PermanentSizeAllocated -= (Block->Size + sizeof(linux_memory_block));
        }
        
        linux_memory_block *LinuxBlock =  ((linux_memory_block*)Block);
        //TODO: Linux
        //munmap(LinuxBlock, LinuxBlock->Block.Size);
        free(LinuxBlock);
    }
}

static void ClearTempMemory()
{
    for(i32 Temp = 0; Temp < LinuxMemoryState.TempCount; Temp++)
    {
        LinuxDeallocateMemory(LinuxMemoryState.Blocks[Temp]);
    }
    
    LinuxMemoryState.TempCount = 0;
    LinuxMemoryState.TempSizeAllocated = 0;
}

int main(int Argc, char** Args)
{
    game_memory GameMemory = {};
    
    GameMemory.ShouldReload = true;
    
    GameMemory.ExitGame = false;
    
    //GameMemory.PlatformAPI.GetAllFilesWithExtension = LinuxFindFilesWithExtensions;
    //GameMemory.PlatformAPI.FileExists = LinuxFileExists;
    GameMemory.PlatformAPI.AllocateMemory = LinuxAllocateMemory;
    GameMemory.PlatformAPI.DeallocateMemory = LinuxDeallocateMemory;
    Platform = GameMemory.PlatformAPI;
    
    linux_state* LinuxState = BootstrapPushStruct(linux_state, PermArena);
    
    char* LibPath = "libgame.so";
    char* TempLibPath = "libgame_temp.so";
    
    memory_arena DebugArena = {};
    
    GameMemory.DebugState = PushStruct(&LinuxState->PermArena, debug_state);
    
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectOrigin = math::v2(50, 780);
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectSize = math::v2(300,0);
    
    config_data ConfigData;
    LoadConfig("../.config", &ConfigData, &LinuxState->PermArena);
    
    GameMemory.ConfigData = ConfigData;
    
    InitKeys();
    render_state RenderState = {};
    renderer Renderer = {};
    
    InitializeOpenGL(RenderState, Renderer, &ConfigData, &LinuxState->PermArena);
    
    game_code Game = LoadGameCode(LibPath, TempLibPath);
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager, LibPath);
    
    u32 FrameCounterForAssetCheck = 0;
    
    sound_device SoundDevice = {};
    oal_devices_list DevicesList = {};
    InitAudio(&SoundDevice, &DevicesList);
    
    sound_commands SoundCommands = {};
    
    if (SoundDevice.IsInitialized)
    {
        ResetCommands(&SoundCommands);
        SoundCommands.SFXVolume = ConfigData.SFXVolume;
        SoundCommands.MusicVolume = ConfigData.MusicVolume;
        SoundCommands.Muted = ConfigData.Muted;
        SoundDevice.PrevMuted = !ConfigData.Muted;
        SoundDevice.PrevStopped = false;
        SoundDevice.PrevPaused = false;
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
        
        ReloadAssets(RenderState, &AssetManager, &LinuxState->PermArena);
        
        ReloadLibs(&Game, LibPath, TempLibPath);
        
        Game.Update(DeltaTime, &GameMemory, Renderer, &InputController, &SoundCommands);
        
        Render(RenderState, Renderer, &LinuxState->PermArena);
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
        
        
#if GLITCH_DEBUG
        Clear(&DebugArena);
        GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount = 0;
        debug_info TempDebugInfo = {};
        TempDebugInfo.Header = PushString(&DebugArena, "Temporary memory");
        AddDebugValue(&DebugArena, &TempDebugInfo, "Blocks", LinuxMemoryState.TempCount);
        AddDebugValue(&DebugArena, &TempDebugInfo, "Total allocated", LinuxMemoryState.TempSizeAllocated);
        
        auto DebugMemoryInfo = GameMemory.DebugState->DebugMemoryInfo;
        
        GameMemory.DebugState->DebugMemoryInfo.DebugInfo[GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount++] = TempDebugInfo;
        
        
        debug_info PermDebugInfo = {};
        PermDebugInfo.Header = PushString(&DebugArena, "Permanent memory");
        AddDebugValue(&DebugArena, &PermDebugInfo, "Blocks", LinuxMemoryState.PermanentBlocks);
        AddDebugValue(&DebugArena, &PermDebugInfo, "Total allocated", LinuxMemoryState.PermanentSizeAllocated);
        
        GameMemory.DebugState->DebugMemoryInfo.DebugInfo[GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount++] = PermDebugInfo;
        
        debug_info TotalDebugInfo = {};
        TotalDebugInfo.Header = PushString(&DebugArena, "Total memory");
        AddDebugValue(&DebugArena, &TotalDebugInfo, "Blocks", LinuxMemoryState.TempCount + LinuxMemoryState.PermanentBlocks);
        AddDebugValue(&DebugArena, &TotalDebugInfo, "Total allocated", LinuxMemoryState.TempSizeAllocated + LinuxMemoryState.PermanentSizeAllocated);
        
        GameMemory.DebugState->DebugMemoryInfo.DebugInfo[GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount++] = TotalDebugInfo;
#endif
        ClearTempMemory();
    }
    
    CleanupSound(&SoundDevice);
    CloseWindow(RenderState);
    //_CrtDumpMemoryLeaks();
    //_CrtMemState MemState;
    //_CrtMemDumpStatistics(&MemState);
}
