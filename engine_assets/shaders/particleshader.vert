#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 2) in vec3 offset;
layout(location = 3) in vec4 color;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec2 fs_uv;
out vec4 fs_color;

void main()
{

	vec3 vertexPositionWorld = 
		offset
		+ cameraRight * position.x * 0.1
		+ cameraUp * position.y * 0.1;

	gl_Position = projectionMatrix * viewMatrix * vec4(vertexPositionWorld, 1.0);

	fs_uv = uv;
	fs_color = color;
}