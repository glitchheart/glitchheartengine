#version 150

uniform vec4 PointLightPos[32];
uniform vec4 PointLightColors[32];
uniform float PointLightRadius[32];
uniform float PointLightIntensity[32];
uniform float AmbientIntensity;
uniform vec4 AmbientColor;
uniform vec2 screenSize;
uniform int NUM_POINTLIGHTS;
uniform mat4 P;
uniform mat4 V;

out vec4 outColor;

void main()
{
	vec4 finalColor = vec4(0.0,0.0,0.0,0.0);
	for(int i = 0; i < NUM_POINTLIGHTS; i++)
	{
		float dist = distance(PointLightPos[i], (inverse(P * V)) * vec4(
			(gl_FragCoord.x/screenSize.x - 0.5) * 2.0,
			(gl_FragCoord.y/screenSize.y - 0.5) * 2.0,0,1));

    	if(dist < PointLightRadius[i])
			finalColor += PointLightColors[i] * (PointLightRadius[i] - dist) * PointLightIntensity[i];
    	else
			finalColor += vec4(0,0,0,1);
	}
	outColor = finalColor + AmbientColor * AmbientIntensity;
}