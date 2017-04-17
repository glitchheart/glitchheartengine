#version 150
uniform mat4 MVP;
in vec2 pos;
in vec2 texcoord;
out vec3 color;
out vec2 Texcoord;

void main()
{
    Texcoord = texcoord;
    gl_Position = MVP * vec4(pos.xy, 0.0, 1.0);
}