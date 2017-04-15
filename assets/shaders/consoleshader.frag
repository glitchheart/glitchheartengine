#version 150
in vec4 color;
out vec4 outColor;

void main()
{
    outColor = vec4(color.x, color.y, color.z, 0.2);
}