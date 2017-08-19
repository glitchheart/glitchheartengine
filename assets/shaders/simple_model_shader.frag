#version 330 core
in vec4 c;
in vec3 n;
in vec3 v;
in vec3 lPos;
in vec2 texCoord;

uniform bool hasUVs;
uniform sampler2D tex;

out vec4 outColor;

void main()
{
	vec4 textureColor = texture(tex, texCoord);
	
	vec3 L = normalize(lPos - v);
	vec3 E = normalize(-v);
	vec3 R = reflect(-L,n);

	vec4 Iamb = vec4(0.1, 0.1, 0.1, 1.0);

	vec4 Idiff = vec4(1.0,1.0,1.0,1.0) * vec4(1.0,1.0,1.0,1.0) * max(dot(n,L), 0.0);
	Idiff = clamp(Idiff, 0.0, 1.0);

	vec4 Ispec = vec4(0.5,0.5,0.5,1.0) * vec4(0.0,1.0,0.0,1.0) * pow(max(dot(R,E),0.0),0.3 * 8);
	Ispec = clamp(Ispec, 0.0, 1.0);

	if(hasUVs)
		outColor = textureColor * (Iamb + Idiff + Ispec);
	else
		outColor = Iamb + Idiff + Ispec;
} 

