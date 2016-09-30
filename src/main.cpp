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

// int main(int argc, char const *argv[]) {
// 	using namespace tnw::octree;
// 	// auto oct = std::make_unique<tnw::octree::Tree>();
// 	BoundingBox bb({0,0,0}, 1);
// 	// Box({.25,.25,-.25}, .5,.5,.5);
// 	// Box({.75,.25,-.75}, .5,.5,.5);
// 	// Sphere s(glm::vec3(0,0,0), 0.5);
// 	// SquarePyramid sp(glm::vec3(0,-1,0), 2, 0.5);

// 	auto oct  = classify(Box({.25,.25,-.25}, .5,.5,.5), bb, 4, 0);
// 	auto oct2 = classify(Box({.75,.25,-.75}, .5,.5,.5), bb, 4, 0);
// 	printf("%s\n", serialize(oct).c_str());
// 	printf("%s\n", serialize(oct2).c_str());
// 	printf("%s\n", serialize(tree_or(oct,oct2)).c_str());
// 	return 0;
// }

// //Classe da interface que representa uma cena com vários modelos
void key_callback(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main(void) {
	GLFWwindow* window;
	std::vector<std::unique_ptr<tnw::Model>> models;

	// Initialize the library
	if (!glfwInit())
		return 1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return 1;
	}

	// Init ImGui
	ImGui_ImplGlfw_Init(window, true);
	// Disable .ini
	ImGui::GetIO().IniFilename = nullptr;
	// Make the window's context current
	glfwMakeContextCurrent(window);

	int window_width, window_height;
	glfwGetWindowSize(window, &window_width, &window_height);

	//Opções de OpenGL
	glClearColor(1.,1.,1.,1.);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Camera initalization
	IsometricCamera camera;

	MainMenu mainMenu(models,camera);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);

		glPushMatrix();

		glm::mat4 view = tnw::isometric(camera.scale, camera.near, camera.far, camera.positive_hor, camera.positive_ver);
		glLoadMatrixf(glm::value_ptr(view));

		tnw::draw_axis();
		for (auto&& model : models)
			model->draw();

		glPopMatrix();

		ImGui::SetNextWindowSize(ImVec2(350,560), ImGuiSetCond_FirstUseEver);

		//Interface construction
		mainMenu.draw();
		// ImGui::ShowTestWindow();

		// Swap front and back buffers
		ImGui::Render();
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	ImGui_ImplGlfw_Shutdown();
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	} else {
		ImGui_ImplGlFw_KeyCallback(window, key, scancode, action, mods);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	auto x = std::min(width,height);
	if (x == width)
		glViewport(0, (height-x)/2, x, x);
	else
		glViewport((width-x)/2, 0, x, x);
}


