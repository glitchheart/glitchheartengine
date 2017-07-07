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
    int32 Width;
    int32 Height;
};

struct tilesheet
{
    char* Name;
    texture Texture;
};

struct ui_render_info
{
    bool32 Rendered = true;
    
    texture* Texture;
    glm::vec2 TextureOffset;
    glm::vec2 FrameSize;
    uint32 ShaderIndex;
    glm::vec2 Size = glm::vec3(1, 1, 1);
    glm::vec4 Color = glm::vec4(1, 1, 1, 1);
};

struct editor_render_info
{
    bool32 Dirty = false;
    GLuint VAO;
    GLuint VBO;
    int32 VBOSize;
};

struct tilemap_render_info
{
    bool32 Dirty = true;
    GLuint VAO;
    GLuint VBO;
    int32 VBOSize;
};

struct render_entity
{
    entity* Entity;
    bool32 Rendered = true;
    texture* Texture;
    uint32 ShaderIndex;
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
    uint32 AtlasWidth;
    uint32 AtlasHeight;
    GLfloat GlyphWidth;
    glm::vec4 Color = glm::vec4(1, 1, 1, 1);
    glm::vec4 AlphaColor = glm::vec4(1, 1, 1, 1);
    
    struct character_info 
    {
        real32 AX; // advance.x
        real32 AY; // advance.y
        
        real32 BW; // bitmap.width;
        real32 BH; // bitmap.rows;
        
        real32 BL; // bitmap_left;
        real32 BT; // bitmap_top;
        
        real32 TX;
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
    real64 DeltaTime;
    
    render_entity RenderEntities[NUM_ENTITIES];
    int32 RenderEntityCount;
    
    bool32 RenderColliders;
    bool32 RenderFPS;
    bool32 RenderPaths;
    bool32 ShouldClose;
    real64 FPS;
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t NormalQuadVerticesSize = 8 * sizeof(GLfloat);
    size_t WireframeQuadVerticesSize = 10 * sizeof(GLfloat);
    size_t AStarPathQuadVerticesSize = 10 * sizeof(GLfloat);
    GLuint BoundVertexBuffer;
    GLuint BoundTexture;
    
    GLfloat SpriteQuadVertices[16] =
    { //pos        //texcoords
        0.0f, 1.0f, 0, 0.0f,
        1.0f, 1.0f, 1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,
        0.0f, 0.0f, 0,  1.0f};
    
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
        0.0f, 1.0f, 0.0625f, 0.9375f,
        1.0f, 1.0f, 0.9375f, 0.9375f,
        1.0f, 0.0f, 0.9375f, 0.0625f,
        0.0f, 0.0f, 0.0625f, 0.0625f
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
    uint32 TilesheetCount;
    
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
        };
    };
    
    texture TextureArray[40];
    int32 TextureIndex;
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