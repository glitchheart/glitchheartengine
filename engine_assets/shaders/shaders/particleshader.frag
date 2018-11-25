#version 330 core

in vec2 fs_uv;
in vec4 fs_color;
out vec4 outColor;

uniform bool withTexture;

uniform sampler2D diffuseTexture;

void main()
{
	if(withTexture)
	{
		vec4 color = texture(diffuseTexture, fs_uv);
		if(color.a == 0.0)
			discard;
		else
		    	outColor = color * fs_color; 
	}
	else
	{
	    if(fs_color.a == 0.0)
		discard;
	    else
		outColor = fs_color;
	}
}
