#version 150
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec4 color;
in vec3 position;

out vec4 c;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	c = color;
}