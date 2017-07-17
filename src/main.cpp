#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h>  

#include <glad/glad.h>
#include "al.h"
#include "alc.h"
#include "game.h"
#include "platform_sound.h"
#include "platform_sound.cpp"
#include "filehandling.h"
#define KEY_INIT
#include "keycontroller.cpp"
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
        printf("Invalid\n");
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
        printf("RELOAD\n");
        ReloadGameCode(Game);
    }
}

int main(void)
{
    printf("Initializing gamestate\n");
    
    game_state GameState = {};
    GameState.ShouldReload = true;
    GameState.InputController = {};
    
    config_data ConfigData;
    LoadConfig("../assets/.config", &ConfigData);
    
    render_state RenderState;
    InitializeOpenGL(&GameState, &RenderState, &ConfigData);
    LoadTilesheetTextures(&GameState, &RenderState);
    GameState.DeathScreenTimer.TimerMax = 1.0f;
    GameState.DeathScreenTimer.TimerHandle = -1;
    
    GameState.RenderState = RenderState;
    GameState.LevelPath = ConfigData.StartingLevelFilePath;
    GameState.InitialZoom = ConfigData.Zoom;
    game_code Game = LoadGameCode();
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager);
    u32 FrameCounterForAssetCheck = 0;
    
    GameState.Console = {};
    
    sound_device SoundDevice = {};
    InitAudio(&SoundDevice);
    
    if (SoundDevice.IsInitialized)
    {
        sound_manager SoundManager = {};
        SoundManager.Muted = ConfigData.Muted;
        LoadSounds(&SoundManager,&SoundDevice);
        ResetSoundQueue(&SoundManager);
        GameState.SoundManager = SoundManager;
    }
    
    r64 LastFrame = GetTime();
    r64 CurrentFrame = 0.0;
    r64 DeltaTime;
    
    while (!ShouldCloseWindow(&GameState.RenderState) && !GameState.RenderState.ShouldClose)
    {
        //calculate deltatime
        CurrentFrame = GetTime();
        DeltaTime = Min(CurrentFrame - LastFrame, 0.1);
        LastFrame = CurrentFrame;
        r64 FPS = 1.0/DeltaTime;
        GameState.RenderState.FPS = FPS;
        
        if(GameState.GameMode == Mode_Exit || GetKeyDown(Key_Q, &GameState) && GetKey(Key_LeftCtrl, &GameState))
            glfwSetWindowShouldClose(GameState.RenderState.Window, GLFW_TRUE);
        
        ReloadAssets(&AssetManager, &GameState);
        GameState.ReloadData = &AssetManager.ReloadData;
        ReloadDlls(&Game);
        
        Game.Update(DeltaTime, &GameState);
        
        CheckLevelVAO(&GameState);
        Render(&GameState);
        PlaySounds(&GameState,&SoundDevice);
        
        SetControllerInvalidKeys(&GameState.InputController);
        SetInvalidKeys(&GameState.InputController);
        SetMouseInvalidKeys(&GameState.InputController);
        
        PollEvents();
        
        if(ControllerPresent(&GameState))
        {
            ControllerKeys(&GameState,GLFW_JOYSTICK_1);
        }
        GetActionButtonsForQueue(&GameState);
        
        FrameCounterForAssetCheck++;
        if(FrameCounterForAssetCheck == 10)
        {
            ListenToFileChanges(&AssetManager);
            FrameCounterForAssetCheck = 0;
        }
    }
    
    CleanupSound(&SoundDevice,&GameState.SoundManager);
    CloseWindow(&GameState);
    _CrtDumpMemoryLeaks();
}
