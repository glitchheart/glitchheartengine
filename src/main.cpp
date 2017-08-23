//#define _CRTDBG_MAP_ALLOC  

//#include <stdlib.h>  
//#include <crtdbg.h>  

#define DEBUG
//#include "windows.h"
#include <glad/glad.h>
#include "al.h"
#include "alc.h"
#include <windows.h>
#include <sys/types.h>  
#include <sys/stat.h>  

#include <GLFW/glfw3.h>

#include "main.h"

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

#ifdef GLM
void PrintGLMMatrix(glm::mat4 In)
{
    DEBUG_PRINT("GLM: \n");
    DEBUG_PRINT("%f %f %f %f\n", In[0][0],In[1][0],In[2][0],In[3][0]);
    DEBUG_PRINT("%f %f %f %f\n", In[0][1],In[1][1],In[2][1],In[3][1]);
    DEBUG_PRINT("%f %f %f %f\n", In[0][2],In[1][2],In[2][2],In[3][2]);
    DEBUG_PRINT("%f %f %f %f\n", In[0][3],In[1][3],In[2][3],In[3][3]);
}
#endif

PLATFORM_ALLOCATE_MEMORY(Win32AllocateMemory)
{
    void* Result = VirtualAlloc(0, Size, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    return Result;
}

PLATFORM_DEALLOCATE_MEMORY(Win32DeallocateMemory)
{
    if(Memory)
    {
        VirtualFree(Memory, 0, MEM_RELEASE);
    }
}

int main(void)
{
    InitKeys();
    
    win32_memory Win32Memory;
    Win32Memory.PermanentStorageSize = Megabytes(128);
    Win32Memory.PermanentStorage = VirtualAlloc(0, Win32Memory.PermanentStorageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    
    Win32Memory.TemporaryStorageSize = Megabytes(256);
    Win32Memory.TemporaryStorage = VirtualAlloc(0, Win32Memory.TemporaryStorageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    
    win32_state Win32State;
    InitializeArena(&Win32State.PermArena, Win32Memory.PermanentStorageSize, (u8*)Win32Memory.PermanentStorage);
    InitializeArena(&Win32State.TempArena, Win32Memory.TemporaryStorageSize, (u8*)Win32Memory.TemporaryStorage);
    
    LPVOID BaseAddress = 0;
    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = Megabytes(128);
    u64 TotalSize = GameMemory.PermanentStorageSize;
    GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, (size_t)TotalSize,
                                               MEM_RESERVE|MEM_COMMIT,
                                               PAGE_READWRITE);
    GameMemory.TemporaryStorageSize = Megabytes(128);
    GameMemory.TemporaryStorage = VirtualAlloc(0, GameMemory.TemporaryStorageSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    
    config_data ConfigData;
    LoadConfig("../assets/.config", &ConfigData, &Win32State.PermArena);
    
    GameMemory.ShouldReload = true;
    GameMemory.ConfigData = ConfigData;
    GameMemory.ExitGame = false;
    
    GameMemory.PlatformAPI.GetAllFilesWithExtension = Win32FindFilesWithExtensions;
    GameMemory.PlatformAPI.FileExists = Win32FileExists;
    GameMemory.PlatformAPI.AllocateMemory = Win32AllocateMemory;
    GameMemory.PlatformAPI.DeallocateMemory = Win32DeallocateMemory;
    
    render_state RenderState;
    renderer Renderer = {};
    
    InitializeOpenGL(RenderState, Renderer, &ConfigData, &Win32State.PermArena);
    
    game_code Game = LoadGameCode();
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager, &Win32State.TempArena);
    
    u32 FrameCounterForAssetCheck = 0;
    
    sound_device SoundDevice = {};
    InitAudio(&SoundDevice);
    
    sound_queue SoundQueue = {};
    sound_effects SoundEffects = {};
    if (SoundDevice.IsInitialized)
    {
        LoadSounds(&SoundEffects, &SoundDevice, &Win32State.TempArena);
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
        r64 FPS = 1.0/DeltaTime;
        
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
        
        ReloadAssets(RenderState, &AssetManager, &Win32State.TempArena);
        GameMemory.ReloadData = &AssetManager.ReloadData;
        ReloadDlls(&Game);
        
        game_update_return GameUpdateStruct = {};
        Game.Update(DeltaTime, &GameMemory, Renderer, &InputController, &SoundQueue, &GameUpdateStruct, &SoundEffects);
        
        //CheckLevelVAO(&GameMemory);
        
        
        
        Render(RenderState, Renderer, &Win32State.TempArena);
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
            ListenToFileChanges(&AssetManager, &Win32State.TempArena);
            FrameCounterForAssetCheck = 0;
        }
        
        Reset(&Win32State.TempArena);
    }
    
    CleanupSound(&SoundDevice);
    CloseWindow(RenderState);
    //_CrtDumpMemoryLeaks();
}
