#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec4 Color;
in vec2 pos;
in vec2 texcoord;
out vec4 color;
out vec2 Texcoord;

void main()
{
	color = Color;
    Texcoord = texcoord;
    gl_Position = Projection * View * Model * vec4(pos.xy, 0.0, 1.0);
}
