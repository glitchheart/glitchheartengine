#version 150
uniform mat4 projectionMatrix;
in vec4 coord;
out vec2 texcoord;

void main(void) 
{
  gl_Position = projectionMatrix * vec4(coord.xy, 0, 1);
  texcoord = coord.zw;
}
