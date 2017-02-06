/*
* ShaderAssist.cpp
* Class by Tim Ambrose
*
* GLSL Setup Code 
* Shader Code from http://antongerdelan.net/opengl/compute.html
*/

#include "ShaderAssist.h"
#include <iostream>

ShaderAssist::ShaderAssist()
{
}


ShaderAssist::~ShaderAssist()
{
}

//throws error message string if file fails to open OR compilation of shader failed
std::vector<GLuint> ShaderAssist::GetShadersFromFile(std::string fileName, GLenum shaderType,
	                                                 std::vector<std::string> *shaderNames) {
	//Get source code for shader.
	std::ifstream file(fileName);
	if(!file.good())
		throw "Could not open file: " + fileName;
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	std::string shaderString = ss.str();
	std::vector<std::string> shaderSource;
	std::vector<GLuint> shaders;
	const char delim[] = "<<>>";

	size_t pos1 = 0, pos2 = 0;
	while (shaderString.find(delim, pos1) != std::string::npos) {
		pos1 = shaderString.find(delim, pos2);
		pos2 = shaderString.find(delim, pos1 + strlen(delim));
		shaderSource.push_back(shaderString.substr(pos1 + strlen(delim), pos2 - pos1 - strlen(delim)));
		pos1 = pos2 + strlen(delim);
	}

	/*
	std::vector<std::string>::iterator it;
	std::cout << "------------------------------\n";
	for (it = shaderSource.begin(); it != shaderSource.end(); ++it)
		std::cout << *it << "\n------------------------------\n";
	exit(0);
	*/
	
	while (shaderSource.size() > 0) {
		//Much of this is from https://www.khronos.org/opengl/wiki/Example/GLSL_Full_Compile_Linking

		//Create an empty vertex shader handle
		shaders.push_back(glCreateShader(shaderType));

		std::string name = shaderSource.front();
		shaderSource.erase(shaderSource.begin());
		if (shaderNames != NULL)
			shaderNames->push_back(name);
		std::string sourceCode = shaderSource.front();
		shaderSource.erase(shaderSource.begin());
		sourceCode = sourceCode.substr(sourceCode.find("#"));

		//Send the shader source code to GL
		//Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = (const GLchar *)sourceCode.c_str();
		glShaderSource(shaders.back(), 1, &source, 0);

		//Compile the shader
		glCompileShader(shaders.back());

		GLint isCompiled = 0;
		glGetShaderiv(shaders.back(), GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaders.back(), GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaders.back(), maxLength, &maxLength, &infoLog[0]);

			//We don't need the shader anymore.
			glDeleteShader(shaders.back());

			//Use the infoLog as you see fit.
			std::string errorMessage(infoLog.begin(), infoLog.end());
			throw "Shader (" + name + ") Compilation Failed: " + errorMessage;
		}
	}
	
	return shaders;
}

//throws error message string if linking failed
void ShaderAssist::LinkProgram(GLuint program)
{
	//From https://www.khronos.org/opengl/wiki/Example/GLSL_Full_Compile_Linking

	glLinkProgram(program);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		//We don't need the program anymore.
		glDeleteProgram(program);

		//Use the infoLog as you see fit.
		std::string errorMessage(infoLog.begin(), infoLog.end());
		throw "Program Link Failed: " + errorMessage;
	}
}
