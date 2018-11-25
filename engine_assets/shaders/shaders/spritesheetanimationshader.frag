#version 150
in vec2 TextureOffset;
in vec2 Texcoord;
uniform sampler2D tex;
uniform float time;
uniform vec4 color;
out vec4 outColor;

void main()
{
	vec4 c = texture(tex, Texcoord);

	if(color.a < 0.01)
	{
		discard;
	}

    outColor = c * color;
}
