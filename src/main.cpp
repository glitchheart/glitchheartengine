#define DEBUG

#include "shared.h"

#if GLITCH_DEBUG
#include "debug.h"
#endif

#include <glad/glad.h>
#include "fmod.h"
#include "fmod_errors.h"

#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>
#include <GLFW/glfw3.h>
#include "time.h"

#include "main.h"

// Global
platform_api Platform;
log_state LogState;
memory_state MemoryState;
// Global

#ifdef _WIN32
#include "win32_platform.cpp"
#elif __linux
#include "linux_platform.cpp"
#elif __APPLE__
#include "osx_platform.cpp"
#endif

#include "gmap.cpp"
#include "keycontroller.h"

#include "opengl_rendering.h"
#include "keycontroller.cpp"
#include "sound.h"
#include "timers.h"
#include "fmod_sound.h"
#include "fmod_sound.cpp"
#include "filehandling.h"

input_controller InputController;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"

static void LoadGameCode(game_code& GameCode, char* GameLibraryPath, char* TempGameLibraryPath)
{
    if(!CopyFile(GameLibraryPath, TempGameLibraryPath, false)) return;
    
    GameCode.Update = UpdateStub;
    GameCode.LastLibraryWriteTime = GetLastWriteTime(GameLibraryPath);
    GameCode.GameCodeLibrary = Platform.LoadDynamicLibrary(TempGameLibraryPath);
    
    if (GameCode.GameCodeLibrary)
    {
        GameCode.Update = (update *)Platform.LoadSymbol(GameCode.GameCodeLibrary, "Update");
        GameCode.IsValid = GameCode.Update != 0;
    }
    
    if (!GameCode.IsValid)
    {
        DEBUG_PRINT("Invalid game code\n");
        GameCode.Update = UpdateStub;
    }
}

static void UnloadGameCode(game_code *GameCode)
{
    if (GameCode->GameCodeLibrary)
    {
        Platform.FreeDynamicLibrary(GameCode->GameCodeLibrary);
        GameCode->GameCodeLibrary = 0;
    }
    
    GameCode->IsValid = false;
    GameCode->Update = UpdateStub;
}

static void ReloadGameCode(game_code *GameCode, char* GameLibraryPath, char* TempGameLibraryPath)
{
    UnloadGameCode(GameCode);
    //Sleep(100);
    LoadGameCode(*GameCode, GameLibraryPath, TempGameLibraryPath);
}

static void ReloadLibraries(game_code *Game, char* GameLibraryPath, char* TempGameLibraryPath)
{
    // @Bug: Not working on Mac
    time_t LastWriteTime = GetLastWriteTime(GameLibraryPath);
    
    if(LastWriteTime != 0)
    {
        if(difftime(Game->LastLibraryWriteTime, LastWriteTime) != 0)
        {
            ReloadGameCode(Game, GameLibraryPath, TempGameLibraryPath);
            Assert(Game);
            DEBUG_PRINT("Reloaded game library\n");
        }
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
            else if(StartsWith(LineBuffer, "skipsplashscreen"))
            {
                sscanf(LineBuffer, "skipsplashscreen %d", &ConfigData->SkipSplashScreen);
            }
        }
        
        fclose(File);
    }
}

int main(int Argc, char** Args)
{
    game_memory GameMemory = {};
    
    GameMemory.ShouldReload = true;
    
    GameMemory.ExitGame = false;
    
    InitPlatform(GameMemory.PlatformAPI);
    
    Platform = GameMemory.PlatformAPI;
    
    platform_state* PlatformState = BootstrapPushStruct(platform_state, PermArena);
    
    LogState = GameMemory.LogState;
    InitLog(LFlag_File | LFlag_Debug, Concat("../log_", "", &PlatformState->PermArena));
    
#ifdef __APPLE__
    char* GameLibraryPath = "libgame.dylib";
    char* TempGameLibraryPath = "libgame_temp.dylib";
#elif _WIN32
    char* GameLibraryPath = "game.dll";
    char* TempGameLibraryPath = "game_temp.dll";
#else
    char* GameLibraryPath = "libgame.so";
    char* TempGameLibraryPath = "libgame_temp.so";
#endif
    
    memory_arena DebugArena = {};
    
    GameMemory.DebugState = PushStruct(&PlatformState->PermArena, debug_state);
    
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectOrigin = math::v2(50, 780);
    GameMemory.DebugState->DebugMemoryInfo.DebugRect.RectSize = math::v2(300,0);
    
    config_data ConfigData;
    LoadConfig("../.config", &ConfigData, &PlatformState->PermArena);
    
    GameMemory.ConfigData = ConfigData;
    
    InitKeys();
    render_state RenderState = {};
    renderer Renderer = {};
    Renderer.PixelsPerUnit = 8;
    Renderer.FrameLock = 0;
    RenderState.FrameDelta = 0.0;
    
    Renderer.Commands.MinimumBlockSize = sizeof(render_command) * MAX_RENDER_COMMANDS;
    Renderer.UICommands.MinimumBlockSize = sizeof(render_command) * MAX_UI_COMMANDS;
    Renderer.LightCommands.MinimumBlockSize = sizeof(render_command) * MAX_LIGHT_COMMANDS;
    Renderer.SpritesheetAnimationCount = 0;
    Renderer.SpritesheetAnimationInfoCount = 0;
    
    InitializeOpenGL(RenderState, Renderer, &ConfigData, &PlatformState->PermArena);
    
    game_code Game = {};
    Game.IsValid = false;
    LoadGameCode(Game, GameLibraryPath, TempGameLibraryPath);
    timer_controller TimerController;
    TimerController.TimerCount = 0;
    
    //setup asset reloading
    asset_manager AssetManager = {};
    StartupFileTimeChecks(&AssetManager, GameLibraryPath);
    
    u32 FrameCounterForAssetCheck = 0;
    
    sound_device SoundDevice = {};
    InitAudio_FMOD(&SoundDevice);
    
    sound_commands SoundCommands = {};
    SoundCommands.SoundArena.MinimumBlockSize = sizeof(sound_effect) * MAX_SOUND_EFFECTS;
    
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
        //calculate deltatime
        CurrentFrame = GetTime();
        DeltaTime = Min(CurrentFrame - LastFrame, 0.1);
        LastFrame = CurrentFrame;
        //Renderer.FPS = 1.0 / DeltaTime;
        //Renderer.CurrentFrame++;
        //Renderer.FPSSum += Renderer.FPS;
        /*
        if(Renderer.CurrentFrame == 60)
        {
            Renderer.CurrentFrame = 0;
            Renderer.AverageFPS = Renderer.FPSSum / 60.0;
            Renderer.FPSSum = 0.0;
        }
        */
        if(GameMemory.ExitGame)
        {
            DEBUG_PRINT("Quit\n");
            glfwSetWindowShouldClose(RenderState.Window, GLFW_TRUE);
        }
        
        ShowMouseCursor(RenderState, Renderer.ShowMouseCursor);
        
        ReloadAssets(RenderState, &AssetManager, &PlatformState->PermArena);
        
        ReloadLibraries(&Game, GameLibraryPath, TempGameLibraryPath);
        
        Game.Update(DeltaTime, &GameMemory, Renderer, &InputController, &SoundCommands, TimerController);
        
        TickTimers(TimerController, DeltaTime);
        PlaySounds(&SoundDevice, &SoundCommands);
        
        Render(RenderState, Renderer, &PlatformState->PermArena, DeltaTime);
        
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
}
