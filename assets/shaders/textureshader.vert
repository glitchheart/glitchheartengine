#version 150
uniform mat4 MVP;

in vec3 col;
attribute vec2 pos;
in vec2 texcoord;
out vec3 color;
out vec2 Texcoord;

void main()
{
    color = col;
    Texcoord = texcoord;
    gl_Position = MVP * vec4(pos.xy, 0.0, 1.0);
}