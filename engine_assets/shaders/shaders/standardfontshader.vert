#version 150
uniform mat4 projectionMatrix;
uniform float z;
in vec4 coord;
out vec2 texcoord;

void main(void) 
{
    gl_Position = projectionMatrix * vec4(coord.xy, z, 1);
    texcoord = coord.zw;
}
