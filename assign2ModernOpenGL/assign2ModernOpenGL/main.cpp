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

vector<vec3> gSplinePointPos;
vector<vec3> gSplinePointColor;


const float gROTATIONSPEED = 0.25f;
const float gWALKSPEED = 50.0f;
Camera gCamera = Camera(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, -1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

float gPrevTime = 0.0f;
float gCurrTime = 0.0f;
float gDeltaTime = 0.0f;

unordered_map<string, GLuint> gTextureDict;

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

			vec3 fp0 = vec3(static_cast<float> (p0.x), static_cast<float> (p0.y), static_cast<float> (p0.z));
			vec3 fp1 = vec3(static_cast<float> (p1.x), static_cast<float> (p1.y), static_cast<float> (p1.z));
			vec3 fp2 = vec3(static_cast<float> (p2.x), static_cast<float> (p2.y), static_cast<float> (p2.z));
			vec3 fp3 = vec3(static_cast<float> (p3.x), static_cast<float> (p3.y), static_cast<float> (p3.z));
			
			mat4x3 controlMatrix(fp0, fp1, fp2, fp3);

			/*mat4x3 controlMatrix(static_cast<float> (p0.x), static_cast<float> (p0.y), static_cast<float> (p0.z),
				static_cast<float> (p1.x), static_cast<float> (p1.y), static_cast<float> (p1.z),
				static_cast<float> (p2.x), static_cast<float> (p2.y), static_cast<float> (p2.z),
				static_cast<float> (p3.x), static_cast<float> (p3.y), static_cast<float> (p3.z));*/

			OutResult.emplace_back(controlMatrix * gBasisMatrix);
		}
	}
}

void RecSubdiv(float InU0, float InU1, float InMaxLineLength, int InSegIndex)
{
	float umid = InU0 + (InU1 - InU0) / 2;

	dvec4 u0Vec = dvec4(pow(InU0, 3), pow(InU0, 2), InU0, 1.0f);
	dvec4 u1Vec = dvec4(pow(InU1, 3), pow(InU1, 2), InU1, 1.0f);

	dvec3 x0 = gSegmentBasisMultiplyControl[InSegIndex] * u0Vec;
	dvec3 x1 = gSegmentBasisMultiplyControl[InSegIndex] * u1Vec;


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
	double newMouseX;
	double newMouseY;

	glfwGetCursorPos(InWindow, &newMouseX, &newMouseY);


	gCamera.LookLeftRight(static_cast<float> (-(newMouseX - gWindowWidth / 2) * gROTATIONSPEED));
	gCamera.LookUpDown(static_cast<float> (-(newMouseY - gWindowHeight / 2) * gROTATIONSPEED));

	glfwSetCursorPos(InWindow, static_cast<double> (gWindowWidth) / 2, static_cast<double> (gWindowHeight) / 2);
}

void OnKeyboardEvent(GLFWwindow* InWindow)
{
	if(glfwGetKey(InWindow, GLFW_KEY_W) == GLFW_PRESS)
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
	glfwSetCursorPos(window, static_cast<double> (gWindowWidth) / 2, static_cast<double> (gWindowHeight) / 2);


	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	/*StaticMesh groundMesh(".//assets//ground.itpmesh");
	groundMesh.mTransform = translate(vec3(0.0f, 0.0f, 0.0f));*/

	StaticMesh groundMesh(".//assets//desert.itpmesh");
	groundMesh.mTransform = translate(vec3(0.0f, -300.0f, 0.0f));

	StaticMesh skybox(".//assets//skybox.itpmesh");
	skybox.mTransform = translate(vec3(0.0f, -400.0f, 0.0f)) * scale(vec3(2.0f, 2.0f, 2.0f));

	gCamera.mPos = vec3(10.0f, 10.0f, 100.0f);

	gTrack.LoadSplines(argv[1]);
	vec4 row1(-gTension, 2 - gTension, gTension - 2, gTension);
	vec4 row2(2 * gTension, gTension - 3, 3 - 2 * gTension, -gTension);
	vec4 row3(-gTension, 0, gTension, 0);
	vec4 row4(0, 1, 0, 0);
	gBasisMatrix = mat4(row1, row2, row3, row4);

	CalcSegmentsControlMatTimesBasis(gSegmentBasisMultiplyControl);
	for(unsigned int i = 0; i < gSegmentBasisMultiplyControl.size(); ++i)
	{
		RecSubdiv(0, 1.0f, 0.1f, i);
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

	GLuint shaderProgramID = Utilities::LoadShaders("vertexShader.glsl", "pixelShader.glsl");
	GLuint pntShaderProgramID = Utilities::LoadShaders("vertexShaderPNT.glsl", "pixelShaderPNT.glsl");


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
	

	GLuint MVPMatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	GLuint MVPMatrixIDPNT = glGetUniformLocation(pntShaderProgramID, "MVP");
	GLuint textureID = glGetUniformLocation(pntShaderProgramID, "textureSampler");
	
	mat4 proj = perspective(60.0f, static_cast<float> (gWindowWidth) / gWindowHeight, 0.1f, 5000.0f);
	
	do
	{
		gCurrTime = static_cast<float> (glfwGetTime());
		gDeltaTime = gCurrTime - gPrevTime;

		OnMouseEvent(window);
		OnKeyboardEvent(window);
		gCamera.UpdateView();
		mat4 VP = proj * gCamera.mViewMatrix;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgramID);
		mat4 MVP = VP * mat4();
		glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &MVP[0][0]);
		
		glBindVertexArray(coordinateSystemVertexArray);
		glLineWidth(5.0f);
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(splineVertexArray);
		glLineWidth(1.0f);
		glDrawArrays(GL_LINE_STRIP, 0, gSplinePointPos.size());
		glBindVertexArray(0);


		glUseProgram(pntShaderProgramID);
		groundMesh.Render(pntShaderProgramID, VP);

		glCullFace(GL_FRONT);
		skybox.Render(pntShaderProgramID, VP);
		glCullFace(GL_BACK);


		/*MVP = VP * groundMesh.mTransform;	
		glUniformMatrix4fv(MVPMatrixIDPNT, 1, GL_FALSE, &MVP[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, groundMesh.mTextureIndex);
		glUniform1i(textureID, 0);
		glBindVertexArray(pntVertexArray);
		glDrawElements(GL_TRIANGLES, groundMesh.mIndex.size(), GL_UNSIGNED_SHORT, nullptr);

		glCullFace(GL_FRONT);
		MVP = VP * skybox.mTransform;
		glUniformMatrix4fv(MVPMatrixIDPNT, 1, GL_FALSE, &MVP[0][0]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skybox.mTextureIndex);
		glUniform1i(textureID, 0);
		glBindVertexArray(pntVertexArray);
		glDrawElements(GL_TRIANGLES, skybox.mIndex.size(), GL_UNSIGNED_SHORT, nullptr);


		glCullFace(GL_BACK);*/
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		gPrevTime = gCurrTime;
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));

	glfwTerminate();
	return 0;
}
