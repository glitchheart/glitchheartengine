#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform float isUI;

in vec2 pos;
in vec2 texcoord;
out vec2 Texcoord;

void main()
{
	Texcoord = vec2(texcoord.x, texcoord.y);
	
	if(isUI == 1)
		gl_Position = Model * vec4(pos.xy, 0.0, 1.0);
	else
		gl_Position = Projection * View * Model * vec4(pos.xy, 0.0, 1.0);
}