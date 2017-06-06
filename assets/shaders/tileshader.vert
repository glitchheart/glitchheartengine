#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec2 textureOffset;
uniform vec2 sheetSize;

in vec2 pos;
in vec2 texcoord;
out vec4 color;
out vec2 Texcoord;

void main()
{
	float OffsetX = 1.0 / 160.0 * textureOffset.x;
	float RelativeTexCoordX = texcoord.x * 32.0 / sheetSize.x;

	float OffsetY = 1.0 / sheetSize.y * textureOffset.y;
	float RelativeTexCoordY = texcoord.y * 32.0 / sheetSize.y;

	Texcoord = vec2(OffsetX + RelativeTexCoordX, OffsetY + RelativeTexCoordY);
	gl_Position = Projection * View * Model  * vec4(pos.xy, 0.0, 1.0);
}