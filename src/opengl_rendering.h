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

const char* ShaderPaths[Shader_Count] =
{
    "../assets/shaders/textureshader",
    "../assets/shaders/tileshader",
    "../assets/shaders/consoleshader",
    "../assets/shaders/standardfontshader",
    "../assets/shaders/spritesheetanimationshader",
    "../assets/shaders/wireframeshader"
};

struct render_entity
{
    uint32 TextureHandle;
    uint32 ShaderIndex;
};

struct shader
{
    Shader_Type Type;
    GLuint Program;
    GLuint VertexShader;
    GLuint FragmentShader;
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
        float AX; // advance.x
        float AY; // advance.y
        
        float BW; // bitmap.width;
        float BH; // bitmap.rows;
        
        float BL; // bitmap_left;
        float BT; // bitmap_top;
        
        float TX;
    } CharacterInfo[255];
};

//TODO(Daniel) move this somewhere else
struct CompareCStrings 
{
    bool operator()(const char* lhs, const char* rhs) const {
        return std::strcmp(lhs, rhs) < 0;
    }
};

struct render_state
{
    GLFWwindow *Window;
    int WindowWidth;
    int WindowHeight;
    GLint Viewport[4];
    
    bool32 RenderColliders;
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t NormalQuadVerticesSize = 8 * sizeof(GLfloat);
    size_t WireframeQuadVerticesSize = 10 * sizeof(GLfloat);
    GLuint BoundVertexBuffer;
    GLuint BoundTexture;
    //sprites
    GLfloat SpriteQuadVertices[16] =
    { //pos        //texcoords
        0.0f, 1.0f, 0,    0,
        1.0f, 1.0f, 1.0f, 0,
        1.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0,    1.0f};
    GLuint SpriteVAO;
    GLuint SpriteQuadVBO;
    
    GLuint SpriteSheetVAO;
    
    //tiles
    GLfloat TileQuadVertices[16] =
    {
        //pos        //texcoords
        0.0f, 1.0f, 0.0625f, 0.0625f,
        1.0f, 1.0f, 0.9375f, 0.0625f,
        1.0f, 0.0f, 0.9375f, 0.9375f,
        0.0f, 0.0f, 0.0625f, 0.9375f
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
    
    //freetype
    FT_Library FTLibrary;
    render_font InconsolataFont;
    
    //animations
    std::map<char*, uint32, CompareCStrings> LoadedTextureHandles;
};

#endif
