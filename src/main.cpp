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
#include "time.h"

#include "main.h"

// Global
PlatformApi Platform;
LogState LogState;
MemoryState MemoryState;
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
#include "vulkan_rendering.h"
#include "opengl_rendering.h"
#include "animation.cpp"
#include "keycontroller.cpp"
#include "sound.h"
#include "timers.h"
#include "fmod_sound.h"
#include "fmod_sound.cpp"
#include "filehandling.h"

InputController InputController;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"
#include "vulkan_rendering.cpp"

static void load_game_code(GameCode& game_code, char* game_library_path, char* temp_game_library_path)
{
    if(!copy_file(game_library_path, temp_game_library_path, false)) return;
    
    game_code.Update = UpdateStub;
    game_code.LastLibraryWriteTime = get_last_write_time(game_library_path);
    game_code.GameCodeLibrary = Platform.LoadDynamicLibrary(temp_game_library_path);
    
    if (game_code.GameCodeLibrary)
    {
        game_code.Update = (update *)Platform.LoadSymbol(game_code.GameCodeLibrary, "Update");
        game_code.IsValid = game_code.Update != 0;
    }
    
    if (!game_code.IsValid)
    {
        Debug("Invalid game code\n");
        game_code.Update = UpdateStub;
    }
}

static void unload_game_code(GameCode *game_code)
{
    if (game_code->GameCodeLibrary)
    {
        Platform.FreeDynamicLibrary(game_code->GameCodeLibrary);
        game_code->GameCodeLibrary = 0;
    }
    
    game_code->IsValid = false;
    game_code->Update = UpdateStub;
}

static void reload_game_code(GameCode *game_code, char* game_library_path, char* temp_game_library_path)
{
    unload_game_code(game_code);
    //Sleep(100);
    load_game_code(*game_code, game_library_path, temp_game_library_path);
}

static void reload_libraries(GameCode *Game, char* game_library_path, char* temp_game_library_path)
{
    // @Bug: Not working on Mac
    time_t last_write_time = get_last_write_time(game_library_path);
    
    if(last_write_time != 0)
    {
        if(difftime(Game->LastLibraryWriteTime, last_write_time) != 0)
        {
            reload_game_code(Game, game_library_path, temp_game_library_path);
            Assert(Game);
            Debug("Reloaded game library\n");
        }
    }
}

static void set_invalid_keys()
{
    InputController.AnyKeyPressed = false;
    for(u32 key_code = 0; key_code < NUM_KEYS; key_code++)
    {
        if(InputController.KeysJustPressed[key_code] == Key_JustPressed)
        {
            InputController.KeysJustPressed[key_code] = Key_Invalid;
        }
        InputController.KeysUp[key_code] = false;
    } 
}

static void set_controller_invalid_keys()
{
    InputController.AnyKeyPressed = false;
    for(u32 key_code = 0; key_code < NUM_JOYSTICK_KEYS; key_code++)
    {
        if(InputController.JoystickKeysJustPressed[key_code] == Key_JustPressed)
        {
            InputController.JoystickKeysJustPressed[key_code] = Key_Invalid;
        }
    }
}

static void set_mouse_invalid_keys()
{
    InputController.AnyKeyPressed = false;
    for(u32 key_code = 0; key_code < NUM_MOUSE_BUTTONS; key_code++)
    {
        if(InputController.MouseButtonJustPressed[key_code] == Key_JustPressed)
        {
            InputController.MouseButtonJustPressed[key_code] = Key_Invalid;
        }
        InputController.MouseButtonsUp[key_code] = false;
    }
    InputController.ScrollX = 0;
    InputController.ScrollY = 0;
}


