#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

out vec3 pixelPos;
out vec3 pixelNormal;
out vec2 pixelUV;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(inPos, 1);
	
	pixelPos = inPos;
	pixelNormal = inNormal;
	pixelUV = inUV;
}