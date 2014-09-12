#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Utilities.h"
#include "glm/glm.hpp"
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

	g_pHeightData = jpeg_read((char*)argv[1], NULL);
	if (!g_pHeightData)
	{
		cout << "error reading %s.\n" << argv[1];
		getchar();
		return -1;
	}


	for(int i = 0; i < g_pHeightData->ny; ++i)
	{
		for(int j = 0; j < g_pHeightData->nx; ++j)
		{
			unsigned char currPix = g_pHeightData->pix[i * g_pHeightData->ny + j];
		}
		cout << endl;
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

	if(glewInit() != GLEW_OK)
	{
		cout << "Cannot initialize GLEW. Press Enter to exit.\n";
		getchar();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

	do 
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	} 
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window));


	glfwTerminate();
	return 0;
}