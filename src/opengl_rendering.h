#ifndef OPENGL_H
#define OPENGL_H

#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_TRUETYPE_IMPLEMENTATION
#pragma warning(push)
#pragma warning(disable : 4365) // int conversions
#pragma warning(disable : 4459)
#include "stb/stb_truetype.h"
#pragma warning(pop)

#define PIXELS_PER_UNIT 32

#define SHADERPAIR(name) {SHADER_ ## name, "Shader_" "" #name}

const static struct
{
    ShaderType val;
    char* str;
    
} shader_conversion [] =
{
    SHADERPAIR(TEXTURE),
    SHADERPAIR(TILE),
    SHADERPAIR(RECT),
    SHADERPAIR(TEXTURE_RECT),
    SHADERPAIR(STANDARD_FONT),
    SHADERPAIR(SPRITESHEET),
    SHADERPAIR(WIREFRAME),
    SHADERPAIR(UI_SPRITE),
    SHADERPAIR(ERROR_SPRITE),
    SHADERPAIR(ERROR_UI),
    SHADERPAIR(FRAME_BUFFER),
    SHADERPAIR(LIGHT_SOURCE),
    SHADERPAIR(SIMPLE_MODEL),
    SHADERPAIR(PASS_THROUGH),
    SHADERPAIR(LINE)
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
    Assert(false);
    return 0;
}

static char* shader_paths[SHADER_COUNT] =
{
    "../engine_assets/shaders/textureshader",
    "../engine_assets/shaders/tileshader",
    "../engine_assets/shaders/rectshader",
    "../engine_assets/shaders/texturerectshader",
    "../engine_assets/shaders/standardfontshader",
    "../engine_assets/shaders/spritesheetanimationshader",
    "../engine_assets/shaders/wireframeshader",
    "../engine_assets/shaders/spriteuishader",
    "../engine_assets/shaders/errorshadersprite",
    "../engine_assets/shaders/errorshaderui",
    "../engine_assets/shaders/framebuffershader",
    "../engine_assets/shaders/lightsourceshader",
    "../engine_assets/shaders/simple_model_shader",
    "../engine_assets/shaders/passthroughshader",
    "../engine_assets/shaders/lineshader"
};


enum RenderMode
{
    RENDER_FILL, RENDER_OUTLINE
};

struct Entity;
struct ObjectEntity;

struct Texture 
{
    GLuint texture_handle;
};

#define TILESHEET_MAX 10

struct Tilesheet
{
    char* name;
    Texture texture;
    i32 tile_width;
    i32 tile_height;
};

// stb_truetype
struct TrueTypeFont
{
    // @Incomplete: 96 is not always enough!
    stbtt_bakedchar char_data[96];
    GLuint texture;
};

// FreeType
struct RenderFont
{
    FT_Face face;
    GLuint vao;
    GLuint vbo;
    GLuint texture;
    u32 atlas_width;
    u32 atlas_height;
    GLfloat glyph_width;
    math::Vec4 color = math::Vec4(1, 1, 1, 1);
    math::Vec4 alpha_color = math::Vec4(1, 1, 1, 1);
    
    struct CharacterInfo 
    {
        r32 ax; // advance.x
        r32 ay; // advance.y
        
        r32 bw; // bitmap.width;
        r32 bh; // bitmap.rows;
        
        r32 bl; // bitmap_left;
        r32 bt; // bitmap_top;
        
        r32 tx;
    } character_info[255];
};

struct Point
{
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
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

struct RenderState
{
    GLFWwindow *window;
    i32 window_width;
    i32 window_height;
    i32 screen_width;
    i32 screen_height;
    i32 scale_from_width;
    i32 scale_from_height;
    i32 dpi_scale;
    i32 pixels_per_unit;
    
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
    
    GLuint original_frame_buffer_vao;
    GLuint original_frame_buffer_vbo;
    GLuint original_frame_buffer;
    
    GLuint frame_buffer_vao;
    GLuint frame_buffer_vbo;
    GLuint frame_buffer;
    GLuint frame_buffer_tex0_loc;
    GLuint frame_buffer_tex1_loc;
    GLuint texture_color_buffer;
    
    // Lighting data
    SpotlightData spotlight_data;
    DirectionalLightData directional_light_data;
    PointLightData point_light_data;
    
    GLuint spotlight_ubo;
    GLuint directional_light_ubo;
    GLuint point_light_ubo;
    
