#ifndef OPENGL_H
#define OPENGL_H

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#endif

#include <GLFW/glfw3.h>

// IMGUI

struct InternalImGuiState
{
    char glsl_version_string[32] = "";
    GLuint font_texture = 0;
    GLuint shader_handle = 0;
    GLuint vert_handle = 0;
    GLuint frag_handle = 0;
    i32 attrib_location_tex = 0;
    i32 attrib_location_proj_mtx = 0;
    i32 attrib_location_vtx_pos = 0;
    i32 attrib_location_vtx_uv = 0;
    i32 attrib_location_vtx_color = 0;
    u32 vbo_handle = 0;
    u32 elements_handle = 0;
};

// ENDIMGUI

struct ShaderGL
{
	GLuint program;
    int handle;
	
	GLuint vert_program;
    GLuint geo_program;
	GLuint frag_program;

    GLint uniform_locations[1024];
    i32 location_count;
};

struct Texture 
{
    GLuint handle;

    i32 width;
    i32 height;
};

struct Buffer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLint vertex_buffer_size;
    GLint vertex_count;
    GLint index_buffer_size;
    GLint index_buffer_count;

    size_t size;
    GLenum usage;
};

struct UniformBuffer
{
    GLuint ubo;
    size_t size;
    GLenum usage;
    u8* memory;
};

struct Framebuffer
{
    u32 width;
    u32 height;
    
    GLuint buffer_handle;
    GLuint tex0_loc;
    GLuint tex_color_buffer_handles[4];
    i32 tex_color_buffer_count;
    
    GLuint depth_buffer_handles[4];
    i32 depth_buffer_count;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    
    b32 multisampled;
    i32 samples;
    b32 hdr;
    
    b32 has_shadow_map;
    
    struct
    {
        i32 width;
        i32 height;
    } shadow_map;
    
    GLuint shadow_map_handle;
};

struct RenderState
{
    InternalImGuiState imgui_state;
    
    struct
    {
        GLuint shader_program;
        GLuint vao;
    } current_state;

    GLFWwindow *window;

    b32 vsync_active;

    i32 window_width;
    i32 window_height;
    i32 framebuffer_width;
    i32 framebuffer_height;
    i32 dpi_scale;
    r32 density_factor;
    
    i32 refresh_rate;
    
    b32 paused;
    
    r64 screen_dpi;
    
    r32 contrast;
    r32 brightness;
    
    char* window_title;
    
    r64 frame_delta;
    r64 total_delta;
    
    GLint viewport[4];
    r64 delta_time;
    
    b32 should_close;
    b32 mouse_locked;
    r64 fps;

    struct
    {
        Framebuffer framebuffers[global_max_framebuffers];
        i32 framebuffer_count;
    } v2;

    Framebuffer shadow_map_buffer;
    
    u32 framebuffer_quad_vertices_size = 16 * sizeof(GLfloat);
    GLuint bound_vertex_buffer;
    GLuint bound_texture;
    
    GLfloat framebuffer_quad_vertices[16] =
    {
        -1.0f, 1.0f, 0, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0, 0.0f
    };

    GLuint quad_indices[6] =
    {
        0, 1, 2, 0, 2, 3
    };
    
    GLuint quad_index_buffer;
    
    GLuint framebuffer_quad_vao;
    GLuint framebuffer_quad_vbo;
    
	ShaderGL *gl_shaders;
	i32 gl_shader_count;
    
    GLFWcursor* cursors[6];
    
    RenderState() {}
    
    MemoryArena* perm_arena; // TODO: Make this into a framebuffer arena maybe?
    MemoryArena framebuffer_arena;
    MemoryArena string_arena;
};

struct RenderingState
{
    RenderState *render_state;
    Renderer *renderer;
};

RenderingState rendering_state;

#endif
