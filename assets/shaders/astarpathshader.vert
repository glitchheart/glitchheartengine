#version 150

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec4 color;
in vec2 pos;
out vec4 c;

void main()
{
	c = color;
    gl_Position = Projection * View * Model * vec4(pos.x, pos.y, 0.0, 1.0);
}





