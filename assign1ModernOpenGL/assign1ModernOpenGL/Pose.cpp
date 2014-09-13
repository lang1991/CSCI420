#include "Pose.h"

Pose::Pose()
{
	mPosition = vec3(0.0f, 0.0f, 0.0f);
	mRotation = vec3(0.0f, 0.0f, 0.0f);
	mScale = vec3(1.0f, 1.0f, 1.0f);

	mUp = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	mRight = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	mForward = vec4(0.0f, 0.0f, 1.0f, 1.0f);

	mTransform = mat4();
}