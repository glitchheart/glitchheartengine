//#define _CRTDBG_MAP_ALLOC  
//#define _DEBUG

//#include <stdlib.h>  
//#include <crtdbg.h>  

#define DEBUG
//#include "windows.h"

#include "platform.h"
#include "shared.h"

#include <glad/glad.h>
#include "al.h"
#include "alc.h"
#include <windows.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <GLFW/glfw3.h>

#include "main.h"

platform_api Platform;

#include "gmap.cpp"
#include "keycontroller.h"

#include "console.h"
#include "opengl_rendering.h"
#include "keycontroller.cpp"
#include "sound.h"
#include "platform_sound.h"
#include "platform_sound.cpp"
#include "filehandling.h"

input_controller InputController;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"

struct game_code
{
    HMODULE GameCodeDLL;
    FILETIME LastDllWriteTime;
    update *Update;
    
    b32 IsValid;
    const char *DllPath = "game.dll";
    const char *TempDllPath = "game_temp.dll";
};

static game_code LoadGameCode()
{
    game_code Result = {};
    
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

static void ReloadGameCode(game_code *GameCode)
{
    UnloadGameCode(GameCode);
    *GameCode = LoadGameCode();
}

static void ReloadDlls(game_code *Game)
{
    FILETIME LastWriteTime = GetLastWriteTime(Game->DllPath);
    
    if (CompareFileTime(&Game->LastDllWriteTime, &LastWriteTime) != 0)
    {
        DEBUG_PRINT("RELOAD\n");
        ReloadGameCode(Game);
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

PLATFORM_ALLOCATE_MEMORY(Win32AllocateMemory)
{
    umm TotalSize = Size + sizeof(win32_memory_block);
    umm BaseOffset = sizeof(win32_memory_block);
    
    
    win32_memory_block* Block  = (win32_memory_block*)VirtualAlloc(0, TotalSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    Assert(Block);
    Block->Block.Base = (u8*)Block + BaseOffset;
    Assert(Block->Block.Used == 0);
    Assert(Block->Block.Prev == 0);
    
    Block->Block.Size = Size;
    //Block->Block.Flags = Flags;
    
    platform_memory_block* PlatBlock =&Block->Block;
    
    return PlatBlock;
}

PLATFORM_DEALLOCATE_MEMORY(Win32DeallocateMemory)
{
    if(Block)
    {
        win32_memory_block *Win32Block =((win32_memory_block*)Block);
        VirtualFree(Block, 0, MEM_RELEASE);
    }
}

int main(void)
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
    
    config_data ConfigData;
    LoadConfig("../assets/.config", &ConfigData, &Win32State->PermArena);
    
    GameMemory.ConfigData = ConfigData;
    
    InitKeys();
    render_state RenderState;
    renderer Renderer = {};
    
    InitializeOpenGL(RenderState, Renderer, &ConfigData, &Win32State->PermArena);
    
    game_code Game = LoadGameCode();
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager, &Win32State->TempArena);
    
    u32 FrameCounterForAssetCheck = 0;
    
    sound_device SoundDevice = {};
    InitAudio(&SoundDevice);
    
    sound_queue SoundQueue = {};
    sound_effects SoundEffects = {};
    if (SoundDevice.IsInitialized)
    {
        LoadSounds(&SoundEffects, &SoundDevice, &Win32State->TempArena);
        ResetSoundQueue(&SoundQueue);
        SoundDevice.SFXVolume = ConfigData.SFXVolume;
        SoundDevice.MusicVolume = ConfigData.MusicVolume;
        SoundDevice.Muted = ConfigData.Muted;
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
        
        if(GameMemory.IsInitialized && GameMemory.ExitGame)
        {
            DEBUG_PRINT("Quit\n");
            glfwSetWindowShouldClose(RenderState.Window, GLFW_TRUE);
        }
        
        if(GetKeyDown(Key_F3, &InputController))
        {
            SoundDevice.PrevMuted = SoundDevice.Muted;
            SoundDevice.Muted = !SoundDevice.Muted;
        }
        
        if(GetKeyDown(Key_F5, &InputController))
        {
            SoundDevice.PrevStopped = SoundDevice.Stopped;
            SoundDevice.Stopped = !SoundDevice.Stopped;
        }
        
        if(GetKeyDown(Key_F6, &InputController))
        {
            SoundDevice.PrevPaused = SoundDevice.Paused;
            SoundDevice.Paused = !SoundDevice.Paused;
        }
        
        ReloadAssets(RenderState, &AssetManager, &Win32State->TempArena);
        GameMemory.ReloadData = &AssetManager.ReloadData;
        ReloadDlls(&Game);
        
        game_update_return GameUpdateStruct = {};
        Game.Update(DeltaTime, &GameMemory, Renderer, &InputController, &SoundQueue, &GameUpdateStruct, &SoundEffects);
        
        //CheckLevelVAO(&GameMemory);
        
        Render(RenderState, Renderer, &Win32State->TempArena);
        PlaySounds(&SoundDevice, &SoundQueue, GameUpdateStruct.EntityPositions, GameUpdateStruct.EntityCount);
        
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
            ListenToFileChanges(&AssetManager, &Win32State->TempArena);
            FrameCounterForAssetCheck = 0;
        }
        
        // NOTE(Niels): Do this for game transient state as well?
        // Or maybe Game itself should do this.. Hmmmm
        Clear(&Win32State->TempArena);
    }
    
    CleanupSound(&SoundDevice);
    CloseWindow(RenderState);
    //_CrtDumpMemoryLeaks();
    //_CrtMemState MemState;
    //_CrtMemDumpStatistics(&MemState);
}
