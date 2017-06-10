#version 150
uniform mat4 M;
uniform vec4 color;
in vec2 pos;
in vec2 texcoord;
out vec4 Color;
out vec2 Texcoord;
out vec2 TextureOffset;

void main()
{
    Texcoord = texcoord;
	Color = color;
    gl_Position = M * vec4(pos.xy, 0.0, 1.0);
}
