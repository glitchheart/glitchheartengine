#version 150
in vec4 c;
in vec2 Texcoord;
uniform sampler2D tex;

out vec4 outColor;

void main()
{
    vec4 color = texture(tex, Texcoord) * c;
    if(color.a == 0.0)
       discard;
    outColor = color;
}