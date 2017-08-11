#ifndef OPENGL_H
#define OPENGL_H

#include <ft2build.h>
#include FT_FREETYPE_H

#define PIXELS_PER_UNIT 32

#define SHADERPAIR(name) {Shader_ ## name, "Shader_" "" #name}

const static struct
{
    Shader_Type Val;
    char* Str;
    
} ShaderConversion [] =
{
    SHADERPAIR(Texture),
    SHADERPAIR(Tile),
    SHADERPAIR(Rect),
    SHADERPAIR(TextureRect),
    SHADERPAIR(StandardFont),
    SHADERPAIR(Spritesheet),
    SHADERPAIR(Wireframe),
    SHADERPAIR(UISprite),
    SHADERPAIR(ErrorSprite),
    SHADERPAIR(ErrorUI),
    SHADERPAIR(AStarPath),
    SHADERPAIR(FrameBuffer),
    SHADERPAIR(LightSource),
    SHADERPAIR(SimpleModel)
};

char* ShaderEnumToStr(Shader_Type Shader)
{
    for(i32 Index = 0; Index < Shader_Count; Index++)
    {
        if(Shader == ShaderConversion[Index].Val)
        {
            return ShaderConversion[Index].Str;
        }
    }
    Assert(false);
    return 0;
}

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
    "../assets/shaders/lightsourceshader",
    "../assets/shaders/simple_model_shader"
};


enum Render_Mode
{
    Render_Fill, Render_Outline
};

struct entity;
struct object_entity;

struct texture 
{
    GLuint TextureHandle;
};

#define TILESHEET_MAX 10

struct tilesheet
{
    char* Name;
    texture Texture;
    i32 TileWidth;
    i32 TileHeight;
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
    math::v4 Color = math::v4(1, 1, 1, 1);
    math::v4 AlphaColor = math::v4(1, 1, 1, 1);
    
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

struct buffer
{
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLint VertexBufferSize;
    GLint IndexBufferSize;
};

struct render_state
{
    GLFWwindow *Window;
    int WindowWidth;
    int WindowHeight;
    r32 Contrast;
    r32 Brightness;
    GLfloat ScaleX;
    GLfloat ScaleY;
    GLint Viewport[4];
    r64 DeltaTime;
    
    b32 RenderColliders;
    b32 RenderFPS;
    b32 RenderPaths;
    b32 ShouldClose;
    r64 FPS;
    
    GLuint FrameBufferVAO;
    GLuint FrameBufferVBO;
    GLuint FrameBuffer;
    GLuint FrameBufferTex0Loc;
    GLuint FrameBufferTex1Loc;
    GLuint TextureColorBuffer;
    
    // Lighting map
    GLuint LightingFrameBuffer;
    GLuint LightingTextureColorBuffer;
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t NormalQuadVerticesSize = 8 * sizeof(GLfloat);
    size_t WireframeQuadVerticesSize = 8 * sizeof(GLfloat);
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
    
    GLuint QuadIndices[6] =
    {
        0, 1, 2, 0, 2, 3
    };
    
    GLuint SpriteVAO;
    GLuint SpriteQuadVBO;
    GLuint QuadIndexBuffer;
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
        0.0f, 0.0f
    };
    
    
    GLfloat IsometricQuadVertices[8] =
    {
        0.5f, 1.0f,
        1.0f, 0.5f,
        0.5f, 0.0f,
        0.0f, 0.5f
    };
    
    GLuint WireframeVAO;
    GLuint WireframeQuadVBO;
    
    GLuint IsometricVAO;
    GLuint IsometricQuadVBO;
    
    GLfloat AStarPathQuadVertices[10] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    
    buffer Buffers[BUFFER_ARRAY_SIZE];
    i32 BufferCount;
    
    GLuint AStarPathVAO;
    GLuint AStarPathQuadVBO;
    
    GLuint RectVAO;
    GLuint TextureRectVAO;
    GLuint NormalQuadVBO;
    
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
            shader LightSourceShader;
            shader SimpleModelShader;
        };
    };
    
    texture TextureArray[150];
    i32 TextureIndex;
    
    //freetype
    FT_Library FTLibrary;
    render_font InconsolataFont;
    render_font SmallerInconsolataFont;
    render_font MenuFont;
    render_font ButtonFont;
    render_font RobotoFont;
    render_font TitleFont;
    render_font DamageFont;
    render_font OutlineFont;
};

#endif