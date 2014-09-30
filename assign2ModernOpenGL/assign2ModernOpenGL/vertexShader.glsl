#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;

out vec3 pixelColor;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(inPos, 1);
	pixelColor = inColor;
}