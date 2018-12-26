#ifndef OPENGL_H
#define OPENGL_H

#include <GLFW/glfw3.h>

struct ShaderGL
{
	GLuint program;
	
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
};

struct Framebuffer
{
    u32 width;
    u32 height;
    
    GLuint buffer_handle;
    GLuint tex0_loc;
    GLuint tex_color_buffer_handles[4];
    i32 tex_color_buffer_count;
    GLuint depth_buffer_handle;
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
    struct
    {
        GLuint shader_program;
        GLuint vao;
    } current_state;

    GLFWwindow *window;

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
    
    WindowMode window_mode;

    GLint viewport[4];
    r64 delta_time;
    
    b32 should_close;
    r64 fps;

    struct
    {
#define MAX_FRAMEBUFFERS 16
        Framebuffer framebuffers[MAX_FRAMEBUFFERS];
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

	Buffer *gl_buffers;
	i32 gl_buffer_count;
    
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
