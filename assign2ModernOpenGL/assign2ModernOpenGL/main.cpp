#include "Utilities.h"
#include "Track.h"
#include "Camera.h"

using namespace std;
using namespace glm;

// Window size
int gWindowWidth = 640;
int gWindowHeight = 480;

Track gTrack;

double gTension = 0.5;
mat4 gBasisMatrix;

vector<mat4x3> gSegmentBasisMultiplyControl;

vector<vec3> gSplinePointPos;
vector<vec3> gSplinePointColor;


const float gROTATIONSPEED = 10.f;
const float gWALKSPEED = 5.0f;
Camera gCamera = Camera(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

float prevTime = 0.0f;
float currTime = 0.0f;
float deltaTime = 0.0f;

void CalcSegmentsControlMatTimesBasis(vector<mat4x3>& OutResult)
{
	// Go through all the splines
	for (unsigned int i = 0; i < gTrack.mNumOfSplines; ++i)
	{
		// Go through all the control points of the spline
		for (unsigned int j = 1; j < gTrack.mSplines[i].mNumControlPoints - 2; ++j)
		{
			dvec3& p0 = gTrack.mSplines[i].mPoints[j - 1];
			dvec3& p1 = gTrack.mSplines[i].mPoints[j];
			dvec3& p2 = gTrack.mSplines[i].mPoints[j + 1];
			dvec3& p3 = gTrack.mSplines[i].mPoints[j + 2];

			mat4x3 controlMatrix(p0.x, p0.y, p0.z,
				p1.x, p1.y, p1.z,
				p2.x, p2.y, p2.z,
				p3.x, p3.y, p3.z);

			OutResult.emplace_back(controlMatrix * gBasisMatrix);
		}
	}
}

void RecSubdiv(float InU0, float InU1, float InMaxLineLength, int InSegIndex)
{
	float umid = InU0 + (InU1 - InU0) / 2;

	vec4 u0Vec = vec4(pow(InU0, 3), pow(InU0, 2), InU0, 1.0f);
	vec4 u1Vec = vec4(pow(InU1, 3), pow(InU1, 2), InU1, 1.0f);

	vec3 x0 = gSegmentBasisMultiplyControl[InSegIndex] * u0Vec;
	vec3 x1 = gSegmentBasisMultiplyControl[InSegIndex] * u1Vec;


	if (length(x1 - x0) > InMaxLineLength)
	{
		RecSubdiv(InU0, umid, InMaxLineLength, InSegIndex);
		RecSubdiv(umid, InU1, InMaxLineLength, InSegIndex);
	}
	else
	{
		gSplinePointPos.emplace_back(vec3(x0.x, x0.y, x0.z)); 
		gSplinePointColor.emplace_back(vec3(1.0f, 0.0f, 0.0f));
		
		gSplinePointPos.emplace_back(vec3(x1.x, x1.y, x1.z));
		gSplinePointColor.emplace_back(vec3(1.0f, 0.0f, 0.0f));
	}
}

void saveScreenshot(char *filename){

	int i, j;
	Pic *in = NULL;
	Pic *out = NULL;

	if (filename == NULL)
	{
		return ;
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
	
}

void OnKeyboardEvent(GLFWwindow* InWindow)
{
	if(glfwGetKey(InWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		gCamera.MoveForwardBack(deltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		gCamera.MoveForwardBack(-deltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		gCamera.MoveLeftRight(-deltaTime * gWALKSPEED);
	}

	if (glfwGetKey(InWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		gCamera.MoveLeftRight(deltaTime * gWALKSPEED);
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
	GLFWwindow* window = glfwCreateWindow(InWidth, InHeight, "Assignment 1", nullptr, nullptr);

	return window;
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("usage: %s <trackfile>\n", argv[0]);
		exit(0);
	}
	
	Track track;
	track.LoadSplines(argv[1]);

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

	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	gTrack.LoadSplines(argv[1]);

	vec4 row1(-gTension, 2 - gTension, gTension - 2, gTension);
	vec4 row2(2 * gTension, gTension - 3, 3 - 2 * gTension, -gTension);
	vec4 row3(-gTension, 0, gTension, 0);
	vec4 row4(0, 1, 0, 0);
	gBasisMatrix = mat4(row1, row2, row3, row4);

	CalcSegmentsControlMatTimesBasis(gSegmentBasisMultiplyControl);
	for(unsigned int i = 0; i < gSegmentBasisMultiplyControl.size(); ++i)
	{
		RecSubdiv(0, 1.0f, 0.1, i);
	}

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

	GLuint coordinateSystemVertexArray;
	GLuint coordinateSystemVerticesBuffer;
	GLuint coordinateSystemColorsBuffer;
	
	GLuint splineVertexArray;
	GLuint splineVerticesBuffer;
	GLuint splineColorBuffer;

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


	glBindVertexArray(0);


	GLuint shaderProgramID = Utilities::LoadShaders("vertexShader.glsl", "pixelShader.glsl");
	GLuint MVPMatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	mat4 proj = perspective(60.0f, static_cast<float> (gWindowWidth) / gWindowHeight, 0.1f, 2000.0f);
	glUseProgram(shaderProgramID);
	do
	{
		currTime = glfwGetTime();
		deltaTime = currTime - prevTime;

		OnKeyboardEvent(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		gCamera.UpdateView();
		mat4 MVP = proj * gCamera.mViewMatrix * mat4();
		glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		glBindVertexArray(coordinateSystemVertexArray);
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(splineVertexArray);
		glDrawArrays(GL_LINE_STRIP, 0, gSplinePointPos.size());

		glfwSwapBuffers(window);
		glfwPollEvents();
		
		prevTime = currTime;
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

	glfwTerminate();
	return 0;
}
