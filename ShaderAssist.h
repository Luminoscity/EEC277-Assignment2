#pragma once
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<GL/glew.h>

class ShaderAssist
{
public:
	ShaderAssist();
	~ShaderAssist();
	//throws error message string if file fails to open OR compilation of shader failed
	static GLuint CreateFromFile(std::string fileName, GLenum shaderType);
	//throws error message string if linking failed
	static void LinkProgram(GLuint program);
};


