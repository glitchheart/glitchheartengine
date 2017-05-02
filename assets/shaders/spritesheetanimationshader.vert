#version 150
uniform mat4 MVP;
uniform vec2 textureOffset;
uniform vec2 sheetSize; // Smells like shit

in vec2 texcoord;
in vec2 pos;
out vec3 color;
out vec2 Texcoord;

void main()
{
    Texcoord = vec2(texcoord.x / sheetSize.x + textureOffset.x, texcoord.y / sheetSize.y + textureOffset.y);
    gl_Position = MVP * vec4(pos.xy, 0.0, 1.0);
}