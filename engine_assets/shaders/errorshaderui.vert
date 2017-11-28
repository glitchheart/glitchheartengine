#version 150
uniform mat4 M;
in vec2 pos;

void main()
{
    gl_Position = M * vec4(pos.xy, 0.0, 1.0);
}
