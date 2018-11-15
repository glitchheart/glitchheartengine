#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 2) in vec3 offset;
layout(location = 3) in vec4 color;
layout(location = 4) in vec2 size;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec2 fs_uv;
out vec4 fs_color;

void main()
{
    
	vec3 vertexPositionWorld = 
		(offset - vec3(size.x / 2.0, size.y / 2.0, 0.0))
		+ cameraRight * position.x * size.x
		+ cameraUp * position.y * size.y;
    
	gl_Position = projectionMatrix * viewMatrix * vec4(vertexPositionWorld, 1.0);
    
	fs_uv = uv;
	fs_color = color;
}