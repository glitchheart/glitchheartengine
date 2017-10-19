#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 M;
uniform float isUI;
uniform vec4 color;
in vec2 pos;
in vec2 texcoord;
out vec4 c;
out vec2 Texcoord;

void main()
{
    Texcoord = texcoord;
	if(isUI == 1.0)
    	gl_Position = M * vec4(pos.xy, 0.0, 1.0);
	else
		gl_Position = Projection * View *  M * vec4(pos.xy, 0.0, 1.0);

	c = color;
}