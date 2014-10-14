#define GLM_FORCE_SIZE_T_LENGTH
#include "Utilities.h"
#include "Track.h"
#include "Camera.h"
#include "StaticMesh.h"

using namespace std;
using namespace glm;

// Window size
int gWindowWidth = 640;
int gWindowHeight = 480;

Track gTrack;

double gTension = 0.5;
mat4 gBasisMatrix;

vector<dmat4x3> gSegmentBasisMultiplyControl;

const float gSUBDIVISION = 0.1f;
unsigned int gCurrentPoint = 0;
vector<vec3> gSplinePointPos;
vector<vec3> gSplinePointNormal;
vector<vec3> gSplinePointTangent;
vector<vec3> gSplinePointBinormal;
vector<vec3> gSplinePointColor;

vector<vec3> gTrackPosLeft;
vector<vec3> gTrackColorLeft;
vector<unsigned int> gTrackIndexLeft;

vector<vec3> gTrackPosRight;
vector<vec3> gTrackColorRight;
vector<unsigned int> gTrackIndexRight;

float gMaxHeight = 0.0f;
const float gGravity = 3.5f;
const float gTrackScale = 0.05f;

const float gROTATIONSPEED = 0.25f;
const float gWALKSPEED = 50.0f;
Camera gCamera = Camera(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
bool gFreeCamera = false;

float gPrevTime = 0.0f;
float gCurrTime = 0.0f;
float gDeltaTime = 0.0f;

int gFrameRemaining = 0;
bool gCapFPS = false;

void BuildTrackMesh(vector<vec3>& OutVertexBufferLeft, vector<vec3>& OutVertexBufferRight, vector<vec3>& OutColorBufferLeft, vector<vec3>& OutColorBufferRight, vector<unsigned int>& OutIndexBufferLeft, vector<unsigned int>& OutIndexBufferRight)
{
	for (unsigned int i = 0; i < gSplinePointPos.size(); ++i)
	{
		if (i != gSplinePointPos.size() - 1)
		{
			// Left Rail
			OutIndexBufferLeft.emplace_back(i * 4);
			OutIndexBufferLeft.emplace_back(i * 4 + 2);
			OutIndexBufferLeft.emplace_back(i * 4 + 1);

			OutIndexBufferLeft.emplace_back(i * 4 + 2);
			OutIndexBufferLeft.emplace_back(i * 4);
			OutIndexBufferLeft.emplace_back(i * 4 + 3);

			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferLeft.emplace_back(i * 4 + 2);
			OutIndexBufferLeft.emplace_back(i * 4 + 3);

			OutIndexBufferLeft.emplace_back(i * 4 + 3);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 3);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 2);

			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 1);
			OutIndexBufferLeft.emplace_back(i * 4 + 1);

			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferLeft.emplace_back(i * 4 + 1);
			OutIndexBufferLeft.emplace_back(i * 4 + 2);

			OutIndexBufferLeft.emplace_back(i * 4);
			OutIndexBufferLeft.emplace_back(i * 4 + 1);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 1);

			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 1);
			OutIndexBufferLeft.emplace_back((i + 1) * 4);
			OutIndexBufferLeft.emplace_back(i * 4);

			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 3);
			OutIndexBufferLeft.emplace_back(i * 4 + 3);
			OutIndexBufferLeft.emplace_back(i * 4);

			OutIndexBufferLeft.emplace_back(i * 4);
			OutIndexBufferLeft.emplace_back((i + 1) * 4);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 3);

			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 3);
			OutIndexBufferLeft.emplace_back((i + 1) * 4);

			OutIndexBufferLeft.emplace_back((i + 1) * 4);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 1);
			OutIndexBufferLeft.emplace_back((i + 1) * 4 + 2);

			//Right Rail
			OutIndexBufferRight.emplace_back(i * 4);
			OutIndexBufferRight.emplace_back(i * 4 + 2);
			OutIndexBufferRight.emplace_back(i * 4 + 1);

			OutIndexBufferRight.emplace_back(i * 4 + 2);
			OutIndexBufferRight.emplace_back(i * 4);
			OutIndexBufferRight.emplace_back(i * 4 + 3);

			OutIndexBufferRight.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferRight.emplace_back(i * 4 + 2);
			OutIndexBufferRight.emplace_back(i * 4 + 3);

			OutIndexBufferRight.emplace_back(i * 4 + 3);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 3);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 2);

			OutIndexBufferRight.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 1);
			OutIndexBufferRight.emplace_back(i * 4 + 1);

			OutIndexBufferRight.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferRight.emplace_back(i * 4 + 1);
			OutIndexBufferRight.emplace_back(i * 4 + 2);

			OutIndexBufferRight.emplace_back(i * 4);
			OutIndexBufferRight.emplace_back(i * 4 + 1);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 1);

			OutIndexBufferRight.emplace_back((i + 1) * 4 + 1);
			OutIndexBufferRight.emplace_back((i + 1) * 4);
			OutIndexBufferRight.emplace_back(i * 4);

			OutIndexBufferRight.emplace_back((i + 1) * 4 + 3);
			OutIndexBufferRight.emplace_back(i * 4 + 3);
			OutIndexBufferRight.emplace_back(i * 4);

			OutIndexBufferRight.emplace_back(i * 4);
			OutIndexBufferRight.emplace_back((i + 1) * 4);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 3);

			OutIndexBufferRight.emplace_back((i + 1) * 4 + 2);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 3);
			OutIndexBufferRight.emplace_back((i + 1) * 4);

			OutIndexBufferRight.emplace_back((i + 1) * 4);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 1);
			OutIndexBufferRight.emplace_back((i + 1) * 4 + 2);
		}

		vec3 p0l = gSplinePointPos[i] - gTrackScale * (4.0f * gSplinePointBinormal[i] + 4.0f * gSplinePointNormal[i]);
		vec3 p1l = gSplinePointPos[i] - gTrackScale * (4.0f * gSplinePointBinormal[i] + 3.0f * gSplinePointNormal[i]);
		vec3 p2l = gSplinePointPos[i] - gTrackScale * (3.0f * gSplinePointBinormal[i] + 3.0f * gSplinePointNormal[i]);
		vec3 p3l = gSplinePointPos[i] - gTrackScale * (3.0f * gSplinePointBinormal[i] + 4.0f * gSplinePointNormal[i]);

		vec3 p0r = gSplinePointPos[i] + gTrackScale * (3.0f * gSplinePointBinormal[i] - 4.0f * gSplinePointNormal[i]);
		vec3 p1r = gSplinePointPos[i] + gTrackScale * (3.0f * gSplinePointBinormal[i] - 3.0f * gSplinePointNormal[i]);
		vec3 p2r = gSplinePointPos[i] + gTrackScale * (4.0f * gSplinePointBinormal[i] - 3.0f * gSplinePointNormal[i]);
		vec3 p3r = gSplinePointPos[i] + gTrackScale * (4.0f * gSplinePointBinormal[i] - 4.0f * gSplinePointNormal[i]);

		OutVertexBufferLeft.emplace_back(p0l);
		OutVertexBufferLeft.emplace_back(p1l);
		OutVertexBufferLeft.emplace_back(p2l);
		OutVertexBufferLeft.emplace_back(p3l);

		OutColorBufferLeft.emplace_back(vec3(0.5f, 0.5f, 0.5f));
		OutColorBufferLeft.emplace_back(vec3(0.5f, 0.5f, 0.5f));
		OutColorBufferLeft.emplace_back(vec3(0.5f, 0.5f, 0.5f));
		OutColorBufferLeft.emplace_back(vec3(0.5f, 0.5f, 0.5f));

		OutVertexBufferRight.emplace_back(p0r);
		OutVertexBufferRight.emplace_back(p1r);
		OutVertexBufferRight.emplace_back(p2r);
		OutVertexBufferRight.emplace_back(p3r);

		OutColorBufferRight.emplace_back(vec3(0.5f, 0.5f, 0.5f));
		OutColorBufferRight.emplace_back(vec3(0.5f, 0.5f, 0.5f));
		OutColorBufferRight.emplace_back(vec3(0.5f, 0.5f, 0.5f));
		OutColorBufferRight.emplace_back(vec3(0.5f, 0.5f, 0.5f));
	}
}

