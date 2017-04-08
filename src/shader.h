#ifndef SHADER_H
#define SHADER_H

#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

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

static const char* load_shader(const std::string filePath)
{
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
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

#endif