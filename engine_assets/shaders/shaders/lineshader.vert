#version 330 core
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;
uniform int isUI;
uniform float thickness;
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 normal;
layout(location = 2) in float miter;

out vec4 c;

void main()
{
	if(isUI == 1)
	{
		vec2 p = position.xy + vec2(normal * thickness/2.0 * miter);
    	gl_Position = model * vec4(p, 0.0, 1.0);
	}
	else
	{
		mat4 projViewModel = projection * view * model;

		vec2 p = position.xy + vec2(normal * thickness/2.0 * miter);
    	gl_Position = projViewModel * vec4(p.x, p.y, position.z, 1.0);
	}

	c = color;
}