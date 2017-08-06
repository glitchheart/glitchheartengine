//#define _CRTDBG_MAP_ALLOC  

//#include <stdlib.h>  
//#include <crtdbg.h>  

#define DEBUG
#include <glad/glad.h>
#include "al.h"
#include "alc.h"

#include "game.h"

#include "platform.h"
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

int main(void)
{
    DEBUG_PRINT("Initializing gamestate\n");
    
    InitKeys();
    
    config_data ConfigData;
    LoadConfig("../assets/.config", &ConfigData);
    
    LPVOID BaseAddress = 0;
    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = Megabytes(512);
    u64 TotalSize = GameMemory.PermanentStorageSize;
    GameMemory.PermanentStorage = VirtualAlloc(BaseAddress, (size_t)TotalSize,
                                               MEM_RESERVE|MEM_COMMIT,
                                               PAGE_READWRITE);
    
    //@Incomplete: We really want to get ALL of game state out of main!!!
    game_state* GameState = (game_state*)GameMemory.PermanentStorage;
    GameMemory.ShouldReload = true;
    GameState->ShouldReload = true;
    GameMemory.ConfigData = ConfigData;
    InitializeOpenGL(&GameMemory, &ConfigData);
    
    game_code Game = LoadGameCode();
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager);
    
    u32 FrameCounterForAssetCheck = 0;
    
    sound_device SoundDevice = {};
    InitAudio(&SoundDevice);
    
    sound_queue SoundQueue = {};
    sound_effects SoundEffects = {};
    if (SoundDevice.IsInitialized)
    {
        LoadSounds(&SoundEffects,&SoundDevice);
        ResetSoundQueue(&SoundQueue);
        SoundDevice.SFXVolume = ConfigData.SFXVolume;
        SoundDevice.MusicVolume = ConfigData.MusicVolume;
        SoundDevice.Muted = ConfigData.Muted;
    }
    
    r64 LastFrame = GetTime();
    r64 CurrentFrame = 0.0;
    r64 DeltaTime;
    
    while (!ShouldCloseWindow(&GameState->RenderState) && !GameState->RenderState.ShouldClose)
    {
        //calculate deltatime
        CurrentFrame = GetTime();
        DeltaTime = Min(CurrentFrame - LastFrame, 0.1);
        LastFrame = CurrentFrame;
        r64 FPS = 1.0/DeltaTime;
        
        if(GameMemory.IsInitialized)
        {
            GameState->RenderState.FPS = FPS;
            
            if(GameState->GameMode == Mode_Exit)
            {
                DEBUG_PRINT("Quit\n");
                glfwSetWindowShouldClose(GameState->RenderState.Window, GLFW_TRUE);
            }
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
        
        ReloadAssets(&GameMemory, &AssetManager);
        GameState->ReloadData = &AssetManager.ReloadData;
        ReloadDlls(&Game);
        
        game_update_return GameUpdateStruct = {};Game.Update(DeltaTime, &GameMemory, &InputController, &SoundQueue, &GameUpdateStruct, &SoundEffects);
        
        CheckLevelVAO(&GameMemory);
        Render(&GameMemory);
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
            ListenToFileChanges(&AssetManager);
            FrameCounterForAssetCheck = 0;
        }
    }
    
    CleanupSound(&SoundDevice);
    CloseWindow(&GameState->RenderState);
    //_CrtDumpMemoryLeaks();
}