inline void load_config(const char* file_path, ConfigData* config_data, MemoryArena* perm_arena)
{
    FILE* file;
    file = fopen(file_path, "r");
    char line_buffer[255];
    
    *config_data = {};
    
    config_data->Title = push_string(perm_arena, 40);
    config_data->Version = push_string(perm_arena, 40);
    
    if(file)
    {
        while(fgets(line_buffer, 255, file))
        {
            if(starts_with(line_buffer, "title"))
            {
                // @Speed: This can probably be done much better and efficient
                i32 index = 0;
                char title_buffer[50];
                
                b32 after_title = false;
                
                for(i32 buffer_index = 0; buffer_index < (i32)strlen(line_buffer) + 1; buffer_index++)
                {
                    if(after_title)
                    {
                        char character = line_buffer[buffer_index];
                        if(character == '\n' || character == '\r')
                        {
                            break;
                        }
                        else
                        {
                            title_buffer[index++] = character;
                        }
                    }
                    
                    if(line_buffer[buffer_index] == ' ')
                    {
                        after_title = true;
                    }
                }
                title_buffer[index] = '\0';
                
                sprintf(config_data->Title, "%s", title_buffer);
            }
            else if(starts_with(line_buffer, "version"))
            {
                sscanf(line_buffer, "version %s", config_data->Version);
            }
            else if(starts_with(line_buffer, "graphics_api"))
            {
                char api_string[32];
                sscanf(line_buffer, "graphics_api %s", api_string);
                
                if(strcmp(api_string, "opengl") == 0)
                {
                    config_data->GraphicsAPI = Graphics_OpenGl;
                }
                else if(strcmp(api_string, "vulkan") == 0)
                {
                    config_data->GraphicsAPI = Graphics_Vulkan;
                }
            }
            else if(starts_with(line_buffer, "screen_width"))
            {
                sscanf(line_buffer, "screen_width %d", &config_data->ScreenWidth);
            }
            else if(starts_with(line_buffer, "screen_height"))
            {
                sscanf(line_buffer, "screen_height %d", &config_data->ScreenHeight);
            }
            else if(starts_with(line_buffer, "scale_from_width"))
            {
                sscanf(line_buffer, "scale_from_width %d", &config_data->ScaleFromWidth);
            }
            else if(starts_with(line_buffer, "scale_from_height"))
            {
                sscanf(line_buffer, "scale_from_height %d", &config_data->ScaleFromHeight);
            }
            else if(starts_with(line_buffer, "contrast"))
            {
                sscanf(line_buffer, "contrast %f", &config_data->Contrast);
            }
            else if(starts_with(line_buffer, "brightness"))
            {
                sscanf(line_buffer, "brightness %f", &config_data->Brightness);
            }
            else if(starts_with(line_buffer, "fullscreen"))
            {
                sscanf(line_buffer, "fullscreen %d", &config_data->Fullscreen);
            } 
            else if(starts_with(line_buffer, "muted"))
            {
                sscanf(line_buffer, "muted %d", &config_data->Muted);
            }
            else if(starts_with(line_buffer, "sfx_volume"))
            {
                sscanf(line_buffer, "sfx_volume %f", &config_data->SFXVolume);
            }
            else if(starts_with(line_buffer, "music_volume"))
            {
                sscanf(line_buffer, "music_volume %f", &config_data->MusicVolume);
            }
            else if(starts_with(line_buffer, "zoom"))
            {
                sscanf(line_buffer, "zoom %f", &config_data->Zoom);
            }
            else if(starts_with(line_buffer, "skipsplashscreen"))
            {
                sscanf(line_buffer, "skipsplashscreen %d", &config_data->SkipSplashScreen);
            }
        }
        
        fclose(file);
    }
}

