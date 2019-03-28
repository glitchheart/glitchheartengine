#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRA_LEAN
#endif

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

#include "opengl_rendering.h"
#include "animation.cpp"
#include "keycontroller.cpp"
#include "sound.h"
#include "timers.h"
#include "fmod_sound.h"
#include "fmod_sound.cpp"
#include "filehandling.h"

#include "shader_loader.cpp"
#include "render_pipeline.cpp"
#include "rendering.cpp"
#include "particles.cpp"
#include "particle_api.cpp"

#include "scene.h"
#include "scene.cpp"

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
    if (!copy_file(game_library_path, temp_game_library_path, false))
        return;

    game_code.update = update_stub;
    game_code.update_editor = update_editor_stub;
    game_code.last_library_write_time = get_last_write_time(game_library_path);
    game_code.game_code_library = platform.load_dynamic_library(temp_game_library_path);

    if (game_code.game_code_library)
    {
        game_code.update = (Update *)platform.load_symbol(game_code.game_code_library, "update");
        game_code.update_editor = (UpdateEditor *)platform.load_symbol(game_code.game_code_library, "update_editor");
        game_code.is_valid = game_code.update != nullptr && game_code.update_editor != nullptr;
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
        game_code.update_editor = update_editor_stub;
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
    game_code->update_editor = update_editor_stub;
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

void save_config(const char *file_path, Renderer* renderer, SoundDevice *sound_device)
{
    FILE *file = fopen(file_path, "w");

    if (file)
    {
        i32 width = core.config_data.screen_width;
        i32 height = core.config_data.screen_height;
        WindowMode window_mode = core.config_data.window_mode;

        if (renderer)
        {
            width = renderer->window_width;
            height = renderer->window_height;
            window_mode = renderer->window_mode;
        }

        fprintf(file, "screen_width %d\n", width);
        fprintf(file, "screen_height %d\n", height);
        fprintf(file, "window_mode %d\n", window_mode);

        b32 muted = false;
        r32 sfx_vol = 1.0f;
        r32 music_vol = 1.0f;
        r32 master_vol = 1.0f;
        b32 vsync = renderer->api_functions.get_v_sync(renderer->api_functions.render_state);

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
        fprintf(file, "vsync %d\n", vsync);

        fclose(file);

        core.config_data.screen_width = width;
        core.config_data.screen_height = height;
        core.config_data.window_mode = window_mode;
        core.config_data.muted = muted;
        core.config_data.sfx_volume = sfx_vol;
        core.config_data.music_volume = music_vol;
        core.config_data.master_volume = master_vol;
        core.config_data.vsync = vsync;
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

    core.config_data = {};

    if (!platform.file_exists(file_path))
    {
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
        config_data->vsync = true;

        save_config(file_path);
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
            else if (starts_with(line_buffer, "vsync"))
            {
                sscanf(line_buffer, "vsync %d", &config_data->vsync);
            }
        }
        fclose(file);
    }
}

[[noreturn]]
static void check_shader_files(WorkQueue *queue, void *data)
{
    // @Incomplete: We might want to sleep
    while(true)
    {
        Renderer *renderer = (Renderer *)data;
        rendering::check_for_shader_file_changes(renderer);
		sleep_ms(100);
    }
}

static void init_renderer(Renderer *renderer, WorkQueue *reload_queue, ThreadInfo *reload_thread, ParticleApi particle_api)
{
    renderer->pixels_per_unit = global_pixels_per_unit;
    renderer->frame_lock = 0;
    
    renderer->particles = {};

    renderer->particles._max_particle_system_count = global_max_particle_systems;

    renderer->particles.particle_systems = push_array(&renderer->particle_arena, global_max_particle_systems, ParticleSystemInfo);
    renderer->particles._internal_handles = push_array(&renderer->particle_arena, global_max_particle_systems, i32);
    renderer->particles._internal_work_queue_handles = push_array(&renderer->particle_arena, global_max_particle_systems, i32);
    renderer->particles.work_queues = push_array(&renderer->particle_arena, global_max_particle_systems, WorkQueue);
    renderer->particles.api = &particle_api;

    renderer->particles.system_work_queue = (WorkQueue*)malloc(sizeof(WorkQueue));
    renderer->particles.system_threads = push_array(&renderer->particle_arena, global_max_particle_systems, ThreadInfo);
    platform.make_queue(renderer->particles.system_work_queue, global_max_particle_systems, renderer->particles.system_threads);
    
    PushParams params = default_push_params();
    params.alignment = math::multiple_of_number_uint(member_size(RandomSeries, state), 16);
    renderer->particles.entropy = push_size(&renderer->particle_arena, sizeof(RandomSeries), RandomSeries, params);
    random_seed(*renderer->particles.entropy, 1234);

    for (i32 index = 0; index < global_max_particle_systems; index++)
    {
        renderer->particles._internal_handles[index] = -1;
        renderer->particles._internal_work_queue_handles[index] = -1;
    }

    renderer->particles.particle_system_count = 0;
    renderer->animation_controllers = push_array(&renderer->animation_arena, global_max_animation_controllers, AnimationController);
    renderer->spritesheet_animations = push_array(&renderer->animation_arena, global_max_spritesheet_animations, SpritesheetAnimation);
    
    renderer->render.textures = push_array(&renderer->texture_arena, global_max_textures, Texture*);

    for(i32 i = 0; i < global_max_textures; i++)
    {
        renderer->render.textures[i] = push_struct(&renderer->texture_arena, Texture);
    }
    renderer->render.texture_count = 0;

    // NEW RENDER PIPELINE
    //renderer.render.render_commands = push_array(&renderer.command_arena, global_max_render_commands, rendering::RenderCommand);
    //renderer.render.depth_free_commands = push_array(&renderer.command_arena, global_max_depth_free_commands, rendering::RenderCommand);
    renderer->render.shadow_commands = push_array(&renderer->command_arena, global_max_shadow_commands, rendering::ShadowCommand);
    renderer->render.pass_commands = push_array(&renderer->command_arena, 16, Pass);
    renderer->tt_font_infos = push_array(&renderer->font_arena, global_max_fonts, TrueTypeFontInfo);
    
    // NEW RENDER PIPELINE
    renderer->render.shadow_commands = push_array(&renderer->command_arena, global_max_shadow_commands, rendering::ShadowCommand);
    renderer->render.queued_commands = push_array(&renderer->command_arena, global_max_render_commands, QueuedRenderCommand);
    
    renderer->render.material_count = 0;
    
    //@Incomplete: Make these dynamically allocated?

    renderer->render.directional_lights = push_array(&renderer->mesh_arena, global_max_directional_lights, DirectionalLight);
    renderer->render.point_lights = push_array(&renderer->mesh_arena, global_max_point_lights, PointLight);
    
    renderer->render.materials = push_array(&renderer->mesh_arena, global_max_materials, rendering::Material);
    renderer->render.material_instances = push_array(&renderer->mesh_arena, global_max_material_instances, rendering::Material);
    renderer->render._internal_material_instance_handles = push_array(&renderer->mesh_arena, global_max_material_instances, i32);
    renderer->render.current_material_instance_index = 0;
    renderer->render.material_instance_count = 0;
    
    // Set all material instance values to their defaults
    for(i32 i = 0; i < global_max_material_instances; i++)
    {
        renderer->render._internal_material_instance_handles[i] = -1;
    }

    renderer->render.shaders = push_array(&renderer->mesh_arena, global_max_shaders, rendering::Shader);
    renderer->render.custom_mappings = push_array(&renderer->mesh_arena, MAX_CUSTOM_UNIFORM_MAPPINGS, rendering::CustomUniformMapping);
    renderer->render._internal_buffer_handles = push_array(&renderer->buffer_arena, global_max_custom_buffers, i32);
    renderer->render._current_internal_buffer_handle = 0;
    
    for(i32 index = 0; index < global_max_custom_buffers; index++)
    {
        renderer->render._internal_buffer_handles[index] = -1;
    }
    
    renderer->render.buffers = push_array(&renderer->buffer_arena, global_max_custom_buffers, Buffer*);

    for(i32 i = 0; i < global_max_custom_buffers; i++)
    {
        renderer->render.buffers[i] = push_struct(&renderer->buffer_arena, Buffer);
    }
          
    renderer->render.removed_buffer_handles = push_array(&renderer->buffer_arena, global_max_custom_buffers, i32);

#if DEBUG
    renderer->render.shader_count = 0;
    renderer->render.shaders_to_reload_count = 0;
    *reload_queue = {};
    *reload_thread = {};

    make_queue(reload_queue, 1, reload_thread);
    platform.add_entry(reload_queue, check_shader_files, renderer);
#endif
    
    rendering::set_fallback_shader(renderer, "../engine_assets/standard_shaders/fallback.shd");
    rendering::set_wireframe_shader(renderer, "../engine_assets/standard_shaders/wireframe.shd");
    rendering::set_bounding_box_shader(renderer, "../engine_assets/standard_shaders/bounding_box.shd");
    rendering::set_debug_line_shader(renderer, "../engine_assets/standard_shaders/line.shd");
    rendering::set_shadow_map_shader(renderer, "../engine_assets/standard_shaders/shadow_map.shd");
    // // rendering::set_light_space_matrices(renderer, math::ortho(-15, 15, -15, 15, 1, 20.0f), math::Vec3(-2.0f, 4.0f, -1.0f), math::Vec3(0.0f, 0.0f, 0.0f));
    // rendering::calculate_light_space_matrices(renderer, );

    rendering::set_bloom_shader(renderer, "../engine_assets/standard_shaders/bloom.shd");
    rendering::set_blur_shader(renderer, "../engine_assets/standard_shaders/blur.shd");
    rendering::set_hdr_shader(renderer, "../engine_assets/standard_shaders/hdr.shd");

    
    rendering::ShaderHandle blur_shader = renderer->render.blur_shader;
    rendering::ShaderHandle bloom_shader = renderer->render.bloom_shader;

    // Create the framebuffers
    rendering::FramebufferInfo standard_resolve_info = rendering::generate_framebuffer_info();
    standard_resolve_info.width = renderer->framebuffer_width;
    standard_resolve_info.height = renderer->framebuffer_height;
    
	rendering::add_color_attachment(rendering::AttachmentType::TEXTURE, rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, standard_resolve_info);
    rendering::add_color_attachment(rendering::AttachmentType::TEXTURE, rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, standard_resolve_info);
    rendering::add_depth_attachment(rendering::AttachmentType::TEXTURE, 0, standard_resolve_info);
    
    rendering::FramebufferHandle standard_resolve_framebuffer = rendering::create_framebuffer(standard_resolve_info, renderer);
    rendering::RenderPassHandle standard_resolve = rendering::create_render_pass("standard_resolve", standard_resolve_framebuffer, renderer);
    
    rendering::FramebufferInfo standard_info = rendering::generate_framebuffer_info();
    standard_info.width = renderer->framebuffer_width;
    standard_info.height = renderer->framebuffer_height;
    
    rendering::add_color_attachment(rendering::AttachmentType::RENDER_BUFFER, rendering::ColorAttachmentFlags::MULTISAMPLED | rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, standard_info, 8);
    rendering::add_color_attachment(rendering::AttachmentType::RENDER_BUFFER, rendering::ColorAttachmentFlags::MULTISAMPLED | rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, standard_info, 8);
    rendering::add_depth_attachment(rendering::AttachmentType::RENDER_BUFFER, rendering::DepthAttachmentFlags::DEPTH_MULTISAMPLED, standard_info, 8);

    rendering::FramebufferHandle standard_framebuffer = rendering::create_framebuffer(standard_info, renderer);
    rendering::RenderPassHandle standard = rendering::create_render_pass(STANDARD_PASS, standard_framebuffer, renderer);
    
    renderer->render.standard_pass = standard;
   
    //rendering::RenderPassHandle read_draw_pass = rendering::create_render_pass("read_draw", standard_framebuffer, renderer);

    rendering::set_read_draw_render_passes(standard, standard_resolve, renderer);

    // Create shadow pass
    rendering::FramebufferInfo shadow_pass_info = rendering::generate_framebuffer_info();
    shadow_pass_info.width = 2048;
    shadow_pass_info.height = 2048;
    shadow_pass_info.size_ratio = 0;

    rendering::add_depth_attachment(rendering::AttachmentType::TEXTURE, 0, shadow_pass_info);

    rendering::FramebufferHandle shadow_fbo = rendering::create_framebuffer(shadow_pass_info, renderer);
    rendering::RenderPassHandle shadow_pass = rendering::create_render_pass(SHADOW_PASS, shadow_fbo, renderer, rendering::RenderPassSettings::FRONTFACE_CULLING);
    renderer->render.shadow_pass = shadow_pass;
    renderer->render.shadow_framebuffer = shadow_fbo;

    renderer->render.shadow_settings.z_near = -18.0f;
    renderer->render.shadow_settings.z_far = 65.0f;
    renderer->render.shadow_settings.fov = 110.0f;
    
    // Final framebuffer
    rendering::FramebufferInfo final_info = rendering::generate_framebuffer_info();
    final_info.width = renderer->framebuffer_width;
    final_info.height = renderer->framebuffer_height;
    
    rendering::add_color_attachment(rendering::AttachmentType::RENDER_BUFFER, rendering::ColorAttachmentFlags::MULTISAMPLED | rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, final_info, 8);
    rendering::add_depth_attachment(rendering::AttachmentType::RENDER_BUFFER, rendering::DepthAttachmentFlags::DEPTH_TEXTURE | rendering::DepthAttachmentFlags::DEPTH_MULTISAMPLED, final_info, 8);

    rendering::FramebufferHandle final_framebuffer = rendering::create_framebuffer(final_info, renderer);
    rendering::set_final_framebuffer(renderer, final_framebuffer);

    // UI
    renderer->render.ui.top_left_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP | rendering::UIAlignment::LEFT, true);
    renderer->render.ui.top_right_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP | rendering::UIAlignment::RIGHT, true);
    renderer->render.ui.bottom_left_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM | rendering::UIAlignment::LEFT, true);
    renderer->render.ui.bottom_right_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM | rendering::UIAlignment::RIGHT, true);
    renderer->render.ui.top_x_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::TOP, true);
    renderer->render.ui.bottom_x_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::BOTTOM, true);
    renderer->render.ui.left_y_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::LEFT, true);
    renderer->render.ui.right_y_centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, rendering::UIAlignment::RIGHT, true);
    renderer->render.ui.centered_textured_quad_buffer = rendering::create_quad_buffer(renderer, 0, true);

    rendering::create_ui_render_pass(renderer);
    renderer->render.ui_quad_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/ui_quad.shd");
    renderer->render.textured_ui_quad_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/ui_texture_quad.shd");
    renderer->render.ui.material = rendering::create_material(renderer, renderer->render.ui_quad_shader);
    renderer->render.ui.textured_material = rendering::create_material(renderer, renderer->render.textured_ui_quad_shader);
    
    // Initialize font material
    renderer->render.font_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/font.shd");
    renderer->render.ui.font_material = rendering::create_material(renderer, renderer->render.font_shader);

    renderer->render.font3d_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/3d_font.shd");
    renderer->render.ui.font3d_material = rendering::create_material(renderer, renderer->render.font3d_shader);

    rendering::RegisterBufferInfo line_info = rendering::create_register_buffer_info();

    add_vertex_attrib(rendering::ValueType::FLOAT3, line_info);
    line_info.usage = rendering::BufferUsage::DYNAMIC;

    renderer->render.line_buffer = rendering::register_buffer(renderer, line_info);
    
    rendering::RegisterBufferInfo font_info = rendering::create_register_buffer_info();

    add_vertex_attrib(rendering::ValueType::FLOAT4, font_info);
    font_info.usage = rendering::BufferUsage::DYNAMIC;

    renderer->render.ui.font_buffer = rendering::register_buffer(renderer, font_info);

    // //HDR PP
    // rendering::FramebufferInfo hdr_info = rendering::generate_framebuffer_info();
    // hdr_info.width = renderer->framebuffer_width;
    // hdr_info.height = renderer->framebuffer_height;

    // rendering::add_color_attachment(rendering::ColorAttachmentType::TEXTURE, rendering::ColorAttachmentFlags::HDR | rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, hdr_info, 0);
    // rendering::add_color_attachment(rendering::ColorAttachmentType::TEXTURE, rendering::ColorAttachmentFlags::HDR | rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, hdr_info, 0);

    // rendering::FramebufferHandle hdr_fbo = rendering::create_framebuffer(hdr_info, renderer);

    // rendering::ShaderHandle hdr_shader = rendering::load_shader(renderer, "../engine_assets/standard_shaders/new_hdr.shd");

    // rendering::PostProcessingRenderPassHandle hdr_pass = rendering::create_post_processing_render_pass("HDR Pass", hdr_fbo, renderer, hdr_shader);
    // // rendering::PostProcessingRenderPassHandle hdr_pass = rendering::create_post_processing_render_pass("HDR Pass", final_framebuffer, renderer, hdr_shader);
    // rendering::set_uniform_value(renderer, hdr_pass, "scene", rendering::get_texture_from_framebuffer(0, framebuffer, renderer));
    // rendering::set_uniform_value(renderer, hdr_pass, "width", (i32)hdr_info.width);
    // rendering::set_uniform_value(renderer, hdr_pass, "height", (i32)hdr_info.height);
    // rendering::set_uniform_value(renderer, hdr_pass, "exposure", 1.8f); // @Incomplete: Hardcoded

    //Pass through post processing

    // rendering::FramebufferInfo blur_framebuffer = rendering::generate_framebuffer_info();
    // blur_framebuffer.width = renderer->framebuffer_width;
    // blur_framebuffer.height = renderer->framebuffer_height;
    // rendering::add_color_attachment(rendering::AttachmentType::TEXTURE, rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, blur_framebuffer, 0);

    // rendering::FramebufferHandle blur_handle = rendering::create_framebuffer(blur_framebuffer, renderer);
    
    // renderer->render.emissive_pass = rendering::create_render_pass("emissive_pass", blur_handle, renderer);
    // rendering::set_render_pass_clear_color(renderer->render.emissive_pass, math::Rgba(0, 0, 0, 0), renderer);
    
    // BLOOM
    rendering::FramebufferInfo blur_info = rendering::generate_framebuffer_info();
    blur_info.width = renderer->framebuffer_width;
    blur_info.height = renderer->framebuffer_height;

    rendering::add_color_attachment(rendering::AttachmentType::TEXTURE, rendering::ColorAttachmentFlags::CLAMP_TO_EDGE, blur_info);

    rendering::TextureHandle src_tex = rendering::get_texture_from_framebuffer(1, standard_resolve_framebuffer, renderer);

    b32 horizontal = true;

    rendering::FramebufferHandle blur_fbo[2] = { rendering::create_framebuffer(blur_info, renderer), rendering::create_framebuffer(blur_info, renderer) };
    
    renderer->render.bloom.active = true;
    renderer->render.bloom.exposure = 1.8f;
    renderer->render.bloom.amount = 5;
    
    for(i32 i = 0; i < renderer->render.bloom.amount; i++)
    {
        // @Incomplete: Duplicate names for passes?
        rendering::PostProcessingRenderPassHandle blur = rendering::create_post_processing_render_pass("Bloom_Blur", blur_fbo[horizontal], renderer, blur_shader);

        rendering::set_uniform_value(renderer, blur, "image", src_tex);
        rendering::set_uniform_value(renderer, blur, "horizontal", horizontal);

        src_tex = rendering::get_texture_from_framebuffer(0, blur_fbo[horizontal], renderer);
        horizontal = !horizontal;
    }

    rendering::PostProcessingRenderPassHandle final_blur_pass = rendering::create_post_processing_render_pass("Blur_To_Final", final_framebuffer, renderer, bloom_shader);
    rendering::set_uniform_value(renderer, final_blur_pass, "scene", rendering::get_texture_from_framebuffer(0, standard_resolve_framebuffer, renderer));
    rendering::set_uniform_value(renderer, final_blur_pass, "depth", rendering::get_depth_texture_from_framebuffer(0, standard_resolve_framebuffer, renderer));
    rendering::set_uniform_value(renderer, final_blur_pass, "blur", src_tex);

