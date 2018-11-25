#version 330 core

in VS_OUT
{
    vec4 c;
    vec2 uv;
} fs_in;

uniform vec2 dimension;
uniform float radius;
uniform float border;
uniform vec2 size;
uniform vec2 position;

out vec4 outColor;

float rrect(vec2 u_uv, vec2 pos, vec2 u_size, float rad, float s)
{
    vec2 sz = u_size - vec2(rad, rad) * 2.0;
    vec2 p = pos + vec2(rad, rad);
    vec2 end = p + sz;
    
    float dx = max(max(u_uv.x - end.x, 0.0), max(p.x - u_uv.x, 0.0));
    float dy = max(max(u_uv.y - end.y, 0.0), max(p.y - u_uv.y, 0.0));
    
    float d = sqrt(dx * dx + dy * dy);
    return 1.0 - smoothstep(rad - s, rad + s, d);
}

void main()
{
    float aspect = dimension.x / dimension.y;
    vec2 u_uv = vec2(aspect, 1.0) * gl_FragCoord.xy / dimension.xy;
    
    vec2 scaled_size = vec2(aspect, 1.0) * size.xy / dimension.xy;
    
    vec2 pos = position / dimension.x;
    
    //float round = rrect(u_uv, pos, scaled_size, radius, 0.0);
    float round = rrect(u_uv, vec2(0.0, 0.4), scaled_size, radius, 0.0);
    
    outColor = mix(vec4(1.0), fs_in.c, round);
}