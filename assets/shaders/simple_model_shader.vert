#version 330 core

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec2 uv;

uniform mat4 normalMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

out vec4 c;
out vec3 n;
out vec3 v;
out vec3 lPos;

void main()
{
	gl_Position = projection * view * model * vec4(position,1.0);
	n = normalize(((view * model * vec4(normal, 0.0)).xyz));
	v = (view * model * vec4(position, 1.0)).xyz;
	lPos = (vec4(-10.0 , 5.0, 0,1.0)).xyz;
}