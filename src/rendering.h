#ifndef RENDERING_H
#define RENDERING_H

#define UI_COORD_DIMENSION 1000.0f
#define MAX_MATERIAL_INSTANCE_ARRAYS 8

#define STB_TRUETYPE_IMPLEMENTATION
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4365) // int conversions
#pragma warning(disable : 4459)
#endif
#include "stb/stb_truetype.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

struct TrueTypeFontInfo
{
	i32 ascent;
	i32 descent;
	i32 line_gap;
	r32 line_height;
	r32 scale;
	i32 baseline;
	i32 first_char;
	i32 char_count;
	i32 size;
    i32 load_size;
	i32 atlas_width;
	i32 atlas_height;
	u32 oversample_x;
	u32 oversample_y;
	r32 largest_character_height;
	stbtt_fontinfo info;
	stbtt_packedchar char_data['~' - ' '];

    struct
    {
        i32 width;
        i32 height;
    } resolution_loaded_for;

    char path[256];
    rendering::TextureHandle texture;

    unsigned char *ttf_buffer;
};

struct DirectionalLight
{
    math::Vec3 direction;
    math::Vec3 ambient;
    math::Vec3 diffuse;
    math::Vec3 specular;
};

struct PointLight
{
    math::Vec3 position;
    r32 constant;
    r32 linear;
    r32 quadratic;
    math::Vec3 ambient;
    math::Vec3 diffuse;
    math::Vec3 specular;
};

// @Incomplete: Remove the prefix from the values 
enum RelativeFlag
{
    RELATIVE_TOP,
    RELATIVE_LEFT,
    RELATIVE_RIGHT,
    RELATIVE_BOTTOM
};

struct RelativeUIQuadInfo
{
	math::Vec2 position;
	math::Vec2 scale;
	math::Vec2 ui_position;
};

r32 plane_vertices[] =
{
    -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, -0.5f,
    0.5f, 0.0f,  0.5f,
    0.5f, 0.0f,  0.5f,  
    -0.5f, 0.0f,  0.5f, 
    -0.5f, 0.0f, -0.5f
};

u16 plane_indices[] = 
{
    /* 0, 1, 2, */
    /* 3, 4, 5 */
    2, 1, 0,
    5, 4, 3
};

r32 plane_normals[] =
{
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f
};

r32 plane_uvs[] =
{
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
    0.0f, 0.0f, 
    1.0f, 0.0f, 
    1.0f, 1.0f
};

r32 cube_normals[] =
{
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f, 
    
    
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f, 

    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,

    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,

    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,

    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f
};

r32 cube_vertices[] =
{
    -0.5f, -0.5f, -0.5f,  
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,  

    0.5f,  0.5f, -0.5f, 
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    
    0.5f,  0.5f,  0.5f, 
    -0.5f, -0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f,
    
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    
    0.5f,  0.5f,  0.5f, 
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f, -0.5f,
    
    0.5f, -0.5f, -0.5f,
    0.5f,  0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f, -0.5f, 
    -0.5f, -0.5f,  0.5f, 

    -0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f,  0.5f,
    0.5f,  0.5f, -0.5f, 

    0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f, -0.5f,  
    -0.5f,  0.5f,  0.5f
};

r32 cube_uvs[] =
{
    0.000059f, 1.0f - 0.000004f,
    0.335973f, 1.0f - 0.335903f,
    0.000103f, 1.0f - 0.336048f,
    
    1.000023f, 1.0f - 0.000013f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,

    0.667979f, 1.0f - 0.335851f,
    0.667969f, 1.0f - 0.671889f,
    0.336024f, 1.0f - 0.671877f,

    1.000023f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.668104f, 1.0f - 0.000013f,

    0.000059f, 1.0f - 0.000004f,
    0.336098f, 1.0f - 0.000071f,
    0.335973f, 1.0f - 0.335903f,

    0.667979f, 1.0f - 0.335851f,
    0.336024f, 1.0f - 0.671877f,
    0.335973f, 1.0f - 0.335903f,

    1.000004f, 1.0f - 0.671847f,
    0.667979f, 1.0f - 0.335851f,
    0.999958f, 1.0f - 0.336064f,

    0.668104f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,

    0.335973f, 1.0f - 0.335903f,
    0.336098f, 1.0f - 0.000071f,
    0.668104f, 1.0f - 0.000013f,

    0.000103f, 1.0f - 0.336048f,
    0.336024f, 1.0f - 0.671877f,
    0.000004f, 1.0f - 0.671870f,
    
    0.000103f, 1.0f - 0.336048f,
    0.335973f, 1.0f - 0.335903f,
    0.336024f, 1.0f - 0.671877f,
    
    0.667969f, 1.0f - 0.671889f,
    0.667979f, 1.0f - 0.335851f,
    1.000004f, 1.0f - 0.671847f
};

