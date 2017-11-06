#version 150
in vec2 Texcoord;
uniform sampler2D tex;
uniform vec4 Color;
out vec4 outColor;

void main()
{
    outColor = texture(tex, Texcoord);
}