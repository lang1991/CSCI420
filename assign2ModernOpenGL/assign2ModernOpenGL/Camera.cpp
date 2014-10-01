#include "Camera.h"

Camera::Camera(const vec3& InPos, const vec3& InForward, const vec3& InRight, const vec3& InUp)
{
	mPos = InPos;
	mForward = InForward;
	mRight = InRight;
	mUp = InUp;
}

void Camera::LookLeftRight(float InDelta)
{
	mForward = normalize(rotate(mForward, InDelta, mUp));
	mRight = normalize(rotate(mRight, InDelta, mUp));
}

void Camera::LookUpDown(float InDelta)
{
	mUp = normalize(rotate(mUp, InDelta, mRight));
	mForward = normalize(rotate(mForward, InDelta, mRight));
}

void Camera::MoveForwardBack(float InDelta)
{
	mPos += mForward * InDelta;
}

void Camera::MoveLeftRight(float InDelta)
{
	mPos += mRight * InDelta;
}

void Camera::UpdateView()
{
	mViewMatrix = lookAt(mPos, mPos + mForward, mUp);
}