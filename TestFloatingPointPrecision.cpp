#pragma region Copyright
/*
 * TestFloatingPointPrecision.cpp
 *
 * GLSL Setup Code from https://www.khronos.org/opengl/wiki/Example/GLSL_Full_Compile_Linking
 * and http://antongerdelan.net/opengl/compute.html
 */
#pragma endregion

#pragma region Includes, Types, and Constants
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "ShaderAssist.h"

#define DEFAULT_WIN_WIDTH 300
#define DEFAULT_WIN_HEIGHT 50

using std::string;
using std::vector;
using std::cout;

void CheckArgs(int argc, char *argv[]);
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void printInfo(GLFWwindow * window);
void runTest();
#pragma endregion

#pragma region Main
int main(int argc, char *argv[]) {
	CheckArgs(argc, argv);

	cout << "Setting up OpenGL context...\n";
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* window = glfwCreateWindow(DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT, argv[0], NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	// start GLEW extension handler
	// glewExperimental = GL_TRUE;
	glewInit();
	glfwSwapInterval(1);
	cout << "Window created.\n";

	printInfo(window);

	//Get shader from file
	GLuint computeShader;
	try {
		cout << "\nGetting shader code from file and compiling...\n";
		computeShader = ShaderAssist::CreateFromFile(string(argv[1]), GL_COMPUTE_SHADER);
	}
	catch (string ex) {
		fprintf(stderr, ex.c_str());
		exit(-1);
	};

	//Vertex and fragment shaders are successfully compiled.
	//Now time to link them together into a program.
	//Get a program object.
	GLuint program = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(program, computeShader);

	//Link our program
	try {
		cout << "Linking program...\n";

		ShaderAssist::LinkProgram(program);
	}
	catch (string ex) {
		glDeleteShader(computeShader);
		fprintf(stderr, ex.c_str());
		exit(-1);
	};

	cout << "\n--------------------------------------------------\n"
		 << "SUCCESS! Shader Compiled and Linked!\n"
		 << "Press [esc] to close window.\n";

	while (!glfwWindowShouldClose(window))
	{
		GLenum err;               /* Everyone is puzzled why this matters. */
		err = glGetError();       /* But this error-checking makes it work */
		glUseProgram(program);    /* on Linux */
		err = glGetError();       /* Weird, huh? */
		runTest();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//Always detach shaders after a successful link.
	glDetachShader(program, computeShader);

	return 0;
}
#pragma endregion

#pragma region Helper Functions
void CheckArgs(int argc, char *argv[]){
	if(argc != 2) {
		fprintf(stderr, "Usage: %s shader_file\n", argv[0]);
		exit(-1);
	}
	std::ifstream file(argv[1]);
	if(!file.good()) {
		fprintf(stderr, "Could not open file: %s\n", argv[1]);
		exit(-1);
	}
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void printInfo(GLFWwindow * window) {
	/* Display the gfx card information */
	printf("--------------------------------------------------\n");
	printf("Vendor:      %s\n", glGetString(GL_VENDOR));
	printf("Renderer:    %s\n", glGetString(GL_RENDERER));
	printf("Version:     %s\n", glGetString(GL_VERSION));
	GLFWmonitor * monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode * vidMode = glfwGetVideoMode(monitor);
	printf("Visual:      RGBA=<%d,%d,%d,%d>  Z=<%d>  double=%d\n",
		vidMode->redBits,
		vidMode->blueBits,
		vidMode->greenBits,
		GLFW_ALPHA_BITS,
		GLFW_DEPTH_BITS,
		1);
	int winWidth, winHeight;
	glfwGetFramebufferSize(window, &winWidth, &winHeight);
	int xPos, yPos;
	glfwGetWindowPos(window, &xPos, &yPos);
	printf("Geometry:    %dx%d+%d+%d\n",
		winWidth,
		winHeight,
		xPos,
		yPos);
	printf("Screen:      %dx%d\n",
		vidMode->width,
		vidMode->height);
	printf("--------------------------------------------------\n");
}
#pragma endregion

#pragma region Test
void runTest() {
	//glfwTerminate();
	//exit(0);
}
#pragma endregion
