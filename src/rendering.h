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
    
    Shader_Count
};

const char* ShaderPaths[Shader_Count] =
{
    "../assets/shaders/textureshader",
    "../assets/shaders/tileshader",
    "../assets/shaders/consoleshader",
    "../assets/shaders/standardfontshader"
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

struct render_state
{
    GLFWwindow *Window;
    int WindowWidth;
    int WindowHeight;
    GLint Viewport[4];
    
    size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
    size_t TileQuadVerticesSize = 8 * sizeof(GLfloat);
    GLuint BoundVertexBuffer;
    GLuint BoundTexture;
    //sprites
    GLfloat SpriteQuadVertices[16] =
    { //pos        //texcoords
        0.0f, 1.0f, 0.0625f, 0.0625f,
        1.0f, 1.0f, 0.9375f, 0.0625f,
        1.0f, 0.0f, 0.9375f, 0.9375f,
        0.0f, 0.0f, 0.0625f, 0.9375f};
    GLuint SpriteVAO;
    GLuint SpriteQuadVBO;
    
    //tiles
    GLfloat TileQuadVertices[8] =
    {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f };
    
    GLuint TileVAO;
    GLuint TileQuadVBO;
    
    GLuint ConsoleVAO;
    GLuint ConsoleQuadVBO;
    
    union 
    {
        shader Shaders[Shader_Count];
        struct
        {
            shader TextureShader;
            shader TileShader;
            shader ConsoleShader;
            shader StandardFontShader;
        };
    };
    
    //freetype
    FT_Library FTLibrary;
    render_font InconsolataFont;
};

#endif
