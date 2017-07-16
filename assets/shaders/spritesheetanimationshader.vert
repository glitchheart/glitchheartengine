#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform vec2 textureOffset;
uniform float frameWidth;
uniform float frameHeight;
uniform vec2 textureSize;
uniform float isUI;

in vec2 texcoord;
in vec2 pos;
out vec2 Texcoord;
out vec2 TextureSize;
out vec2 TextureOffset;

void main()
{
	TextureSize = textureSize;    
	TextureOffset = vec2(1.0 / textureSize.x * textureOffset.x, 1.0 / textureSize.y * textureOffset.y);

	Texcoord = vec2(1.0 / textureSize.x * textureOffset.x + texcoord.x / (textureSize.x / frameWidth), 1.0 / textureSize.y * textureOffset.y + texcoord.y / (textureSize.y / 		frameHeight));

	if(isUI == 1.0)
		gl_Position = Model * vec4(pos.xy, 0.0, 1.0);
	else
    	gl_Position = Projection * View * Model * vec4(pos.xy, 0.0, 1.0);
}