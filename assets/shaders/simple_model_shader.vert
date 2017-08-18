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
    gl_Position = projection * view * model * vec4(position, 1.0);
	float DepthColor = position.z;
	c = vec4(DepthColor, DepthColor, DepthColor, color.a);
	n = normalize(mat3(transpose(inverse(view * model))) * normal);
	v = (view * model * vec4(position, 1.0)).xyz;
	pos = vec3(model * vec4(position, 1.0));
}