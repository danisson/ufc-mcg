#include "model.h"
#include "helper.h"
#include "interface.h"
#include "shapes.h"
#include "raycast.h"
#include "csgtree.h"
#include "wed.h"

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

	// glfwSetWindowIcon(window,1,)

	// Init ImGui
	ImGui_ImplGlfw_Init(window, true);
	// Disable .ini
	ImGui::GetIO().IniFilename = nullptr;
	// Make the window's context current
	glfwMakeContextCurrent(window);

	//Opções de OpenGL
	glClearColor(1.,1.,1.,1.);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);

	//Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	camera.aspect = 480/640.;
	// models.push_back(std::make_unique<tnw::BRep>(4,3,2));
	models.push_back(std::make_unique<tnw::BRep>(5,5,3));
	MainMenu mainMenu(models,camera);

	using namespace tnw::wed;
	// auto mdl = (tnw::BRep*)models[0].get();
	// {mdl->edges.emplace_front(1);
	// WEdge *a = &mdl->edges.front();
	// mdl->edges.emplace_front(2);
	// WEdge *b = &mdl->edges.front();


	// mdl->vertices.emplace_front(1, glm::vec3{0,0,.25}, a);
	// Vertex *A = &mdl->vertices.front();
	// mdl->vertices.emplace_front(2, glm::vec3{0,1,.25}, a);
	// Vertex *B = &mdl->vertices.front();
	// mdl->vertices.emplace_front(3, glm::vec3{1,0,.25}, a);
	// Vertex *C = &mdl->vertices.front();

	// mdl->loops.emplace_front(1,a);
	// Loop *l1 = &mdl->loops.front();

	// a->vstart = A;
	// a->vend = B;
	// a->ccwloop = l1;
	// a->cwloop = l1;
	// a->ccwsucc =
	// a->cwpred = a;
	// a->ccwpred =
	// a->cwsucc = b;

	// b->vstart = A;
	// b->vend = C;
	// b->ccwloop = l1;
	// b->cwloop = l1;
	// b->ccwsucc =
	// b->cwpred = b;
	// b->ccwpred =
	// b->cwsucc = a;
	// }

	// quadrado
	auto mdl = (tnw::BRep*)models[0].get(); {
	mdl->edges.emplace_front(1);
	WEdge *a = &mdl->edges.front();
	mdl->edges.emplace_front(2);
	WEdge *b = &mdl->edges.front();
	mdl->edges.emplace_front(3);
	WEdge *c = &mdl->edges.front();
	mdl->edges.emplace_front(4);
	WEdge *d = &mdl->edges.front();


	mdl->vertices.emplace_front(1, glm::vec3{0,0,.5}, a);
	Vertex *A = &mdl->vertices.front();
	mdl->vertices.emplace_front(2, glm::vec3{0,1,.5}, b);
	Vertex *B = &mdl->vertices.front();
	mdl->vertices.emplace_front(3, glm::vec3{1,1,.5}, c);
	Vertex *C = &mdl->vertices.front();
	mdl->vertices.emplace_front(4, glm::vec3{1,0,.5}, d);
	Vertex *D = &mdl->vertices.front();

	mdl->loops.emplace_front(1,a);
	Loop *l1 = &mdl->loops.front();
	mdl->loops.emplace_front(2,a);
	Loop *l2 = &mdl->loops.front();

	a->vstart = A;
	a->vend = B;
	a->ccwloop = l2;
	a->cwloop = l1;
	a->ccwsucc =
	a->cwpred = b;
	a->ccwpred =
	a->cwsucc = d;

	b->vstart = B;
	b->vend = C;
	b->ccwloop = l2;
	b->cwloop = l1;
	b->cwpred =
	b->ccwsucc = c;
	b->cwsucc =
	b->ccwpred = a;

	c->vstart = C;
	c->vend = D;
	c->ccwloop = l2;
	c->cwloop = l1;
	c->cwpred =
	c->ccwsucc = d;
	c->cwsucc =
	c->ccwpred = b;

	d->vstart = A;
	d->vend = D;
	d->ccwloop = l1;
	d->cwloop = l2;
	d->cwsucc =
	d->ccwpred = a;
	d->cwpred =
	d->ccwsucc = c;}

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPushMatrix();

		glm::mat4 view;
		view = glm::scale(glm::mat4(),{camera.scale,camera.scale,camera.scale}) * view;
		view = glm::translate(glm::mat4(),camera.pos) * view;
		view = tnw::isometric(camera.aspect, camera.near, camera.far, camera.positive_hor, camera.positive_ver) * view;
		glLoadMatrixf(glm::value_ptr(view));

		tnw::draw_axis();

		for (auto&& model : models)
			model->draw();

		glPopMatrix();

		ImGui::SetNextWindowSize(ImVec2(350,350), ImGuiSetCond_FirstUseEver);

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
	camera.aspect = height/((float)width);
	glViewport(0,0,width,height);
}
