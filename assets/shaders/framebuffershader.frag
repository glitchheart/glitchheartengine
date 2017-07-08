#version 150
in vec4 c;
in vec2 Texcoord;
uniform sampler2D tex;
out vec4 outColor;

void main()
{
	float distance =  1 - distance(vec2(Texcoord.x, Texcoord.y), vec2(0.5, 0.5)) * 1.5;
	vec4 color = vec4(distance, distance, distance, 1.0);
    outColor = texture(tex, Texcoord) * color;
}