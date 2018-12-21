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
Core core;
struct LogState *log_state;
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

#include "opengl_rendering.h"
#include "animation.cpp"
#include "keycontroller.cpp"
#include "sound.h"
#include "timers.h"
#include "fmod_sound.h"
#include "fmod_sound.cpp"
#include "filehandling.h"

#if ENABLE_ANALYTICS
#include "GameAnalytics.h"

#include "analytics.h"
#include "analytics.cpp"
#endif

#include "shader_loader.cpp"
#include "render_pipeline.cpp"
#include "rendering.cpp"
#include "particles.cpp"

#if defined(__linux) || defined(__APPLE__)
#include "dlfcn.h"
#endif

static InputController input_controller;

#include "keys_glfw.h"
#include "opengl_rendering.cpp"

#if defined(__linux) || defined(_WIN32)
//#include "vulkan_rendering.cpp"
#endif

static void load_game_code(GameCode &game_code, char *game_library_path, char *temp_game_library_path, MemoryArena *arena = nullptr)
{
    if (!copy_file(game_library_path, temp_game_library_path, false, arena))
        return;

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
        if (err_str)
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

static void sleep_ms(i32 ms)
{
#ifdef _WIN32
    Sleep(ms);
#elif _POSIX_C_SOURCE > 199309L
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(ms * 1000);
#endif
}

static void reload_game_code(GameCode *game_code, char *game_library_path, char *temp_game_library_path, MemoryArena *arena = nullptr)
{
    unload_game_code(game_code);

    sleep_ms(100);

    load_game_code(*game_code, game_library_path, temp_game_library_path, arena);
}

static b32 reload_libraries(GameCode *game, char *game_library_path, char *temp_game_library_path, MemoryArena *arena = nullptr)
{
    // @Bug: Not working on Mac
    time_t last_write_time = get_last_write_time(game_library_path);

    if (last_write_time != 0)
    {
        if (difftime(game->last_library_write_time, last_write_time) != 0)
        {
            reload_game_code(game, game_library_path, temp_game_library_path, arena);
            assert(game);
            debug_log("Reloaded game library\n");
            return true;
        }
    }

    return false;
}

inline void save_config(const char *file_path, ConfigData &old_config_data, RenderState *render_state = nullptr, SoundDevice *sound_device = nullptr)
{
    FILE *file = fopen(file_path, "w");

    if (file)
    {
        // fprintf(file, "title %s\n", old_config_data.title);
        // fprintf(file, "version %s\n", old_config_data.version);

        i32 width = old_config_data.screen_width;
        i32 height = old_config_data.screen_height;
        WindowMode window_mode = old_config_data.window_mode;

        if (render_state)
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
        r32 master_vol = 1.0f;

        if (sound_device)
        {
            muted = sound_device->muted;
            sfx_vol = sound_device->sfx_volume;
            music_vol = sound_device->music_volume;
            master_vol = sound_device->master_volume;
        }

        fprintf(file, "muted %d\n", muted);
        fprintf(file, "sfx_volume %.2f\n", sfx_vol);
        fprintf(file, "music_volume %.2f\n", music_vol);
        fprintf(file, "master_volume %.2f\n", master_vol);

        fclose(file);

        old_config_data.screen_width = width;
        old_config_data.screen_height = height;
        old_config_data.window_mode = window_mode;
        old_config_data.muted = muted;
        old_config_data.sfx_volume = sfx_vol;
        old_config_data.music_volume = music_vol;
        old_config_data.master_volume = master_vol;
    }
}

inline void load_version(const char *file_path, char *version, char *title)
{
    FILE *file;
    file = fopen(file_path, "r");
    char line_buffer[255];

    if (file)
    {
        while (fgets(line_buffer, 255, file))
        {
            if (starts_with(line_buffer, "version"))
            {
                // @Note(Niels): Format for version is: [type] v#.#.#
                // Example 1: alpha v0.1.0
                // Example 2: beta v0.4.3
                char type_buf[64];
                char version_buf[64];
                sscanf(line_buffer, "version %s %s", type_buf, version_buf);
                snprintf(version, strlen(type_buf) + strlen(version_buf) + 2, "%s %s", type_buf, version_buf);
            }
            if (starts_with(line_buffer, "title"))
            {
                sscanf(line_buffer, "title %[^\n]", title);
            }
        }
    }
}

inline void load_config(const char *file_path, ConfigData *config_data, MemoryArena *perm_arena)
{
    FILE *file;
    file = fopen(file_path, "r");
    char line_buffer[255];

    *config_data = {};

    if (!platform.file_exists(file_path))
    {
        // auto title = "Altered";
        // snprintf(config_data->title, strlen(title) + 1, "%s", title);

        // @Note: We read the version from a .version file
        char version[64];
        char title[128];
        load_version("../.version", version, title);
        snprintf(config_data->version, strlen(version) + 1, "%s", version);
        snprintf(config_data->title, strlen(title) + 1, "%s", title);

        // @Note: Default is windowed borderless (aka windowed fullscreen)
        config_data->window_mode = FM_BORDERLESS;

        // @Note: Setting the dimensions to zero will force the renderer to initialize it properly
        config_data->screen_width = 0;
        config_data->screen_height = 0;
        config_data->muted = false;
        config_data->sfx_volume = 1.0f;
        config_data->music_volume = 1.0f;
        config_data->master_volume = 1.0f;

        save_config(file_path, *config_data);
    }
    else
    {
        char version[64];
        char title[128];
        load_version("../.version", version, title);
        snprintf(config_data->version, strlen(version) + 1, "%s", version);
        snprintf(config_data->title, strlen(title) + 1,"%s", title);
	
        while(fgets(line_buffer, 255, file))
        {
            if (starts_with(line_buffer, "screen_width"))
            {
                sscanf(line_buffer, "screen_width %d", &config_data->screen_width);
            }
            else if (starts_with(line_buffer, "screen_height"))
            {
                sscanf(line_buffer, "screen_height %d", &config_data->screen_height);
            }
            else if (starts_with(line_buffer, "contrast"))
            {
                sscanf(line_buffer, "contrast %f", &config_data->contrast);
            }
            else if (starts_with(line_buffer, "brightness"))
            {
                sscanf(line_buffer, "brightness %f", &config_data->brightness);
            }
            else if (starts_with(line_buffer, "window_mode"))
            {
                sscanf(line_buffer, "window_mode %d", &config_data->window_mode);
            }
            else if (starts_with(line_buffer, "muted"))
            {
                sscanf(line_buffer, "muted %d", &config_data->muted);
            }
            else if (starts_with(line_buffer, "sfx_volume"))
            {
                sscanf(line_buffer, "sfx_volume %f", &config_data->sfx_volume);
            }
            else if (starts_with(line_buffer, "music_volume"))
            {
                sscanf(line_buffer, "music_volume %f", &config_data->music_volume);
            }
            else if (starts_with(line_buffer, "master_volume"))
            {
                sscanf(line_buffer, "master_volume %f", &config_data->master_volume);
            }
        }
        fclose(file);
    }
}

static void check_shader_files(WorkQueue *queue, void *data)
{
    // @Incomplete: We might want to sleep
    while(true)
    {
        Renderer *renderer = (Renderer *)data;
        rendering::check_for_shader_file_changes(*renderer);
    }
}

static void init_renderer(Renderer &renderer, WorkQueue *reload_queue, ThreadInfo *reload_thread)
{
    renderer.pixels_per_unit = global_pixels_per_unit;
    renderer.frame_lock = 0;

    renderer.render.bloom.active = true;
    renderer.render.bloom.exposure = 1.8f;
    renderer.render.bloom.amount = 10;
    
    renderer.particles = {};

    renderer.particles._max_particle_system_count = global_max_particle_systems;

    renderer.particles.particle_systems = push_array(&renderer.particle_arena, global_max_particle_systems, ParticleSystemInfo);
    renderer.particles._internal_handles = push_array(&renderer.particle_arena, global_max_particle_systems, i32);

    PushParams params = default_push_params();
    params.alignment = math::multiple_of_number_uint(member_size(RandomSeries, state), 16);
    renderer.particles.entropy = push_size(&renderer.particle_arena, sizeof(RandomSeries), RandomSeries, params);
    random_seed(*renderer.particles.entropy, 1234);

    for (i32 index = 0; index < global_max_particle_systems; index++)
    {
        renderer.particles._internal_handles[index] = -1;
    }

    renderer.particles.particle_system_count = 0;
    renderer.animation_controllers = push_array(&renderer.animation_arena, 64, AnimationController);
    renderer.spritesheet_animations = push_array(&renderer.animation_arena, global_max_spritesheet_animations, SpritesheetAnimation);
    renderer.buffers = push_array(&renderer.buffer_arena, global_max_custom_buffers, BufferData);
    renderer.updated_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer.texture_data = push_array(&renderer.texture_arena, global_max_textures, TextureData);
    renderer.spritesheet_animation_count = 0;
    renderer.animation_controller_count = 0;
    renderer.meshes = push_array(&renderer.mesh_arena, global_max_meshes, Mesh);
    renderer.tt_font_infos = push_array(&renderer.font_arena, global_max_fonts, TrueTypeFontInfo);
    renderer._internal_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer._current_internal_buffer_handle = 0;
    
    for (i32 index = 0; index < global_max_custom_buffers; index++)
    {
        renderer._internal_buffer_handles[index] = -1;
    }
    
    renderer.removed_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);

    // NEW RENDER PIPELIN
    renderer.render.render_commands = push_array(&renderer.command_arena, global_max_render_commands, rendering::RenderCommand);
    renderer.render.shadow_commands = push_array(&renderer.command_arena, global_max_shadow_commands, rendering::ShadowCommand);
    renderer.render.queued_commands = push_array(&renderer.command_arena, global_max_render_commands, QueuedRenderCommand);
    
    renderer.render.buffers = push_array(&renderer.buffer_arena, global_max_custom_buffers, rendering::RegisterBufferInfo);
    renderer.render.updated_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer.render.material_count = 0;
    
    //@Incomplete: Make these dynamically allocated?

    renderer.render.directional_lights = push_array(&renderer.mesh_arena, global_max_directional_lights, DirectionalLight);
    renderer.render.point_lights = push_array(&renderer.mesh_arena, global_max_point_lights, PointLight);
    
    renderer.render.materials = push_array(&renderer.mesh_arena, global_max_materials, rendering::Material);
    //renderer.render.material_instances = push_array(&renderer.mesh_arena, global_max_materials, rendering::Material);

    // Set all material instance values to their defaults
    for(i32 i = 0; i < MAX_MATERIAL_INSTANCE_ARRAYS; i++)
    {
        renderer.render._internal_material_instance_array_handles[i] = -1;
        renderer.render.material_instance_array_counts[i] = 0;
        renderer.render.material_instance_arrays[i] = nullptr;
    }
    
    renderer.render.shaders = push_array(&renderer.mesh_arena, global_max_shaders, rendering::Shader);
    renderer.render._internal_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);
    renderer.render._current_internal_buffer_handle = 0;
    
    for(i32 index = 0; index < global_max_custom_buffers; index++)
    {
        renderer.render._internal_buffer_handles[index] = -1;
    }
    
    renderer.render.removed_buffer_handles = push_array(&renderer.buffer_arena, global_max_custom_buffers, i32);

