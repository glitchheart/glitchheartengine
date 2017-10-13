//#define _CRTDBG_MAP_ALLOC  
//#define _DEBUG

//#include <stdlib.h>  
//#include <crtdbg.h>  

#define DEBUG
//#include "windows.h"

#include "platform.h"
#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

#include <glad/glad.h>
#include "al.h"
#include "alc.h"
#include <windows.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <GLFW/glfw3.h>

#include "win32_main.h"

#include "win32_load_oal.h"
#include "win32_load_oal.cpp"

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
            else if(StartsWith(LineBuffer, "starting_level_path"))
            {
                ConfigData->StartingLevelFilePath = PushString(PermArena, 40);
                sscanf(LineBuffer, "starting_level_path %s", ConfigData->StartingLevelFilePath);
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

int main(int Argc, char** Args)
{
    game_memory GameMemory = {};
    
    GameMemory.ShouldReload = true;
    
    GameMemory.ExitGame = false;
    
    GameMemory.PlatformAPI.GetAllFilesWithExtension = Win32FindFilesWithExtensions;
    GameMemory.PlatformAPI.FileExists = Win32FileExists;
    GameMemory.PlatformAPI.AllocateMemory = Win32AllocateMemory;
    GameMemory.PlatformAPI.DeallocateMemory = Win32DeallocateMemory;
    Platform = GameMemory.PlatformAPI;
    
    win32_state* Win32State = BootstrapPushStruct(win32_state, PermArena);
    
    char* DllPath = "game.dll";
    char* TempDllPath = "game_temp.dll";
    
    if(Argc == 2)
    {
        DllPath = Concat(Args[1], "_game.dll", &Win32State->PermArena);
        TempDllPath = Concat(Args[1], "_game_temp.dll", &Win32State->PermArena);
    }
    
    memory_arena DebugArena = {};
    
    GameMemory.DebugState = PushStruct(&Win32State->PermArena, debug_state);
    
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectOrigin = math::v2(50, 780);
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectSize = math::v2(300,0);
    
    config_data ConfigData;
    LoadConfig("../assets/.config", &ConfigData, &Win32State->PermArena);
    
    GameMemory.ConfigData = ConfigData;
    
    InitKeys();
    render_state RenderState = {};
    renderer Renderer = {};
    
    InitializeOpenGL(RenderState, Renderer, &ConfigData, &Win32State->PermArena);
    
    game_code Game = LoadGameCode(DllPath, TempDllPath);
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager);
    
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
    
    while (!ShouldCloseWindow(RenderState) && !RenderState.ShouldClose)
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
        GameMemory.ReloadData = &AssetManager.ReloadData;
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
        
        
#if GLITCH_DEBUG
        Clear(&DebugArena);
        GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount = 0;
        debug_info TempDebugInfo = {};
        TempDebugInfo.Header = PushString(&DebugArena, "Temporary memory");
        AddDebugValue(&DebugArena, &TempDebugInfo, "Blocks", Win32MemoryState.TempCount);
        AddDebugValue(&DebugArena, &TempDebugInfo, "Total allocated", Win32MemoryState.TempSizeAllocated);
        
        auto DebugMemoryInfo = GameMemory.DebugState->DebugMemoryInfo;
        
        GameMemory.DebugState->DebugMemoryInfo.DebugInfo[GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount++] = TempDebugInfo;
        
        
        debug_info PermDebugInfo = {};
        PermDebugInfo.Header = PushString(&DebugArena, "Permanent memory");
        AddDebugValue(&DebugArena, &PermDebugInfo, "Blocks", Win32MemoryState.PermanentBlocks);
        AddDebugValue(&DebugArena, &PermDebugInfo, "Total allocated", Win32MemoryState.PermanentSizeAllocated);
        
        GameMemory.DebugState->DebugMemoryInfo.DebugInfo[GameMemory.DebugState->DebugMemoryInfo.DebugInfoCount++] = PermDebugInfo;
        
        debug_info TotalDebugInfo = {};
        TotalDebugInfo.Header = PushString(&DebugArena, "Total memory");
        AddDebugValue(&DebugArena, &TotalDebugInfo, "Blocks", Win32MemoryState.TempCount + Win32MemoryState.PermanentBlocks);
        AddDebugValue(&DebugArena, &TotalDebugInfo, "Total allocated", Win32MemoryState.TempSizeAllocated + Win32MemoryState.PermanentSizeAllocated);
        
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
