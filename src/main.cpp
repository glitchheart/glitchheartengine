#include "shared.h"

#if DEBUG
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

#include "scene.h"
#include "particles.cpp"
#include "opengl_rendering.h"
#include "animation.cpp"
#include "keycontroller.cpp"
#include "sound.h"
#include "timers.h"
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
        game_code.is_valid = game_code.update != nullptr;
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
            debug("%s", err_str);
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
        game_code->game_code_library = nullptr;
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

inline void save_config(const char* file_path, ConfigData &old_config_data, RenderState* render_state = nullptr, SoundDevice *sound_device = nullptr)
{
    FILE* file = fopen(file_path, "w");
    
    if(file)
    {
        fprintf(file, "title %s\n", old_config_data.title);
        fprintf(file, "version %s\n", old_config_data.version);
        
        i32 width = 0;
        i32 height = 0;
        WindowMode window_mode = FM_WINDOWED;
        
        if(render_state)
        {
            width = render_state->window_width;
            height = render_state->window_height;
            window_mode = render_state->window_mode;
        }
        
        fprintf(file, "screen_width %d\n", width);
        fprintf(file, "screen_height %d\n", height);
        fprintf(file, "window_mode %d\n", window_mode);
        
        b32 muted = false;
        r32 sfx_vol = 1.0f;
        r32 music_vol = 1.0f;
        
        if(sound_device)
        {
            muted = sound_device->muted;
            sfx_vol = sound_device->sfx_volume;
            music_vol = sound_device->music_volume;
        }
        
        fprintf(file, "muted %d\n", muted);
        fprintf(file, "sfx_volume %.2f\n", sfx_vol);
        fprintf(file, "music_volume %.2f\n", music_vol);
        
        fclose(file);
        
        
        old_config_data.screen_width = width;
        old_config_data.screen_height = height;
        old_config_data.window_mode = window_mode;
        old_config_data.muted = muted;
        old_config_data.sfx_volume = sfx_vol;
        old_config_data.music_volume = music_vol;
    }
}

