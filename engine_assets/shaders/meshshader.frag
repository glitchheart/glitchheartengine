#version 330 core
uniform vec4 color;
uniform sampler2D tex;
out vec4 fragColor;

void main()
{
    fragColor = color;
}