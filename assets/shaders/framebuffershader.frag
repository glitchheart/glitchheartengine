#version 150
in vec4 c;
in vec2 Texcoord;

uniform mat4 model;

uniform sampler2D tex;
uniform sampler2D lightingTex;
out vec4 outColor;

void main()
{
	float distance =  1 - distance(vec2(Texcoord.x, Texcoord.y), vec2(0.5, 		0.5)) * 1.5;
	vec4 color = vec4(distance, distance, distance, 1.0);
	vec4 lightingColor = texture(lightingTex, Texcoord);
	vec4 endColor = texture(tex, Texcoord);

	if(lightingColor.r < 0.1 && lightingColor.g < 0.1 && lightingColor.b < 0.1)
	{
		outColor = endColor * color;
	}
	else
		outColor = endColor * color + lightingColor * 0.4;
}