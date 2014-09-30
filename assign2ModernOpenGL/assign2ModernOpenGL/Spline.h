#pragma once
#include "Utilities.h"

using namespace std;
using namespace glm;

class Spline
{
public:
	int mNumControlPoints;
	vector<dvec3> mPoints;
};