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
    SHADERPAIR(FrameBuffer),
    SHADERPAIR(LightSource),
    SHADERPAIR(SimpleModel),
    SHADERPAIR(Passthrough)
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

static char* ShaderPaths[Shader_Count] =
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
    "../engine_assets/shaders/passthroughshader"
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
    GLint IndexBufferCount;
};

struct render_state
{
    GLFWwindow *Window;
    i32 WindowWidth;
    i32 WindowHeight;
    i32 ScreenWidth;
    i32 ScreenHeight;
    i32 ScaleFromWidth;
    i32 ScaleFromHeight;
    i32 DpiScale;
    i32 PixelsPerUnit;
    
    r32 Contrast;
    r32 Brightness;
    
    char* WindowTitle;
    
    r64 FrameDelta;
    
    Window_Mode WindowMode;
    
    GLfloat ScaleX;
    GLfloat ScaleY;
    GLint Viewport[4];
    r64 DeltaTime;
    
    b32 ShouldClose;
    r64 FPS;
    
    GLuint OriginalFrameBufferVAO;
    GLuint OriginalFrameBufferVBO;
    GLuint OriginalFrameBuffer;
    
    GLuint FrameBufferVAO;
    GLuint FrameBufferVBO;
    GLuint FrameBuffer;
    GLuint FrameBufferTex0Loc;
    GLuint FrameBufferTex1Loc;
    GLuint TextureColorBuffer;
    
    // Lighting data
    spotlight_data SpotlightData;
    directional_light_data DirectionalLightData;
    point_light_data PointLightData;
    
    GLuint SpotlightUBO;
    GLuint DirectionalLightUBO;
    GLuint PointLightUBO;
    
    // Lighting map
    GLuint LightingFrameBuffer;
    GLuint LightingTextureColorBuffer;
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t NormalQuadVerticesSize = 8 * sizeof(GLfloat);
    size_t WireframeQuadVerticesSize = 8 * sizeof(GLfloat);
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
    GLuint PassthroughVAO;
    
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
    
    GLuint PrimitiveVAO;
    GLuint PrimitiveVBO;
    
    GLfloat WireframeQuadVertices[10] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };
    
    GLfloat WireframeCubeVertices[32] =
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
    GLuint WireframeCubeIndices[CUBE_INDICES] = 
    {
        0, 1, 2, 3,
        4, 5, 6, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    GLuint CubeIndexBuffer;
    
    GLfloat IsometricQuadVertices[8] =
    {
        0.5f, 1.0f,
        1.0f, 0.5f,
        0.5f, 0.0f,
        0.0f, 0.5f
    };
    
    GLuint WireframeVAO;
    GLuint WireframeQuadVBO;
    GLuint WireframeCubeVAO;
    GLuint WireframeCubeVBO;
    
    GLuint IsometricVAO;
    GLuint IsometricQuadVBO;
    
    buffer Buffers[BUFFER_ARRAY_SIZE];
    i32 BufferCount;
    
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
            shader FrameBufferShader;
            shader LightSourceShader;
            shader SimpleModelShader;
            shader PassthroughShader;
        };
    };
    
    shader ExtraShaders[150];
    i32 ExtraShaderIndex;
    
    i32 CurrentExtraShader;
    shader_attribute* ShaderAttributes;
    i32 ShaderAttributeCount;
    
    texture TextureArray[150];
    i32 TextureIndex;
    
    //freetype
    FT_Library FTLibrary;
    
    render_font Fonts[64];
    i32 FontCount;
    
    render_state() {}
    
    memory_arena Arena;
};

#endif
