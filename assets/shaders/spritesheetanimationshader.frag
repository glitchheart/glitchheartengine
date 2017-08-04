#version 150
in vec2 TextureSize;
in vec2 TextureOffset;
in vec2 Texcoord;
uniform sampler2D tex;
uniform bool glow;
uniform float time;
uniform vec4 spriteColor;
out vec4 outColor;

void main()
{
	vec4 color = texture(tex, Texcoord);

	if(glow)
	{
		if(color.a == 0.0)
    	{
			if((Texcoord.x - 1.0 / TextureSize.x > TextureOffset.x && texture(tex, vec2(Texcoord.x - 1.0 / TextureSize.x, Texcoord.y)).a > 0.001) || 
				(Texcoord.x + 1.0 / TextureSize.x < TextureOffset.x + TextureSize.x && texture(tex, vec2(Texcoord.x + 1.0 / TextureSize.x, Texcoord.y)).a > 0.001) || 
				(Texcoord.y - 1.0 / TextureSize.y > TextureOffset.y && texture(tex, vec2(Texcoord.x, Texcoord.y - 1.0 / TextureSize.y)).a > 0.001) || 
				(Texcoord.y + 1.0 / TextureSize.y < TextureOffset.y + TextureSize.y && texture(tex, vec2(Texcoord.x, Texcoord.y + 1.0 / TextureSize.y)).a > 0.001))
			{
				color = vec4(1, 1, 0.5, (sin(time * 4) / 2) + 0.5);
			}
		}
	}

    outColor =  vec4(vec3(color.rgb * spriteColor.rgb) * 1.0, color.a * spriteColor.a);
//	if(outColor.a == 0.0)
	//	outColor = vec4(1, 0, 0, 1);
}
