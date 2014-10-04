#include "Track.h"

Track::Track() : 
mNumOfSplines(0)
{}

int Track::LoadSplines(const string& InTrackPath, const dmat4& InTransform)
{
	char *cName = (char *)malloc(128 * sizeof(char));
	FILE *fileList;
	FILE *fileSpline;
	int iType, j, iLength;

	/* load the track file */
	fileList = fopen(InTrackPath.c_str(), "r");
	if (fileList == NULL) 
	{
		printf("can't open file\n");
		exit(1);
	}

	/* stores the number of splines in a global variable */
	fscanf(fileList, "%d", &mNumOfSplines);

	mSplines.reserve(mNumOfSplines);

	/* reads through the spline files */
	for (j = 0; j < mNumOfSplines; j++) {
		fscanf(fileList, "%s", cName);
		fileSpline = fopen(cName, "r");

		if (fileSpline == NULL) {
			printf("can't open file\n");
			exit(1);
		}

		/* gets length for spline file */
		fscanf(fileSpline, "%d %d", &iLength, &iType);

		/* allocate memory for all the points */
		Spline newSpline;
		newSpline.mNumControlPoints = iLength;
		newSpline.mPoints.reserve(iLength);

		/* saves the data to the struct */
		double x = 0.0f;
		double y = 0.0f;
		double z = 0.0f;

		while (fscanf(fileSpline, "%lf %lf %lf",
			&x,
			&y,
			&z) != EOF) 
		{
			dvec4 transformedPos = dvec4(x, y, z, 1.0);
			transformedPos = transformedPos * InTransform;

			newSpline.mPoints.emplace_back(dvec3(transformedPos.x, transformedPos.y, transformedPos.z));
		}
		mSplines.emplace_back(newSpline);
	}

	free(cName);

	return 0;
}