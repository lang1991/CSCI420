#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

class Pose
{
public:
	Pose();

public:
	vec3 mPosition;
	vec3 mRotation;
	vec3 mScale;
	vec4 mUp;
	vec4 mRight;
	vec4 mForward;

	mat4 mTransform;
};