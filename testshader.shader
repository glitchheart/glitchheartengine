#vert

uniform float speed;
uniform bool glowing;

@mat_mvp
@vert_norm_uv

void main()
{
    gl_Position = projection * view * model * vec4(vertex, 1.0);
}
    
#frag

@specdata
@col0
@fragout

void main()
{
    outColor = col0 * texture2D(tex0, uv);
}

MASTER FILE
    
@mat_mvp
struct MVP
{
    mat4 model;
    mat4 view;
    mat4 projection;
};
    
uniform MVP mvp;
@end
    
@vert
layout(location = 0) in vec3 vertex;
@end
    
@vert_norm
#vert
layout(location = 1) in vec3 normal;
@end
    
@vert_norm_uv
#vert_norm
layout(location = 2) in vec2 uv;
@end
    
@col
uniform vec4 col0;
@end
    
@specdata
uniform float specularIntensity; : NS
uniform sampler2D specularTexture; : SPECULAR_TEXTURE
@end