u16 cube_indices[] = 
{
    0, 1, 2,
    3, 4, 5,
    6, 7, 8,
    9, 10, 11,
    12, 13, 14,
    15, 16, 17,
    
    18, 19, 20,
    21, 22, 23,
    24, 25, 26,
    27, 28, 29,
    30, 31, 32,
    33, 34, 35
};

r32 tetrahedron_vertices[] =
{
    // Bottom
    -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    -0.5f, 0.0f, -1.0f,
    
    //Sides
    -0.5f, 1.0f, -0.5f,
    -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    
    -0.5f, 1.0f, -0.5f,
    0.0f, 0.0f,  0.0f,
    -0.5f, 0.0f, -1.0f,
    
    -0.5f, 1.0f, -0.5f,
    -1.0f, 0.0f, 0.0f,
    -0.5f, 0.0f, -1.0f
};

u16 tetrahedron_indices[] =
{
    0, 1, 2, 
    3, 4, 5,
    6, 7, 8,
    9, 10, 11
};

r32 tetrahedron_normals[] =
{
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    
    -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};

struct Vertex
{
	math::Vec3 position;
	math::Vec2 uv;
	math::Vec3 normal;
	math::Rgba color;
};

struct Face
{
	u16 indices[3];
	math::Vec3 normal;
};

enum RenderMaterialType
{
    RM_INVALID,
    RM_COLOR,
    RM_TEXTURED
};

struct Mesh
{
	i32 buffer_handle;
    
	Vertex* vertices;
	i32 vertex_count;
    
	Face* faces;
	i32 face_count;
    
	i32 instance_offset_buffer_handle;
	i32 instance_color_buffer_handle;
	i32 instance_rotation_buffer_handle;
	i32 instance_scale_buffer_handle;
};

struct CombinedCommand
{
    rendering::MaterialInstanceHandle material_handle;
    rendering::Transform transform;
    b32 casts_shadows;
};

struct QueuedRenderCommand
{
    rendering::BufferHandle buffer_handle;
    rendering::MaterialHandle original_material;
    b32 ignore_depth;

    CombinedCommand commands[1024];
    i32 count;
};

enum CommandBlendMode
{
    ONE,
    ONE_MINUS_SRC_ALPHA
};

enum FadingMode
{
    FADING_NONE,
    FADING_IN,
    FADING_OUT,
    FADING_OUT_IN
};

enum UIScalingFlag
{
    KEEP_ASPECT_RATIO = (1 << 0),
    NO_SCALING = (1 << 1),
    SCALE_WITH_WIDTH = (1 << 2),
    SCALE_WITH_HEIGHT = (1 << 3),
};

enum TextureFiltering
{
    LINEAR,
    NEAREST
};

enum TextureWrap
{
    REPEAT,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
};

enum TextureFormat
{
    RGBA,
    RGB,
    RED
};

enum class TextureUsage
{
    STATIC,
    DYNAMIC
};

struct Resolution
{
	i32 width;
	i32 height;
};

//@Cleanup: Move out of rendering
enum CursorType
{
    CURSOR_ARROW,
    CURSOR_CROSSHAIR,
    CURSOR_HAND,
    CURSOR_HRESIZE,
    CURSOR_IBEAM,
    CURSOR_VRESIZE
};

struct ParticleSystemInfo;

struct RenderState;
struct Buffer;
struct UniformBuffer;
struct Texture;
struct Framebuffer;

//@Cleanup: Move out of rendering
typedef void (*SetWindowCursor)(RenderState* render_state, CursorType cursor);

