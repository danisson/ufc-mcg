#include "model.h"
#include "helper.h"
#include "interface.h"
#include "shapes.h"
#include "raycast.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <sstream>
#include <limits>
#include <glm/gtx/string_cast.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#undef near
#undef far
#endif

void key_callback(GLFWwindow*, int, int, int, int);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

bool imgChanged = false, genImg = true;

// struct Raycast
// {
// 	size_t width, height;
// 	float dx, dy, near, far;

// 	Raycast(size_t _width, size_t _height, float _near, float _far) : width(_width), height(_height), near(_near), far(_far) {
// 		dx = 1/_width;
// 		dy = 1/_height;
// 	}
// };

int main(void) {
	GLFWwindow* window;
	std::vector<std::unique_ptr<tnw::Model>> models;

	// Initialize the library
	if (!glfwInit())
		return 1;

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(640, 480, "Sickle", NULL, NULL);
	swidth = 640;
	sheight = 480;
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
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glFrontFace(GL_CCW);
	glShadeModel(GL_SMOOTH);

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Camera initalization
	IsometricCamera camera;

	auto w = 41;
	auto h = 40;
	// Raycast rc(w,h,1,-100);
	genImg = true;

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// camera.aspect = 480/640.;
	MainMenu mainMenu(models,camera);

	std::vector<tnw::Shape*> scene;
	tnw::Shape *obj1 = new tnw::Sphere(glm::vec3(0,0,-1),1);
	scene.push_back(obj1);
	scene.push_back(new tnw::Box(glm::vec3(4,1,-3),1,1,1));
	tnw::Raycast raycast(scene, camera, w, h, tex);


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		ImGui::SetNextWindowSize(ImVec2(350,350), ImGuiSetCond_FirstUseEver);
		// mainMenu.draw();
		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// if (genImg) {
		// 	std::cout << "Generating image...\n";
		// 	std::flush(std::cout);
		// 	// std::cout << "Generating image...";
		// 	// std::flush(std::cout);
		// 	genImg = false;

		// 	for (size_t i = 0; i < rc.width; ++i) {
		// 		for (size_t j = 0; j < rc.height; ++j) {

		// 			auto nx = -2.f*(1.f/2.f - float(i+1)/rc.width);
		// 			auto ny = 2.f*(1.f/2.f - float(j+1)/rc.height);
		// 			auto x = -2.f*(1.f/2.f - float(i)/rc.width);
		// 			auto y = 2.f*(1.f/2.f - float(j)/rc.height);

		// 			float pospixx = x+nx/2;
		// 			float pospixy = y+ny/2;

		// 			glm::vec3 a(pospixx, pospixy, rc.near);
		// 			glm::vec3 b(pospixx, pospixy, rc.far);
		// 			// std::cout << glm::to_string(a) << std::endl;

		// 			tnw::Ray r = tnw::Ray(a,b);

		// 			float minInter = rc.far;
		// 			int drawIndice = -1;
		// 			for (size_t k = 0; k < scene.size(); k++) {
		// 				tnw::IntersectionList ilist = scene[k]->intersect_ray(r);
		// 				if (ilist.size() >= 2) {
		// 					if (minInter < std::abs(std::get<1>(ilist[0]))) {
		// 						minInter = std::abs(std::get<1>(ilist[0]));
		// 						drawIndice = k;
		// 					}
		// 				}
		// 			}

		// 			if (drawIndice >= 0) {

		// 				// std::cout << "\ni: " << i << " j: " << j << " drawIndice: " << drawIndice;
		// 				// tnw::IntersectionList ilist = scene[0]->intersect_ray(r);
		// 				// std::cout << "\nilist size: " << ilist.size();
		// 				// std::cout << "\na: " << glm::to_string(a) << "\nb: " << glm::to_string(b) << "\n";

		// 				img(i,j) = std::make_tuple(0,0,0);
		// 			}
		// 		}
		// 	}

		// 	glTexSubImage2D(GL_TEXTURE_2D, 0 ,0, 0, rc.width, rc.height, GL_RGB, GL_FLOAT, img);
		// 	std::cout << " generated image\n";
		// 	std::flush(std::cout);
		// }

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
	} else
		ImGui_ImplGlFw_KeyCallback(window, key, scancode, action, mods);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	camera.aspect = height/((float)width);
	glViewport(0,0,width,height);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {

	}
}