void CalcSegmentsControlMatTimesBasis(vector<dmat4x3>& OutResult)
{
	// Go through all the splines
	for (unsigned int i = 0; i < gTrack.mSplines.size(); ++i)
	{
		// Go through all the control points of the spline
		for (unsigned int j = 1; j < gTrack.mSplines[i].mPoints.size() - 2; ++j)
		{
			dvec3& p0 = gTrack.mSplines[i].mPoints[j - 1];
			dvec3& p1 = gTrack.mSplines[i].mPoints[j];
			dvec3& p2 = gTrack.mSplines[i].mPoints[j + 1];
			dvec3& p3 = gTrack.mSplines[i].mPoints[j + 2];

			mat4x3 controlMatrix(p0, p1, p2, p3);

			OutResult.emplace_back(controlMatrix * gBasisMatrix);
		}
	}
}

void RecSubdiv(float InU0, float InU1, float InMaxLineLength, int InSegIndex)
{
	float umid = InU0 + (InU1 - InU0) / 2;

	dvec4 u0Vec = dvec4(pow(InU0, 3), pow(InU0, 2), InU0, 1.0f);
	dvec4 u1Vec = dvec4(pow(InU1, 3), pow(InU1, 2), InU1, 1.0f);

	dvec4 u0Tangent = dvec4(3 * pow(InU0, 2), 2 * InU0, 1, 0);
	dvec4 u1Tangent = dvec4(3 * pow(InU1, 2), 2 * InU1, 1, 0);

	dvec3 x0 = gSegmentBasisMultiplyControl[InSegIndex] * u0Vec;
	dvec3 x1 = gSegmentBasisMultiplyControl[InSegIndex] * u1Vec;

	dvec3 x0Tangent = gSegmentBasisMultiplyControl[InSegIndex] * u0Tangent;
	dvec3 x1Tangent = gSegmentBasisMultiplyControl[InSegIndex] * u1Tangent;

	if (length(x1 - x0) > InMaxLineLength)
	{
		RecSubdiv(InU0, umid, InMaxLineLength, InSegIndex);
		RecSubdiv(umid, InU1, InMaxLineLength, InSegIndex);
	}
	else
	{
		gSplinePointPos.emplace_back(vec3(x0.x, x0.y, x0.z));
		gSplinePointTangent.emplace_back(normalize(x0Tangent));
		gSplinePointColor.emplace_back(vec3(1.0f, 0.0f, 0.0f));

		gSplinePointPos.emplace_back(vec3(x1.x, x1.y, x1.z));
		gSplinePointTangent.emplace_back(normalize(x1Tangent));
		gSplinePointColor.emplace_back(vec3(1.0f, 0.0f, 0.0f));
	}
}

