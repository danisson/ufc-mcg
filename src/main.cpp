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
	MainMenu mainMenu(models,camera);

	models.push_back(std::make_unique<tnw::BRep>());
	auto mdl = (tnw::BRep*)models[0].get();

	using namespace tnw::wed;
	// mdl->edges.reserve(6);
	mdl->edges.emplace_front(1);
	WEdge *a = &mdl->edges.front();
	mdl->edges.emplace_front(2);
	WEdge *b = &mdl->edges.front();
	mdl->edges.emplace_front(3);
	WEdge *c = &mdl->edges.front();
	mdl->edges.emplace_front(4);
	WEdge *d = &mdl->edges.front();
	mdl->edges.emplace_front(5);
	WEdge *e = &mdl->edges.front();
	mdl->edges.emplace_front(6);
	WEdge *f = &mdl->edges.front();


	// mdl->vertices.reserve(4);
	mdl->vertices.emplace_front(1, glm::vec3{0,0,0}, a);
	Vertex *A = &mdl->vertices.front();
	mdl->vertices.emplace_front(2, glm::vec3{1,0,1}, b);
	Vertex *B = &mdl->vertices.front();
	mdl->vertices.emplace_front(3, glm::vec3{2,0,0}, e);
	Vertex *C = &mdl->vertices.front();
	mdl->vertices.emplace_front(4, glm::vec3{1,1,0}, e);
	Vertex *D = &mdl->vertices.front();

	mdl->loops.emplace_front(1,a);
	Loop *l1 = &mdl->loops.front();
	mdl->loops.emplace_front(2,c);
	Loop *l2 = &mdl->loops.front();
	mdl->loops.emplace_front(3,a);
	Loop *l3 = &mdl->loops.front();
	mdl->loops.emplace_front(4,b);
	Loop *l4 = &mdl->loops.front();

	a->vstart = A;
	a->vend = D;
	a->cwloop = l3;
	a->ccwloop = l1;
	a->cwpred = e;
	a->cwsucc = f;
	a->ccwpred = b;
	a->ccwsucc = c;

	b->vstart = A;
	b->vend = B;
	b->cwloop = l1;
	b->ccwloop = l4;
	b->cwpred = c;
	b->cwsucc = a;
	b->ccwpred = f;
	b->ccwsucc = d;

	c->vstart = B;
	c->vend = D;
	c->cwloop = l1;
	c->ccwloop = l2;
	c->cwpred = a;
	c->cwsucc = b;
	c->ccwpred = d;
	c->ccwsucc = e;

	d->vstart = B;
	d->vend = C;
	d->cwloop = l2;
	d->ccwloop = l4;
	d->cwpred = e;
	d->cwsucc = c;
	d->ccwpred = b;
	d->ccwsucc = f;

	e->vstart = C;
	e->vend = D;
	e->cwloop = l2;
	e->ccwloop = l3;
	e->cwpred = c;
	e->cwsucc = d;
	e->ccwpred = f;
	e->ccwsucc = a;

	f->vstart = A;
	f->vend = C;
	f->cwloop = l4;
	f->ccwloop = l3;
	f->cwpred = d;
	f->cwsucc = b;
	f->ccwpred = a;
	f->ccwsucc = c;

	// std::vector<WEdge*> ae = l1->adjedge();

	// std::cout << "Face 1 adj edges\n";
	// for (WEdge*& we : ae) {
		// std::cout << we->id << " ";
		// mdl->marked.emplace(we->id,1);
	// }
	// std::cout << "\n---\n";

	// std::vector<Vertex*> av = l1->adjvertex();

	// std::cout << "Face 1 adj vertex\n";
	// for (Vertex*& wv : av) {
	// 	std::cout << wv->id << " ";
	// }
	// std::cout << "\n---\n";

	// std::vector<Loop*> al = l1->adjloop();

	// std::cout << "Face 1 adj loop\n";
	// for (Loop*& wl : al) {
	// 	std::cout << wl->id << " ";
	// }
	// std::cout << "\n---\n";

	// std::vector<WEdge*> be = a->adjedge();
	// std::cout << "Edge a adj edge\n";
	// mdl->selected_edge = a->id;
	// for (WEdge*& we : be) {
	// 	std::cout << we->id << " ";
		// mdl->marked.emplace(we->id,1);
	// }
	// std::cout << "\n---\n";

	// std::vector<WEdge*> ce = B->adjedge();
	// std::cout << "Vertex A adj edge\n";
	// mdl->marked.emplace(B->id,0);
	// for (WEdge*& we : ce) {
	// 	std::cout << we->id << " ";
	// 	mdl->marked.emplace(we->id,1);
	// }
	// std::cout << "\n---\n";

	// std::vector<Vertex*> cv = A->adjvertex();
	// std::cout << "Vertex A adj vertex\n";
	// for (Vertex*& we : cv) {
	// 	std::cout << we->id << " ";
	// }
	// std::cout << "\n---\n";

	// std::vector<Loop*> cl = A->adjloop();
	// std::cout << "Vertex A adj loop\n";
	// for (Loop*& we : cl) {
	// 	std::cout << we->id << " ";
	// }
	// std::cout << "\n---\n";

	//EULER OPERATORS TESTS
	// std::cout << "Euler operators\n";

	models.push_back(std::make_unique<tnw::BRep>());
	auto brep = (tnw::BRep*)models[0].get();
	
	std::cout << "MVFS:\n";
	brep->mvfs({0,0,0});
	std::cout << "MVFS ok\n";
	std::cout << "SMEV:\n";
	brep->smev(1,1,{0,0.5,0});
	brep->smev(1,2,{0.5,0,0});
	std::cout << "SMEV ok\n";


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