    // Lighting map
    GLuint lighting_frame_buffer;
    GLuint lighting_texture_color_buffer;
    
    size_t sprite_quad_vertices_size = 16 * sizeof(GLfloat);
    size_t tile_quad_vertices_size = 16 * sizeof(GLfloat);
    size_t normal_quad_vertices_size = 8 * sizeof(GLfloat);
    size_t wireframe_quad_vertices_size = 8 * sizeof(GLfloat);
    GLuint bound_vertex_buffer;
    GLuint bound_texture;
    
    GLfloat frame_buffer_vertices[16] =
    {
        -1.0f, 1.0f, 0, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0, 0.0f
    };
    
    GLfloat sprite_quad_vertices[16] =
    { //pos        //texcoords
        0.0f, 1.0f, 0, 0.0f,
        1.0f, 1.0f, 1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,
        0.0f, 0.0f, 0,  1.0f
    };
    
    GLuint quad_indices[6] =
    {
        0, 1, 2, 0, 2, 3
    };
    
    GLuint sprite_vao;
    GLuint sprite_quad_vbo;
    GLuint quad_index_buffer;
    
    GLuint ui_sprite_vao;
    GLuint sprite_sheet_vao;
    
    GLuint sprite_error_vao;
    GLuint ui_error_vao;
    GLuint passthrough_vao;
    
    GLuint line_vbo;
    GLuint line_vao;
    GLuint line_ebo;
    
#define LINE_INDICES 6
    
    GLuint line_indices[LINE_INDICES] = 
    {
        0, 1, 2,
        1, 2, 3
    };
    
    //tiles
    GLfloat tile_quad_vertices[16] =
    {
        //pos        //texcoords
        0.0f, 1.0f, 0.125f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.125f,
        0.0f, 0.0f, 0.125f, 0.125f
    };
    
    GLuint tile_vao;
    GLuint tile_quad_vbo;
    
    GLfloat normal_quad_vertices[8] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    
    GLuint primitive_vao;
    GLuint primitive_vbo;
    
    GLfloat wireframe_quad_vertices[10] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    
    GLfloat wireframe_cube_vertices[32] =
    {
        -0.5, -0.5, -0.5, 1.0,
        0.5, -0.5, -0.5, 1.0,
        0.5,  0.5, -0.5, 1.0,
        -0.5,  0.5, -0.5, 1.0,
        -0.5, -0.5,  0.5, 1.0,
        0.5, -0.5,  0.5, 1.0,
        0.5,  0.5,  0.5, 1.0,
        -0.5,  0.5,  0.5, 1.0,
    };
    
#define CUBE_INDICES 16
    GLuint wireframe_cube_indices[CUBE_INDICES] = 
    {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    GLuint cube_index_buffer;
    
    GLfloat isometric_quad_vertices[8] =
    {
        0.5f, 1.0f,
        1.0f, 0.5f,
        0.5f, 0.0f,
        0.0f, 0.5f
    };
    
    GLuint wireframe_vao;
    GLuint wireframe_quad_vbo;
    GLuint wireframe_cube_vao;
    GLuint wireframe_cube_vbo;
    
    GLuint isometric_vao;
    GLuint isometric_quad_vbo;
    
    Buffer buffers[BUFFER_ARRAY_SIZE];
    i32 buffer_count;
    
    GLuint rect_vao;
    GLuint texture_rect_vao;
    GLuint normal_quad_vbo;
    
    union 
    {
        Shader shaders[SHADER_COUNT];
        struct
        {
            Shader texture_shader;
            Shader tile_shader;
            Shader rect_shader;
            Shader texture_rect_shader;
            Shader standard_font_shader;
            Shader spritesheet_shader;
            Shader wireframe_shader;
            Shader ui_sprite_shader;
            Shader error_shader_sprite;
            Shader error_shader_ui;
            Shader frame_buffer_shader;
            Shader light_source_shader;
            Shader simple_model_shader;
            Shader passthrough_shader;
            Shader line_shader;
        };
    };
    
    Shader extra_shaders[150];
    i32 extra_shader_index;
    
    i32 current_extra_shader;
    ShaderAttribute* shader_attributes;
    i32 shader_attribute_count;
    
    Texture texture_array[150];
    i32 texture_index;
    
    //freetype
    FT_Library ft_library;
    
    TrueTypeFont true_type_fonts[64];
    RenderFont fonts[64];
    i32 font_count;
    
    RenderState() {}
    
    MemoryArena arena;
};

#endif
