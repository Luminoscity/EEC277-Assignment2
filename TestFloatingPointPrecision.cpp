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
float runTest(int test, GLFWwindow *window);
#pragma endregion

#pragma region Main
int main(int argc, char *argv[]) {
	CheckArgs(argc, argv);

	int testNum = atoi(argv[2]);
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
		 << "SUCCESS! Shader Compiled and Linked!\n\n";

	printLimits();

	while (!glfwWindowShouldClose(window))
	{
		GLenum err;               /* Everyone is puzzled why this matters. */
		err = glGetError();       /* But this error-checking makes it work */
		glUseProgram(program);    /* on Linux */
		err = glGetError();       /* Weird, huh? */
		runTest(testNum, window);
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
	char tests[] = { "tests:\n1: sin add\n2: cos add\n3: exp\n" };
	int numTests = 3;
	if(argc != 3) {
		fprintf(stderr, "Usage: %s shader_file test_number\n", argv[0]);
		printf("%s", tests);
		exit(-1);
	}
	if (atoi(argv[2]) < 1 || atoi(argv[2]) > numTests) {
		printf("Tests range from 1 to %d\n", numTests);
		printf("%s", tests);

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
float runTest(int test, GLFWwindow *window) {
	//int tex_w = 16, tex_h = 16;
	// create test data
	//float* data = (float*)malloc(tex_w*tex_h * sizeof(float));
	//float* result = (float*)calloc(tex_w*tex_h, sizeof(float));
	//for (int i = 0; i<tex_w*tex_h; i++)
	//	data[i] = i + 1.0;
	
	float data[] = { 1.0 };
	vector<float> results;

	//Angela: comment out double usage because precision qualifiers don't apply to double
	//double data[] = { 1.0 };
	//vector<double> results;
	results.push_back(data[0]);
	
	cout << "\nInput: " << results.back() << "\n";
	GLuint buffers[2];
	glGenBuffers(2, buffers);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, buffers[0]);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, buffers[1]);

	//input buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), data, GL_DYNAMIC_DRAW);
	//output buffer
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(data), NULL, GL_DYNAMIC_READ);

	const int iterations = 10000;
	double correctAnswer; //Angela: might be worthwhile to state in comments where this comes from (Wolfram?)
	switch (test)
	{
	case 3:			//Angela: exp test
		correctAnswer = 1.0;	//Angela: #####THIS IS NOT CORRECT YET#####
		break;
	case 2:			//cos add test
		correctAnswer = 5404.023058681397174009;
		break;
	default:		//sin add test
		correctAnswer = 8415.709848078965066525;
		break;
	}

	for (int i = 0; i < iterations; ++i) {
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
		//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(data), data);
		//results.push_back(data[0]);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(data), data);
	results.push_back(data[0]);
	printf("After run %d: \t%0.19f\n", iterations, results.back());
	printf("Correct Answer: \t%0.19f\n", correctAnswer);
	printf("Difference: %0.19f\n", correctAnswer - results.back());
	glfwSetWindowShouldClose(window, GLFW_TRUE);
	return results.back();
}
#pragma endregion
