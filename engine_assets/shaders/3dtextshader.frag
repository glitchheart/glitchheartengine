#version 330 core

in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 outColor;

void main()
{
    // Think about lighting/shadows?
    outColor = vec4(1, 1, 1, texture(tex, texcoord).r) * color;
}