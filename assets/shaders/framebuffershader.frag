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
	vec4 color = vec4(distance, distance, distance, 1.0) * 0.1;

	//color = vec4(0.08, 0.08, 0.08, 1);

	vec4 lightingColor = texture(lightingTex, Texcoord);
	vec4 endColor = texture(tex, Texcoord);
	
	vec4 ambient = vec4(0.7,0.05,0.1,1) * endColor * 0.8;
	vec4 linearColor = ambient + lightingColor + color;

	

	//outColor = endColor * (color + lightingColor + ambient);
	if(!ignoreLight)
		endColor = endColor * vec4(linearColor.rgb, lightingColor.a);
	
	endColor.rgb /= endColor.a;

	//Contrast 1.64
	endColor.rgb += ((endColor.rgb - 0.5) * max(contrast, 0.0)) + 0.5f;
	
	// Brightness 1.0 - 2.0/2.8
	endColor.rgb += 1.0 - 2.0/brightness;

	endColor.rgb *= endColor.a;

	float gamma = 1.8;
	endColor.rgb = pow(endColor.rgb, vec3(1.0/gamma));
	outColor = endColor;
}
