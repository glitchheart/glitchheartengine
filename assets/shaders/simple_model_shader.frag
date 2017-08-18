#version 330 core
in vec4 c;
in vec3 n;
in vec3 v;
in vec3 pos;

out vec4 outColor;

void main()
{
	vec3 lightDir = normalize(vec3(0, 1, 1) - pos);
	vec3 viewDir = normalize(-v);
	vec3 reflectDir = -reflect(lightDir, n);

	// Ambient term
	vec4 ambient = vec4(0, 0.1, 0.3, 1);

	// Diffuse term
	float diff = max(dot(n, lightDir), 0.0);
	
	vec3 lightColor = vec3(1.0, 1.0, 1.0);
	vec3 diffuse = diff * lightColor;
	diffuse = clamp(diffuse, 0.0, 1.0);

	float specularStrength = 0.5;
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	specular = clamp(specular, 0.0, 1.0);

	outColor = (ambient + vec4(diffuse, 1) + vec4(specular,1));
}

