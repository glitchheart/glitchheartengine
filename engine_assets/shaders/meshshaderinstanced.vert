#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 uv;

layout(location = 3) in vec3 offset;
layout(location = 4) in vec4 color;
layout(location = 5) in vec3 rotation;
layout(location = 6) in vec3 scale;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform mat4 depthBiasMatrix;
uniform mat4 depthModelMatrix;
uniform mat4 depthViewMatrix;
uniform mat4 depthProjectionMatrix;

uniform vec3 lightPosWorld;

out VS_OUT
{
	vec4 color;
	vec4 shadowCoord;
	vec3 normal;
	vec2 uv;
	vec3 posWorld;
	vec3 eyeView;
	vec3 lightDir;
} vs_out;

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
	
	mat4 x_rotMat = rotationMatrix(vec3(1, 0, 0), rotation.x);	
	mat4 y_rotMat = rotationMatrix(vec3(0, 1, 0), rotation.y);	
	mat4 z_rotMat = rotationMatrix(vec3(0, 0, 1), rotation.z);
	mat4 rotMat = x_rotMat * y_rotMat * z_rotMat;	
    
	mat4 scaling;
	scaling[0] = vec4(scale.x, 0.0,     0.0,     0.0);
	scaling[1] = vec4(0.0,     scale.y, 0.0,     0.0);
	scaling[2] = vec4(0.0,     0.0,     scale.z, 0.0);
	scaling[3] = vec4(0.0,     0.0,     0.0,     1.0);
    
	gl_Position = projectionMatrix * viewMatrix * translate * rotMat * scaling * modelMatrix * vec4(position, 1);
    
	vs_out.posWorld  = (translate * rotMat * scaling * modelMatrix * vec4(position, 1)).xyz;
    
	// Shadow mapping
	mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
	mat4 depthBiasMVP = depthMVP;
	vs_out.shadowCoord = depthBiasMVP * vec4(vs_out.posWorld.xyz, 1);
    
	vec3 viewPos = (viewMatrix * translate * rotMat * scaling * modelMatrix * vec4(position, 1)).xyz;
	vs_out.eyeView = vec3(0, 0, 0) - viewPos;
    
	vec3 lightPosView = (viewMatrix * vec4(lightPosWorld, 1)).xyz;
	vs_out.lightDir = lightPosView + vs_out.eyeView;
    
	vs_out.normal = mat3(transpose(inverse(viewMatrix * translate * rotMat * 	scaling * modelMatrix))) * vertexNormal;
	vs_out.uv = uv;
	vs_out.color = color;
}
