#version 330 core

in vec3 color;
out vec3 outColor;

void main()
{
	outColor = color;
	
	// these have nothing to do with "height"
	outColor.x = 0.0f;
	outColor.z = outColor.y;
	outColor.y = 0.0f;
	outColor = outColor / vec3(255.0f, 255.0f, 255.0f); // this is to convert 255 color to 1.0f color
}