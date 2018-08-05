#version 330 core

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 M;
uniform float isUI;
uniform vec4 color;
uniform vec2 dimensions;
uniform float radius;


in vec2 pos;
out vec4 c;

void main()
{
	vec2 coords = pos * dimensions;
    if (length(coords - vec2(0)) < radius || 
		length(coords - vec2(0, dimensions.y)) < radius ||
        length(coords - vec2(dimensions.x, 0)) < radius ||
        length(coords - dimensions) < radius) 
	{
        c = vec4(0.0);
    }
	else
	{
		if(isUI == 1.0)
    		gl_Position = Projection * M * vec4(pos.xy, 0.0, 1.0);
		else
    		gl_Position = Projection * View * M * vec4(pos.xy, 0.0, 1.0);
	
		c = color;
	}

	
}
