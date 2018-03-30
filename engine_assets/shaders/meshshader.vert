#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 lightPosWorld;
uniform vec4 color;

out vec3 normal;
out vec3 posWorld;
out vec3 eyeView;
out vec3 lightDir;
out vec4 c;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1);

	posWorld  = (modelMatrix * vec4(position, 1)).xyz;

	vec3 viewPos = (viewMatrix * modelMatrix * vec4(position, 1)).xyz;
	eyeView = vec3(0, 0, 0) - viewPos;

	vec3 lightPosView = (viewMatrix * vec4(lightPosWorld, 1)).xyz;
	lightDir = lightPosView + eyeView;

	normal = mat3(transpose(inverse(viewMatrix * modelMatrix))) * vertexNormal;
	c = color;
}