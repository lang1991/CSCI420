#include "Utilities.h"

GLuint Utilities::LoadShaders(const string& InVertexShaderPath, const string& InPixelShaderPath)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint pixelShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	
	string vertexShaderCode;
	ifstream fin(InVertexShaderPath);
	if(fin.is_open())
	{
		while(fin.good())
		{
			string currLine;
			getline(fin, currLine);
			vertexShaderCode += currLine + "\n";
		}
		fin.close();
	}
	else
	{
		cout << "Cannot open \"" << InVertexShaderPath << "\"\nProcess Terminated.\n";
		getchar();
		exit(-1);
	}

	string pixelShaderCode;
	fin.open(InPixelShaderPath);
	if(fin.is_open())
	{
		while(fin.good())
		{
			string currLine;
			getline(fin, currLine);
			pixelShaderCode += currLine + "\n";
		}
		fin.close();
	}
	else
	{
		cout << "Cannot open \"" << InPixelShaderPath << "\"\nProcess Terminated.\n";
		getchar();
		exit(-1);
	}

	GLint result = GL_FALSE;
	int logLength;
	cout << "Compiling vertex shader " << InVertexShaderPath << endl;
	const GLchar* temp = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &temp, nullptr);
	glCompileShader(vertexShaderID);

	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1)
	{
		vector<char> vertexShaderErrorMessage(logLength + 1);
		glGetShaderInfoLog(vertexShaderID, logLength, nullptr, &vertexShaderErrorMessage[0]);
		cout << "Vertex Shader Compile Error:\n" << &vertexShaderErrorMessage[0] << endl;
	}

	cout << "Compiling pixel shader" << InPixelShaderPath << endl;
	temp = pixelShaderCode.c_str();
	glShaderSource(pixelShaderID, 1, &temp, nullptr);
	glCompileShader(pixelShaderID);

	glGetShaderiv(pixelShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(pixelShaderID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1)
	{
		vector<char> pixelShaderErrorMessage(logLength + 1);
		glGetShaderInfoLog(pixelShaderID, logLength, nullptr, &pixelShaderErrorMessage[0]);
		cout << "Pixel Shader Compile Error:\n" << &pixelShaderErrorMessage[0] << endl;
	}

	cout << "Linking to a program\n";
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, pixelShaderID);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 1)
	{
		vector<char> programErrorMessage(logLength + 1);
		glGetProgramInfoLog(programID, logLength, nullptr, &programErrorMessage[0]);
		cout << "Program Creation Error:\n" << &programErrorMessage[0] << endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(pixelShaderID);

	return programID;
}