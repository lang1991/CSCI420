#include "Utilities.h"
using namespace std;
using namespace glm;

class StaticMesh
{
public:
	GLuint mIndexBuffer;
	GLuint mPosBuffer;
	GLuint mNormalBuffer;
	GLuint mUVBuffer;

	vector<vec3> mPos;
	vector<vec3> mNormal;
	vector<vec2> mUV;
	vector<unsigned short> mIndex;

	GLuint mTextureIndex;

public:
	StaticMesh(const string& InMeshPath);

private:
	vec3 ReadVec3(ifstream& InFin);
	vec2 ReadVec2(ifstream& InFin);

};