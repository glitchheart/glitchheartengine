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
	//gl_Position = projection * view * model * vec4(position,1.0);

	vec4 p = projection * view * model * vec4(position, 1.0);
	
	if(p.w != 1)
	{
//		p.x = p.x / p.w;
//		p.y = p.y / p.w;
//		p.z = p.z / p.w;
	}

	gl_Position = p;

	n = mat3(transpose(inverse(view * model))) * normal;
	fragPos = vec3(view * model * vec4(position, 1.0));

    lightPos = (view * vec4(6.0f, 6.0f, 0, 1.0)).xyz; // Get actual position from lights
    viewPos = vec3(0); // eye in view coords
	t = time;
	texCoord = uv;
}