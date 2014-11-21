#pragma once
#include <cmath>

class vector3
{
public:
	double x;
	double y;
	double z;

public:
	vector3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	vector3(double inX, double inY, double inZ)
	{
		x = inX;
		y = inY;
		z = inZ;
	}

	double dot(const vector3& inV)
	{
		return x * inV.x + y * inV.y + z * inV.z;
	}

	vector3 cross(const vector3& inV)
	{
		vector3 result;
		result.x = y * inV.z - z * inV.y;
		result.y = z * inV.x - x * inV.z;
		result.z = x * inV.y - y * inV.x;

		return result;
	}

	vector3 normalize()
	{
		vector3 result;
		double divider = sqrt(x * x + y * y + z * z);
		result.x = x / divider;
		result.y = y / divider;
		result.z = z / divider;

		return result;
	}

	double magnitudeSqr()
	{
		return (x * x + y * y + z * z);
	}

	vector3 operator*(double scale)
	{
		return vector3(x * scale, y * scale, z * scale);
	}

	vector3 operator/(double scale)
	{
		return vector3(x / scale, y / scale, z / scale);
	}

	vector3 operator-(const vector3& inV)
	{
		return vector3(x - inV.x, y - inV.y, z - inV.z);
	}

	vector3 operator+(const vector3& inV)
	{
		return vector3(x + inV.x, y + inV.y, z + inV.z);

	}

};