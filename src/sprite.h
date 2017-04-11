#ifndef SPRITE_H
#define SPRITE_H

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
	glm::vec2 Position;
	glm::vec3 Rotation;
    GLuint VertexBuffer;
	GLuint Texture;
	GLint TexLocation;
    sprite_shader Shader;
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

static void RenderSprite(const sprite &Spr, glm::mat4 ProjectionMatrix)
{
    glBindTexture(GL_TEXTURE_2D, Spr.Texture);

	glm::mat4 Model(1.0f);
    glm::mat4 View(1.0f);
    Model = glm::translate(Model, glm::vec3(Spr.Position.x, Spr.Position.y, 0.0f)); 
    Model = glm::scale(Model, glm::vec3(250.0f, 250.0f, 1.0f));
    
    UseShader(Spr.Shader, Model, View, ProjectionMatrix);
    glDrawArrays(GL_QUADS, 0, 4);
}

#endif