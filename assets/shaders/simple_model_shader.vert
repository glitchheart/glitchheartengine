#version 330 core

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec2 uv;
layout(location = 3)in vec4 boneIndices;
layout(location = 4)in vec4 weights;

uniform mat4 normalMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;
uniform float time;
uniform mat4 bones[100];

out vec4 c;
out vec3 n;
out vec3 fragPos;
out vec3 lightPos;
out vec3 viewPos;
out float t;
out vec2 texCoord;

void main()
{
    mat4 boneTransform = bones[int(boneIndices.x)] * weights.x;
    boneTransform += bones[int(boneIndices.y)] * weights.y;
    boneTransform += bones[int(boneIndices.z)] * weights.z;
    boneTransform += bones[int(boneIndices.w)] * weights.w;

	vec4 pos = boneTransform * vec4(position, 1.0);
	vec4 p = projection * view * model * pos;
	//vec4 p = projection * view * model * vec4(position, 1.0f);
	gl_Position = p;
	
	vec4 norm = boneTransform * vec4(normal, 1.0);
	n = mat3(transpose(inverse(view * model))) * norm.xyz;
	fragPos = vec3(view * model * vec4(position, 1.0));

    lightPos = (view * vec4(6.0f, 6.0f, 0, 1.0)).xyz; // Get actual position from lights
    viewPos = vec3(0); // eye in view coords
	t = time;
	texCoord = uv;
}