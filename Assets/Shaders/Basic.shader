#version 450

#if VERTEX_SHADER

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Color;

layout(binding = 0) uniform MvpStruct
{
	mat4 Projection;
	mat4 View;
	mat4 Model;
} Mpv;

layout(location = 0) out vec3 FragColor;

void main()
{
	gl_Position = Mpv.Projection * Mpv.View * Mpv.Model * vec4(Position, 1.0);
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
