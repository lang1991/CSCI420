#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Utilities.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"
#include "picLibrary/pic.h"

using namespace std;
using namespace glm;

/* see <your pic directory>/pic.h for type Pic */
Pic * g_pHeightData;

// Window size
int windowWidth = 640;
int windowHeight = 480;

// Mesh configurations
int XStep = 1;
int ZStep = 1;


void saveScreenshot(char *filename)
{
	int i;
	Pic *in = NULL;

	if (filename == NULL)
		return;

	/* Allocate a picture buffer */
	in = pic_alloc(640, 480, 3, NULL);

	printf("File to save to: %s\n", filename);

	for (i = 479; i >= 0; i--) {
		glReadPixels(0, 479 - i, 640, 1, GL_RGB, GL_UNSIGNED_BYTE,
			&in->pix[i*in->nx*in->bpp]);
	}

	if (jpeg_write(filename, in))
		printf("File saved Successfully\n");
	else
		printf("Error in Saving\n");

	pic_free(in);
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

void BuildHeightmap(int inXStart, int inYStart, int inZStart, unsigned int inXStep, unsigned int inZStep, vector<vec3>& outVertices, vector<unsigned short>& outIndices)
{
	// Get (x, y, z) for all vertices
	outVertices.reserve(g_pHeightData->ny * g_pHeightData->nx);
	for (int i = 0; i < g_pHeightData->ny; ++i)
	{
		int zPos = inZStart + i * inZStep; 
		for (int j = 0; j < g_pHeightData->nx; ++j)
		{
			int xPos = inXStart + j * inXStep;
			int yPos = static_cast<int> (g_pHeightData->pix[i * g_pHeightData->ny + j]);

			outVertices.emplace_back(vec3(xPos, yPos, zPos));
		}
	}

	// Get the indices for all triangles 
	// for index buffer
	unsigned int triangleCount = 0;
	outIndices.resize((g_pHeightData->ny - 1) * (g_pHeightData->nx - 1 )* 2 * 3);
	for(int i = 0; i < g_pHeightData->ny - 1; ++i)
	{
		for(int j = 0; j < g_pHeightData->nx - 1; ++j)
		{
			outIndices[triangleCount] = i * g_pHeightData->ny + j;	
			outIndices[triangleCount + 1] = i * g_pHeightData->ny + (j + 1);
			outIndices[triangleCount + 2] = (i + 1) * g_pHeightData->ny + j;
			outIndices[triangleCount + 3] = (i + 1) * g_pHeightData->ny + j;
			outIndices[triangleCount + 4] = i * g_pHeightData->ny + (j + 1);
			outIndices[triangleCount + 5] = (i + 1) * g_pHeightData->ny + (j + 1);
			
			triangleCount += 6;
		}
	}
}

int main(int argc, char** argv)
{
	// I've set the argv[1] to spiral.jpg.
	// To change it, on the "Solution Explorer",
	// right click "assign1", choose "Properties",
	// go to "Configuration Properties", click "Debugging",
	// then type your texture name for the "Command Arguments"
	if (argc < 2)
	{
		cout << "usage: %s heightfield.jpg\n" << argv[0];
		getchar();
		return -1;
	}

	if(!glfwInit())
	{
		cout << "Cannot initialize GLFW. Press Enter to exit.\n";
		getchar();
		return -1;
	}

	GLFWwindow* window = CreateWindow(windowWidth, windowHeight);
	if(!window)
	{
		cout << "Cannot create GLFW window. Maybe update your GPU driver?\nPress Enter to exit.\n";
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if(glewInit() != GLEW_OK)
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

	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	
	GLuint shaderProgramID = Utilities::LoadShaders("vertexShader.glsl", "pixelShader.glsl");
	GLuint MVPMatrixID = glGetUniformLocation(shaderProgramID, "MVP");
	mat4 proj = perspective(60.0f, static_cast<float> (windowWidth) / windowHeight, 0.1f, 1000.0f);
	mat4 view = lookAt(vec3(0, 500, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	mat4 model = mat4(1.0f);
	mat4 MVP = proj * view * model;

	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
		cout << "error reading %s.\n" << argv[1];
		getchar();
		return -1;
	}

	vector<vec3> vertices;
	vector<unsigned short> indices;
	int upperLeftX = -g_pHeightData->nx / 2;
	int upperLeftZ = -g_pHeightData->ny / 2;
	BuildHeightmap(upperLeftX, 0, upperLeftZ, XStep, ZStep, vertices, indices);

	

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	GLuint indexBuffer;
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glUseProgram(shaderProgramID);


	do 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniformMatrix4fv(MVPMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));


	glfwTerminate();
	return 0;
}