typedef math::Vec2i (*GetTextureSize)(Texture* texture);
typedef void (*LoadTexture)(Texture* texture, TextureFiltering filtering, TextureWrap wrap, TextureFormat format, i32 width, i32 height, unsigned char* image_data, RenderState* render_state, Renderer* renderer, TextureUsage usage);
typedef void (*CreateFramebuffer)(rendering::FramebufferInfo &framebuffer_info, RenderState *render_state, Renderer *renderer);
typedef void (*ReloadFramebuffer)(rendering::FramebufferHandle handle, RenderState* render_state, Renderer* renderer, i32 width, i32 height);
typedef void(*CreateInstanceBuffer)(Buffer *buffer, size_t buffer_size, rendering::BufferUsage usage, RenderState *render_state, Renderer *renderer);
typedef rendering::BufferUsage (*GetBufferUsage)(Buffer *buffer);
typedef void (*DeleteInstanceBuffer)(Buffer *buffer, RenderState *render_state, Renderer *renderer);
typedef void (*DeleteAllInstanceBuffers)(RenderState *render_state, Renderer *renderer);
typedef void (*UpdateBuffer)(Buffer *buffer, rendering::BufferType buffer_type, void *data, size_t count, size_t size, rendering::BufferUsage buffer_usage, RenderState *render_state, Renderer *renderer);
typedef void (*CreateBuffer)(Buffer *buffer, rendering::RegisterBufferInfo info, RenderState *render_state, Renderer *renderer);
typedef void (*DeleteBuffer)(Buffer *buffer, RenderState *render_state, Renderer *renderer);
typedef void (*SetMouseLock)(b32 locked, RenderState *render_state);
typedef void (*ShowMouseCursor)(b32 show, RenderState *render_state);
typedef b32 (*GetMouseLock)(RenderState *render_state);
typedef void (*SetWindowMode)(RenderState* render_state, Renderer* renderer, Resolution resolution, WindowMode window_mode);
typedef void (*SetVSync)(RenderState *render_state, b32 value);
typedef b32 (*GetVSync)(RenderState *render_state);
typedef void (*LoadShader)(RenderState *render_state, Renderer *renderer, rendering::Shader &shader);

typedef void (*CreateUniformBuffer)(UniformBuffer* buffer, rendering::BufferUsage, size_t size, Renderer *renderer);
typedef void (*UpdateUniformBuffer)(UniformBuffer* buffer, rendering::UniformBufferUpdate update, Renderer *renderer);


struct GraphicsAPI
{
    SetWindowCursor set_window_cursor;
    GetTextureSize get_texture_size;
    LoadTexture load_texture;
    CreateFramebuffer create_framebuffer;
    ReloadFramebuffer reload_framebuffer;
	CreateInstanceBuffer create_instance_buffer;
    CreateUniformBuffer create_uniform_buffer;
    UpdateUniformBuffer update_uniform_buffer;
	GetBufferUsage get_buffer_usage;
    DeleteInstanceBuffer delete_instance_buffer;
    DeleteAllInstanceBuffers delete_all_instance_buffers;

    UpdateBuffer update_buffer;
    CreateBuffer create_buffer;
    DeleteBuffer delete_buffer;
    
    SetMouseLock set_mouse_lock;
    GetMouseLock get_mouse_lock;

    ShowMouseCursor show_mouse_cursor;

    SetWindowMode set_window_mode;
    SetVSync set_v_sync;
    GetVSync get_v_sync;

    LoadShader load_shader;
    
    RenderState *render_state;
};

struct ParticleApi;

struct Pass
{
    rendering::RenderPassHandle pass_handle;
    QueuedRenderCommand queued_commands[64];
    i32 command_count;
};

struct Renderer
{
    GraphicsAPI api_functions;
    
	b32 should_close;
	r64 fps;
	r64 average_fps;
	r64 fps_sum;
	u64 current_frame;
	i32 pixels_per_unit;
	i32 frame_lock;
    
	WindowMode window_mode;

    /* Camera camera; */
	
	MemoryArena command_arena;
    
	Mesh *meshes;
	i32 mesh_count;
    
	struct
	{
		i32 *_internal_handles;
		i32 _current_internal_handle;
		i32 _max_particle_system_count;
        
		ParticleSystemInfo *particle_systems;
		i32 particle_system_count;

		RandomSeries* entropy;
        rendering::BufferHandle quad_buffer;
        rendering::BufferHandle textured_quad_buffer;

        i32 *_internal_work_queue_handles;
        i32 _current_internal_work_queue_handle;
        
