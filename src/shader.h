#ifndef SHADER_H
#define SHADER_H

#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <fstream>
#include "linmath.h"

struct spriteshader
{
    GLuint vertexBuffer;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;

	GLint mvpLocation;
	GLint vposLocation;
	GLint vcolLocation;
	GLint texLocation;
};

static GLint shader_compilation_error_checking(GLuint shader)
{
    GLint isCompiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
        
        std::cout << "SHADER Compilation error" << std::endl;

        for (std::vector<GLchar>::const_iterator i = errorLog.begin(); i != errorLog.end(); ++i)
            std::cout << *i;

        // Provide the infolog in whatever manor you deem best.
        // Exit with failure.
        glDeleteShader(shader); // Don't leak the shader.
    }
    return isCompiled;
}

static const char* load_file(const std::string path)
{
    std::string content;
    std::ifstream fileStream(path, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << path << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();

    return content.c_str();
}

static GLuint load_shaders(const std::string filePath, spriteshader* shd)
{
    shd->vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertex_text = load_file(filePath + std::string(".vert"));
    glShaderSource(shd->vertexShader, 1, &vertex_text, NULL);
    glCompileShader(shd->vertexShader);
    
    if(shader_compilation_error_checking(shd->vertexShader) == GL_FALSE)
        return GL_FALSE;
        
    shd->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragment_text = load_file(filePath + std::string(".frag"));
    glShaderSource(shd->fragmentShader, 1, &fragment_text, NULL);
    glCompileShader(shd->fragmentShader);
    
    if(shader_compilation_error_checking(shd->fragmentShader) == GL_FALSE)
        return GL_FALSE;

    shd->program = glCreateProgram();

    glAttachShader(shd->program, shd->vertexShader);
    glAttachShader(shd->program, shd->fragmentShader);
    glLinkProgram(shd->program);

	shd->mvpLocation = glGetUniformLocation(shd->program, "MVP");
    shd->vposLocation = glGetAttribLocation(shd->program, "vPos");
    shd->vcolLocation = glGetAttribLocation(shd->program, "vCol");

    glEnableVertexAttribArray(shd->vposLocation);
    glVertexAttribPointer(shd->vposLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
    glEnableVertexAttribArray(shd->vcolLocation);
    glVertexAttribPointer(shd->vcolLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*) (sizeof(float) * 2));

    shd->texLocation = glGetAttribLocation(shd->program, "texcoord");
    glEnableVertexAttribArray(shd->texLocation);
    glVertexAttribPointer(shd->texLocation, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));

    return GL_TRUE;
}

static void use_shader(spriteshader shader, const mat4x4 mvp)
{
	glUseProgram(shader.program);
    glUniformMatrix4fv(shader.mvpLocation, 1, GL_FALSE, (const GLfloat*) mvp);
	glDrawArrays(GL_QUADS, 0, 4);
}
#endif