//     rendering::set_uniform_value(renderer, bloom, "bloom", renderer->render.bloom.active);
//     rendering::set_uniform_value(renderer, bloom, "exposure", renderer->render.bloom.exposure);

    //END BLOOM

    // Add tonemapping pass?

    renderer->render.instancing.internal_float_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer->render.instancing.internal_float2_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer->render.instancing.internal_float3_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer->render.instancing.internal_float4_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);
    renderer->render.instancing.internal_mat4_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, Buffer*);

    renderer->render.instancing.dirty_float_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.dirty_float2_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.dirty_float3_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.dirty_float4_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.dirty_mat4_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);

    renderer->render.instancing.free_float_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.free_float2_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.free_float3_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.free_float4_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);
    renderer->render.instancing.free_mat4_buffers = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, b32);

    renderer->render.instancing.float_buffer_counts = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.float2_buffer_counts = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.float3_buffer_counts = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.float4_buffer_counts = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.mat4_buffer_counts = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);

    renderer->render.instancing.float_buffer_max = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.float2_buffer_max = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.float3_buffer_max = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.float4_buffer_max = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);
    renderer->render.instancing.mat4_buffer_max = push_array(&renderer->buffer_arena, MAX_INSTANCE_BUFFERS, i32);

    for(i32 i = 0; i < MAX_INSTANCE_BUFFERS; i++)
    {
        renderer->render.instancing.free_float_buffers[i] = true;
        renderer->render.instancing.free_float2_buffers[i] = true;
        renderer->render.instancing.free_float3_buffers[i] = true;
        renderer->render.instancing.free_float4_buffers[i] = true;
        renderer->render.instancing.free_mat4_buffers[i] = true;
        
        renderer->render.instancing.dirty_float_buffers[i] = false;
        renderer->render.instancing.dirty_float2_buffers[i] = false;
        renderer->render.instancing.dirty_float3_buffers[i] = false;
        renderer->render.instancing.dirty_float4_buffers[i] = false;
        renderer->render.instancing.dirty_mat4_buffers[i] = false;

        renderer->render.instancing.internal_float_buffers[i] = push_struct(&renderer->buffer_arena, Buffer);
        renderer->render.instancing.internal_float2_buffers[i] = push_struct(&renderer->buffer_arena, Buffer);
        renderer->render.instancing.internal_float3_buffers[i] = push_struct(&renderer->buffer_arena, Buffer);
        renderer->render.instancing.internal_float4_buffers[i] = push_struct(&renderer->buffer_arena, Buffer);
        renderer->render.instancing.internal_mat4_buffers[i] = push_struct(&renderer->buffer_arena, Buffer);
    }

    TemporaryMemory temp_mem = begin_temporary_memory(&renderer->buffer_arena);
    
    rendering::RegisterBufferInfo particle_buffer = rendering::create_register_buffer_info();
    particle_buffer.usage = rendering::BufferUsage::STATIC;
    add_vertex_attrib(rendering::ValueType::FLOAT3, particle_buffer);

    i32 vertex_size = 3;
    particle_buffer.data.vertex_count = 4;
    particle_buffer.data.vertex_buffer_size = particle_buffer.data.vertex_count * vertex_size * (i32)(sizeof(r32));
    particle_buffer.data.vertex_buffer = push_size(&renderer->buffer_arena, particle_buffer.data.vertex_buffer_size, r32);
        
    r32 quad_vertices[12] =
        {
            -0.5f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
        };
    
    for(i32 i = 0; i < particle_buffer.data.vertex_count * vertex_size; i++)
    {
        particle_buffer.data.vertex_buffer[i] = quad_vertices[i];
    }

    i32 index_count = 6;
    particle_buffer.data.index_buffer_size = index_count * (i32)sizeof(u16);
    particle_buffer.data.index_buffer_count = index_count;

    particle_buffer.data.index_buffer = push_size(&renderer->buffer_arena, particle_buffer.data.index_buffer_size, u16);

    u16 quad_indices[6] =
        {
            0, 1, 2,
            0, 2, 3};
    
    for (i32 i = 0; i < index_count; i++)
    {
        particle_buffer.data.index_buffer[i] = quad_indices[i];
    }

    renderer->particles.quad_buffer = rendering::register_buffer(renderer, particle_buffer);
    
    add_vertex_attrib(rendering::ValueType::FLOAT2, particle_buffer);
    
    r32 textured_quad_vertices[20] =
    {
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f
    };

    vertex_size = 5;
    particle_buffer.data.vertex_buffer_size = particle_buffer.data.vertex_count * vertex_size * (i32)sizeof(r32);
    particle_buffer.data.vertex_buffer = push_size(&renderer->buffer_arena, particle_buffer.data.vertex_buffer_size, r32);

    for (i32 i = 0; i < particle_buffer.data.vertex_count * vertex_size; i++)
    {
        particle_buffer.data.vertex_buffer[i] = textured_quad_vertices[i];
    }

    renderer->particles.textured_quad_buffer = rendering::register_buffer(renderer, particle_buffer);

    end_temporary_memory(temp_mem);
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

    load_config("../.config", &core.config_data, &platform_state->perm_arena);

    init_keys();
    RenderState *render_state_ptr = push_struct(&platform_state->perm_arena, RenderState);
    RenderState &render_state = *render_state_ptr;
    render_state = {};
    render_state.v2 = {};

    for(i32 j = 0; j < MAX_FRAMEBUFFERS; j++)
    {
        Framebuffer &framebuffer = render_state.v2.framebuffers[j];
        for(i32 i = 0; i < 4; i++)
        {
            framebuffer.tex_color_buffer_handles[i] = 0;       
        }
    }

    render_state.should_close = false;
    render_state.dpi_scale = 0;
    render_state.window = nullptr;
    render_state.frame_delta = 0.0;

    render_state.string_arena = {};
    render_state.gl_shader_count = 0;
    render_state.gl_shaders = push_array(&platform_state->perm_arena, 64, ShaderGL);
  

    Renderer *renderer_alloc = push_struct(&platform_state->perm_arena, Renderer);
    Renderer *renderer = renderer_alloc;
    *renderer = {};

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
        initialize_opengl(render_state, renderer, &core.config_data, &platform_state->perm_arena, &do_save_config);
    }

    ParticleApi particle_api = {};
    particle_api.get_particle_system_info = &get_particle_system_info;
    particle_api.start_particle_system = &start_particle_system;
    particle_api.stop_particle_system = &stop_particle_system;
    particle_api.pause_particle_system = &pause_particle_system;
    particle_api.remove_all_particle_systems = &remove_all_particle_systems;
    particle_api.particle_system_is_running = &particle_system_is_running;
    
    particle_api.get_default_attributes = &get_default_particle_system_attributes;
    particle_api.create_particle_system = &create_particle_system;
    particle_api.remove_particle_system = &remove_particle_system;
    particle_api.update_particle_system = &update_particle_system;

    particle_api.add_burst_over_time_key = &add_burst_over_time_key;
    particle_api.add_angle_key = &add_angle_key;
    particle_api.remove_angle_key = &remove_angle_key;
    particle_api.add_color_key = &add_color_key;
    particle_api.remove_color_key = &remove_color_key;
    particle_api.add_size_key = &add_size_key;
    particle_api.remove_size_key = &remove_size_key;

    particle_api.add_speed_key = &add_speed_key;
    particle_api.remove_speed_key = &remove_speed_key;
    
    WorkQueue reload_queue;
    ThreadInfo reload_thread;
    init_renderer(renderer, &reload_queue, &reload_thread, particle_api);

    scene::SceneManager *scene_manager = scene::create_scene_manager(&platform_state->perm_arena, renderer);
    
    GameCode game = {};
    game.is_valid = false;

    load_game_code(game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
    TimerController *timer_controller_ptr = (TimerController*)malloc(sizeof(TimerController));
    TimerController &timer_controller = *timer_controller_ptr;
    timer_controller.timer_count = 0;
    timer_controller.paused = false;
    
    SoundDevice sound_device = {};
    sound_device.system = nullptr;
    
    debug_log("Initializing FMOD");

    sound_device.channel_count = 0;
    sound_device.sound_count = 0;
    sound_device.sfx_volume = core.config_data.sfx_volume;
    sound_device.music_volume = core.config_data.music_volume;
    sound_device.master_volume = core.config_data.master_volume;
    sound_device.muted = core.config_data.muted;

    sound::SoundSystem sound_system = {};
    sound_system.command_count = 0;
    sound_system.sound_count = 0;
    sound_system.commands = push_array(&sound_system.arena, global_max_sound_commands, sound::SoundCommand);
    sound_system.sounds = push_array(&sound_system.arena, global_max_sounds, sound::SoundHandle);
    sound_system.audio_sources = push_array(&sound_system.arena, global_max_audio_sources, sound::AudioSource);
    sound_system.channel_groups = push_array(&sound_system.arena, global_max_channel_groups, sound::ChannelGroup);
    sound_system.sfx_volume = core.config_data.sfx_volume;
    sound_system.music_volume = core.config_data.music_volume;
    sound_system.master_volume = core.config_data.master_volume;
    sound_system.muted = core.config_data.muted;

    sound::SoundSystemData data;
    data.device = &sound_device;
    data.system = &sound_system;
    
    /*WorkQueue fmod_queue = {};
    ThreadInfo fmod_thread = {};
    make_queue(&fmod_queue, 1, &fmod_thread);
    platform.add_entry(&fmod_queue, init_audio_fmod_thread, &data);
*/
	init_audio_fmod(&data);

    r64 last_second_check = get_time();
    i32 frames = 0;

    //i32 refresh_rate = render_state.refresh_rate;
    //u32 target_fps = (u32)refresh_rate;
    //r32 expected_frames_per_update = 1.0f;
    //r32 seconds_per_frame = expected_frames_per_update / target_fps;

    r64 last_frame = get_time();
    r64 delta_time = 0.0;
    renderer->frame_lock = 0;

    scene::EntityTemplateState template_state = {};
    template_state.template_count = 0;

    template_state.templates = push_array(&platform_state->perm_arena, global_max_entity_templates, scene::EntityTemplate);
    
    core.renderer = renderer;
    core.input_controller = &input_controller;
    core.timer_controller = &timer_controller;
    core.sound_system = &sound_system;
    core.scene_manager = scene_manager;
    core.delta_time = delta_time;
    core.current_time = get_time();
    game_memory.core = core;

    while (!should_close_window(render_state) && !renderer->should_close)
    {
        if (game_memory.exit_game)
        {
            debug_log("Quit\n");
            glfwSetWindowShouldClose(render_state.window, GLFW_TRUE);
        }
        
        //show_mouse_cursor(render_state, renderer->show_mouse_cursor);

        reload_libraries(&game, game_library_path, temp_game_library_path, &platform_state->perm_arena);
        //#endif
        //auto game_temp_mem = begin_temporary_memory(game_memory.temp_arena);

        poll_events();
        
        if (controller_present())
        {
            controller_keys(GLFW_JOYSTICK_1);
        }
        
        if(scene_manager->scene_loaded)
        {
            if(scene_manager->mode == scene::SceneMode::RUNNING)
            {
                game.update(&game_memory);
            }
            else
                game.update_editor(&game_memory);
            
            update_scene_editor(scene_manager->loaded_scene, &input_controller, delta_time);
            
            push_scene_for_rendering(scene::get_scene(scene_manager->loaded_scene), renderer);
        }
        else
            game.update(&game_memory);
        
        update_particle_systems(renderer, delta_time);

        tick_animation_controllers(renderer, &sound_system, &input_controller, timer_controller, delta_time);
        tick_timers(timer_controller, delta_time);
        update_sound_commands(&sound_device, &sound_system, delta_time, &do_save_config);

        render(render_state, renderer, delta_time);
        if (do_save_config)
        {
            save_config("../.config", renderer, &sound_device);
        }
        do_save_config = false;

        set_controller_invalid_keys();
        set_invalid_keys();
        set_mouse_invalid_keys();
        

        update_log();
        
        swap_buffers(render_state);

        frames++;
        r64 end_counter = get_time();
        if (end_counter - last_second_check >= 1.0)
        {
            last_second_check = end_counter;
            renderer->fps = frames;
            frames = 0;
        }

        delta_time = get_time() - last_frame;

        // @Incomplete: Fix this!
        delta_time = math::clamp(0.0, delta_time, 0.9);
        game_memory.core.delta_time = delta_time;
        game_memory.core.current_time = get_time();
        last_frame = end_counter;
    }

    close_log();
    cleanup_sound(&sound_device);
    close_window(render_state);
}
