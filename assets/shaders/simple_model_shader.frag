#version 330 core
in vec4 c;
in vec3 n;
in vec3 fragPos;
in vec3 lightPos;
in vec3 viewPos;
in float t;

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;
	
	float constant;
	float linear;
	float quadratic;
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 outColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(-light.direction);
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec;
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(light.position - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance 
							 + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient;
	vec3 diffuse = light.diffuse * diff;
	vec3 specular = light.specular * spec;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

// @Incomplete: Temp function until we get actual lights
void CreatePointLight(out PointLight pointLight)
{
	pointLight.position = lightPos;
	
	pointLight.constant = 1.0;
	pointLight.linear = 0.09;
	pointLight.quadratic = 0.032;
	
	pointLight.ambient = vec3(0.0,0.0,0.0);
	pointLight.diffuse = vec3(0.5,0.3,0.1);
	pointLight.specular = vec3(1.0, 1.0, 1.0);
}

void setMaterial(out Material material)
{
	material.ambient = vec3(1.0, 0.5, 0.31);
	material.diffuse = vec3(0.5, 0.5, 0.31);
	material.specular = vec3(1.0, 1.0, 1.0);
	material.shininess = 32.0;
}

void whitePlastic(out Material material)
{
	material.ambient = vec3(0.0);
	material.diffuse = vec3(0.55);
	material.specular = vec3(0.70);
	material.shininess = 0.25;
}



void main()
{
	Material material;
	setMaterial(material);

	vec3 norm = normalize(n);
	vec3 viewDir = normalize(viewPos - fragPos);	

	DirLight dirLight;
	dirLight.direction = vec3(-0.2, -1.0, -0.3);
	dirLight.ambient = vec3(0.2, 0.2, 0.2);
	dirLight.diffuse = vec3(0.4, 0.2, 0.25);
	dirLight.specular = vec3(1.0, 1.0, 1.0);
	vec3 result = CalcDirLight(dirLight, norm, viewDir, material);
	result = vec3(0);	

	PointLight pointLights[4];
	PointLight point1;
	PointLight point2;
	PointLight point3;
	PointLight point4;

	CreatePointLight(point1);
	CreatePointLight(point2);
	CreatePointLight(point3);
	CreatePointLight(point4);

	pointLights[0] = point1;
	pointLights[1] = point2;
	pointLights[2] = point3;
	pointLights[3] = point4;

	for(int i = 0; i < 4; i++)
		result += CalcPointLight(pointLights[i], norm, fragPos, viewDir, material);

	outColor = vec4(result, 1.0);
} 

