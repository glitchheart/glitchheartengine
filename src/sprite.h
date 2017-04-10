#ifndef SPRITE_H
#define SPRITE_H

#include "linmath.h"
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>
#include "shader.h"
#include "keycontroller.h"
#include <math.h>
#include "types.h"

static const struct
{
    float x, y;
    float r, g, b;
    float u, v;
} Vertices[4] =
{      //pos        //color        //texcoords
    { -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
    {  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
    {  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
    { -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f }
};

#define NUM_SPRITES 4

struct sprite
{
    GLuint VertexBuffer;
	GLuint Texture;
	vec2 Position;
    sprite_shader Shader;
	GLint TexLocation;
};

struct sprite_manager
{
    uint16 Count;
    sprite Sprites[NUM_SPRITES];
};


static GLuint LoadSprite(const std::string TexturePath, const std::string ShaderPath, uint16 *SpriteHandle, sprite_manager *SpriteManager)
{
    sprite Spr = {};
	//setup vertex buffer
	glGenBuffers(1, &Spr.VertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, Spr.VertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	
    LoadShaders(ShaderPath, &Spr.Shader);

	//load and setup texture
    glEnable(GL_TEXTURE_2D);

    glGenTextures(1, &Spr.Texture);
    glBindTexture(GL_TEXTURE_2D, Spr.Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //enable alpha for textures
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glEnable(GL_BLEND);

    int Width, Height;
    unsigned char* Image = SOIL_load_image(TexturePath.c_str(), &Width, &Height, 0, SOIL_LOAD_RGBA);

    if(!Image)
        return GL_FALSE;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
              GL_UNSIGNED_BYTE, Image);
    
    SOIL_free_image_data(Image);

    *SpriteHandle = SpriteManager->Count;
    SpriteManager->Sprites[SpriteManager->Count++] = Spr;

    return GL_TRUE;
}

static void RenderSprite(const sprite &Spr, mat4x4 P)
{
    glBindTexture(GL_TEXTURE_2D, Spr.Texture);

	mat4x4 M, MVP;

	mat4x4_identity(M);
	mat4x4_translate(M, Spr.Position[0], Spr.Position[1], 0);

    // vec2 res = { mouseX - spr.position[0], mouseY - spr.position[1] };
    
    // auto angle = atan2((double)res[1], (double)res[0]);

    // mat4x4_rotate_Z(m, m, angle);
    
	mat4x4_mul(MVP, P, M);

    UseShader(Spr.Shader, MVP);
}

#endif