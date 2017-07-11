#version 150
in vec4 c;
in vec2 Texcoord;

uniform mat4 model;

uniform sampler2D tex;
uniform sampler2D lightingTex;
uniform bool ignoreLight;

out vec4 outColor;

void main()
{
	float distance =  1 - distance(vec2(Texcoord.x, Texcoord.y), vec2(0.5,0.5)) * 1.5;
	distance -= 0.2;
	vec4 color = vec4(distance, distance, distance, 1.0);

	color = vec4(0.2, 0.2, 0.2, 1);

	vec4 lightingColor = texture(lightingTex, Texcoord);
	vec4 endColor = texture(tex, Texcoord);
	
	if(!ignoreLight)
		outColor = endColor * (color  + lightingColor);
	else
		outColor = endColor;
}