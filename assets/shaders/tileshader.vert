#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec2 textureOffset;

in vec2 texcoord;
in vec2 pos;
in vec2 sheetSize;
out vec3 color;
out vec2 Texcoord;

void main()
{
	Texcoord = vec2(texcoord.x / sheetSize.x + 0.6,0);
//    Texcoord = vec2(texcoord.x / 5.0 + textureOffset.x / sheetSize.x, texcoord.y / 5.0 + textureOffset.y / sheetSize.y);
    gl_Position = Projection * View * Model  * vec4(pos.xy, 0.0, 1.0);
}