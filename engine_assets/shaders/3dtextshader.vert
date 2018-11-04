#version 330 core
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 model;
uniform vec4 color;

in vec4 coord;

out vec2 texcoord;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * model * vec4(coord.xy, 1.0, 1.0);
    texcoord = coord.zw;
}
