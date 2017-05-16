#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec2 textureOffset;
uniform vec2 sheetSize;
uniform vec4 color;

in vec2 texcoord;
in vec2 pos;
out vec4 C;
out vec2 Texcoord;

void main()
{
    C = color;
    Texcoord = vec2(texcoord.x / sheetSize.x + textureOffset.x, texcoord.y / sheetSize.y + textureOffset.y);
    gl_Position = Projection * View * Model * vec4(pos.xy, 0.0, 1.0);
}