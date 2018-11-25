#version 330 core

layout(location = 0) out float fragmentDepth;

void main()
{
	fragmentDepth = 0.0f; //gl_FragCoord.z;
}

