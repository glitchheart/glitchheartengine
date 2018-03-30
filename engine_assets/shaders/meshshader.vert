#version 330 core
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

layout(location = 0) in vec3 pos;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(pos.xyz, 1.0);
}