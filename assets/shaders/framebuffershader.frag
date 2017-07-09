#version 150
in vec4 c;
in vec2 Texcoord;

uniform mat4 model;

uniform sampler2D tex;
uniform sampler2D lightingTex;
out vec4 outColor;

void main()
{
	vec4 lightingColor = texture(lightingTex, Texcoord);
	vec4 endColor = texture(tex, Texcoord);

	if(lightingColor.r < 0.1 && lightingColor.g < 0.1 && lightingColor.b < 0.1)
	{
		outColor = endColor;
	}
	else
		outColor = endColor + lightingColor * 0.2;
}