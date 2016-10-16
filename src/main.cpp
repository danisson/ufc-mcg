#include "model.h"
#include "helper.h"
#include "interface.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <sstream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#undef near
#undef far
#endif

void key_callback(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//Camera initalization
IsometricCamera camera;

int main(void) {
	GLFWwindow* window;
	std::vector<std::unique_ptr<tnw::Model>> models;

	// Initialize the library
	if (!glfwInit())
		return 1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Sickle", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return 1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Init ImGui
	ImGui_ImplGlfw_Init(window, true);
	// Disable .ini
	ImGui::GetIO().IniFilename = nullptr;

	//Opções de OpenGL
	glClearColor(0.,0.,0.,0.);
	// glEnable(GL_LINE_SMOOTH);
	// glEnable(GL_CULL_FACE);
	// glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	tnw::Image img(2,2);

	img(0,0) = img(1,1) = std::make_tuple(0,0,1);
	img(1,0) = img(0,1) = std::make_tuple(0,1,0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, img);

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	camera.aspect = 480/640.;
	MainMenu mainMenu(models,camera);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		ImGui::SetNextWindowSize(ImVec2(350,350), ImGuiSetCond_FirstUseEver);
		mainMenu.draw();
		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 1);
		glVertex2f(-1.,-1.);
		glTexCoord2d(1, 1);
		glVertex2f( 1.,-1.);
		glTexCoord2d(1, 0);
		glVertex2f( 1., 1.);
		glTexCoord2d(0, 0);
		glVertex2f(-1., 1.);
		glEnd();

		ImGui::Render();
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	glDeleteTextures(1, &tex);
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	camera.aspect = height/((float)width);
	glViewport(0,0,width,height);
}


