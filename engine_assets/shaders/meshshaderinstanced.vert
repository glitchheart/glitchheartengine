#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 uv;

layout(location = 3) in vec3 offset;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 depthBiasMatrix;
uniform mat4 depthModelMatrix;
uniform mat4 depthViewMatrix;
uniform mat4 depthProjectionMatrix;

uniform vec3 lightPosWorld;
uniform vec4 color;

out VS_OUT
{
	vec4 color;
	vec4 shadowCoord;
	vec3 normal;
	vec2 uv;
	vec3 posWorld;
	vec3 eyeView;
	vec3 lightDir;
} vs_out;

void main()
{
	mat4 translate;
	translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
	translate[1] = vec4(0.0, 1.0, 0.0, 0);
	translate[2] = vec4(0.0, 0.0, 1.0, 0);
	translate[3] = vec4(offset[0], offset[1], offset[2], 1.0);
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * translate * vec4(position, 1);

	vs_out.posWorld  = (modelMatrix * translate * vec4(position, 1)).xyz;

	// Shadow mapping
	mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	mat4 depthBiasMVP = depthMVP;
	vs_out.shadowCoord = depthBiasMVP * vec4(vs_out.posWorld.xyz, 1);

	vec3 viewPos = (viewMatrix * modelMatrix * translate * vec4(position, 1)).xyz;
	vs_out.eyeView = vec3(0, 0, 0) - viewPos;

	vec3 lightPosView = (viewMatrix * vec4(lightPosWorld, 1)).xyz;
	vs_out.lightDir = lightPosView + vs_out.eyeView;

	vs_out.normal = mat3(transpose(inverse(viewMatrix * modelMatrix * translate))) * vertexNormal;
	vs_out.uv = uv;
	vs_out.color = color;
}

