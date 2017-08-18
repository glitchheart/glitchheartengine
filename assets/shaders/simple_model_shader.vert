#version 330 core

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec2 uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;

out vec4 c;
out vec3 n;
out vec3 pos;
out vec3 v;

void main()
{
	v = vec3(view * model * vec4(position,1.0)).xyz;
	n = normalize(mat3(transpose(inverse(view * model))) * normal);
	pos = vec3(model * vec4(position, 1.0));
	c = vec4(0.3,0.3,0.3,1.0);

	gl_Position = projection * view * model * vec4(position,1.0);
}