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
void printLimits();
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
		 << "SUCCESS! Shader Compiled and Linked!\n";

	// declare texture size, the actual data will be a vector 
	// of size texSize*texSize*4
	int tex_w = 16, tex_h = 16;
	// create test data
	float* data = (float*)malloc(tex_w*tex_h * sizeof(float));
	float* result = (float*)malloc(tex_w*tex_h * sizeof(float));
	for (int i = 0; i<tex_w*tex_h; i++)
		data[i] = i + 1.0;

	printLimits();
	cout << "\nStoring input data.\n";
	GLuint buffers[2];
	glGenBuffers(2, buffers);
	//input buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
	//output buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), NULL, GL_DYNAMIC_READ);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[1]);
	printf("Data before computation:\n");
	for (int i = 0; i<tex_w*tex_h; i++)
		printf("%f\n", data[i]);
	cout << "Press [esc] to close window.\n";

	while (!glfwWindowShouldClose(window))
	{
		GLenum err;               /* Everyone is puzzled why this matters. */
		err = glGetError();       /* But this error-checking makes it work */
		glUseProgram(program);    /* on Linux */
		err = glGetError();       /* Weird, huh? */
		//runTest();
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// and read back
	glUseProgram(0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(result), result);
	// print out results
	printf("Data after computation:\n");
	for (int i = 0; i<tex_w*tex_h; i++)
		printf("%f\n", result[i]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// clean up
	free(data);
	free(result);
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

void printLimits() {
	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);

	GLint work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);
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
