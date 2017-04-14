#ifndef RENDERING_H
#define RENDERING_H

enum Shader_Type
{
    Shader_Texture,
    Shader_Tile,
    Shader_Count
};

struct shader
{
    Shader_Type Type;
    GLuint Program;
    GLuint VertexShader;
    GLuint FragmentShader;
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
	    -1.0f, 1.0f,
	    1.0f, 1.0f,
	    1.0f, -1.0f,
	    -1.0f, -1.0f};
    GLuint TileVAO;
    GLuint TileQuadVBO;

    union {
	shader Shaders[Shader_Count];
	struct
	{
	    shader TextureShader;
	    shader TileShader;
	};
    };
};

#endif