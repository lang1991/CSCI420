#version 330 core

in vec3 inPos;
out vec3 color;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(inPos, 1);
	color = inPos;
}