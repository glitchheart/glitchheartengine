#version 150
in vec4 C;
in vec2 Texcoord;
uniform sampler2D tex;
out vec4 outColor;

void main()
{
    outColor = texture(tex, Texcoord) * C;
}
