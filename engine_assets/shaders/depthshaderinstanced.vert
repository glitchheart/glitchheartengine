#version 330 core

layout(location = 0) in vec3 position;
layout(location = 0) in vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform vec3 offsets[900];

void main()
{
	vec3 offset = offsets[gl_InstanceID];
	
	mat4 translate;
	translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
	translate[1] = vec4(0.0, 1.0, 0.0, 0);
	translate[2] = vec4(0.0, 0.0, 1.0, 0);
	translate[3] = vec4(offset[0], offset[1], offset[2], 1.0);
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * translate * vec4(position, 1);
}