        WorkQueue *work_queues;
        WorkQueue *system_work_queue;
        ThreadInfo *system_threads;
        
        ParticleApi *api;
	} particles;
    
	i32 current_camera_handle;
    
	AnimationController* animation_controllers;
	i32 animation_controller_count;
    
	SpritesheetAnimation *spritesheet_animations;
	i32 spritesheet_animation_count;
    
	union
	{
		i32 viewport[4];
		math::Vec4i v;
		struct
		{
			i32 viewport_x;
			i32 viewport_y;
			i32 viewport_width;
			i32 viewport_height;
		};
	};
    
	union
	{
		Resolution resolution;
		struct
		{
			i32 window_width;
			i32 window_height;
		};
	};
    
	Resolution *available_resolutions;
	i32 available_resolutions_count;
	i32 current_resolution_index;
    
	math::Rgba clear_color;
    
	b32 show_mouse_cursor;
    
	TrueTypeFontInfo *tt_font_infos;
	i32 tt_font_count;
    
	i32 framebuffer_width;
	i32 framebuffer_height;
    
	MemoryArena mesh_arena;
    MemoryArena material_arena;
    MemoryArena light_arena;
	MemoryArena texture_arena;
	MemoryArena animation_arena;
	MemoryArena font_arena;
	MemoryArena particle_arena;
	MemoryArena buffer_arena;
    MemoryArena ubo_arena;
	MemoryArena shader_arena;
    
	MemoryArena temp_arena;

	struct
	{
		rendering::Shader *shaders;
		i32 shader_count;

        rendering::ShaderHandle fallback_shader;
        rendering::ShaderHandle wireframe_shader;
        rendering::MaterialInstanceHandle wireframe_material;
        rendering::ShaderHandle bounding_box_shader;
        rendering::MaterialInstanceHandle bounding_box_material;
        rendering::BufferHandle bounding_box_buffer;
        rendering::ShaderHandle shadow_map_shader;
        rendering::ShaderHandle bloom_shader;
        rendering::ShaderHandle blur_shader;
        rendering::ShaderHandle hdr_shader;
        rendering::ShaderHandle ui_quad_shader;
        rendering::ShaderHandle textured_ui_quad_shader;
        rendering::ShaderHandle font_shader;
        rendering::ShaderHandle font3d_shader;
        
        rendering::MaterialInstanceHandle line_material;
        rendering::ShaderHandle line_shader;
        rendering::BufferHandle line_buffer;
        
		i32 shaders_to_reload[2];
		i32 shaders_to_reload_count;
	
		rendering::Material *materials;
		i32 material_count;

		rendering::Material *material_instances;
        i32 material_instance_count;
        i32 *_internal_material_instance_handles;
        i32 current_material_instance_index;

        Buffer **buffers;
        i32 buffer_count;
    
		i32 *_internal_buffer_handles;
		i32 _current_internal_buffer_handle;

		i32 *updated_buffer_handles;
		i32 updated_buffer_handle_count;
    
		i32 *removed_buffer_handles;
		i32 removed_buffer_handle_count;

        Mesh* loaded_meshes;
        i32 loaded_mesh_count;

        struct
        {
            r32 z_near;
            r32 z_far;
            r32 fov;
            i32 size;
        } shadow_settings;

        struct
        {
            b32 *dirty_float_buffers;
            b32 *dirty_float2_buffers;
            b32 *dirty_float3_buffers;
            b32 *dirty_float4_buffers;
            b32 *dirty_mat4_buffers;
            
            // All instance buffers
            r32 *float_buffers[MAX_INSTANCE_BUFFERS];
            math::Vec2 *float2_buffers[MAX_INSTANCE_BUFFERS];
            math::Vec3 *float3_buffers[MAX_INSTANCE_BUFFERS];
            math::Vec4 *float4_buffers[MAX_INSTANCE_BUFFERS];
            math::Mat4 *mat4_buffers[MAX_INSTANCE_BUFFERS];

            // All internal instance buffers (API specific)
            Buffer **internal_float_buffers;
            Buffer **internal_float2_buffers;
            Buffer **internal_float3_buffers;
            Buffer **internal_float4_buffers;
            Buffer **internal_mat4_buffers;

