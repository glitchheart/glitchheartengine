#version 330 core

in GS_OUT
{
    vec4 color;
	vec4 shadowCoord;
    vec3 normal;
    vec2 uv;
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
uniform bool hasTexture;
uniform bool receivesShadows;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

float calculateShadow(vec4 fragPosLightSpace, vec3 n, vec3 lDir)
{
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// if we're outside the far plane
	if(projCoords.z > 1.0)
		return 0.0;

	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	float currentDepth = projCoords.z;
	
	float bias = max(0.05 * (1.0 - dot(n, lDir)), 0.005);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	
	// percentage-closer filtering
	// sampling surrounding texels to get smoother shadows
	for(int x = -1; x <= 1; ++x)
	{
    	for(int y = -1; y <= 1; ++y)
    	{
        	float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * 			   	texelSize).r; 
        	shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
    	}    
	}

	// average
	shadow /= 9.0;
	
	return shadow;
}

void main()
{
    if(drawMesh)
    {
		vec3 col =  diffuseColor;
		vec3 normal = normalize(fs_in.normal);
		
		// ambient
		vec3 ambient = 0.15 * col;
		
		// diffuse	
		vec3 lightDir = normalize(fs_in.lightDir);
		float diff = max(dot(lightDir, normal), 0.0f);
		vec3 diffuse = diff * lightColor;		
		
		// specular
		vec3 viewDir = normalize(fs_in.eyeView);
		float spec = 0.0;
		vec3 halfwayDir = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0f);
		vec3 specular = vec3(0, 0, 0);//spec * lightColor;
		
		// shadows
		float shadow = calculateShadow(fs_in.shadowCoord, normal, lightDir);
		
		vec3 lighting;
		if(receivesShadows)
		{
			lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * col;
		}
		else
		{
			lighting = vec3(1, 1, 1);//(ambient + (diffuse + specular)) * col;
		}
		
		if(hasTexture)
		{
			if(texture(diffuseTexture, fs_in.uv).a == 0.0)
				discard;
			else
				color = vec4(lighting, 1.0f) * texture(diffuseTexture, fs_in.uv) * fs_in.color;		
		} else {
			color = vec4(lighting, 1.0f) * fs_in.color;		
		}
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