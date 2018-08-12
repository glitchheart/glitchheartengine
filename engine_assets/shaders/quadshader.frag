#version 330 core

uniform float aspect;
uniform float border_width;

in vec4 c;
in vec2 uv;

out vec4 outColor;

void main()
{
    float maxX = 1.0 - border_width;
    float minX = border_width;
    float maxY = maxX / aspect;
    float minY = minX / aspect;
    
    bool x = uv.x < maxX && uv.x > minX && uv.y < maxY && uv.y > minY;
    
    vec4 result = c;
    
    if(border_width > 0.0)
    {
        if(x)
        {
            result = c;
        }
        else
        {
            result = vec4(0.5);
        }
    }
    outColor = result;
}