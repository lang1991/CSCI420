#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "glew/include/GL/glew.h"

using namespace std;

class Utilities
{
public:
	static GLuint LoadShaders(const string& InVertexShaderPath, const string& InPixelShaderPath);
};