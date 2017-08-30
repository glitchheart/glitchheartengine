#version 330 core

layout(location = 0)in vec3 position;
layout(location = 1)in vec3 normal;
layout(location = 2)in vec2 uv;
layout(location = 3)in int boneCount;
layout(location = 4)in ivec4 boneIndices;
layout(location = 5)in vec4 weights;

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
	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	
	for(int i = 0; i < 4; i++)
	{
		mat4 boneTransform = bones[boneIndices[i]];
		vec4 posePosition = boneTransform * vec4(position, 1.0);
		 		
		totalLocalPos += posePosition;// * weights[i];
		
		//vec4 worldNormal = boneTransform * vec4(in_normal, 0.0);
		//totalNormal += worldNormal * in_weights[i];
	}
	
	gl_Position = projection * view * model * totalLocalPos;

	//vec4 p = projection * view * model * vec4(position, 1.0);

//	gl_Position = p;

	n = mat3(transpose(inverse(view * model))) * normal;
	fragPos = vec3(view * model * vec4(position, 1.0));

    lightPos = (view * vec4(6.0f, 6.0f, 0, 1.0)).xyz; // Get actual position from lights
    viewPos = vec3(0); // eye in view coords
	t = time;
	texCoord = uv;
}