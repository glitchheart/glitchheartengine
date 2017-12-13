#version 150
in vec4 coord;
out vec2 texcoord;

void main(void) 
{
  gl_Position = vec4(coord.xyz, 1);
  texcoord = coord.zw;
}
