#version 150
in vec4 c;
in vec2 Texcoord;
uniform mat4 model;
uniform vec4 ambientColor;
uniform float ambientIntensity;
uniform sampler2D tex;
uniform sampler2D lightingTex;
uniform bool ignoreLight;
uniform float contrast;
uniform float brightness;

out vec4 outColor;

void main()
{
	float distance =  1 - distance(vec2(Texcoord.x, Texcoord.y), vec2(0.5,0.5)) * 1.5;
	distance -= 0.2;
	vec4 color = vec4(distance, distance, distance, 1.0) * 0.3;
		
	//color = vec4(0.08, 0.08, 0.08, 1);

	vec4 lightingColor = texture(lightingTex, Texcoord);
	vec4 endColor = texture(tex, Texcoord);
	
	endColor.rgb *= 0.6;

	vec4 ambient = ambientColor * endColor * ambientIntensity;
	vec4 linearColor = (ambient + lightingColor + color);
	
	//outColor = endColor * (color + lightingColor + ambient);
	if(!ignoreLight)
	{
		endColor = endColor + vec4(linearColor.rgb, lightingColor.a);
		
		endColor.rgb /= endColor.a;

		//Contrast
		endColor.rgb += (endColor.rgb - 0.5) * contrast + 0.5f;
		
		// Brightness
		endColor.rgb += brightness - 0.20;

		endColor.rgb *= endColor.a;

		float gamma = 1.0;
		endColor.rgb = pow(endColor.rgb, vec3(1.0/gamma));
	}
	outColor = endColor;
}
