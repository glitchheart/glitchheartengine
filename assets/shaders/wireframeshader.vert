#version 150
uniform mat4 MVP;
uniform vec4 color;
in vec2 pos;
out vec4 c;

void main()
{
	c = color;
    gl_Position = MVP * vec4(pos.x, pos.y, 0.0, 1.0);
}



