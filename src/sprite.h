#ifndef SPRITE_H
#define SPRITE_H

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include "shader.h"
#include "keycontroller.h"
#include <math.h>

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

struct sprite
{
    GLuint vertexBuffer;
	GLuint texture;
	vec2 position;
    spriteshader shader;
	GLint texLocation;
};

static GLuint load_sprite(const std::string texturePath, const std::string shaderPath, sprite *spr)
{
	//setup vertex buffer
	glGenBuffers(1, &spr->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, spr->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
    load_shaders(shaderPath, &spr->shader);

	//load and setup texture
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &spr->texture);
    glBindTexture(GL_TEXTURE_2D, spr->texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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

static void render_sprite(const sprite &spr, mat4x4 p)
{
    glBindTexture(GL_TEXTURE_2D, spr.texture);

	mat4x4 m, mvp;

	mat4x4_identity(m);
	mat4x4_translate(m, spr.position[0], spr.position[1], 0);

    //this stuff should be done in an update method maybe?
//      var v3 = v3B - v3A;
//  var angle = Mathf.Atan2(v3.y, v3.x);
    
    vec2 res = { mouseX - spr.position[0], mouseY - spr.position[1] };
    
    auto angle = atan2((double)res[1], (double)res[0]);

    mat4x4_rotate_Z(m, m, angle);
    
	mat4x4_mul(mvp, p, m);

    use_shader(spr.shader, mvp);
}

#endif