inline void load_config(const char* file_path, ConfigData* config_data, MemoryArena* perm_arena)
{
    FILE* file;
    file = fopen(file_path, "r");
    char line_buffer[255];
    
    *config_data = {};
    
    if(!file)
    {
        auto title = "ALTER";
        snprintf(config_data->title, strlen(title) + 1, "%s", title);
        auto version = "v0.1.3";
        snprintf(config_data->version, strlen(version) + 1, "%s", version);
        config_data->screen_width = 0;
        config_data->window_mode = FM_WINDOWED;
        config_data->screen_height = 0;
        config_data->muted = false;
        config_data->sfx_volume = 1.0f;
        config_data->music_volume = 1.0f;
        
        save_config(file_path, *config_data);
    }
    else
    {
        while(fgets(line_buffer, 255, file))
        {
            if(starts_with(line_buffer, "title"))
            {
                auto title = &line_buffer[6];
                snprintf(config_data->title, strlen(title),"%s", title);
            }
            else if(starts_with(line_buffer, "version"))
            {
                sscanf(line_buffer, "version %s", config_data->version);
            }
            else if(starts_with(line_buffer, "screen_width"))
            {
                sscanf(line_buffer, "screen_width %d", &config_data->screen_width);
            }
            else if(starts_with(line_buffer, "screen_height"))
            {
                sscanf(line_buffer, "screen_height %d", &config_data->screen_height);
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
        }
        fclose(file);
    }
}

static void init_renderer(Renderer &renderer)
{
    renderer.pixels_per_unit = global_pixels_per_unit;
    renderer.frame_lock = 0;
    
    renderer.particles = {};
    
    renderer.particles._max_particle_system_count = global_max_particle_systems;
    
    renderer.particles.particle_systems = push_array(&renderer. particle_arena, global_max_particle_systems, ParticleSystemInfo);
    renderer.particles._internal_handles = push_array(&renderer. particle_arena, global_max_particle_systems, i32);
    renderer.particles._tagged_removed = push_array(&renderer. particle_arena, global_max_particle_systems, i32);
    renderer.particles._tagged_removed_count = 0;
    
    for(i32 index = 0; index < global_max_particle_systems; index++)
    {
        renderer.particles._internal_handles[index] = -1;
    }
    
    renderer.particles.particle_system_count = 0;
    renderer.animation_controllers = push_array(&renderer.animation_arena, 64, AnimationController);
    renderer.spritesheet_animations = push_array(&renderer.animation_arena, global_max_spritesheet_animations, SpritesheetAnimation);
    renderer.commands = push_array(&renderer.command_arena, global_max_render_commands, RenderCommand);
    renderer.ui_commands = push_array(&renderer.command_arena, global_max_ui_commands, RenderCommand);
    renderer.light_commands.minimum_block_size = sizeof(RenderCommand) * global_max_light_commands;
    renderer.buffers = push_array(&renderer.buffer_arena, global_max_custom_buffers, BufferData);
    renderer.updated_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer.texture_data = push_array(&renderer.texture_arena, global_max_textures, TextureData);
    renderer.spritesheet_animation_count = 0;
    renderer.animation_controller_count = 0;
    renderer.material_count = 0;
    renderer.materials = push_array(&renderer.mesh_arena, global_max_materials, Material);
    renderer.meshes = push_array(&renderer.mesh_arena, global_max_meshes, Mesh);
    renderer.shader_data = push_array(&renderer.shader_arena, global_max_shaders, ShaderData);
    renderer.fonts = push_array(&renderer.font_arena, global_max_fonts, FontData);
    renderer.tt_font_infos = push_array(&renderer.font_arena, global_max_fonts, TrueTypeFontInfo);
    renderer._internal_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer._current_internal_buffer_handle = 0;
    for(i32 index = 0; index < global_max_custom_buffers; index++)
    {
        renderer._internal_buffer_handles[index] = -1;
    }
    renderer.removed_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
}

#if defined(_WIN32) && !defined(DEBUG)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char **args)
#endif
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
    
#if !defined(_WIN32)
    // If we're on an UNIX system we have to check if the executable was run from the terminal or not.
    // If the executable was double-clicked, we have to change the current directory for relative paths to work as expected
    char *relative_path = args[0];
    if(!starts_with(relative_path, "./"))
    {
        i32 last_index = -1;
        
        for(i32 index = 0; index <= (i32)strlen(relative_path); index++)
        {
            char c = relative_path[index];
            if(c == '/')
            {
                last_index = index;
            }
        }
        
        if(last_index != -1)
        {
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
    init_log(L_FLAG_FILE | L_FLAG_DEBUG, "../log.txt");
    
    
#if defined(__APPLE__)
    char* game_library_path = "libgame.dylib";
    char* temp_game_library_path = "libgame_temp.dylib";
#elif defined(_WIN32)
    char* game_library_path = "game.dll";
    char* temp_game_library_path = "game_temp.dll";
#else
    char* game_library_path = "libgame.so";
    char* temp_game_library_path = "libgame_temp.so";
#endif
    
#if DEBUG
    MemoryArena debug_arena = {};
    
    game_memory.debug_state = push_struct(&debug_arena, DebugState);
    
    game_memory.debug_state->debug_memory_info.debug_rect.rect_origin = math::Vec2(50, 780);
    game_memory.debug_state->debug_memory_info.debug_rect.rect_size = math::Vec2(300,0);
#endif
    
    ConfigData config_data;
    load_config("../.config", &config_data, &platform_state->perm_arena);
    
    game_memory.config_data = config_data;
    
    init_keys();
    RenderState render_state = {};
    render_state.framebuffer = {};
    render_state.should_close = false;
    render_state.buffer_count = 0;
    render_state.dpi_scale = 0;
    render_state.window = nullptr;
    render_state.texture_index = 0;
    render_state.frame_delta = 0.0;
    render_state.extra_shader_index = 0;
    
    render_state.font_arena = {};
    render_state.string_arena = {};
    Renderer renderer = {};
    
    b32 do_save_config = false;
    
    init_renderer(renderer);
    if constexpr(global_graphics_api == GRAPHICS_VULKAN)
    {
#if defined(__linux) || defined(_WIN32)
        //VkRenderState vk_render_state;
        //initialize_vulkan(vk_render_state, renderer, config_data);
        //vk_render(vk_render_state, renderer);
#endif
    }
    else if constexpr(global_graphics_api == GRAPHICS_OPEN_GL)
    {
        log("Initializing OpenGl");
        initialize_opengl(render_state, renderer, &config_data, &platform_state->perm_arena, &do_save_config);
    }
    
    GameCode game = {};
    game.is_valid = false;
    
    load_game_code(game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
    TimerController timer_controller;
    timer_controller.timer_count = 0;
    
    //setup asset reloading
    //AssetManager asset_manager = {};
    //startup_file_time_checks(&platform_state->perm_arena, &asset_manager, game_library_path);
    
    //u32 frame_counter_for_asset_check = 0;
    
    SoundDevice sound_device = {};
    debug_log("Initializing FMOD");
    
    sound_device.sfx_volume = config_data.sfx_volume;
    sound_device.music_volume = config_data.music_volume;
    sound_device.muted = config_data.muted;
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
    
    r64 last_second_check = get_time();
    i32 frames = 0;
    
    //i32 refresh_rate = render_state.refresh_rate;
    //u32 target_fps = (u32)refresh_rate;
    //r32 expected_frames_per_update = 1.0f;
    //r32 seconds_per_frame = expected_frames_per_update / target_fps;
    
    r64 last_frame = get_time();
    r64 delta_time = 0.0;
    renderer.frame_lock = 0;
    
    scene::EntityTemplateState template_state = {};
    template_state.template_count = 0;
    
    template_state.templates = push_array(&platform_state->perm_arena, global_max_entity_templates, scene::EntityTemplate);
    
    while (!should_close_window(render_state) && !renderer.should_close)
    {
        if(game_memory.exit_game)
        {
            debug("Quit\n");
            glfwSetWindowShouldClose(render_state.window, GLFW_TRUE);
        }
        
        show_mouse_cursor(render_state, renderer.show_mouse_cursor);
        
        //reload_assets(render_state, &asset_manager, &platform_state->perm_arena);
        
        reload_libraries(&game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
        
        auto game_temp_mem = begin_temporary_memory(game_memory.temp_arena);
        game.update(delta_time, &game_memory, renderer, template_state, &input_controller, &sound_system, timer_controller);
        update_particle_systems(renderer, delta_time);
        
        tick_animation_controllers(renderer, &sound_system, &input_controller, timer_controller, delta_time);
        tick_timers(timer_controller, delta_time);
        update_sound_commands(&sound_device, &sound_system, delta_time, &do_save_config);
        
        render(render_state, renderer, delta_time, &do_save_config);
        
        if(do_save_config)
        {
            save_config("../.config", config_data, &render_state, &sound_device);
        }
        do_save_config = false;
        
        set_controller_invalid_keys();
        set_invalid_keys();
        set_mouse_invalid_keys();
        
        poll_events();
        
        if(controller_present())
        {
            controller_keys(GLFW_JOYSTICK_1);
        }
        
		// frame_counter_for_asset_check++;
        // if(frame_counter_for_asset_check == 10)
        // {
        //     listen_to_file_changes(&platform_state->perm_arena, &asset_manager);
        //     frame_counter_for_asset_check = 0;
        // }
		
        update_log();
        
        swap_buffers(render_state);
        
#if __APPLE__
        static b32 first_load = true;
        if(first_load)
        {
            mojave_workaround(render_state);
            first_load = false;
        }
#endif
        
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
