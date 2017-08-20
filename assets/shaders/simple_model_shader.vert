#version 330 core

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec2 uv;

uniform mat4 normalMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;
uniform float time;

out vec4 c;
out vec3 n;
out vec3 fragPos;
out vec3 lightPos;
out vec3 viewPos;
out float t;
out vec2 texCoord;

void main()
{
	gl_Position = projection * view * model * vec4(position,1.0);
	n = mat3(transpose(inverse(view * model))) * normal;
	fragPos = vec3(view * model * vec4(position, 1.0));
	lightPos = (view * vec4(-2.0, 2.0, 0.0,1.0)).xyz;
    viewPos = vec3(0); // eye in view coords
	t = time;
	texCoord = uv;
}