#version 330 core

in GS_OUT
{
    vec4 color;
	vec4 shadowCoord;
    vec3 normal;
    vec3 posWorld;
    vec3 eyeView;
    vec3 lightDir;
    
    noperspective vec3 wireframeDist;
} fs_in;

out vec4 color;

uniform vec3 diffuseColor;
uniform vec3 lightPosWorld;
uniform vec3 lightColor;
uniform float lightPower;
uniform vec3 specularColor;
uniform float alpha;
uniform bool drawWireframe;
uniform bool drawMesh;
uniform vec4 wireframeColor;

uniform sampler2DShadow shadowMap;

void main()
{
    if(drawMesh)
    {
        vec3 n = normalize(fs_in.normal);
        
        vec3 l = normalize(fs_in.lightDir);
        
        vec3 E = normalize(fs_in.eyeView);
        
        vec3 R = reflect(-l,n);
        
        float cosAlpha = clamp(dot(E, R), 0, 1);
        
        float cosTheta = clamp( dot(n, l), 0, 1);
        float distance = length(lightPosWorld - fs_in.posWorld);
        vec3 ambientColor = vec3(0.1, 0.1, 0.1);
        color.rgb = ambientColor + fs_in.color.rgb * lightPower * lightColor * cosTheta / (distance*distance) + specularColor * lightColor * pow(cosAlpha, 5) / (distance*distance);
	
		// find the visibility from the shadow map
		float visibility = texture(shadowMap, vec3(fs_in.shadowCoord.xy, (fs_in.shadowCoord.z) / fs_in.shadowCoord.w));
        //color *= visibility;
		
        color.a = fs_in.color.a;
    }
    else
    {
        color = vec4(0.0, 0.0, 0.0, 0.0);
    }
    
    if(drawWireframe)
    {
        vec3 d = fwidth(fs_in.wireframeDist);
        
        vec3 a3 = smoothstep(vec3(0.0), d * 2.5, fs_in.wireframeDist);
        float edgeFactor = min(min(a3.x, a3.y), a3.z);
        
        if(!drawMesh && edgeFactor == 1.0)
        {
            discard;
        }
        else
        {
            color = vec4(mix(wireframeColor, color, edgeFactor));
        }
        
    }
}