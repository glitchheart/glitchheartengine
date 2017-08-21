#version 330 core
in vec4 c;
in vec3 n;
in vec3 fragPos;
in vec3 lightPos;
in vec3 viewPos;
in vec2 texCoord;

in float t;
uniform bool hasUVs;
uniform sampler2D tex;

struct SpotLight
{
	vec4 position;
	vec4 direction;
	float cutOff;
	float outerCutOff;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirLight
{
	vec4 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

struct PointLight
{
	vec4 position;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;	

	float constant;
	float linear;
	float quadratic;
};

layout(std140) uniform spotlights
{ 
  int numSpotlights;
  SpotLight sLights[20];
};

layout(std140) uniform directionalLights
{
  int numDirLights;
  DirLight dLights[20];
};

layout(std140) uniform pointLights
{ 
  int numPointLights;
  PointLight pLights[20];
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

out vec4 outColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(-light.direction).xyz;
	
	float diff = max(dot(normal, lightDir), 0.0);
	
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	vec3 ambient = light.ambient.xyz;
	vec3 diffuse = light.diffuse.xyz * diff;
	vec3 specular = light.specular.xyz * spec;

	if(hasUVs)
	{
		diffuse *= vec3(texture(tex, texCoord));
		ambient *= vec3(texture(tex, texCoord));
		specular *= vec3(texture(tex, texCoord));
	}
	else
	{
		diffuse *= material.diffuse;
		ambient *= material.ambient;
		specular *= material.specular;
	}

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, Material material)
{
	vec3 lightDir = normalize(light.position.xyz - fragPos);
	
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(light.position.xyz - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance 
							 + light.quadratic * (distance * distance));

<<<<<<< HEAD
	vec3 ambient = light.ambient.xyz;
	vec3 diffuse = light.diffuse.xyz * diff;
	vec3 specular = light.specular.xyz * spec;

	if(hasUVs)
	{
		diffuse *= vec3(texture(tex, texCoord));
		ambient *= vec3(texture(tex, texCoord));	
		specular *= vec3(texture(tex, texCoord));
	}
	else
	{
		diffuse *= material.diffuse;
		ambient *= material.ambient;
		specular *= material.specular;
	}

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

// @Incomplete: Temp function until we get actual lights
void CreatePointLight(out PointLight pointLight)
{
	pointLight.position = vec4(lightPos, 1);
	
	pointLight.constant = 1.0;
	pointLight.linear = 0.09;
	pointLight.quadratic = 0.032;
	
	pointLight.ambient = vec4(0.1);
	pointLight.diffuse = vec4(1,1,1,1);
	pointLight.specular = vec4(1.0, 1.0, 1.0, 1.0);
}

void setMaterial(out Material material)
{
	material.ambient = vec3(0.135, 0.2225, 0.1575);
	material.diffuse = vec3(0.54, 0.89, 0.63);
	material.specular = vec3(0.316228, 0.316228, 0.316228);
	material.shininess = 0.1 * 128;
}

void textureMaterial(out Material material)
{
	material.ambient = vec3(1.0, 0.5, 0.31);
	material.diffuse = vec3(0.5, 0.5, 0.31);
	material.specular = vec3(1.0, 1.0, 1.0);
	material.shininess = 32.0;
}

void main()
{
	Material material;
	if(hasUVs)
		textureMaterial(material);
	else
		setMaterial(material);

	vec3 norm = normalize(n);
	vec3 viewDir = normalize(viewPos - fragPos);	

	DirLight dirLight = dLights[0];
	vec3 result = CalcDirLight(dirLight, norm, viewDir, material);
	/*
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

	for(int i = 0; i < 1; i++)
		result += CalcPointLight(pointLights[i], norm, fragPos, viewDir, material);

*/	
	
    //outColor = vec4(1, 1, 1, 1);
	//outColor = vec4(texture(tex, texCoord));
	outColor = vec4(result, 1.0);
} 