int main(int argc, char** args)
{
    game_memory game_memory = {};
    
    game_memory.ShouldReload = true;
    
    game_memory.ExitGame = false;
    
    init_platform(game_memory.PlatformAPI);
    
    Platform = game_memory.PlatformAPI;
    
    PlatformState* platform_state = BootstrapPushStruct(platform_state, PermArena);
    
    LogState = game_memory.LogState;
    init_log(LFlag_File, concat("../log_", "", &platform_state->PermArena));
    
#ifdef __APPLE__
    char* GameLibraryPath = "libgame.dylib";
    char* TempGameLibraryPath = "libgame_temp.dylib";
#elif _WIN32
    char* GameLibraryPath = "game.dll";
    char* TempGameLibraryPath = "game_temp.dll";
#else
    char* game_library_path = "libgame.so";
    char* temp_game_library_path = "libgame_temp.so";
#endif
    
    MemoryArena debug_arena = {};
    
    game_memory.DebugState = PushStruct(&PlatformState->PermArena, debug_state);
    
    game_memory.DebugState->DebugMemoryInfo.DebugRect.RectOrigin = math::v2(50, 780);
    game_memory.DebugState->DebugMemoryInfo.DebugRect.RectSize = math::v2(300,0);
    
    ConfigData config_data;
    load_config("../.config", &config_data, &platform_state->PermArena);
    
    game_memory.ConfigData = config_data;
    
    init_keys();
    RenderState render_state = {};
    render_state.Arena = {};
    Renderer renderer = {};
    Renderer.PixelsPerUnit = 8;
    Renderer.FrameLock = 0;
    render_state.FrameDelta = 0.0;
    
    Renderer.AnimationControllers = PushArray(&Renderer.AnimationArena, 64, animation_controller);
    Renderer.Commands.MinimumBlockSize = sizeof(RenderCommand) * MAX_RENDER_COMMANDS;
    Renderer.UICommands.MinimumBlockSize = sizeof(RenderCommand) * MAX_UI_COMMANDS;
    Renderer.LightCommands.MinimumBlockSize = sizeof(RenderCommand) * MAX_LIGHT_COMMANDS;
    Renderer.SpritesheetAnimationCount = 0;
    Renderer.AnimationControllerCount = 0;
    
    if(config_data.GraphicsAPI == Graphics_Vulkan)
    {
#if __LINUX || _WIN32
        vk_render_state VkRenderState;
        InitializeVulkan(VkRenderState, Renderer, ConfigData);
        VkRender(VkRenderState, Renderer);
#endif
    }
    else if(config_data.GraphicsAPI == Graphics_OpenGl)
    {
        initialize_open_gl(render_state, Renderer, &config_data, &platform_state->PermArena);
    }
    
    GameCode game = {};
    game.IsValid = false;
    load_game_code(game, game_library_path, temp_game_library_path);
    TimerController timer_controller;
    timer_controller.TimerCount = 0;
    
    //setup asset reloading
    AssetManager asset_manager = {};
    startup_file_time_checks(&asset_manager, game_library_path);
    
    u32 frame_counter_for_asset_check = 0;
    
    SoundDevice sound_device = {};
    init_audio_fmod(&sound_device);
    
    SoundCommands sound_commands = {};
    sound_commands.SoundArena.MinimumBlockSize = sizeof(SoundEffect) * MAX_SOUND_EFFECTS;
    
    if (sound_device.IsInitialized)
    {
        reset_commands(&sound_commands);
        sound_commands.SFXVolume = config_data.SFXVolume;
        sound_commands.MusicVolume = config_data.MusicVolume;
        sound_commands.Muted = config_data.Muted;
    }
    
    r64 last_frame = get_time();
    r64 current_frame = 0.0;
    r64 delta_time;
    Renderer.FrameLock = 0;
    
    while (!should_close_window(render_state) && !Renderer.ShouldClose)
    {
        //calculate deltatime
        CurrentFrame = get_time();
        delta_time = Min(CurrentFrame - LastFrame, 0.1);
        LastFrame = CurrentFrame;
        
        if(game_memory.ExitGame)
        {
            Debug("Quit\n");
            glfwSetWindowShouldClose(render_state.Window, GLFW_TRUE);
        }
        
        show_mouse_cursor(render_state, Renderer.ShowMouseCursor);
        
        reload_assets(render_state, &asset_manager, &platform_state->PermArena);
        
        reload_libraries(&game, game_library_path, temp_game_library_path);
        
        game.Update(delta_time, &game_memory, Renderer, &InputController, &sound_commands, timer_controller);
        
        tick_animation_controllers(Renderer, &sound_commands, &InputController, timer_controller, delta_time);
        tick_timers(timer_controller, delta_time);
        
        play_sounds(&sound_device, &sound_commands);
        
        render(render_state, Renderer, &platform_state->PermArena, delta_time);
        
        set_controller_invalid_keys();
        set_invalid_keys();
        set_mouse_invalid_keys();
        
        poll_events();
        
        if(controller_present())
        {
            controller_keys(GLFW_JOYSTICK_1);
        }
        
        frame_counter_for_asset_check++;
        if(frame_counter_for_asset_check == 10)
        {
            listen_to_file_changes(&asset_manager);
            frame_counter_for_asset_check = 0;
        }
        
        update_log();
        clear_temp_memory();
    }
    
    close_log();
    cleanup_sound(&sound_device);
    close_window(render_state);
}