            // Flags to keep track of all free buffers
            b32 *free_float_buffers;
            b32 *free_float2_buffers;
            b32 *free_float3_buffers;
            b32 *free_float4_buffers;
            b32 *free_mat4_buffers;

            // The current count
            i32 *float_buffer_counts;
            i32 *float2_buffer_counts;
            i32 *float3_buffer_counts;
            i32 *float4_buffer_counts;
            i32 *mat4_buffer_counts;

            // The allocated max count
            i32 *float_buffer_max;
            i32 *float2_buffer_max;
            i32 *float3_buffer_max;
            i32 *float4_buffer_max;
            i32 *mat4_buffer_max;

            // The internal handle last used + 1
            i32 current_internal_float_handle;
            i32 current_internal_float2_handle;
            i32 current_internal_float3_handle;
            i32 current_internal_float4_handle;
            i32 current_internal_mat4_handle;
        } instancing;

        Texture **textures;
        i32 texture_count;

        rendering::RenderPass *passes;
        i32 pass_count;

        rendering::RenderPass post_processing_passes[16];
        i32 post_processing_pass_count;

        rendering::FramebufferInfo *framebuffers;
        i32 framebuffer_count;

        struct
        {
            rendering::BufferHandle top_left_textured_quad_buffer;
            rendering::BufferHandle top_right_textured_quad_buffer;
            rendering::BufferHandle bottom_left_textured_quad_buffer;
            rendering::BufferHandle bottom_right_textured_quad_buffer;
            rendering::BufferHandle top_x_centered_textured_quad_buffer;
            rendering::BufferHandle bottom_x_centered_textured_quad_buffer;
            rendering::BufferHandle left_y_centered_textured_quad_buffer;
            rendering::BufferHandle right_y_centered_textured_quad_buffer;
            rendering::BufferHandle centered_textured_quad_buffer;
            rendering::RenderPass pass;
            rendering::MaterialHandle material;
            rendering::MaterialHandle textured_material;

            rendering::MaterialHandle font_material;
            rendering::MaterialHandle font3d_material;
            rendering::BufferHandle font_buffer;
        } ui;

        MemoryArena render_pass_arena;
        
        QueuedRenderCommand *queued_commands;
        i32 queued_command_count;

        Pass *pass_commands;

#define MAX_CUSTOM_UNIFORM_MAPPINGS 64
        rendering::CustomUniformMapping *custom_mappings;
        i32 custom_mapping_count;

        rendering::ShadowCommand *shadow_commands;
        i32 shadow_command_count;

        DirectionalLight *directional_lights;
        i32 dir_light_count;

        PointLight *point_lights;
        i32 point_light_count;

        math::Mat4 light_space_matrix;
        math::Mat4 light_view_matrix;
        math::Vec3 shadow_view_position;

        struct
        {
            b32 active;
            r32 exposure;
            i32 amount;
        } bloom;

        struct
        {
            b32 active;
            r32 exposure;
        } hdr;

        rendering::FramebufferHandle final_framebuffer;
        rendering::FramebufferHandle shadow_framebuffer;
        rendering::RenderPassHandle emissive_pass;
        rendering::RenderPassHandle standard_pass;
        rendering::RenderPassHandle shadow_pass;
        rendering::MaterialHandle shadow_map_material;

        rendering::UniformBufferLayout ubo_layouts[(i32)rendering::UniformBufferMappingType::MAX];
        rendering::Structure ubo_struct_definitions[32];
        rendering::UniformValue ubo_struct_uniforms[32][16];
        i32 ubo_struct_count;

        rendering::UniformBufferMappingType ubo_struct_constructing;

        UniformBuffer ** uniform_buffers;
        i32 uniform_buffer_count;

        rendering::UniformBufferHandle mapped_ubos[(i32)rendering::UniformBufferMappingType::MAX];

        rendering::UniformValue ubo_uniforms[global_max_uniform_buffers][16];
        rendering::UniformArray ubo_array_uniforms[global_max_uniform_buffers][16];

        /* rendering::Structure structures[32]; */
        /* i32 structure_count; */
        
	} render;
};

#define MAX_LINES 16

struct LineData
{
	math::Vec2 line_sizes[MAX_LINES];
	i32 line_count;
	r32 total_height;
	r32 line_spacing;
};

struct TextLengthInfo
{
	size_t length;
    
	r32* widths;
};

#endif
