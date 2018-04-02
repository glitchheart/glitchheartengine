#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec4 color;
	vec3 normal;
	vec3 posWorld;
	vec3 eyeView;
	vec3 lightDir;
} gs_in[];

out GS_OUT
{
	vec4 color;
	vec3 normal;
	vec3 posWorld;
	vec3 eyeView;
	vec3 lightDir;

	noperspective vec3 wireframeDist;

} gs_out;

void main() {    
	for(int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		gs_out.color = gs_in[i].color;
		gs_out.normal = gs_in[i].normal;
		gs_out.posWorld = gs_in[i].posWorld;
		gs_out.eyeView = gs_in[i].eyeView;
		gs_out.lightDir = gs_in[i].lightDir;
		gs_out.wireframeDist = vec3(0.0);
        gs_out.wireframeDist[i] = 1.0;

    	EmitVertex();
	}    

    EndPrimitive();
} 