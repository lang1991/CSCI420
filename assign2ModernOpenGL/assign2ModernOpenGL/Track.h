#pragma once
#include "Spline.h"

class Track
{
public:
	int mNumOfSplines;
	vector<Spline> mSplines;

public:
	Track();
	int LoadSplines(const string& InTrackPath);
};