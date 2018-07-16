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
struct LogState* log_state;
static MemoryState memory_state;
// Global

#ifdef _WIN32
#include "win32_platform.cpp"
#elif __linux
#include "linux_platform.cpp"
#elif __APPLE__
#include "osx_platform.cpp"
#endif

#include "keycontroller.h"

#if defined(__linux) || defined(_WIN32)
//#include "vulkan_rendering.h"
#endif
#include "particles.cpp"
#include "opengl_rendering.h"
#include "animation.cpp"
#include "keycontroller.cpp"
#include "sound.h"
#include "timers.h"
#include "components.h"
#include "fmod_sound.h"
#include "fmod_sound.cpp"
#include "filehandling.h"

#if defined(__linux) || defined(__APPLE__)
#include "dlfcn.h"
#endif

static InputController input_controller;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"

#if defined(__linux) || defined(_WIN32)
//#include "vulkan_rendering.cpp"
#endif

static void load_game_code(GameCode& game_code, char* game_library_path, char* temp_game_library_path, MemoryArena* arena = nullptr)
{
    if(!copy_file(game_library_path, temp_game_library_path, false, arena)) return;
    
    game_code.update = update_stub;
    game_code.last_library_write_time = get_last_write_time(game_library_path);
    game_code.game_code_library = platform.load_dynamic_library(temp_game_library_path);
    
    if (game_code.game_code_library)
    {
        game_code.update = (Update *)platform.load_symbol(game_code.game_code_library, "update");
        game_code.is_valid = game_code.update != 0;
    }
    else
        debug("The game library file could not be loaded\n");
    
    if (!game_code.is_valid)
    {
        debug("Invalid game code\n");
        
        // ONLY UNIX
#if defined(__linux) || defined(__APPLE__)
        char *err_str;
        err_str = dlerror();
        if(err_str)
        {
            debug(err_str);
        }
#endif
        
        game_code.update = update_stub;
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
    game_code->update = update_stub;
}

static void reload_game_code(GameCode *game_code, char* game_library_path, char* temp_game_library_path, MemoryArena* arena = nullptr)
{
    unload_game_code(game_code);
    //Sleep(100);
    load_game_code(*game_code, game_library_path, temp_game_library_path, arena);
}

static void reload_libraries(GameCode *Game, char* game_library_path, char* temp_game_library_path, MemoryArena* arena = nullptr)
{
    // @Bug: Not working on Mac
    time_t last_write_time = get_last_write_time(game_library_path);
    
    if(last_write_time != 0)
    {
        if(difftime(Game->last_library_write_time, last_write_time) != 0)
        {
            reload_game_code(Game, game_library_path, temp_game_library_path, arena);
            assert(Game);
            debug("Reloaded game library\n");
        }
    }
}

inline void load_config(const char* file_path, ConfigData* config_data, MemoryArena* perm_arena)
{
    FILE* file;
    file = fopen(file_path, "r");
    char line_buffer[255];
    
    *config_data = {};
    
    
    if(file)
    {
        while(fgets(line_buffer, 255, file))
        {
            if(starts_with(line_buffer, "screen_width"))
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
            else if(starts_with(line_buffer, "window_mode"))
            {
                sscanf(line_buffer, "window_mode %d", &config_data->window_mode);
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
            else if(starts_with(line_buffer, "skipsplashscreen"))
            {
                sscanf(line_buffer, "skipsplashscreen %d", &config_data->skip_splash_screen);
            }
        }
        
        fclose(file);
    }
}

static void init_renderer(Renderer &renderer)
{
    renderer.pixels_per_unit = global_pixels_per_unit;
    renderer.frame_lock = 0;
    
    renderer.particle_systems = push_array(&renderer. particle_arena, global_max_particle_systems, ParticleSystemInfo);
    renderer.animation_controllers = push_array(&renderer.animation_arena, 64, AnimationController);
    renderer.spritesheet_animations = push_array(&renderer.animation_arena, global_max_spritesheet_animations, SpritesheetAnimation);
    renderer.commands.minimum_block_size = sizeof(RenderCommand) * global_max_render_commands;
    renderer.ui_commands.minimum_block_size = sizeof(RenderCommand) * global_max_ui_commands;
    renderer.light_commands.minimum_block_size = sizeof(RenderCommand) * global_max_light_commands;
    renderer.buffers = push_array(&renderer.buffer_arena, global_max_custom_buffers, BufferData);
    renderer.updated_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer.texture_data = push_array(&renderer.texture_arena, global_max_textures, TextureData);
    renderer.spritesheet_animation_count = 0;
    renderer.animation_controller_count = 0;
    renderer.meshes = push_array(&renderer.mesh_arena, global_max_meshes, Mesh);
    renderer.shader_data = push_array(&renderer.shader_arena, global_max_shaders, ShaderData);
}

int main(int argc, char** args)
{    
    GameMemory game_memory = {};
    
    game_memory.should_reload = true;
    
    MemoryArena game_temp_arena = {};
    game_memory.temp_arena = &game_temp_arena;
    
    game_memory.exit_game = false;
    
    init_platform(game_memory.platform_api);
    
    platform = game_memory.platform_api;
    
    PlatformState* platform_state = bootstrap_push_struct(PlatformState, perm_arena);
    game_memory.log_state = push_struct(&platform_state->perm_arena, LogState);
    
#if !_WIN32
    // If we're on an UNIX system we have to check if the executable was run from the terminal or not.
    // If the executable was double-clicked, we have to change the current directory for relative paths to work as expected
    char *relative_path = args[0];
    if(!starts_with(relative_path, "./"))
    {
        i32 last_index = -1;
        
        for(i32 index = 0; index <= strlen(relative_path); index++)
        {
            char c = relative_path[index];
            if(c == '/')
            {
                last_index = index;
            }
        }
        
        if(last_index != -1)
        {
            size_t relative_path_length = strlen(relative_path);
            size_t diff = strlen(relative_path) - last_index;
            size_t new_path_length = strlen(relative_path) - diff + 1;
            
            auto temp_memory = begin_temporary_memory(game_memory.temp_arena);
            char * new_dir = push_string(temp_memory.arena, new_path_length);
            strncpy(new_dir, relative_path, new_path_length);
            chdir(new_dir);
            end_temporary_memory(temp_memory);
        }
    }
#endif
    
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
    
    game_memory.debug_state = push_struct(&debug_arena, DebugState);
    
    game_memory.debug_state->debug_memory_info.debug_rect.rect_origin = math::Vec2(50, 780);
    game_memory.debug_state->debug_memory_info.debug_rect.rect_size = math::Vec2(300,0);
    
    ConfigData config_data;
    load_config("../.config", &config_data, &platform_state->perm_arena);
    
    game_memory.config_data = config_data;
    
    init_keys();
    RenderState render_state = {};
    render_state.frame_delta = 0.0;
    
    render_state.font_arena = {};
    render_state.shader_arena = {};
    render_state.string_arena = {};
    Renderer renderer = {};
    
    init_renderer(renderer);
    
    if(global_graphics_api == GRAPHICS_VULKAN)
    {
        
#if defined(__linux) || defined(_WIN32)
        //VkRenderState vk_render_state;
        //initialize_vulkan(vk_render_state, renderer, config_data);
        //vk_render(vk_render_state, renderer);
#endif
    }
    else if(global_graphics_api == GRAPHICS_OPEN_GL)
    {
        initialize_opengl(render_state, renderer, &config_data, &platform_state->perm_arena);
    }
    
    GameCode game = {};
    game.is_valid = false;
    load_game_code(game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
    TimerController timer_controller;
    timer_controller.timer_count = 0;
    
    //setup asset reloading
    AssetManager asset_manager = {};
    startup_file_time_checks(&platform_state->perm_arena, &asset_manager, game_library_path);
    
    u32 frame_counter_for_asset_check = 0;
    
    SoundDevice sound_device = {};
    init_audio_fmod(&sound_device);
    
    SoundSystem sound_system = {};
    sound_system.sound_commands.minimum_block_size = sizeof(SoundCommand) * global_max_sound_commands;
    sound_system.sounds = push_array(&sound_system.arena, global_max_sounds, SoundHandle);
    sound_system.audio_sources = push_array(&sound_system.arena, global_max_audio_sources, AudioSource);
    sound_system.channel_groups = push_array(&sound_system.arena, global_max_channel_groups, ChannelGroup);
    
    if (sound_device.is_initialized)
    {
        sound_system.sfx_volume = config_data.sfx_volume;
        sound_system.music_volume = config_data.music_volume;
        sound_system.muted = config_data.muted;
    }
    
    i32 refresh_rate = render_state.refresh_rate;
    u32 target_fps = refresh_rate;
    r32 expected_frames_per_update = 1.0f;
    
    r64 last_second_check = get_time();
    i32 frames = 0;
    i32 fps = 0;
    r32 seconds_per_frame = expected_frames_per_update / target_fps;
    
    r64 last_frame = get_time();
    r64 delta_time;
    renderer.frame_lock = 0;
    
    while (!should_close_window(render_state) && !renderer.should_close)
    {
        if(game_memory.exit_game)
        {
            debug("Quit\n");
            glfwSetWindowShouldClose(render_state.window, GLFW_TRUE);
        }
        
        show_mouse_cursor(render_state, renderer.show_mouse_cursor);
        
        reload_assets(render_state, &asset_manager, &platform_state->perm_arena);
        
        reload_libraries(&game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
        
        auto game_temp_mem = begin_temporary_memory(game_memory.temp_arena);
        game.update(delta_time, &game_memory, renderer, &input_controller, &sound_system, timer_controller);
        update_particle_systems(renderer, delta_time);
        
        tick_animation_controllers(renderer, &sound_system, &input_controller, timer_controller, delta_time);
        tick_timers(timer_controller, delta_time);
        update_sound_commands(&sound_device, &sound_system, delta_time);
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
            listen_to_file_changes(&platform_state->perm_arena, &asset_manager);
            frame_counter_for_asset_check = 0;
        }
        
        update_log();
        
        swap_buffers(render_state);
        
        frames++;
        r64 end_counter = get_time();
        if(end_counter - last_second_check >= 1.0)
        {
            last_second_check = end_counter;
            renderer.fps = frames;
            frames = 0;
        }
        delta_time = get_time() - last_frame;
        last_frame = end_counter;
        
        end_temporary_memory(game_temp_mem);
    }
    
    close_log();
    cleanup_sound(&sound_device);
    close_window(render_state);
}