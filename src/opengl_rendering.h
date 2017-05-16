#ifndef RENDERING_H
#define RENDERING_H

#include <ft2build.h>
#include FT_FREETYPE_H

enum Shader_Type
{
    Shader_Texture,
    Shader_Tile,
    Shader_Console,
    Shader_StandardFont,
    Shader_SpriteSheetShader,
    Shader_Wireframe,
    
    Shader_Count
};

enum Texture_Type
{
    Texture_Tiles,
    Texture_Player,
    Texture_Barrel,
    Texture_Crosshair,
    Texture_SwordTopRight,
    
    Texture_Count
};

const char* ShaderPaths[Shader_Count] =
{
    "../assets/shaders/textureshader",
    "../assets/shaders/tileshader",
    "../assets/shaders/consoleshader",
    "../assets/shaders/standardfontshader",
    "../assets/shaders/spritesheetanimationshader",
    "../assets/shaders/wireframeshader"
};

const char* TexturePaths[Texture_Count] =
{
    "../assets/textures/tiles.png",
    "../assets/textures/spritesheets/new_player.png",
    "../assets/textures/barrel.png",
    "../assets/textures/crosshair.png",
    "../assets/textures/spritesheets/sword_attack_top_right.png"
};

enum Render_Mode
{
    Render_Fill, Render_Outline
};


#define NUM_ENTITIES 2048

struct entity;

struct render_entity
{
    entity* Entity;
    bool32 Rendered = true;
    uint32 TextureHandle;
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

struct render_state
{
    GLFWwindow *Window;
    int WindowWidth;
    int WindowHeight;
    GLfloat ScaleX;
    GLfloat ScaleY;
    GLint Viewport[4];
    
    render_entity RenderEntities[NUM_ENTITIES];
    uint32 RenderEntityCount;
    
    bool32 RenderColliders;
    bool32 RenderFPS;
    bool32 ShouldClose;
    real64 FPS;
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t NormalQuadVerticesSize = 8 * sizeof(GLfloat);
    size_t WireframeQuadVerticesSize = 10 * sizeof(GLfloat);
    GLuint BoundVertexBuffer;
    GLuint BoundTexture;
    //sprites
    GLfloat SpriteQuadVertices[16] =
    { //pos        //texcoords
        0.0f, 1.0f, 0.00625f, 0.9375f,
        1.0f, 1.0f, 0.9375f,  0.9375f,
        1.0f, 0.0f, 0.9375f,  0.0625f,
        0.0f, 0.0f, 0.0625f,  0.0625};
    GLuint SpriteVAO;
    GLuint SpriteQuadVBO;
    
    GLuint SpriteSheetVAO;
    
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
    
    GLuint ConsoleVAO;
    GLuint NormalQuadVBO;
    
    union 
    {
        shader Shaders[Shader_Count];
        struct
        {
            shader TextureShader;
            shader TileShader;
            shader ConsoleShader;
            shader StandardFontShader;
            shader SpriteSheetShader;
            shader WireframeShader;
        };
    };
    
    union
    {
        uint32 Textures[Texture_Count];
        struct
        {
            uint32 TileTexture;
            uint32 PlayerTexture;
            uint32 BarrelTexture;
            uint32 CrosshairTexture;
            uint32 SwordTopRightTexture;
        };
    };
    //freetype
    FT_Library FTLibrary;
    render_font InconsolataFont;
    render_font MenuFont;
};

#endif
