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
PlatformApi platform;
struct LogState log_state;
MemoryState memory_state;
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

InputController input_controller;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"
#include "vulkan_rendering.cpp"

static void load_game_code(GameCode& game_code, char* game_library_path, char* temp_game_library_path)
{
    if(!copy_file(game_library_path, temp_game_library_path, false)) return;
    
    game_code.update = UpdateStub;
    game_code.last_library_write_time = get_last_write_time(game_library_path);
    game_code.game_code_library = platform.load_dynamic_library(temp_game_library_path);
    
    if (game_code.game_code_library)
    {
        game_code.update = (update *)platform.load_symbol(game_code.game_code_library, "Update");
        game_code.is_valid = game_code.update != 0;
    }
    
    if (!game_code.is_valid)
    {
        Debug("Invalid game code\n");
        game_code.update = UpdateStub;
    }
}

static void unload_game_code(GameCode *game_code)
{
    if (game_code->game_code_library)
    {
        platform.free_dynamic_library(game_code->game_code_library);
        game_code->game_code_library = 0;
    }
    
    game_code->is_valid = false;
    game_code->update = UpdateStub;
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
        if(difftime(Game->last_library_write_time, last_write_time) != 0)
        {
            reload_game_code(Game, game_library_path, temp_game_library_path);
            Assert(Game);
            Debug("Reloaded game library\n");
        }
    }
}

static void set_invalid_keys()
{
    input_controller.any_key_pressed = false;
    for(u32 key_code = 0; key_code < NUM_KEYS; key_code++)
    {
        if(input_controller.keys_just_pressed[key_code] == KEY_JUST_PRESSED)
        {
            input_controller.keys_just_pressed[key_code] = KEY_INVALID;
        }
        input_controller.keys_up[key_code] = false;
    } 
}

static void set_controller_invalid_keys()
{
    input_controller.any_key_pressed = false;
    for(u32 key_code = 0; key_code < NUM_JOYSTICK_KEYS; key_code++)
    {
        if(input_controller.joystick_keys_just_pressed[key_code] == KEY_JUST_PRESSED)
        {
            input_controller.joystick_keys_just_pressed[key_code] = KEY_INVALID;
        }
    }
}

static void set_mouse_invalid_keys()
{
    input_controller.any_key_pressed = false;
    for(u32 key_code = 0; key_code < NUM_MOUSE_BUTTONS; key_code++)
    {
        if(input_controller.mouse_button_just_pressed[key_code] == KEY_JUST_PRESSED)
        {
            input_controller.mouse_button_just_pressed[key_code] = KEY_INVALID;
        }
        input_controller.mouse_buttons_up[key_code] = false;
    }
    input_controller.scroll_x = 0;
    input_controller.scroll_y = 0;
}


inline void load_config(const char* file_path, ConfigData* config_data, MemoryArena* perm_arena)
{
    FILE* file;
    file = fopen(file_path, "r");
    char line_buffer[255];
    
    *config_data = {};
    
    config_data->title = push_string(perm_arena, 40);
    config_data->version = push_string(perm_arena, 40);
    
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
                
                sprintf(config_data->title, "%s", title_buffer);
            }
            else if(starts_with(line_buffer, "version"))
            {
                sscanf(line_buffer, "version %s", config_data->version);
            }
            else if(starts_with(line_buffer, "graphics_api"))
            {
                char api_string[32];
                sscanf(line_buffer, "graphics_api %s", api_string);
                
                if(strcmp(api_string, "opengl") == 0)
                {
                    config_data->graphics_api = GRAPHICS_OPEN_GL;
                }
                else if(strcmp(api_string, "vulkan") == 0)
                {
                    config_data->graphics_api = GRAPHICS_VULKAN;
                }
            }
            else if(starts_with(line_buffer, "screen_width"))
            {
                sscanf(line_buffer, "screen_width %d", &config_data->screen_width);
            }
            else if(starts_with(line_buffer, "screen_height"))
            {
                sscanf(line_buffer, "screen_height %d", &config_data->screen_height);
            }
            else if(starts_with(line_buffer, "scale_from_width"))
            {
                sscanf(line_buffer, "scale_from_width %d", &config_data->scale_from_width);
            }
            else if(starts_with(line_buffer, "scale_from_height"))
            {
                sscanf(line_buffer, "scale_from_height %d", &config_data->scale_from_height);
            }
            else if(starts_with(line_buffer, "contrast"))
            {
                sscanf(line_buffer, "contrast %f", &config_data->contrast);
            }
            else if(starts_with(line_buffer, "brightness"))
            {
                sscanf(line_buffer, "brightness %f", &config_data->brightness);
            }
            else if(starts_with(line_buffer, "fullscreen"))
            {
                sscanf(line_buffer, "fullscreen %d", &config_data->fullscreen);
            } 
            else if(starts_with(line_buffer, "muted"))
            {
                sscanf(line_buffer, "muted %d", &config_data->muted);
            }
            else if(starts_with(line_buffer, "sfx_volume"))
            {
                sscanf(line_buffer, "sfx_volume %f", &config_data->sfx_volume);
            }
            else if(starts_with(line_buffer, "music_volume"))
            {
                sscanf(line_buffer, "music_volume %f", &config_data->music_volume);
            }
            else if(starts_with(line_buffer, "zoom"))
            {
                sscanf(line_buffer, "zoom %f", &config_data->zoom);
            }
            else if(starts_with(line_buffer, "skipsplashscreen"))
            {
                sscanf(line_buffer, "skipsplashscreen %d", &config_data->skip_splash_screen);
            }
        }
        
        fclose(file);
    }
}

