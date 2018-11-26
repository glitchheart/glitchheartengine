@vert
    
#version 330 core

uniform float time;
    
uniform sampler2D tex0;
    
@std_vert_in
    
@model_view
    
void main()
{
    
}
    
@frag

@std_frag_out

void main()
{
    
}

<<<<<<< TAGS >>>>>>>
@vert = vertex shader start
@frag = fragment shader start
    
@vert_norm_uv =
{
    layout(location = 0) in vec3 vertex;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 uv;
}

    
SHADERS
1. Standard in attributes
2. Standard uniforms (Matrices, textures etc.)
3. Standard structure for material info
4. Array of key/value pairs with uniform name + value
   - Split into type?
   - Saved in material?
5. Reload function for all shaders
6. Shaders in templates