#pragma once
#include "vector3.h"

class Ray
{
public:
	vector3 direction;
	vector3 origin;

	Ray(const vector3& inDirection, const vector3& inOrigin)
	{
		direction = inDirection;
		origin = inOrigin;
	}

	bool IntersectSphere(const vector3& inCenter, double inRadius, vector3& outIntersectionPosisiton, vector3& outSurfaceNormal, double& outT)
	{
		double b = 2 * (direction.x * (origin.x - inCenter.x) + direction.y * (origin.y - inCenter.y) + direction.z * (origin.z - inCenter.z));
		double c =	(origin.x - inCenter.x) * (origin.x - inCenter.x) + 
					(origin.y - inCenter.y) * (origin.y - inCenter.y) + 
					(origin.z - inCenter.z) * (origin.z - inCenter.z) - 
					inRadius * inRadius;

		double determinant = b * b - 4 * c;
		if (determinant < 0.0f)
		{
			outT = DBL_MAX;
			return false;
		}

		double t0 = (-b + sqrt(determinant)) / 2;
		double t1 = (-b - sqrt(determinant)) / 2;


		//outT = min(t0, t1);
		//if (outT < 0)
		//{
		//	return false;
		//}

		// Both behind origin, abort
		if(t0 < 0.0f && t1 < 0.0f)
		{
			outT = DBL_MAX;
			return false;
		}

		// Both after origin, take mini
		if(t0 >= 0.0f && t1 >= 0.0f)
		{
			outT = min(t0, t1);
		}
		else	// One behind, one after, take the after one
		{
			outT = max(t0, t1);
		}

		outIntersectionPosisiton = (origin + direction * outT);
		outSurfaceNormal = (outIntersectionPosisiton - inCenter) / inRadius;
		return true;
	}

	bool IntersectTriangle(vector3& inV1, vector3& inV2, vector3& inV3, double& outT, double& outU, double& outV)
	{
		vector3 e1;
		vector3 e2;
		vector3 P;
		vector3 Q;
		vector3 T;
		double det;
		double invDet;
		
		e1 = inV2 - inV1;
		e2 = inV3 - inV1;

		P = direction.cross(e2);
		det = e1.dot(P);

		if(det > DBL_EPSILON && det < DBL_EPSILON)
		{
			outT = DBL_MAX;
			return false;
		}

		invDet = 1.0f / det;

		T = origin - inV1;
		outU = T.dot(P) * invDet;
		if(outU < 0.0f || outU > 1.0f)
		{
			outT = DBL_MAX;
			return false;
		}

		Q = T.cross(e1);
		outV = direction.dot(Q) * invDet;
		if(outV < 0.0f || outV + outU > 1.0f)
		{
			outT = DBL_MAX;
			return false;
		}

		outT = e2.dot(Q) * invDet;

		if (outT > DBL_EPSILON)
		{
			return true;
		}

		outT = DBL_MAX;
		return false;
	}
};