#version 150
uniform mat4 MVP;

in vec3 vCol;
attribute vec2 vPos;
in vec2 texcoord;
out vec3 color;
out vec2 Texcoord;

void main()
{
    color = vCol;
    Texcoord = texcoord;
    gl_Position = MVP * vec4(vPos.xy, 0.0, 1.0);
}