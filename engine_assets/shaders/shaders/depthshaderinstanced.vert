#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 offset;
layout(location = 4) in vec4 color;
layout(location = 5) in vec3 rotation;
layout(location = 6) in vec3 scale;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

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
	mat4 translate;
	translate[0] = vec4(1.0, 0.0, 0.0, 0.0);
	translate[1] = vec4(0.0, 1.0, 0.0, 0);
	translate[2] = vec4(0.0, 0.0, 1.0, 0);
	translate[3] = vec4(offset[0], offset[1], offset[2], 1.0);

	mat4 rotMat = rotationMatrix(vec3(0, 1, 0), rotation.y);

	mat4 scaling;
	scaling[0] = vec4(scale.x, 0.0,     0.0,     0.0);
	scaling[1] = vec4(0.0,     scale.y, 0.0,     0.0);
	scaling[2] = vec4(0.0,     0.0,     scale.z, 0.0);
	scaling[3] = vec4(0.0,     0.0,     0.0,     1.0);

	gl_Position = projectionMatrix * viewMatrix * translate * rotMat * scaling * modelMatrix * vec4(position, 1);
}