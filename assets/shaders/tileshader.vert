#version 150
uniform mat4 MVP;
uniform vec2 textureOffset;

in vec2 texcoord;
in vec2 pos;
out vec3 color;
out vec2 Texcoord;

void main()
{
    Texcoord = vec2(texcoord.x / 5.0 - textureOffset.x, texcoord.y + textureOffset.y);
    gl_Position = MVP * vec4(pos.xy, 0.0, 1.0);
}


