#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
in vec2 pos;
in vec2 texcoord;
out vec3 color;
out vec2 Texcoord;

void main()
{
    Texcoord = texcoord;
    gl_Position = Projection * View * Model * vec4(pos.xy, 0.0, 1.0);
}
