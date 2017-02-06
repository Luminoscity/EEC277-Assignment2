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
	//throws error message string if file fails to open OR compilation of a shader fails
	static std::vector<GLuint> GetShadersFromFile(std::string fileName, GLenum shaderType,
		                                     std::vector<std::string> *shaderNames = NULL);
	//throws error message string if linking fails
	static void LinkProgram(GLuint program);
};


