#ifndef RENDERING_H
#define RENDERING_H

#include <GLFW/glfw3.h>

struct shader
{
	GLuint Program;
	GLuint VertexShader;
	GLuint FragmentShader;
};

struct render_state
{
	const struct
	{
		float x, y;
		float u, v;
	} QuadVertices[4] =
	{      //pos        //texcoords
		{ -0.5f,  0.5f, 0.0f, 0.0f },
		{  0.5f,  0.5f, 1.0f, 0.0f },
		{  0.5f, -0.5f, 1.0f, 1.0f },
		{ -0.5f, -0.5f, 0.0f, 1.0f }
	};
	GLuint QuadVertexBuffer;
	union
	{
		shader Shaders[1];
		struct 
		{
			shader TextureShader;
		};
	};
};

struct texture_manager
{

};

#endif