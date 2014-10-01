#version 330 core

in vec3 pixelPos;
in vec3 pixelNormal;
in vec2 pixelUV;

out vec3 outColor;

uniform sampler2D textureSampler;

void main()
{
	outColor = texture2D(textureSampler, pixelUV).rgb;
}