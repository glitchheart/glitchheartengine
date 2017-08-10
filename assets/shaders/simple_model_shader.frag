#version 330 core
in vec4 c;

out vec4 outColor;

void main()
{
    outColor = gl_FragCoord;
}