int main(int argc, char** args)
{
    GameMemory game_memory = {};
    
    game_memory.should_reload = true;
    
    game_memory.exit_game = false;
    
    init_platform(game_memory.platform_api);
    
    platform = game_memory.platform_api;
    
    PlatformState* platform_state = bootstrap_push_struct(PlatformState, perm_arena);
    
    log_state = game_memory.log_state;
    init_log(L_FLAG_FILE, concat("../log_", "", &platform_state->perm_arena));
    
#ifdef __APPLE__
    char* game_library_path = "libgame.dylib";
    char* temp_game_library_path = "libgame_temp.dylib";
#elif _WIN32
    char* game_library_path = "game.dll";
    char* temp_game_library_path = "game_temp.dll";
#else
    char* game_library_path = "libgame.so";
    char* temp_game_library_path = "libgame_temp.so";
#endif
    
    MemoryArena debug_arena = {};
    
    game_memory.debug_state = push_struct(&platform_state->perm_arena, DebugState);
    
    game_memory.debug_state->debug_memory_info.debug_rect.rect_origin = math::Vec2(50, 780);
    game_memory.debug_state->debug_memory_info.debug_rect.rect_size = math::Vec2(300,0);
    
    ConfigData config_data;
    load_config("../.config", &config_data, &platform_state->perm_arena);
    
    game_memory.config_data = config_data;
    
    init_keys();
    RenderState render_state = {};
    render_state.arena = {};
    Renderer renderer = {};
    renderer.pixels_per_unit = 8;
    renderer.frame_lock = 0;
    render_state.frame_delta = 0.0;
    
    renderer.animation_controllers = push_array(&renderer.animation_arena, 64, AnimationController);
    renderer.commands.minimum_block_size = sizeof(RenderCommand) * MAX_RENDER_COMMANDS;
    renderer.ui_commands.minimum_block_size = sizeof(RenderCommand) * MAX_UI_COMMANDS;
    renderer.light_commands.minimum_block_size = sizeof(RenderCommand) * MAX_LIGHT_COMMANDS;
    renderer.spritesheet_animation_count = 0;
    renderer.animation_controller_count = 0;
    
    if(config_data.graphics_api == GRAPHICS_VULKAN)
    {
#if __LINUX || _WIN32
        VkRenderState vk_render_state;
        initialize_vulkan(vk_render_state, renderer, config_data);
        vk_render(vk_render_state, renderer);
#endif
    }
    else if(config_data.graphics_api == GRAPHICS_OPEN_GL)
    {
        initialize_open_gl(render_state, renderer, &config_data, &platform_state->perm_arena);
    }
    
    GameCode game = {};
    game.is_valid = false;
    load_game_code(game, game_library_path, temp_game_library_path);
    TimerController timer_controller;
    timer_controller.timer_count = 0;
    
    //setup asset reloading
    AssetManager asset_manager = {};
    startup_file_time_checks(&asset_manager, game_library_path);
    
    u32 frame_counter_for_asset_check = 0;
    
    SoundDevice sound_device = {};
    init_audio_fmod(&sound_device);
    
    SoundCommands sound_commands = {};
    sound_commands.sound_arena.minimum_block_size = sizeof(SoundEffect) * MAX_SOUND_EFFECTS;
    
    if (sound_device.is_initialized)
    {
        reset_commands(&sound_commands);
        sound_commands.sfx_volume = config_data.sfx_volume;
        sound_commands.music_volume = config_data.music_volume;
        sound_commands.muted = config_data.muted;
    }
    
    r64 last_frame = get_time();
    r64 current_frame = 0.0;
    r64 delta_time;
    renderer.frame_lock = 0;
    
    while (!should_close_window(render_state) && !renderer.should_close)
    {
        //calculate deltatime
        current_frame = get_time();
        delta_time = Min(current_frame - last_frame, 0.1);
        last_frame = current_frame;
        
        if(game_memory.exit_game)
        {
            Debug("Quit\n");
            glfwSetWindowShouldClose(render_state.window, GLFW_TRUE);
        }
        
        show_mouse_cursor(render_state, renderer.show_mouse_cursor);
        
        reload_assets(render_state, &asset_manager, &platform_state->perm_arena);
        
        reload_libraries(&game, game_library_path, temp_game_library_path);
        
        game.update(delta_time, &game_memory, renderer, &input_controller, &sound_commands, timer_controller);
        
        tick_animation_controllers(renderer, &sound_commands, &input_controller, timer_controller, delta_time);
        tick_timers(timer_controller, delta_time);
        
        play_sounds(&sound_device, &sound_commands);
        
        render(render_state, renderer, &platform_state->perm_arena, delta_time);
        
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
