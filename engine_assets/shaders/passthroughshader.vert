#version 330 core
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec4 Color;
uniform bool IsUI;
layout(location = 0) in vec3 Pos;

out vec4 c;

void main()
{
	if(IsUI)
	{
		gl_Position = View * Model * vec4(Pos, 1.0);	
	}
	else
	{
		gl_Position = Projection * View * Model * vec4(Pos, 1.0);	
	}

	c = Color;
}