#pragma once
#include "Utilities.h"

using namespace glm;
using namespace std;

class Camera
{
public:
	vec3 mPos;
	vec3 mForward;
	vec3 mUp;
	vec3 mRight;
	mat4x4 mViewMatrix;

public:
	Camera(const vec3& InPos, const vec3& InForward, const vec3& InRight, const vec3& InUp);
	void LookLeftRight(float InDelta);
	void LookUpDown(float InDelta);
	void MoveForwardBack(float InDelta);
	void MoveLeftRight(float InDelta);
	void UpdateView();
};