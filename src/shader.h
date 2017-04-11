#ifndef SHADER_H
#define SHADER_H

#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

struct sprite_shader
{
    GLuint VertexBuffer;
	GLuint VertexShader;
	GLuint FragmentShader;
	GLuint Program;

	GLint MVPLocation;
	GLint VPosLocation;
	GLint VColLocation;
	GLint TexLocation;
};

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

static const char* LoadFile(const std::string Path)
{
    std::string Content;
    std::ifstream FileStream(Path, std::ios::in);

    if(!FileStream.is_open()) {
        std::cerr << "Could not read file " << Path << ". File does not exist." << std::endl;
        return "";
    }

    std::string Line = "";
    while(!FileStream.eof()) {
        std::getline(FileStream, Line);
        Content.append(Line + "\n");
    }

    FileStream.close();

    return Content.c_str();
}

static GLuint LoadShaders(const std::string FilePath, sprite_shader* Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* VertexText = LoadFile(FilePath + std::string(".vert"));
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if(ShaderCompilationErrorChecking(Shd->VertexShader) == GL_FALSE)
        return GL_FALSE;
        
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* FragmentText = LoadFile(FilePath + std::string(".frag"));
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if(ShaderCompilationErrorChecking(Shd->FragmentShader) == GL_FALSE)
        return GL_FALSE;

    Shd->Program = glCreateProgram();

    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);

	Shd->MVPLocation = glGetUniformLocation(Shd->Program, "MVP");
    Shd->VPosLocation = glGetAttribLocation(Shd->Program, "vPos");
    Shd->VColLocation = glGetAttribLocation(Shd->Program, "vCol");

    glEnableVertexAttribArray(Shd->VPosLocation);
    glVertexAttribPointer(Shd->VPosLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
    glEnableVertexAttribArray(Shd->VColLocation);
    glVertexAttribPointer(Shd->VColLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*) (sizeof(float) * 2));

    Shd->TexLocation = glGetAttribLocation(Shd->Program, "texcoord");
    glEnableVertexAttribArray(Shd->TexLocation);
    glVertexAttribPointer(Shd->TexLocation, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));

    return GL_TRUE;
}

static void UseShader(sprite_shader Shader, const glm::mat4 Model, const glm::mat4 View, const glm::mat4 Projection)
{
	glUseProgram(Shader.Program);
    glm::mat4 MVP = Projection * View * Model;
    glUniformMatrix4fv(Shader.MVPLocation, 1, GL_FALSE, &MVP[0][0]);
}
#endif