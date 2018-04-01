#version 330 core
uniform vec4 color;
in vec2 pos;
in vec2 texcoord;
out vec4 c;
out vec2 Texcoord;

void main()
{
    Texcoord = texcoord;
	gl_Position = vec4(pos.xy, 0.0, 1.0); 
}