void saveScreenshot(char *filename){

	Pic *in = NULL;
	Pic *out = NULL;

	if (filename == NULL)
	{
		return;
	}

	in = pic_alloc(640, 480, 3, NULL);
	out = pic_alloc(640, 480, 3, NULL);

	printf("File to save to: %s\n", filename);

	glReadPixels(0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, &in->pix[0]);

	for (int j = 0; j < 480; j++)
	{
		for (int i = 0; i < 640; i++)
		{
			PIC_PIXEL(out, i, j, 0) = PIC_PIXEL(in, i, 480 - 1 - j, 0);
			PIC_PIXEL(out, i, j, 1) = PIC_PIXEL(in, i, 480 - 1 - j, 1);
			PIC_PIXEL(out, i, j, 2) = PIC_PIXEL(in, i, 480 - 1 - j, 2);
		}
	}

	if (jpeg_write(filename, out))
	{
		printf("File saved Successfully\n");
	}
	else
	{
		printf("Error in Saving\n");
	}

	pic_free(in);
	pic_free(out);
}

void OnMouseEvent(GLFWwindow* InWindow)
{
	double newMouseX;
	double newMouseY;

	glfwGetCursorPos(InWindow, &newMouseX, &newMouseY);


	gCamera.LookLeftRight(static_cast<float> (-(newMouseX - gWindowWidth / 2) * gROTATIONSPEED));
	gCamera.LookUpDown(static_cast<float> (-(newMouseY - gWindowHeight / 2) * gROTATIONSPEED));

	glfwSetCursorPos(InWindow, static_cast<double> (gWindowWidth) / 2, static_cast<double> (gWindowHeight) / 2);
}

