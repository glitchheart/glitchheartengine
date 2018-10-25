#ifndef OPENGL_H
#define OPENGL_H

#include <GLFW/glfw3.h>

#define SHADERPAIR(name) {SHADER_ ## name, "Shader_" "" #name}

const static struct
{
    ShaderType val;
    char* str;
    
} shader_conversion [] =
{
    SHADERPAIR(MESH),
    SHADERPAIR(MESH_INSTANCED),
    SHADERPAIR(DEPTH),
    SHADERPAIR(DEPTH_INSTANCED),
    SHADERPAIR(QUAD),
    SHADERPAIR(TEXTURE_QUAD),
    SHADERPAIR(STANDARD_FONT),
    SHADERPAIR(SPRITESHEET),
    SHADERPAIR(FRAME_BUFFER),
    SHADERPAIR(SIMPLE_MODEL),
    SHADERPAIR(LINE),
    SHADERPAIR(PARTICLES),
    SHADERPAIR(ROUNDED_QUAD)
};

char* shader_enum_to_str(ShaderType shader)
{
    for(i32 index = 0; index < SHADER_COUNT; index++)
    {
        if(shader == shader_conversion[index].val)
        {
            return shader_conversion[index].str;
        }
    }
    assert(false);
    return nullptr;
}

static char* shader_paths[SHADER_COUNT] =
{
    "../engine_assets/shaders/meshshader",
    "../engine_assets/shaders/meshshaderinstanced",
    "../engine_assets/shaders/depthshader",
    "../engine_assets/shaders/depthshaderinstanced",
    "../engine_assets/shaders/quadshader",
    "../engine_assets/shaders/texturequadshader",
    "../engine_assets/shaders/standardfontshader",
    "../engine_assets/shaders/spritesheetanimationshader",
    "../engine_assets/shaders/framebuffershader",
    "../engine_assets/shaders/simple_model_shader",
    "../engine_assets/shaders/lineshader",
    "../engine_assets/shaders/particleshader",
    "../engine_assets/shaders/roundedquadshader",
};

struct Shader
{
    ShaderType type;
    b32 loaded;
    GLuint program;
    GLuint vertex_shader;
    GLuint fragment_shader;
    GLuint geometry_shader; // Optional

    struct
    {
        // Matrices
        GLint projection_matrix;

        // Color
        GLint diffuse_color;
        
        union
        {
            struct
            {
                GLint alpha_color;
                GLint z;
            
            } font;
        };
    } uniform_locations;
};

enum RenderMode
{
    RENDER_FILL, RENDER_OUTLINE
};

struct Texture 
{
    GLuint texture_handle;
};

// stb_truetype
struct GLFontBuffer
{
    GLuint texture;
    GLuint vao;
    GLuint vbo;
    
    struct
    {
        i32 width;
        i32 height;
    } resolution_loaded_for;
};

struct CharacterData
{
    r32 x;
    r32 y;
    r32 tx;
    r32 ty;
};

struct Buffer
{
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLint vertex_buffer_size;
    GLint index_buffer_size;
    GLint index_buffer_count;
};

struct Framebuffer
{
    GLuint buffer_handle;
    GLuint tex0_loc;
    GLuint tex_color_buffer_handle;
    GLuint depth_buffer_handle;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    
    b32 multisampled;
    
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
    CharacterData *character_buffer;
    GLFWwindow *window;
    i32 window_width;
    i32 window_height;
    i32 framebuffer_width;
    i32 framebuffer_height;
    i32 dpi_scale;
    r32 density_factor;
    i32 pixels_per_unit;
    
    i32 refresh_rate;
    
    b32 paused;
    
    r64 screen_dpi;
    
    r32 contrast;
    r32 brightness;
    
    char* window_title;
    
    r64 frame_delta;
    r64 total_delta;
    
    WindowMode window_mode;
    
    GLfloat scale_x;
    GLfloat scale_y;
    GLint viewport[4];
    r64 delta_time;
    
    b32 should_close;
    r64 fps;
    
    Framebuffer framebuffer;
    Framebuffer shadow_map_buffer;
    // Lighting data
    SpotlightData spotlight_data;
    DirectionalLightData directional_light_data;
    PointLightData point_light_data;
    
    u32 framebuffer_quad_vertices_size = 16 * sizeof(GLfloat);
    u32 texture_quad_vertices_size = 16 * sizeof(GLfloat);
    u32 rounded_quad_vertices_size = 16 * sizeof(GLfloat);
    u32 quad_vertices_size = 16 * sizeof(GLfloat);
    u32 billboard_vertices_size = 20 * sizeof(GLfloat);
    GLuint bound_vertex_buffer;
    GLuint bound_texture;
    
    GLfloat framebuffer_quad_vertices[16] =
    {
        -1.0f, 1.0f, 0, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0, 0.0f
    };
    
    GLfloat quad_vertices[16] =
    {
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };
    
    GLfloat billboard_vertices[20] =
    {
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f
    };
    
    GLfloat texture_quad_vertices[16] =
    {
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    GLfloat rounded_quad_vertices[16] =
    {
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    GLuint quad_indices[6] =
    {
        0, 1, 2, 0, 2, 3
    };
    
    GLuint texture_quad_vbo;
    GLuint texture_quad_index_buffer;
    
    GLuint rounded_quad_vao;
    GLuint rounded_quad_vbo;
    GLuint rounded_quad_index_buffer;
    GLuint quad_index_buffer;
    
    GLuint billboard_vao;
    GLuint billboard_vbo;
    GLuint billboard_ibo;
    
    GLuint line_vbo;
    GLuint line_vao;
    GLuint line_ebo;
    
#define LINE_INDICES 6
    
    GLuint line_indices[LINE_INDICES] = 
    {
        0, 1, 2,
        1, 2, 3
    };
    
    Buffer *buffers;
    i32 buffer_count;
    
    GLuint quad_vao;
    GLuint texture_quad_vao;
    GLuint quad_vbo;
    
    union 
    {
        Shader shaders[SHADER_COUNT];
        struct
        {
            Shader mesh_shader;
            Shader mesh_instanced_shader;
            Shader depth_shader;
            Shader depth_instanced_shader;
            Shader quad_shader;
            Shader texture_quad_shader;
            Shader standard_font_shader;
            Shader spritesheet_shader;
            Shader frame_buffer_shader;
            Shader simple_model_shader;
            Shader line_shader;
            Shader particle_shader;
            Shader rounded_quad_shader;
        };
    };
    
    Shader extra_shaders[150];
    i32 extra_shader_index;
    
    i32 current_extra_shader;
    ShaderAttribute* shader_attributes;
    i32 shader_attribute_count;
    
    Texture texture_array[150];
    i32 texture_index;
    
    GLFontBuffer gl_fonts[64];
    i32 font_count;
    
    GLFWcursor* cursors[6];
    
    struct
    {
        math::Rgba specular_color;
        math::Rgba diffuse_color;
        math::Rgba ambient_color;
        math::Vec3 position;
    } sun_light;
    
    RenderState() {}
    
    MemoryArena* perm_arena; // TODO: Make this into a framebuffer arena maybe?
    MemoryArena framebuffer_arena;
    MemoryArena font_arena;
    MemoryArena string_arena;
    //MemoryArena perm_arena;
};

#endif
