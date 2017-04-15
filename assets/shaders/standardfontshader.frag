#version 150
in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 outColor;

void main() 
{
  	outColor = vec4(1, 1, 1, texture2D(tex, texcoord).a) * vec4(0, 1, 0, 1);
}