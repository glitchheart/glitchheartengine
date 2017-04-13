#include "rendering.h"
#include "filehandling.h"
#include <SOIL/SOIL.h>
#include "glm/gtc/matrix_transform.hpp"
#include <vector>
#include "world.cpp"

static GLint ShaderCompilationErrorChecking(GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> ErrorLog(MaxLength);
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, &ErrorLog[0]);

        std::cout << "SHADER Compilation error" << std::endl;

        for (std::vector<GLchar>::const_iterator i = ErrorLog.begin(); i != ErrorLog.end(); ++i)
            std::cout << *i;

        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadShader(const std::string FilePath, shader *Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar *VertexText = LoadShaderFromFile(FilePath + std::string(".vert"));
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);

    if (!ShaderCompilationErrorChecking(Shd->VertexShader))
        return GL_FALSE;

    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *FragmentText = LoadShaderFromFile(FilePath + std::string(".frag"));
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);

    if (!ShaderCompilationErrorChecking(Shd->FragmentShader))
        return GL_FALSE;

    Shd->Program = glCreateProgram();

    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
   
    return GL_TRUE;
}

static void UseShader(shader *Shader)
{
    auto PositionLocation = glGetAttribLocation(Shader->Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(Shader->Program, "texcoord");

    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glUseProgram(Shader->Program);
}

static void LoadAllShaders(render_state *RenderState)
{
    glGenVertexArrays(1, &RenderState->SpriteVAO);
    glBindVertexArray(RenderState->SpriteVAO);

    glGenBuffers(1, &RenderState->SpriteQuadVBO);

    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);

    RenderState->TextureShader.Type = Shader_Texture;
    LoadShader("./assets/shaders/textureshader", &RenderState->TextureShader);
    glBindVertexArray(0);

    glGenVertexArrays(1, &RenderState->TileVAO);
    glBindVertexArray(RenderState->TileVAO);

    glGenBuffers(1, &RenderState->TileQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->TileQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);

    RenderState->TileShader.Type = Shader_Tile;
    LoadShader("./assets/shaders/tileshader", &RenderState->TileShader);
    glBindVertexArray(0);
}

static GLuint LoadTexture(const char *FilePath)
{
    GLuint TextureHandle;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //enable alpha for textures
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    int Width, Height;
    unsigned char *Image = SOIL_load_image(FilePath, &Width, &Height, 0, SOIL_LOAD_RGBA);

    if (!Image)
        return GL_FALSE;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, Image);

    SOIL_free_image_data(Image);

    return TextureHandle;
}

static void SetVec2Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec2 Value)
{
    glUniform2f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y);
}

static void SetVec3Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec3 Value)
{
    glUniform3f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z);
}

static void SetVec4Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec4 Value)
{
    glUniform4f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z, Value.w);
}

static void SetMat4Uniform(GLuint ShaderHandle, const char *UniformName, glm::mat4 Value)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderHandle, UniformName), 1, GL_FALSE, &Value[0][0]);
}

static GLuint BoundVAO;

static void RenderEntity(render_state *RenderState, const entity &entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->SpriteVAO);

    if (RenderState->BoundTexture != entity.TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, entity.TextureHandle);
        RenderState->BoundTexture = entity.TextureHandle;
    }

    glm::mat4 Model(1.0f);

    Model = glm::translate(Model, glm::vec3(entity.Position.x, entity.Position.y, 0.0f));
    Model = glm::rotate(Model, entity.Rotation.z, glm::vec3(0, 0, 1));
    Model = glm::scale(Model, entity.Scale);

    auto Shader = RenderState->Shaders[entity.ShaderIndex];

    glm::mat4 MVP = ProjectionMatrix * View * Model;

    UseShader(&Shader);

    SetMat4Uniform(Shader.Program, "MVP", MVP);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void RenderTileChunk(render_state *RenderState, const tile_chunk &Chunk, GLuint TilesetTextureHandle, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    real32 scale = 1.0f;
    
    glBindVertexArray(RenderState->TileVAO);

    if (RenderState->BoundTexture != TilesetTextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, TilesetTextureHandle);
        RenderState->BoundTexture = TilesetTextureHandle;
    }

    auto Shader = RenderState->TileShader;

    UseShader(&Shader);

    for (int i = 0; i < TILE_CHUNK_SIZE; i++)
    {
        for (int j = 0; j < TILE_CHUNK_SIZE; j++)
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3((Chunk.Offset.x + i) * scale, (Chunk.Offset.y + j) * scale, 0.0f));
            Model = glm::scale(Model, glm::vec3(scale, scale, 1.0f));
            glm::mat4 MVP = ProjectionMatrix * View * Model;

            SetVec2Attribute(Shader.Program, "textureOffset", Chunk.Data[i][j].TextureOffset);
            SetMat4Uniform(Shader.Program, "MVP", MVP);
            glDrawArrays(GL_QUADS, 0, 4);
        }
    }
    glBindVertexArray(0);
}