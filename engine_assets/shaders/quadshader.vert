#version 330 core
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 M;
uniform float isUI;
uniform vec4 color;

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 t_uv;

out vec4 c;
out vec2 uv;

void main()
{
    uv = t_uv;
    if(isUI == 1.0)
        gl_Position = Projection * M * vec4(pos.xy, 1.0, 1.0);
    else
        gl_Position = Projection * View * M * vec4(pos.xy, 1.0, 1.0);
    
    c = color;
}
