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

struct Translucency
{
    bool hasTranslucency;
    float distortion;
    float power;
    float scale;
    vec3 subColor;
};

struct Material
{
    sampler2D diffuseTexture;
    vec3 diffuseColor;
    bool hasTexture;
    
    sampler2D specularTexture;
    vec3 specularColor;
    bool hasSpecular;
    
    sampler2D ambientTexture;
    vec3 ambientColor;
    bool hasAmbient;
    
    sampler2D specularIntensityTexture;
    float specularExponent;
    bool hasSpecularIntensity;
    
    Translucency translucency;
};

uniform Material material;

uniform sampler2D shadowMap;

uniform vec3 lightPosWorld;
uniform float lightPower;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
uniform vec3 lightAmbient;

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

vec3 computeAmbient()
{
    if(material.hasAmbient)
    {
        return lightAmbient * material.ambientColor * vec3(texture(material.ambientTexture, fs_in.uv));
    }
    else
    {
        return lightAmbient * material.ambientColor;
    }
}

vec3 computeDiffuse(vec3 albedo, vec3 normal, vec3 lightDir)
{
    // diffuse
    float diff = max(0.0f, dot(normal, lightDir));
    
    if(material.hasTexture)
    {
        if(texture(material.diffuseTexture, fs_in.uv).a == 0.0f)
        {
            discard;
        }
        else
        {
            return albedo * lightDiffuse * diff;		
        }
    }
    else
    {
        return diff * material.diffuseColor * lightDiffuse;
    }
}

float computeSpecularValue(vec3 normal, vec3 halfwayDir, float gloss)
{
    float nh = max(0.0f, dot(normal, halfwayDir));
    float spec = 0.0;
    
    if(material.hasSpecularIntensity)
    {
        spec = pow(nh, material.specularExponent * texture(material.specularIntensityTexture, fs_in.uv).x) * gloss;
        //spec = pow(nh, specularExponent * 128.0f);
    }
    else
    {
        spec = pow(nh, material.specularExponent * 128.0f) * gloss;
    }
    
    return spec;
}

vec3 computeSpecular(float spec, vec3 lightDir, vec3 viewDir, vec3 halfwayDir)
{
    if(material.hasSpecular)
    {
        return lightDiffuse * (spec * texture(material.specularTexture, fs_in.uv).rgb);
    }
    else
    {
        return lightDiffuse * spec * material.specularColor;
    }
}

vec3 computeTranslucency(float spec, vec3 albedo, vec3 normal, vec3 lightDir, vec3 viewDir)
{
    if(!material.translucency.hasTranslucency)
    {
        return vec3(0.0f);
    }
    
    vec3 transLightDir = lightDir + normal + material.translucency.distortion;
    float transDot = pow(max(0.0f, dot(viewDir, -transLightDir)), material.translucency.power) * material.translucency.scale;
    vec3 transLight = (transDot) * 0.5f * material.translucency.subColor;
    vec3 transAlbedo = albedo * lightDiffuse * transLight;
    
    return transAlbedo;
}

vec3 computeLight(vec3 normal, vec3 lightDir, vec3 ambient, vec3 diffuse, vec3 specular)
{
    // shadows
    float shadow = calculateShadow(fs_in.shadowCoord, normal, lightDir);
    
    vec3 lighting;
    if(receivesShadows)
    {
        return (ambient + (1.0 - shadow) * (diffuse + specular)) * fs_in.color.xyz;
    }
    else
    {
        return (ambient + (fs_in.color.xyz * diffuse + specular));
    }
}

void main()
{
    if(drawMesh)
    {
        vec3 normal = normalize(fs_in.normal);
        
        vec3 lightDir = normalize(fs_in.lightDir - fs_in.posWorld);
        vec3 viewDir = normalize(fs_in.eyeView - fs_in.posWorld);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        
        vec3 albedo = texture(material.diffuseTexture, fs_in.uv).rgb * material.diffuseColor;
        
        float gloss = texture(material.diffuseTexture, fs_in.uv).a;
        
        vec3 ambient = computeAmbient();
        vec3 diffuse = computeDiffuse(albedo, normal, lightDir);
        
        float spec = computeSpecularValue(normal, halfwayDir, gloss);
        
        vec3 specular = computeSpecular(spec, lightDir, viewDir, halfwayDir);
        vec3 translucency = computeTranslucency(spec, albedo, normal, lightDir, viewDir);
        vec3 lighting = computeLight(normal, lightDir, ambient, diffuse, specular);
        
        color.rgb = lighting + translucency;		
        
        if(material.translucency.hasTranslucency)
        {
            color.a = 1.0f * texture(material.specularTexture, fs_in.uv).a * spec;
        }
        else
        {
            color.a = 1.0f;
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