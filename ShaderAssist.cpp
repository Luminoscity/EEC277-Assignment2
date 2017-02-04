/*
* ShaderAssist.cpp
* Class by Tim Ambrose
*
* GLSL Setup Code 
* Shader Code from http://antongerdelan.net/opengl/compute.html
*/

#include "ShaderAssist.h"

ShaderAssist::ShaderAssist()
{
}


ShaderAssist::~ShaderAssist()
{
}

//throws error message string if file fails to open OR compilation of shader failed
GLuint ShaderAssist::CreateFromFile(std::string fileName, GLenum shaderType)
{
	//Get source code for shader.
	std::ifstream file(fileName);
	if(!file.good())
		throw "Could not open file: " + fileName;
	std::stringstream ss;
	ss << file.rdbuf();
	file.close();
	std::string shaderSource = ss.str();

	//From https://www.khronos.org/opengl/wiki/Example/GLSL_Full_Compile_Linking

	//Create an empty vertex shader handle
	GLuint shader = glCreateShader(shaderType);

	//Send the shader source code to GL
	//Note that std::string's .c_str is NULL character terminated.
	const GLchar *source = (const GLchar *)shaderSource.c_str();
	glShaderSource(shader, 1, &source, 0);

	//Compile the shader
	glCompileShader(shader);

	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		//We don't need the shader anymore.
		glDeleteShader(shader);

		//Use the infoLog as you see fit.
		std::string errorMessage(infoLog.begin(), infoLog.end());
		throw "Shader (" + fileName + ") Compilation Failed: " + errorMessage;
	}

	return shader;
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
