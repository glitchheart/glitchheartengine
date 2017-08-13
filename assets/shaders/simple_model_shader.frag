#version 330 core
in vec4 c;
in vec3 n;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec4 ambient = vec4(0, 0.1, 0.3, 1);
	vec3 normal = normalize(n);
	vec3 lightDir = normalize(vec3(0, 1, 1) - pos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 diffuse = diff * vec3(1, 1, 0);
	outColor = (ambient + vec4(diffuse, 1));
}

