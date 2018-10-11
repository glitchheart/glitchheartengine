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

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D ambientTexture;
uniform sampler2D specularIntensityTexture;
uniform sampler2D shadowMap;

uniform vec3 diffuseColor;
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform float specularExponent;

uniform vec3 lightPosWorld;
uniform float lightPower;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 lightAmbient;

uniform bool hasTexture;
uniform bool hasSpecular;
uniform bool hasAmbient;
uniform bool hasSpecularIntensity;

uniform bool drawWireframe;
uniform bool drawMesh;
uniform vec4 wireframeColor;

uniform bool receivesShadows;

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
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
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
        vec3 normal = normalize(fs_in.normal);
        
        vec3 ambient = vec3(0, 0, 0);
        
        if(hasAmbient)
        {
            ambient = lightAmbient * ambientColor * vec3(texture(ambientTexture, fs_in.uv));
        }
        else
        {
            ambient = lightAmbient * ambientColor;
        }
        
        vec3 lightDir = normalize(fs_in.lightDir - fs_in.posWorld);
        vec3 viewDir = normalize(fs_in.eyeView - fs_in.posWorld);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float nh = max(0.0f, dot(normal, halfwayDir));
        
        float distortion = 0.5f;
        float power = 1.0f;
        float scale = 0.5f;
        vec3 subColor = vec3(1, 1, 1);
        
        
        vec3 albedo = texture(diffuseTexture, fs_in.uv).rgb * diffuseColor;
        float gloss = texture(diffuseTexture, fs_in.uv).a;
        vec3 transLightDir = lightDir + normal + distortion;
        float transDot = pow(max(0.0f, dot(viewDir, -transLightDir)), power) * scale;
        vec3 transLight = (transDot) * 0.5f * subColor;
        vec3 transAlbedo = albedo * lightDiffuse * transLight;
        
        // diffuse
        float diff = max(0.0f, dot(normal, lightDir));
        
        vec3 diffuse = vec3(0, 0, 0);
        if(hasTexture)
        {
            if(texture(diffuseTexture, fs_in.uv).a == 0.0f)
            {
                discard;
            }
            else
            {
                diffuse = albedo * lightDiffuse * diff;		
            }
        }
        else
        {
            diffuse = diff * diffuseColor * lightDiffuse;
        }
        
        float spec = 0.0;
        
        if(hasSpecularIntensity)
        {
            spec = pow(nh, specularExponent * texture(specularIntensityTexture, fs_in.uv).x) * gloss;
            //spec = pow(nh, specularExponent * 128.0f);
        }
        else
        {
            spec = pow(nh, specularExponent * 128.0f) * gloss;
        }
        
        // specular
        vec3 specular = vec3(0, 0, 0);
        
        if(hasSpecular)
        {
            specular = lightDiffuse * (spec * texture(specularTexture, fs_in.uv).rgb);
        }
        else
        {
            specular = lightDiffuse * spec * specularColor;
        }
        
        // shadows
        float shadow = calculateShadow(fs_in.shadowCoord, normal, lightDir);
        
        vec3 lighting;
        if(receivesShadows)
        {
            lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * fs_in.color.xyz;
        }
        else
        {
            lighting = (ambient + (fs_in.color.xyz * diffuse + specular));
        }
        
        color.rgb = lighting + transAlbedo;		
        color.a = 1.0f * texture(specularTexture, fs_in.uv).a * spec;
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