#version 150
uniform float radius;
uniform float intensity;
uniform vec4 color;
uniform vec4 LightPos[32];
uniform vec2 screenSize;
uniform int NUM_LIGHTS;
uniform mat4 P;
uniform mat4 V;

out vec4 outColor;

void main()
{
	vec4 finalColor = vec4(0.0,0.0,0.0,0.0);
	for(int i = 0; i < NUM_LIGHTS; i++)
	{
		float dist = distance(LightPos[i], (inverse(P * V)) * vec4(
			(gl_FragCoord.x/screenSize.x - 0.5) * 2.0,
			(gl_FragCoord.y/screenSize.y - 0.5) * 2.0,0,1));

    	if(dist < radius)
			finalColor += color * (radius - dist);
    	else
			finalColor += vec4(0,0,0,1);
	}
	outColor = finalColor;
}