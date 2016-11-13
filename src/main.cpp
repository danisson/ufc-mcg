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
using namespace tnw::wed;
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
	MainMenu mainMenu(models,camera);

	WEdge *a = new WEdge(1);
	WEdge *b = new WEdge(2);
	WEdge *c = new WEdge(3);
	WEdge *d = new WEdge(4);
	WEdge *e = new WEdge(5);
	WEdge *f = new WEdge(6);


	Vertex *A = new Vertex(1, {0,0,0}, a);
	Vertex *B = new Vertex(2, {1,0,1}, b);
	Vertex *C = new Vertex(3, {2,0,0}, e);
	Vertex *D = new Vertex(4, {1,1,0}, e);

	Loop *l1 = new Loop(1, a);
	Loop *l2 = new Loop(2, c);
	Loop *l3 = new Loop(3, a);
	Loop *l4 = new Loop(4, b);

	a->vstart = A;
	a->vend = D;
	a->lloop = l3;
	a->rloop = l1;
	a->lpred = e;
	a->lsucc = f;
	a->rpred = b;
	a->rsucc = c;

	b->vstart = A;
	b->vend = B;
	b->lloop = l1;
	b->rloop = l4;
	b->lpred = c;
	b->lsucc = a;
	b->rpred = f;
	b->rsucc = d;

	c->vstart = B;
	c->vend = D;
	c->lloop = l1;
	c->rloop = l2;
	c->lpred = a;
	c->lsucc = b;
	c->rpred = d;
	c->rsucc = e;

	d->vstart = B;
	d->vend = C;
	d->lloop = l2;
	d->rloop = l4;
	d->lpred = e;
	d->lsucc = c;
	d->rpred = b;
	d->rsucc = f;

	e->vstart = C;
	e->vend = D;
	e->lloop = l2;
	e->rloop = l3;
	e->lpred = c;
	e->lsucc = d;
	e->rpred = f;
	e->rsucc = a;

	f->vstart = A;
	f->vend = C;
	f->lloop = l4;
	f->rloop = l3;
	f->lpred = d;
	f->lsucc = b;
	f->rpred = a;
	f->rsucc = c;

	std::vector<WEdge*> ae = l1->adjedge();

	std::cout << "Face 1 adj edges\n";
	for (WEdge*& we : ae) {
		std::cout << we->id << " ";
	}
	std::cout << "\n---\n";

	std::vector<Vertex*> av = l1->adjvertex();

	std::cout << "Face 1 adj vertex\n";
	for (Vertex*& wv : av) {
		std::cout << wv->id << " ";
	}
	std::cout << "\n---\n";

	std::vector<Loop*> al = l1->adjloop();

	std::cout << "Face 1 adj loop\n";
	for (Loop*& wl : al) {
		std::cout << wl->id << " ";
	}
	std::cout << "\n---\n";

	std::vector<WEdge*> be = a->adjedge();
	std::cout << "Edge a adj edge\n";
	for (WEdge*& we : be) {
		std::cout << we->id << " ";
	}
	std::cout << "\n---\n";

	std::vector<WEdge*> ce = A->adjedge();
	std::cout << "Vertex A adj edge\n";
	for (WEdge*& we : ce) {
		std::cout << we->id << " ";
	}
	std::cout << "\n---\n";

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
