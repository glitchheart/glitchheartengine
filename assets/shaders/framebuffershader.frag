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
	vec4 lightingColor = texture(lightingTex, Texcoord);
	vec4 endColor = texture(tex, Texcoord);
	
	vec4 ambient = clamp(ambientColor * endColor *  ambientIntensity,0.0,1.0);
	vec4 linearColor = lightingColor;
	linearColor = clamp(linearColor,0.0,1.0);
    linearColor = linearColor + ambient;
	linearColor = clamp(linearColor, 0.0,1.0);
	
	if(!ignoreLight)
	{
		endColor = endColor * vec4(linearColor.rgb, lightingColor.a);
		
		endColor.rgb /= endColor.a;

		//Contrast
		endColor.rgb += (endColor.rgb - 0.5) * (contrast) + 0.5f;
		
		// Brightness
		endColor.rgb += brightness;

		endColor.rgb *= endColor.a;

		float gamma = 2.2;
		endColor.rgb = pow(endColor.rgb, vec3(1.0/gamma));
	}
	
	outColor = clamp(endColor,0.0,1.0);
}
