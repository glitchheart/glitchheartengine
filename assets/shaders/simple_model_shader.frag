#version 330 core
in vec4 c;
in vec3 n;
in vec3 v;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec3 L = normalize(vec3(1.0,1.0,1.0) - pos);
	vec4 Idiff = max(dot(n,L), 0.0) * vec4(1.0,1.0,0.0,1.0);
	Idiff = clamp(Idiff, 0.0, 1.0);

	outColor = c + Idiff;
}

