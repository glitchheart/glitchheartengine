#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec2 textureOffset;
uniform float frameWidth;
uniform float frameHeight;
uniform vec2 sheetSize;
uniform vec4 Color;
uniform float isUI;

in vec2 texcoord;
in vec2 pos;
out vec4 C;
out vec2 Texcoord;
out vec2 TextureSize;
out vec2 TextureOffset;

void main()
{
    C = Color;
	TextureSize = sheetSize;    
	TextureOffset = vec2(1.0 / sheetSize.x * textureOffset.x, 1.0 / sheetSize.y * textureOffset.y);

	Texcoord = vec2(1.0 / sheetSize.x * textureOffset.x + texcoord.x / (sheetSize.x / frameWidth), 1.0 / sheetSize.y * textureOffset.y + texcoord.y / (sheetSize.y / 		frameHeight));

	if(isUI == 1.0)
		gl_Position = Model * vec4(pos.xy, 0.0, 1.0);
	else
    	gl_Position = Projection * View * Model * vec4(pos.xy, 0.0, 1.0);
}