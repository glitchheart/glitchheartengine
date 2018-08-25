#version 330 core

uniform float aspect;
uniform float border_width;
uniform vec4 border_color;
uniform vec2 scale;

in vec4 c;
in vec2 uv;

out vec4 outColor;

void main()
{
    float maxX = scale.x - border_width;
    float minX = border_width;
    float maxY = scale.y - border_width;
    float minY = border_width;
    
    vec2 scaled_uv = uv * scale;
    
    bool x = scaled_uv.x < maxX && scaled_uv.x > minX && scaled_uv.y < maxY && scaled_uv.y > minY;
    
    vec4 result = c;
    
    if(border_width > 0.0)
    {
        if(x)
        {
            result = c;
        }
        else
        {
            result = border_color;
        }
    }
    outColor = result;
}