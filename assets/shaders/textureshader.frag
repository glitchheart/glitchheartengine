#version 150
in vec2 Texcoord;
uniform vec2 textureSize;
uniform vec4 Color;
uniform sampler2D tex;
uniform float time;
uniform bool glow;
out vec4 outColor;

void main()
{
	vec4 C = texture(tex, Texcoord);
	if(glow)
	{
		if(C.a == 0.0)
    	{
			bool doGlow = false;
			
			float xDiff = 1.0 / textureSize.x;
			float yDiff = 1.0 / textureSize.y;
			for(int i = 1; i < 2; i++)
			{
				if(texture(tex, vec2(Texcoord.x - xDiff * i, Texcoord.y)).a > 0.001 || 
				texture(tex, vec2(Texcoord.x + xDiff * i, Texcoord.y)).a > 0.001 || 
				texture(tex, vec2(Texcoord.x, Texcoord.y - yDiff * i)).a > 0.001 || 
				texture(tex, vec2(Texcoord.x, Texcoord.y + yDiff * i)).a > 0.001)
				{
					doGlow = true;
					break;
				}
			}

			if(doGlow)
			{
				C = vec4(1, 1, 0.5, (sin(time * 4) / 2) + 0.5);
			}
		}
	}
    outColor = C; // * Color;
}