#version 330 core
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 M;
uniform float isUI;
uniform vec4 color;


layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 texcoord;

out VS_OUT
{
    vec4 c;
    vec2 uv;
} vs_out;


void compute_position()
{
    if(isUI == 1.0)
        gl_Position = Projection * M * vec4(pos.xy, 0.0, 1.0);
    else
        gl_Position = Projection * View *  M * vec4(pos.xy, 0.0, 1.0);
}

void main()
{
    vs_out.uv = texcoord;
    compute_position();
    vs_out.c = color;
}