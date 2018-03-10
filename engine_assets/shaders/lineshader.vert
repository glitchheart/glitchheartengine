#version 330 core
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;
uniform int isUI;
uniform vec2 normal;
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 normal;
layout(location = 2) in float miter;

out vec4 c;

void main()
{
	mat4 projViewModel = projection * view * model;

	vec2 p = Pos.xy + vec2(Normal * thickness/2.0 * miter);
    gl_Position = projViewModel * vec4(p, 0.0, 1.0);

/*
	if(IsUI == 1)
	{
		gl_Position = View * Model * vec4(Pos, 1.0);	
	}
	else
	{
		gl_Position = Projection * View * Model * vec4(Pos, 1.0);	
	}
*/
	c = Color;
}