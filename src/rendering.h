#ifndef RENDERING_H
#define RENDERING_H

#include <GLFW/glfw3.h>

enum Shader_Type
{
	Shader_Texture,
	Shader_Tile
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
	const size_t SpriteQuadVerticesSize = 16 * sizeof(GLfloat);
	const size_t TileQuadVerticesSize = 8 * sizeof(GLfloat);
	GLuint BoundVertexBuffer;
	GLuint BoundTexture;
	//sprites
	const struct
	{
		float x, y;
		float u, v;
	};

	GLfloat SpriteQuadVertices[16] =
	{      //pos        //texcoords
		-1.0f,  1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 1.0f
	};
	GLuint SpriteVAO;
	GLuint SpriteQuadVBO;
	
	//tiles
	GLfloat TileQuadVertices[8] =
	{
		-1.0f,  1.0f,
		 1.0f,  1.0f,
		 1.0f, -1.0f,
		-1.0f, -1.0f 
	};
	GLuint TileVAO;
	GLuint TileQuadVBO;
	union
	{
		shader Shaders[2];
		struct 
		{
			shader TextureShader;
			shader TileShader;
		};
	};
};

struct texture_manager
{

};

#endif