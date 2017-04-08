#ifndef SPRITE_H
#define SPRITE_H

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include "shader.h"

static const struct
{
    float x, y;
    float r, g, b;
    float u, v;
} vertices[4] =
{      //pos        //color        //texcoords
    { -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    {  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
    {  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
    { -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }
};

struct Sprite
{
    GLuint vertexBuffer;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint shaderProgram;
	GLuint texture;

	GLint mvpLocation;
	GLint vposLocation;
	GLint vcolLocation;
	GLint texLocation;

	vec2 position;
};

static GLuint load_sprite(const std::string texturePath, const std::string shaderPath, Sprite *spr)
{
	//setup vertex buffer
	glGenBuffers(1, &spr->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, spr->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//shaders
    spr->vertexShader = glCreateShader(GL_VERTEX_SHADER);

    const char* vertex_text = load_shader(shaderPath + std::string(".vert"));
    glShaderSource(spr->vertexShader, 1, &vertex_text, NULL);
    glCompileShader(spr->vertexShader);
    
    if(shader_compilation_error_checking(spr->vertexShader) == GL_FALSE)
        return GL_FALSE;
	
    spr->fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* fragment_text = load_shader(shaderPath + std::string(".frag"));

    glShaderSource(spr->fragmentShader, 1, &fragment_text, NULL);
    glCompileShader(spr->fragmentShader);
    
    if(shader_compilation_error_checking(spr->fragmentShader) == GL_FALSE)
        return GL_FALSE;

    spr->shaderProgram = glCreateProgram();

    glAttachShader(spr->shaderProgram, spr->vertexShader);
    glAttachShader(spr->shaderProgram, spr->fragmentShader);
    glLinkProgram(spr->shaderProgram);

	spr->mvpLocation = glGetUniformLocation(spr->shaderProgram, "MVP");
    spr->vposLocation = glGetAttribLocation(spr->shaderProgram, "vPos");
    spr->vcolLocation = glGetAttribLocation(spr->shaderProgram, "vCol");

    glEnableVertexAttribArray(spr->vposLocation);
    glVertexAttribPointer(spr->vposLocation, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 7, 0);
    glEnableVertexAttribArray(spr->vcolLocation);
    glVertexAttribPointer(spr->vcolLocation, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*) (sizeof(float) * 2));

    spr->texLocation = glGetAttribLocation(spr->shaderProgram, "texcoord");
    glEnableVertexAttribArray(spr->texLocation);
    glVertexAttribPointer(spr->texLocation, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));

	//load and setup texture
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &spr->texture);
    glBindTexture(GL_TEXTURE_2D, spr->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glGenerateMipmap(GL_TEXTURE_2D);

    //enable alpha for textures
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glEnable(GL_BLEND);

    int width, height;
    unsigned char* image = SOIL_load_image(texturePath.c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
    if(!image)
        return GL_FALSE;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
              GL_UNSIGNED_BYTE, image);

    SOIL_free_image_data(image);

    return GL_TRUE;
}

static void render_sprite(const Sprite &spr, mat4x4 p)
{
	mat4x4 m, mvp;
	mat4x4_identity(m);
	mat4x4_translate(m, spr.position[0], spr.position[1], 0);
	mat4x4_mul(mvp, p, m);

	glUseProgram(spr.shaderProgram);
	glUniformMatrix4fv(spr.mvpLocation, 1, GL_FALSE, (const GLfloat*) mvp);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
}

#endif