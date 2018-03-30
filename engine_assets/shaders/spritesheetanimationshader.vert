#version 150
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 M;
uniform vec2 textureOffset;
uniform vec2 frameSize;
uniform float isUI;
uniform vec2 textureSize;

in vec2 texcoord;
in vec3 pos;
out vec2 Texcoord;
out vec2 TextureOffset;

void main()
{    
	vec2(1.0 / textureSize.x * textureOffset.x, 1.0 / textureSize.y * textureOffset.y);

	Texcoord = vec2(1.0 / textureSize.x * textureOffset.x + texcoord.x / (textureSize.x / frameSize.x), 1.0 / textureSize.y * textureOffset.y + texcoord.y / (textureSize.y / frameSize.y));

	if(isUI == 1.0)
		gl_Position = Projection * M * vec4(pos.xy, 0.0, 1.0);
	else
    	gl_Position = Projection * View * M * vec4(pos.xyz, 1.0);
}