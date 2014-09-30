#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "glew/include/GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glfw/include/GLFW/glfw3.h"
#include "picLibrary/pic.h"

using namespace std;

class Utilities
{
public:
	static GLuint LoadShaders(const string& InVertexShaderPath, const string& InPixelShaderPath);
};