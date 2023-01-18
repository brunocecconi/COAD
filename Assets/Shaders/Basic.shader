#version 450

#if VERTEX_SHADER

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;

layout(location = 0) out vec3 FragColor;

void main()
{
	gl_Position = vec4(Position, 1.0);
	FragColor = Color;
}

#endif

#if FRAGMENT_SHADER

layout(location = 0) in vec3 FragColor;
layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(FragColor,1.0);
}

#endif
