#version 330 core
in vec4 c;
in vec3 n;
in vec3 v;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec3 L = normalize(vec3(1.0,1.0,1.0) - v);
	vec4 Idiff = vec4(1.0,1.0,1.0,1.0) * max(dot(n,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);

	outColor = c + Idiff;
}

