#version 150

uniform vec4 PointLightPos[32];
uniform vec4 PointLightColors[32];
uniform float PointLightRadius[32];
uniform float PointLightIntensity[32];
uniform float PointLightConstantAtt[32];
uniform float PointLightLinearAtt[32];
uniform float PointLightExpAtt[32];
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

		vec4 dir = normalize(PointLightPos[i] - ((inverse(P*V)) * vec4((gl_FragCoord.x/screenSize.x - 0.5) * 2.0,
			(gl_FragCoord.y/screenSize.y - 0.5) * 2.0,0,1)));
		float lp = 1/(PointLightConstantAtt[i] + PointLightLinearAtt[i] * dist + PointLightExpAtt[i] * dist *  dist); 
    	if(dist < PointLightRadius[i])
			finalColor += (lp *  PointLightColors[i]) * PointLightIntensity[i];// * (PointLightRadius[i] - dist);
    	else
			finalColor += vec4(0,0,0,1);
	}
	outColor = finalColor;
}