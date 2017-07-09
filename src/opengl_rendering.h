#ifndef RENDERING_H
#define RENDERING_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define PIXELS_PER_UNIT 32

enum Shader_Type
{
    Shader_Texture,
    Shader_Tile,
    Shader_Rect,
    Shader_TextureRect,
    Shader_StandardFont,
    Shader_Spritesheet,
    Shader_Wireframe,
    Shader_UISprite,
    Shader_ErrorSprite,
    Shader_ErrorUI,
    Shader_AStarPath,
    Shader_FrameBuffer,
    
    Shader_Count
};

const char* ShaderPaths[Shader_Count] =
{
    "../assets/shaders/textureshader",
    "../assets/shaders/tileshader",
    "../assets/shaders/rectshader",
    "../assets/shaders/texturerectshader",
    "../assets/shaders/standardfontshader",
    "../assets/shaders/spritesheetanimationshader",
    "../assets/shaders/wireframeshader",
    "../assets/shaders/spriteuishader",
    "../assets/shaders/errorshadersprite",
    "../assets/shaders/errorshaderui",
    "../assets/shaders/astarpathshader",
    "../assets/shaders/framebuffershader",
};


enum Render_Mode
{
    Render_Fill, Render_Outline
};


#define NUM_ENTITIES 100

struct entity;


struct texture 
{
    char* Name;
    GLuint TextureHandle;
    i32 Width;
    i32 Height;
};

struct tilesheet
{
    char* Name;
    texture Texture;
};

struct ui_render_info
{
    b32 Rendered = true;
    
    texture* Texture;
    glm::vec2 TextureOffset;
    glm::vec2 FrameSize;
    u32 ShaderIndex;
    glm::vec2 Size = glm::vec3(1, 1, 1);
    glm::vec4 Color = glm::vec4(1, 1, 1, 1);
};

struct editor_render_info
{
    b32 Dirty = false;
    GLuint VAO;
    GLuint VBO;
    i32 VBOSize;
};

struct tilemap_render_info
{
    b32 Dirty = true;
    GLuint VAOS[2];
    GLuint VBOS[2];
    i32 VBOSizes[2];
};

struct render_entity
{
    entity* Entity;
    b32 Rendered = true;
    texture* Texture;
    u32 ShaderIndex;
    glm::vec4 Color = glm::vec4(1, 1, 1, 1);
};

struct shader
{
    Shader_Type Type;
    GLuint Program;
    GLuint VertexShader;
    GLuint FragmentShader;
};

enum Alignment
{
    Alignment_Left,
    Alignment_Right,
    Alignment_Center
};

struct render_font
{
    FT_Face Face;
    GLuint VAO;
    GLuint VBO;
    GLuint Texture;
    u32 AtlasWidth;
    u32 AtlasHeight;
    GLfloat GlyphWidth;
    glm::vec4 Color = glm::vec4(1, 1, 1, 1);
    glm::vec4 AlphaColor = glm::vec4(1, 1, 1, 1);
    
    struct character_info 
    {
        r32 AX; // advance.x
        r32 AY; // advance.y
        
        r32 BW; // bitmap.width;
        r32 BH; // bitmap.rows;
        
        r32 BL; // bitmap_left;
        r32 BT; // bitmap_top;
        
        r32 TX;
    } CharacterInfo[255];
};

struct point
{
    GLfloat X;
    GLfloat Y;
    GLfloat S;
    GLfloat T;
};

struct cmp_str
{
    bool operator()(const char *a, const char *b) const
    {
        return std::strcmp(a, b) < 0;
    }
};

struct render_state
{
    GLFWwindow *Window;
    int WindowWidth;
    int WindowHeight;
    GLfloat ScaleX;
    GLfloat ScaleY;
    GLint Viewport[4];
    r64 DeltaTime;
    
    render_entity RenderEntities[NUM_ENTITIES];
    i32 RenderEntityCount;
    
    b32 RenderColliders;
    b32 RenderFPS;
    b32 RenderPaths;
    b32 ShouldClose;
    r64 FPS;
    
    GLuint FrameBufferVAO;
    GLuint FrameBufferVBO;
    GLuint FrameBuffer;
    GLuint TextureColorBuffer;
    
    // Lighting map
    GLuint LightingFrameBuffer;
    GLuint LightingTextureColorBuffer;
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t NormalQuadVerticesSize = 8 * sizeof(GLfloat);
    size_t WireframeQuadVerticesSize = 10 * sizeof(GLfloat);
    size_t AStarPathQuadVerticesSize = 10 * sizeof(GLfloat);
    GLuint BoundVertexBuffer;
    GLuint BoundTexture;
    
    GLfloat FrameBufferVertices[16] =
    {
        -1.0f, 1.0f, 0, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f, 
        -1.0f, -1.0f, 0, 0.0f
    };
    
    GLfloat SpriteQuadVertices[16] =
    { //pos        //texcoords
        0.0f, 1.0f, 0, 0.0f,
        1.0f, 1.0f, 1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,
        0.0f, 0.0f, 0,  1.0f
    };
    
    GLuint SpriteVAO;
    GLuint SpriteQuadVBO;
    GLuint UISpriteVAO;
    GLuint SpriteSheetVAO;
    
    GLuint SpriteErrorVAO;
    GLuint UIErrorVAO;
    
    //tiles
    GLfloat TileQuadVertices[16] =
    {
        //pos        //texcoords
        0.0f, 1.0f, 0.125f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.125f,
        0.0f, 0.0f, 0.125f, 0.125f
    };
    
    GLuint TileVAO;
    GLuint TileQuadVBO;
    
    GLfloat NormalQuadVertices[8] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    
    GLuint PrimitiveVBO;
    
    GLfloat WireframeQuadVertices[10] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    
    GLuint WireframeVAO;
    GLuint WireframeQuadVBO;
    
    GLfloat AStarPathQuadVertices[10] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    
    GLuint AStarPathVAO;
    GLuint AStarPathQuadVBO;
    
    GLuint RectVAO;
    GLuint TextureRectVAO;
    GLuint NormalQuadVBO;
    
    tilesheet* Tilesheets;
    u32 TilesheetCount;
    
    union 
    {
        shader Shaders[Shader_Count];
        struct
        {
            shader TextureShader;
            shader TileShader;
            shader RectShader;
            shader TextureRectShader;
            shader StandardFontShader;
            shader SpritesheetShader;
            shader WireframeShader;
            shader UISpriteShader;
            shader ErrorShaderSprite;
            shader ErrorShaderUI;
            shader AStarPathShader;
            shader FrameBufferShader;
        };
    };
    
    texture TextureArray[50];
    i32 TextureIndex;
    std::map<const char*, texture*, cmp_str> Textures;
    
    //freetype
    FT_Library FTLibrary;
    render_font InconsolataFont;
    render_font MenuFont;
    render_font ButtonFont;
    render_font RobotoFont;
    render_font TitleFont;
    render_font DamageFont;
};

#endif