#include "StaticMesh.h"
#include <stdexcpt.h>

StaticMesh::StaticMesh(const string& InMeshPath)
{
	mTransform = mat4();

	ifstream fin(InMeshPath.c_str());
	if(fin.is_open())
	{
		string texturePath;
		getline(fin, texturePath);
		int lastSlashPos = texturePath.find_last_of('/');
		texturePath = texturePath.substr(lastSlashPos + 1, texturePath.size() - lastSlashPos);
		texturePath = ".//assets//" + texturePath;
		mTextureIndex = Utilities::LoadTexture(texturePath);

		unsigned int triangleCount = 0;
		fin >> triangleCount;
		for(unsigned int i = 0 ; i < triangleCount; ++i)
		{
			char discard = ' ';
			unsigned short index = 0;

			fin >> index;
			mIndex.emplace_back(index);
			fin >> discard;
			
			fin >> index;
			mIndex.emplace_back(index);
			fin >> discard;
			
			fin >> index;
			mIndex.emplace_back(index);
		}

		unsigned int vertexCount = 0;
		fin >> vertexCount;
		for(unsigned int i = 0; i < vertexCount; ++i)
		{
			mPos.emplace_back(ReadVec3(fin));
			mNormal.emplace_back(ReadVec3(fin));
			mUV.emplace_back(ReadVec2(fin));
		}
	}
	else
	{
		throw new exception("Cannot open mesh file. Exit.");
	}
	
	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &mPosBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mPosBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * mPos.size(), &mPos[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &mNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)* mNormal.size(), &mNormal[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &mUVBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mUVBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)* mUV.size(), &mUV[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &mIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * mIndex.size(), &mIndex[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void StaticMesh::Render(GLuint InShaderProgramID, const mat4& InVP)
{

	glBindVertexArray(mVertexArray);
	mat4 MVP = InVP * mTransform;
	GLuint MVPMatrixIDPNT = glGetUniformLocation(InShaderProgramID, "MVP");
	GLuint textureID = glGetUniformLocation(InShaderProgramID, "textureSampler");
	glUniformMatrix4fv(MVPMatrixIDPNT, 1, GL_FALSE, &MVP[0][0]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureIndex);
	glUniform1i(textureID, 0);
	glDrawElements(GL_TRIANGLES, mIndex.size(), GL_UNSIGNED_SHORT, nullptr);

	glBindVertexArray(0);
}

vec3 StaticMesh::ReadVec3(ifstream& InFin)
{
	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
	char discard = ' ';

	InFin >> x;
	InFin >> discard;
	InFin >> y;
	InFin >> discard;
	InFin >> z;

	return vec3(x, y, z);
}

vec2 StaticMesh::ReadVec2(ifstream& InFin)
{
	float u = 0.0f;
	float v = 0.f;
	char discard = ' ';

	InFin >> u;
	InFin >> discard;
	InFin >> v;

	return vec2(u, v);
}