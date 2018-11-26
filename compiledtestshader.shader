#version 330 core

uniform float speed;
uniform bool glowing;

struct MVP
{
    mat4 model;
    mat4 view;
    mat4 projection;
};
    
uniform MVP mvp;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal
layout(location = 2) in vec2 uv;

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
}

#version 330 core

uniform float specularIntensity;
uniform sampler2D specularTexture;
uniform vec4 col0;

out vec4 outColor;

void main()
{
    outColor = col0 * texture2D(tex0, uv);
}