void OnKeyboardEvent(GLFWwindow* InWindow)
{
	if (glfwGetKey(InWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		gCamera.MoveForwardBack(gDeltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		gCamera.MoveForwardBack(-gDeltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		gCamera.MoveLeftRight(-gDeltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		gCamera.MoveLeftRight(gDeltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_F) == GLFW_PRESS)
	{
		gFreeCamera = !gFreeCamera;
	}

	if (glfwGetKey(InWindow, GLFW_KEY_C) == GLFW_PRESS)
	{
		gCapFPS = !gCapFPS;
	}

	if (glfwGetKey(InWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if(gFrameRemaining < 1000)
		{
			char myFilenm[2048];
			sprintf(myFilenm, "anim.%04d.jpg", gFrameRemaining);
			saveScreenshot(myFilenm);
			++gFrameRemaining;
		}
	}
}

GLFWwindow* CreateWindow(int InWidth, int InHeight)
{
	// Make sure OpenGL 3.2+ is supported
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	GLFWwindow* window = glfwCreateWindow(InWidth, InHeight, "Assignment 2", nullptr, nullptr);

	return window;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}

	if (!glfwInit())
	{
		cout << "Cannot initialize GLFW. Press Enter to exit.\n";
		getchar();
		return -1;
	}

	GLFWwindow* window = CreateWindow(gWindowWidth, gWindowHeight);
	if (!window)
	{
		cout << "Cannot create GLFW window. Maybe update your GPU driver?\nPress Enter to exit.\n";
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		cout << "Cannot initialize GLEW. Press Enter to exit.\n";
		getchar();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, static_cast<double> (gWindowWidth) / 2, static_cast<double> (gWindowHeight) / 2);


	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	StaticMesh groundMesh(".//assets//ground.itpmesh");
	groundMesh.mTransform = translate(vec3(0.0f, 0.0f, 0.0f));

	StaticMesh skybox(".//assets//skybox.itpmesh");
	skybox.mTransform = translate(vec3(0.0f, -100.0f, 0.0f)) * scale(vec3(2.0f, 4.0f, 2.0f));

	StaticMesh house(".//assets//house.itpmesh");
	house.mTransform = translate(vec3(-300.0f, 0.0f, 750.0f)) * rotate(90.0f, vec3(0.0f, -1.0f, 0.0f)) * scale(vec3(10.0f, 10.0f, 10.0f));

	StaticMesh tank(".//assets//tank.itpmesh");
	tank.mTransform = translate(vec3(100.0f, 0.0f, 750.0f)) * rotate(90.0f, vec3(0.0f, 1.0f, 0.0f)) * scale(vec3(0.3f, 0.3f, 0.3f));

	StaticMesh dragon(".//assets//dragon.itpmesh");
	dragon.mTransform = translate(vec3(40.0f, 450.0f, 400.0f)) * rotate(90.0f, vec3(0.0f, 1.0f, 0.0f)) * scale(vec3(0.2f, 0.2f, 0.2f));

	StaticMesh helicopter(".//assets//helicopter.itpmesh");
	helicopter.mTransform = translate(vec3(150.0f, 1000.0f, -300.0f)) * rotate(-130.0f, vec3(1.0f, 0.0f, 0.0f)) * scale(vec3(3.0f, 3.0f, 3.0f));

	gCamera.mPos = vec3(0.0f, 0.0f, 0.0f);

	dmat4 translateMat = translate(dmat4(1), dvec3(-125, 50, -100));
	dmat4 splineTransform = translateMat * scale(dvec3(20, 20, 20));

	gTrack.LoadSplines(argv[1], splineTransform);

	vec4 row1(-gTension, 2 - gTension, gTension - 2, gTension);
	vec4 row2(2 * gTension, gTension - 3, 3 - 2 * gTension, -gTension);
	vec4 row3(-gTension, 0, gTension, 0);
	vec4 row4(0, 1, 0, 0);
	gBasisMatrix = mat4(row1, row2, row3, row4);

	CalcSegmentsControlMatTimesBasis(gSegmentBasisMultiplyControl);
	for (unsigned int i = 0; i < gSegmentBasisMultiplyControl.size(); ++i)
	{
		RecSubdiv(0, 1.0f, gSUBDIVISION, i);
	}

	gCurrentPoint = 0;
	gMaxHeight = 800.0f;


	gSplinePointBinormal.reserve(gSplinePointPos.size());
	gSplinePointNormal.reserve(gSplinePointPos.size());

	gSplinePointBinormal.emplace_back(vec3(-1.0f, 0.0f, 0.0f));
	gSplinePointNormal.emplace_back(normalize(cross(gSplinePointBinormal[0], gSplinePointTangent[0])));
	for (unsigned int i = 1; i < gSplinePointPos.size(); ++i)
	{
		gSplinePointNormal.emplace_back(normalize(cross(gSplinePointBinormal[i - 1], gSplinePointTangent[i])));
		gSplinePointBinormal.emplace_back(normalize(cross(gSplinePointTangent[i], gSplinePointNormal[i])));
	}


	BuildTrackMesh(gTrackPosLeft, gTrackPosRight, gTrackColorLeft, gTrackColorRight, gTrackIndexLeft, gTrackIndexRight);

	vector<vec3> coordinateSystemVertices;
	vector<vec3> coordinateSystemColors;
	coordinateSystemVertices.emplace_back(vec3(0.f, 0.f, 0.f));
	coordinateSystemVertices.emplace_back(vec3(100.f, 0.f, 0.f));
	coordinateSystemColors.emplace_back(vec3(1.f, 0.f, 0.f));
	coordinateSystemColors.emplace_back(vec3(1.f, 0.f, 0.f));

	coordinateSystemVertices.emplace_back(vec3(0.f, 0.f, 0.f));
	coordinateSystemVertices.emplace_back(vec3(0.f, 100.f, 0.f));
	coordinateSystemColors.emplace_back(vec3(0.f, 1.f, 0.f));
	coordinateSystemColors.emplace_back(vec3(0.f, 1.f, 0.f));

	coordinateSystemVertices.emplace_back(vec3(0.f, 0.f, 0.f));
	coordinateSystemVertices.emplace_back(vec3(0.f, 0.f, 100.f));
	coordinateSystemColors.emplace_back(vec3(0.f, 0.f, 1.f));
	coordinateSystemColors.emplace_back(vec3(0.f, 0.f, 1.f));

	GLuint shaderProgramID = Utilities::LoadShaders("vertexShader.glsl", "pixelShader.glsl");
	GLuint pntShaderProgramID = Utilities::LoadShaders("vertexShaderPNT.glsl", "pixelShaderPNT.glsl");


	GLuint coordinateSystemVertexArray;
	GLuint coordinateSystemVerticesBuffer;
	GLuint coordinateSystemColorsBuffer;

	GLuint splineVertexArray;
	GLuint splineVerticesBuffer;
	GLuint splineColorBuffer;

	GLuint trackVertexArrayLeft;
	GLuint trackVerticesBufferLeft;
	GLuint trackColorBufferLeft;
	GLuint trackIndexBufferLeft;

	GLuint trackVertexArrayRight;
	GLuint trackVerticesBufferRight;
	GLuint trackColorBufferRight;
	GLuint trackIndexBufferRight;

	glGenVertexArrays(1, &coordinateSystemVertexArray);
	glBindVertexArray(coordinateSystemVertexArray);

	glGenBuffers(1, &coordinateSystemVerticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, coordinateSystemVerticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * coordinateSystemVertices.size(), &coordinateSystemVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &coordinateSystemColorsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, coordinateSystemColorsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * coordinateSystemColors.size(), &coordinateSystemColors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenVertexArrays(1, &splineVertexArray);
	glBindVertexArray(splineVertexArray);

	glGenBuffers(1, &splineVerticesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, splineVerticesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gSplinePointPos.size(), &gSplinePointPos[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &splineColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, splineColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gSplinePointColor.size(), &gSplinePointColor[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Left
	glGenVertexArrays(1, &trackVertexArrayLeft);
	glBindVertexArray(trackVertexArrayLeft);

	glGenBuffers(1, &trackVerticesBufferLeft);
	glBindBuffer(GL_ARRAY_BUFFER, trackVerticesBufferLeft);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gTrackPosLeft.size(), &gTrackPosLeft[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &trackColorBufferLeft);
	glBindBuffer(GL_ARRAY_BUFFER, trackColorBufferLeft);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gTrackColorLeft.size(), &gTrackColorLeft[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &trackIndexBufferLeft);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trackIndexBufferLeft);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * gTrackIndexLeft.size(), &gTrackIndexLeft[0], GL_STATIC_DRAW);

	// Right
	glGenVertexArrays(1, &trackVertexArrayRight);
	glBindVertexArray(trackVertexArrayRight);

	glGenBuffers(1, &trackVerticesBufferRight);
	glBindBuffer(GL_ARRAY_BUFFER, trackVerticesBufferRight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gTrackPosRight.size(), &gTrackPosRight[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &trackColorBufferRight);
	glBindBuffer(GL_ARRAY_BUFFER, trackColorBufferRight);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * gTrackColorRight.size(), &gTrackColorRight[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &trackIndexBufferRight);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, trackIndexBufferRight);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * gTrackIndexRight.size(), &gTrackIndexRight[0], GL_STATIC_DRAW);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	GLuint MVPMatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	GLuint MVPMatrixIDPNT = glGetUniformLocation(pntShaderProgramID, "MVP");
	GLuint textureID = glGetUniformLocation(pntShaderProgramID, "textureSampler");

	mat4 proj = perspective(60.0f, static_cast<float> (gWindowWidth) / gWindowHeight, 0.1f, 5000.0f);



	do
	{
		gCurrTime = static_cast<float> (glfwGetTime());
		gDeltaTime = gCurrTime - gPrevTime;
		
		if(gCapFPS)
		{
			float sleepTime = std::max(0.067f - gDeltaTime, 0.0f);
			this_thread::sleep_for(chrono::milliseconds(static_cast<long long> (sleepTime * 1000)));
		}






		OnMouseEvent(window);
		OnKeyboardEvent(window);
		gCamera.UpdateView();
		mat4 VP = proj * gCamera.mViewMatrix;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramID);
		mat4 MVP = VP * mat4();
		glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &MVP[0][0]);

		/*glBindVertexArray(coordinateSystemVertexArray);
		glLineWidth(5.0f);
		glDrawArrays(GL_LINES, 0, 6);*/

		/*glBindVertexArray(splineVertexArray);
		glLineWidth(1.0f);
		glDrawArrays(GL_LINE_STRIP, 0, gSplinePointPos.size());*/

		glBindVertexArray(trackVertexArrayLeft);
		glDrawElements(GL_TRIANGLES, gTrackIndexLeft.size(), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(trackVertexArrayRight);
		glDrawElements(GL_TRIANGLES, gTrackIndexRight.size(), GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);


		glUseProgram(pntShaderProgramID);
		groundMesh.Render(pntShaderProgramID, VP);
		house.Render(pntShaderProgramID, VP);
		tank.Render(pntShaderProgramID, VP);
		dragon.Render(pntShaderProgramID, VP);
		helicopter.Render(pntShaderProgramID, VP);

		glCullFace(GL_FRONT);
		skybox.Render(pntShaderProgramID, VP);
		glCullFace(GL_BACK);

		if (!gFreeCamera)
		{
			gCamera.mPos = gSplinePointPos[gCurrentPoint];
			gCamera.mForward = gSplinePointTangent[gCurrentPoint];
			gCamera.mUp = gSplinePointNormal[gCurrentPoint];
			gCamera.mRight = gSplinePointBinormal[gCurrentPoint];

			float speed = static_cast<float> (pow(2 * gGravity * (gMaxHeight - gCamera.mPos.y), 0.5));
			gCurrentPoint += static_cast<int> (speed);

			if (gCurrentPoint >= gSplinePointPos.size())
			{
				gCurrentPoint = 0;
			}
		}



		glfwSwapBuffers(window);
		glfwPollEvents();

		gPrevTime = gCurrTime;
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

	glfwTerminate();
	return 0;
}
