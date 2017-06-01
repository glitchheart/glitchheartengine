#version 150
uniform mat4 M;
uniform vec4 color;
in vec2 pos;
out vec4 c;

void main()
{
    gl_Position = M * vec4(pos.xy, 0.0, 1.0);
	c = color;
}
