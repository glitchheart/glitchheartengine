#version 150
in vec2 texcoord;
uniform sampler2D tex;
uniform vec4 color;
out vec4 outColor;

void main() 
{
	vec4 color = vec4(1, 1, 1, texture(tex, texcoord).r) * color;
	if(color.a == 0.0f)
	  discard;
	outColor = color;
}
