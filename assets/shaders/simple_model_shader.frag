#version 330 core
in vec4 c;
in vec3 n;
in vec3 v;
in vec3 lPos;

out vec4 outColor;

void main()
{
	vec3 light = normalize(lPos - v);
	float diffuse = max(dot(light, n), 0.0);
	outColor = vec4(diffuse, diffuse, diffuse, 1.0);
}

