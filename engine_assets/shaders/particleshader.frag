#version 330 core

in vec2 fs_uv;
in vec4 fs_color;
out vec4 outColor;

uniform sampler2D diffuseTexture;

void main()
{
    outColor = fs_color; //texture(diffuseTexture, fs_uv) * fs_color;
}

