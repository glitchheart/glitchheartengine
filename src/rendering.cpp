#include "rendering.h"
#include "filehandling.h"
#include <SOIL/SOIL.h>
#include "glm/gtc/matrix_transform.hpp"
#include <vector>

static GLint ShaderCompilationErrorChecking(GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if(IsCompiled == GL_FALSE)
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

static GLuint LoadShader(const std::string FilePath, shader* Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar* VertexText = LoadShaderFromFile(FilePath + std::string(".vert"));
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if(ShaderCompilationErrorChecking(Shd->VertexShader) == GL_FALSE)
        return GL_FALSE;
        
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar* FragmentText = LoadShaderFromFile(FilePath + std::string(".frag"));
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if(ShaderCompilationErrorChecking(Shd->FragmentShader) == GL_FALSE)
        return GL_FALSE;

    Shd->Program = glCreateProgram();

    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);

	//TODO(daniel) if(IsTextureShader) - Or maybe find a different solution
    auto PositionLocation = glGetAttribLocation(Shd->Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(Shd->Program, "texcoord");

    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    return GL_TRUE;
}

static BindRenderStateBuffer(render_state *RenderState)
{
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->QuadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RenderState->QuadVertices), RenderState->QuadVertices, GL_STATIC_DRAW);
}

static void LoadAllShaders(render_state *State)
{
	glGenBuffers(1, &State->QuadVertexBuffer);
	BindRenderStateBuffer(&*State);
	LoadShader("./assets/shaders/textureshader", &State->TextureShader);
}

static GLuint LoadTexture(const char* FilePath)
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
    unsigned char* Image = SOIL_load_image(FilePath, &Width, &Height, 0, SOIL_LOAD_RGBA);

    if(!Image)
        return GL_FALSE;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
              GL_UNSIGNED_BYTE, Image);
    
    SOIL_free_image_data(Image);

	return TextureHandle;
}

static void SetVec2Attribute(GLuint ShaderHandle, const char* UniformName, glm::vec2 Value)
{
	
}

static void SetVec3Attribute(GLuint ShaderHandle, const char* UniformName, glm::vec3 Value)
{

}

static void SetVec4Attribute(GLuint ShaderHandle, const char* UniformName, glm::vec4 Value)
{

}

static void SetMat4Uniform(GLuint ShaderHandle, const char* UniformName, glm::mat4 Value)
{
	glUniformMatrix4fv(glGetUniformLocation(ShaderHandle, UniformName), 1, GL_FALSE, &Value[0][0]);
}

static Render(render_state* RenderState, entity entity, glm::mat4 ProjectionMatrix)
{
    glBindTexture(GL_TEXTURE_2D, entity.TextureHandle);

	glm::mat4 Model(1.0f);
    glm::mat4 View(1.0f);

    Model = glm::translate(Model, glm::vec3(entity.Position.x, entity.Position.y, 0.0f)); 
    Model = glm::scale(Model, glm::vec3(250.0f, 250.0f, 1.0f));
    
	auto Shader = RenderState->Shaders[entity.ShaderIndex];

	glm::mat4 MVP = ProjectionMatrix * View * Model;

	glUseProgram(Shader.Program);
	SetMat4Uniform(Shader.Program, "MVP", MVP);
    glDrawArrays(GL_QUADS, 0, 4);
}