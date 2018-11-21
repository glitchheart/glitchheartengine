#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 2) in vec3 offset;
layout(location = 3) in vec4 color;
layout(location = 4) in vec2 size;
layout(location = 5) in float angle;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

out vec2 fs_uv;
out vec4 fs_color;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

void main()
{
    
	vec3 vertexPositionWorld =
		+ cameraRight * position.x * size.x
		+ cameraUp * position.y * size.y;

	vec3 cameraForward = cross(cameraRight, cameraUp);

	mat4 rotMat = rotationMatrix(cameraForward, angle);

	vertexPositionWorld = (vec4(offset, 1.0) + (rotMat * (vec4(vertexPositionWorld, 1.0)))).xyz;

	gl_Position = projectionMatrix * viewMatrix * vec4(vertexPositionWorld, 1.0);
    
	fs_uv = uv;
	fs_color = color;
}