#if DEBUG
    renderer.render.shader_count = 0;
    renderer.render.shaders_to_reload_count = 0;
    *reload_queue = {};
    *reload_thread = {};
    make_queue(reload_queue, 1, reload_thread);
    platform.add_entry(reload_queue, check_shader_files, &renderer);
#endif

    rendering::set_fallback_shader(renderer, "../engine_assets/standard_shaders/fallback.shd");
    rendering::set_shadow_map_shader(renderer, "../engine_assets/standard_shaders/shadow_map.shd");
    rendering::set_light_space_matrices(renderer, math::ortho(-25, 25, -25, 25, 1, 20.0f), math::Vec3(-2.0f, 4.0f, -1.0f), math::Vec3(0.0f, 0.0f, 0.0f));

    rendering::set_bloom_shader(renderer, "../engine_assets/standard_shaders/bloom.shd");
    rendering::set_blur_shader(renderer, "../engine_assets/standard_shaders/blur.shd");
    rendering::set_hdr_shader(renderer, "../engine_assets/standard_shaders/hdr.shd");
    
    // Final framebuffer
    rendering::FramebufferInfo final_info = rendering::generate_framebuffer_info();
    final_info.width = renderer.framebuffer_width;
    final_info.height = renderer.framebuffer_height;
    rendering::add_color_attachment(rendering::ColorAttachmentType::RENDER_BUFFER, 0, final_info);
    rendering::add_depth_attachment(0, final_info);
    
    rendering::FramebufferHandle final_framebuffer = rendering::create_framebuffer(final_info, renderer);
    rendering::set_final_framebuffer(renderer, final_framebuffer);

    renderer.render.ui.top_left_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP | rendering::UIAlignment::LEFT);
    renderer.render.ui.top_left_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP | rendering::UIAlignment::LEFT, true);
    renderer.render.ui.top_right_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP | rendering::UIAlignment::RIGHT);
    renderer.render.ui.top_right_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP | rendering::UIAlignment::RIGHT, true);
    renderer.render.ui.bottom_left_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM | rendering::UIAlignment::LEFT);
    renderer.render.ui.bottom_left_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM | rendering::UIAlignment::LEFT, true);
    renderer.render.ui.bottom_right_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM | rendering::UIAlignment::RIGHT);
    renderer.render.ui.bottom_right_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM | rendering::UIAlignment::RIGHT, true);
    renderer.render.ui.top_x_centered_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP);
    renderer.render.ui.top_x_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP, true);
    renderer.render.ui.bottom_x_centered_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM);
    renderer.render.ui.bottom_x_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM, true);
    renderer.render.ui.left_y_centered_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::LEFT);
    renderer.render.ui.left_y_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::LEFT, true);
    renderer.render.ui.right_y_centered_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::RIGHT);
    renderer.render.ui.right_y_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::RIGHT, true);
    renderer.render.ui.centered_quad_buffer = rendering::create_quad_buffer(renderer, 0);
    renderer.render.ui.centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, 0, true);

    rendering::create_ui_render_pass(renderer);
    renderer.render.ui_quad_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/ui_quad.shd");
    renderer.render.textured_ui_quad_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/ui_texture_quad.shd");
    renderer.render.ui.material = rendering::create_material(renderer, renderer.render.ui_quad_shader);
    renderer.render.ui.textured_material = rendering::create_material(renderer, renderer.render.textured_ui_quad_shader);

    // Initialize font material
    renderer.render.font_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/font.shd");
    renderer.render.ui.font_material = rendering::create_material(renderer, renderer.render.font_shader);

    rendering::RegisterBufferInfo font_info = rendering::create_register_buffer_info();

    add_vertex_attrib(rendering::ValueType::FLOAT4, font_info);
    font_info.usage = rendering::BufferUsage::DYNAMIC;

    renderer.render.ui.font_buffer = rendering::register_buffer(renderer, font_info);

    // Add a hdr framebuffer as the standard pass framebuffer
    rendering::FramebufferInfo info = rendering::generate_framebuffer_info();
    info.width = renderer.framebuffer_width;
    info.height = renderer.framebuffer_height;
    rendering::add_color_attachment(rendering::ColorAttachmentType::TEXTURE, rendering::ColorAttachmentFlags::HDR, info, 0);
    rendering::add_depth_attachment(rendering::DepthAttachmentFlags::DEPTH_MULTISAMPLED, info, 0);
    
    rendering::FramebufferHandle framebuffer = rendering::create_framebuffer(info, renderer);
    rendering::RenderPassHandle standard = rendering::create_render_pass(STANDARD_PASS, framebuffer, renderer);

    //HDR PP
    rendering::FramebufferInfo hdr_info = rendering::generate_framebuffer_info();
    hdr_info.width = renderer.framebuffer_width;
    hdr_info.height = renderer.framebuffer_height;

    rendering::add_color_attachment(rendering::ColorAttachmentType::TEXTURE, rendering::ColorAttachmentFlags::HDR, hdr_info, 0);
    rendering::add_color_attachment(rendering::ColorAttachmentType::TEXTURE, rendering::ColorAttachmentFlags::HDR, hdr_info, 0);

    rendering::FramebufferHandle hdr_fbo = rendering::create_framebuffer(hdr_info, renderer);

    rendering::ShaderHandle hdr_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/new_hdr.shd");

    //rendering::PostProcessingRenderPassHandle hdr_pass = rendering::create_post_processing_render_pass("HDR Pass", hdr_fbo, renderer, hdr_shader);
    rendering::PostProcessingRenderPassHandle hdr_pass = rendering::create_post_processing_render_pass("HDR Pass", final_framebuffer, renderer, hdr_shader);
    rendering::set_uniform_value(renderer, hdr_pass, "scene", rendering::get_texture_from_framebuffer(0, framebuffer, renderer));
    rendering::set_uniform_value(renderer, hdr_pass, "width", (i32)hdr_info.width);
    rendering::set_uniform_value(renderer, hdr_pass, "height", (i32)hdr_info.height);
    rendering::set_uniform_value(renderer, hdr_pass, "exposure", 1.8f); // @Incomplete: Hardcoded
    
    // //BLOOM
    // rendering::FramebufferInfo bloom_info = rendering::generate_framebuffer_info();
    // bloom_info.width = renderer.framebuffer_width;
    // bloom_info.height = renderer.framebuffer_height;

    // rendering::add_color_attachment(rendering::ColorAttachmentType::TEXTURE, rendering::ColorAttachmentFlags::HDR, bloom_info, 0);

    // rendering::FramebufferHandle bloom_1_fbo = rendering::create_framebuffer(bloom_info, renderer);
    // rendering::FramebufferHandle bloom_2_fbo = rendering::create_framebuffer(bloom_info, renderer);

    // rendering::ShaderHandle blur_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/blur.shd");
    // rendering::ShaderHandle bloom_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/bloom.shd");
    
    // rendering::PostProcessingRenderPassHandle blur_1 = rendering::create_post_processing_render_pass("Bloom_blur_1", bloom_1_fbo, renderer, blur_shader);
    // rendering::PostProcessingRenderPassHandle blur_2 = rendering::create_post_processing_render_pass("Bloom_blur_2", final_framebuffer, renderer, blur_shader);
    
    // rendering::set_uniform_value(renderer, blur_1, "image", rendering::get_texture_from_framebuffer(1, hdr_fbo, renderer));
    // rendering::set_uniform_value(renderer, blur_1, "horizontal", true);

    // rendering::set_uniform_value(renderer, blur_2, "image", rendering::get_texture_from_framebuffer(0, bloom_1_fbo, renderer));
    // rendering::set_uniform_value(renderer, blur_2, "horizontal", false);

    //END BLOOM
    
    // Add tonemapping pass?

    renderer.render.instancing.internal_float_buffers = push_array(&renderer.buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer.render.instancing.internal_float2_buffers = push_array(&renderer.buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer.render.instancing.internal_float3_buffers = push_array(&renderer.buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer.render.instancing.internal_float4_buffers = push_array(&renderer.buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer.render.instancing.internal_mat4_buffers = push_array(&renderer.buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);

    for(i32 i = 0; i < MAX_INSTANCE_BUFFERS; i++)
    {
        renderer.render.instancing.free_float_buffers[i] = true;
        renderer.render.instancing.free_float2_buffers[i] = true;
        renderer.render.instancing.free_float3_buffers[i] = true;
        renderer.render.instancing.free_float4_buffers[i] = true;
        renderer.render.instancing.free_mat4_buffers[i] = true;

        renderer.render.instancing.internal_float_buffers[i] = push_struct(&renderer.buffer_arena, Buffer);
        renderer.render.instancing.internal_float2_buffers[i] = push_struct(&renderer.buffer_arena, Buffer);
        renderer.render.instancing.internal_float3_buffers[i] = push_struct(&renderer.buffer_arena, Buffer);
        renderer.render.instancing.internal_float4_buffers[i] = push_struct(&renderer.buffer_arena, Buffer);
        renderer.render.instancing.internal_mat4_buffers[i] = push_struct(&renderer.buffer_arena, Buffer);
    }
    
    rendering::RegisterBufferInfo particle_buffer = rendering::create_register_buffer_info();
    particle_buffer.usage = rendering::BufferUsage::STATIC;
    add_vertex_attrib(rendering::ValueType::FLOAT3, particle_buffer);
    add_vertex_attrib(rendering::ValueType::FLOAT2, particle_buffer);
    
    r32 quad_vertices[20] =
    {
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f
    };

    i32 vertex_size = 5;
    particle_buffer.data.vertex_count = 4;
    particle_buffer.data.vertex_buffer_size = particle_buffer.data.vertex_count * vertex_size * (i32)sizeof(r32);
    particle_buffer.data.vertex_buffer = push_size(&renderer.buffer_arena, particle_buffer.data.vertex_buffer_size, r32);

    for (i32 i = 0; i < particle_buffer.data.vertex_count * vertex_size; i++)
    {
        particle_buffer.data.vertex_buffer[i] = quad_vertices[i];
    }

    i32 index_count = 6;
    particle_buffer.data.index_buffer_size = index_count * (i32)sizeof(u16);
    particle_buffer.data.index_buffer_count = index_count;

    particle_buffer.data.index_buffer = push_size(&renderer.buffer_arena, particle_buffer.data.index_buffer_size, u16);

    u16 quad_indices[6] =
        {
            0, 1, 2,
            0, 2, 3};
    
    for (i32 i = 0; i < index_count; i++)
    {
        particle_buffer.data.index_buffer[i] = quad_indices[i];
    }

    renderer.particles.quad_buffer = rendering::register_buffer(renderer, particle_buffer);
}

#if ENABLE_ANALYTICS
void process_analytics_events(AnalyticsEventState &analytics_state, WorkQueue *queue)
{
    for (u32 i = 0; i < analytics_state.event_count; i++)
    {
        AnalyticsEventData *event = &analytics_state.events[i];
        event->state = &analytics_state;
        send_analytics_event(queue, event);
    }

    analytics_state.event_count = 0;
}
#endif

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
    core = game_memory.core;

    PlatformState *platform_state = bootstrap_push_struct(PlatformState, perm_arena);

    game_memory.log_state = push_struct(&platform_state->perm_arena, LogState);

#if !defined(_WIN32)
    // If we're on an UNIX system we have to check if the executable was run from the terminal or not.
    // If the executable was double-clicked, we have to change the current directory for relative paths to work as expected
    char *relative_path = args[0];
    if (!starts_with(relative_path, "./"))
    {
        i32 last_index = -1;

        for (i32 index = 0; index <= (i32)strlen(relative_path); index++)
        {
            char c = relative_path[index];
            if (c == '/')
            {
                last_index = index;
            }
        }

        if (last_index != -1)
        {
            size_t diff = strlen(relative_path) - last_index;
            size_t new_path_length = strlen(relative_path) - diff + 1;

            auto temp_memory = begin_temporary_memory(game_memory.temp_arena);
            char *new_dir = push_string(temp_memory.arena, new_path_length);
            strncpy(new_dir, relative_path, new_path_length);
            chdir(new_dir);
            end_temporary_memory(temp_memory);
        }
    }
#endif

    log_state = game_memory.log_state;
    init_log(L_FLAG_FILE | L_FLAG_DEBUG, "../log.txt");

#if defined(__APPLE__)
    char *game_library_path = "libgame.dylib";
    char *temp_game_library_path = "libgame_temp.dylib";
#elif defined(_WIN32)
    char *game_library_path = "game.dll";
    char *temp_game_library_path = "game_temp.dll";
#else
    char *game_library_path = "libgame.so";
    char *temp_game_library_path = "libgame_temp.so";
#endif

    ConfigData config_data;
    load_config("../.config", &config_data, &platform_state->perm_arena);

    game_memory.config_data = config_data;

    init_keys();
    RenderState *render_state_ptr = push_struct(&platform_state->perm_arena, RenderState);
    RenderState &render_state = *render_state_ptr;
    render_state = {};
    render_state.should_close = false;
    render_state.dpi_scale = 0;
    render_state.window = nullptr;
    render_state.texture_index = 0;
    render_state.frame_delta = 0.0;

    render_state.string_arena = {};
    render_state.gl_shader_count = 0;
    render_state.gl_buffer_count = 0;
    render_state.gl_shaders = push_array(&platform_state->perm_arena, 64, ShaderGL);
    
    Renderer *renderer_alloc = push_struct(&platform_state->perm_arena, Renderer);
    Renderer &renderer = *renderer_alloc;
    renderer = {};

    scene::SceneManager *scene_manager = scene::create_scene_manager(&platform_state->perm_arena, renderer);
    
    b32 do_save_config = false;
    
    if constexpr(global_graphics_api == GRAPHICS_VULKAN)
                {
#if defined(__linux) || defined(_WIN32)
        //VkRenderState vk_render_state;
        //initialize_vulkan(vk_render_state, renderer, config_data);
        //vk_render(vk_render_state, renderer);
#endif
    }
    else if constexpr (global_graphics_api == GRAPHICS_OPEN_GL)
    {
        log("Initializing OpenGl");
        initialize_opengl(render_state, renderer, &config_data, &platform_state->perm_arena, &do_save_config);
    }

    WorkQueue reload_queue;
    ThreadInfo reload_thread;
    init_renderer(renderer, &reload_queue, &reload_thread);
    
    GameCode game = {};
    game.is_valid = false;

    load_game_code(game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
    TimerController *timer_controller_ptr = (TimerController*)malloc(sizeof(TimerController));
    TimerController &timer_controller = *timer_controller_ptr;
    timer_controller.timer_count = 0;

    SoundDevice sound_device = {};
    sound_device.system = nullptr;
    debug_log("Initializing FMOD");

    sound_device.channel_count = 0;
    sound_device.sound_count = 0;
    sound_device.sfx_volume = config_data.sfx_volume;
    sound_device.music_volume = config_data.music_volume;
    sound_device.master_volume = config_data.master_volume;
    sound_device.muted = config_data.muted;

    WorkQueue fmod_queue = {};
    ThreadInfo fmod_thread = {};
    make_queue(&fmod_queue, 1, &fmod_thread);
    platform.add_entry(&fmod_queue, init_audio_fmod_thread, &sound_device);

    SoundSystem sound_system = {};
    sound_system.command_count = 0;
    sound_system.sound_count = 0;
    sound_system.commands = push_array(&sound_system.arena, global_max_sound_commands, SoundCommand);
    sound_system.sounds = push_array(&sound_system.arena, global_max_sounds, SoundHandle);
    sound_system.audio_sources = push_array(&sound_system.arena, global_max_audio_sources, AudioSource);
    sound_system.channel_groups = push_array(&sound_system.arena, global_max_channel_groups, ChannelGroup);
    sound_system.sfx_volume = config_data.sfx_volume;
    sound_system.music_volume = config_data.music_volume;
    sound_system.master_volume = config_data.master_volume;
    sound_system.muted = config_data.muted;

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


#if ENABLE_ANALYTICS
#define ANALYTICS_GAME_KEY "3a3552e363e3ca17a17f98d568f25c75"
#define ANALYTICS_SECRET_KEY "c34eacd91bcd41a33b37b0e8c978c17ee5c18f53"
    AnalyticsEventState analytics_state = {};
    gameanalytics::GameAnalytics::setEnabledInfoLog(false);
    gameanalytics::GameAnalytics::configureBuild("alpha 0.1");
    gameanalytics::GameAnalytics::initialize(ANALYTICS_GAME_KEY, ANALYTICS_SECRET_KEY);
    gameanalytics::GameAnalytics::startSession();

    ThreadInfo analytics_info[1] = {};
    WorkQueue analytics_queue = {};
    make_queue(&analytics_queue, 1, analytics_info);
    game_memory.analytics_state = &analytics_state;

    r64 start_frame_for_total_time = get_time();
#endif

    core.renderer = &renderer;
    core.input_controller = &input_controller;
    core.timer_controller = &timer_controller;
    core.sound_system = &sound_system;
    core.scene_manager = scene_manager;
    core.delta_time = delta_time;
    core.current_time = get_time();
    game_memory.core = core;

    while (!should_close_window(render_state) && !renderer.should_close)
    {
        if (game_memory.exit_game)
        {
            debug_log("Quit\n");
            glfwSetWindowShouldClose(render_state.window, GLFW_TRUE);
        }
        
        show_mouse_cursor(render_state, renderer.show_mouse_cursor);

        reload_libraries(&game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
        //#endif
        //auto game_temp_mem = begin_temporary_memory(game_memory.temp_arena);

        game.update(&game_memory);

        if(scene_manager->scene_loaded)
        {
            push_scene_for_rendering(scene::get_scene(scene_manager->loaded_scene), renderer);
        }
        
        update_particle_systems(renderer, delta_time);
#if ENABLE_ANALYTICS
        process_analytics_events(analytics_state, &analytics_queue);
#endif
        tick_animation_controllers(renderer, &sound_system, &input_controller, timer_controller, delta_time);
        tick_timers(timer_controller, delta_time);
        update_sound_commands(&sound_device, &sound_system, delta_time, &do_save_config);

        render(render_state, renderer, delta_time, &do_save_config);
        if (do_save_config)
        {
            save_config("../.config", config_data, &render_state, &sound_device);
        }
        do_save_config = false;

        set_controller_invalid_keys();
        set_invalid_keys();
        set_mouse_invalid_keys();

        poll_events();

        if (controller_present())
        {
            controller_keys(GLFW_JOYSTICK_1);
        }

//        update_log();
        
        swap_buffers(render_state);

#if defined(__APPLE__)
        static b32 first_load = true;
        if (first_load)
        {
            mojave_workaround(render_state);
            first_load = false;
        }
#endif
        frames++;
        r64 end_counter = get_time();
        if (end_counter - last_second_check >= 1.0)
        {
            last_second_check = end_counter;
            renderer.fps = frames;
            frames = 0;
        }

        delta_time = get_time() - last_frame;

        // @Incomplete: Fix this!
        delta_time = math::clamp(0.0, delta_time, 0.9);
        game_memory.core.delta_time = delta_time;
        game_memory.core.current_time = get_time();
        last_frame = end_counter;
    }

#if ENABLE_ANALYTICS
    AnalyticsEventData event = {};
    event.state = &analytics_state;
    event.type = AnalyticsEventType::SESSION;
    event.play_time = get_time() - start_frame_for_total_time;
    gameanalytics::GameAnalytics::endSession();
#endif

    close_log();
    cleanup_sound(&sound_device);
    close_window(render_state);
}
