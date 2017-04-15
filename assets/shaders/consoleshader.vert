#version 150
uniform mat4 MVP;
in vec2 pos;
attribute vec4 inColor;
out vec4 color;

void main()
{
	color = inColor;
    gl_Position = MVP * vec4(pos.xy, 0.0, 1.0);
}