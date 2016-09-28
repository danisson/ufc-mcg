#include "model.h"
#include "helper.h"

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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#undef near
#undef far
#endif

// int main(int argc, char const *argv[])
// {
// 		auto oct = std::make_unique<tnw::octree::Tree>();
// 		tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(-1,-1,1), 2);
// 		tnw::octree::Sphere s(glm::vec3(0,0,0), 0.5);
// 		tnw::octree::SquarePyramid sp(glm::vec3(0,-1,0), 2, 0.5);

// 		oct->classify(sp, bb, 3, 0);
// 	return 0;
// }

//Classe da interface que representa uma cena com vários modelos
class Scene
{
public:
	Scene();
	void addModel(tnw::Model*);
	std::vector<tnw::Model*> getModels();
	void setSelected(unsigned int);
	unsigned int getSelected();
	void draw();
private:
	//Array de modelos
	std::vector<tnw::Model*> models;
	//Modelo selecionado atualmente
	unsigned int selected;
};

Scene::Scene() { models(); }
void Scene::addModel(tnw::Model* model) { models.push_back(model); }
std::vector<tnw::Model*> Scene::getModels() {return models;}
void Scene::setSelected(unsigned int s) {s = selected;}
unsigned int Scene::getSelected() {return s};
void Scene::draw() {
	for (auto& model : models) {
		model->draw();
	}
}

struct IsometricCamera
{
	float scale = 2.0, near = -10.0, far = 10.0;
	bool positive_hor = true, positive_ver = true;
};

void key_callback(GLFWwindow*, int, int, int, int);
void desenharEixos();
glm::mat4 isometric(float, float, float, bool, bool);

int main(void) {
	GLFWwindow* window;

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
		//glfwSetMouseButtonCallback(window, mouse_button_callback);
		//glfwSetCursorPosCallback(window, mouse_callback);


	//Camera initalization
	IsometricCamera camera;

	// tnw::octree::BoundingBox bb2(glm::vec3(0,0,0), 4);
	// tnw::octree::BoundingBox bb3(glm::vec3(2,-1,1), 2);

	// auto oct = std::make_unique<tnw::octree::Tree>();
	tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(0,0,0), 1);
	tnw::octree::Sphere s(glm::vec3(0,0,0), 0.5);
	tnw::octree::SquarePyramid sp(glm::vec3(0,-1,0), 2, 0.5);
	tnw::octree::Box bx(glm::vec3(0.5,0.25,-0.25), 1, 0.5, 0.5);
	auto oct = classify(bx, bb, 4, 0);
	auto oct2 = classify(sp, bb, 4, 0);

	std::cout << "Interseção bb1, esfera: " << (tnw::sphere_box_intersection(glm::vec3(0), 3, bb.getCenter(), bb.depth, bb.depth, bb.depth)) ? "true" : "false" << std::endl;

	// std::cout << "Interseção bb1, bb2: " << (tnw::box_intersection(bb.getCenter(), bb.depth, bb.depth, bb.depth, bb2.getCenter(), bb2.depth, bb2.depth, bb2.depth) ? std::string("true") : std::string("false")) << std::endl;
	// std::cout << "Interseção bb1, bb3: " << (tnw::box_intersection(bb.getCenter(), bb.depth, bb.depth, bb.depth, bb3.getCenter(), bb3.depth, bb3.depth, bb3.depth) ? std::string("true") : std::string("false")) << std::endl;
	// std::cout << "Interseção bb2, bb3: " << (tnw::box_intersection(bb2.getCenter(), bb2.depth, bb2.depth, bb2.depth, bb3.getCenter(), bb3.depth, bb3.depth, bb3.depth) ? std::string("true") : std::string("false")) << std::endl;


	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);

		glPushMatrix();

		glm::mat4 view = isometric(camera.scale, camera.near, camera.far, camera.positive_hor, camera.positive_ver);
		glLoadMatrixf(glm::value_ptr(view));

		desenharEixos();

		oct->draw(bb);
		// glColor3f(1,0,0);
		// bb.draw();
		// glColor3f(0,1,0);
		// bb2.draw();
		// glColor3f(0,0,1);
		// bb3.draw();


		glPopMatrix();

		//Interface construction
		ImGui::Begin("Menu");

		if (ImGui::Button("+ Scale")) {
			camera.scale += 1;
		}
		if (ImGui::Button("- Scale")) {
			camera.scale -= 1;
		}
		if (ImGui::Button("Flip X")) {
			camera.positive_hor = !camera.positive_hor;
		}
		if (ImGui::Button("Flip Y")) {
			camera.positive_ver = !camera.positive_ver;
		}
		ImGui::End();

		//ImGui::ShowTestWindow();

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    	glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

}

void desenharEixos(){
	glBegin(GL_LINES);
		glColor3f(1.000000f, 0.000000f, 0.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(1, 0, 0);
		glColor3f(0.000000f, 1.000000f, 0.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 1, 0);
		glColor3f(0.000000f, 0.000000f, 1.000000f);
		glVertex3d(0, 0, 0);
		glVertex3d(0, 0, 1);
		glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

glm::mat4 isometric(float scale, float near, float far, bool positive_hor, bool positive_ver)
{
	float rot_y = glm::radians(45.0);
	float rot_x = std::asin(std::tan(glm::radians(30.0f)));

	if (!positive_hor){
		// rot_x = -rot_x;
		rot_y = glm::radians(3*45.0);
	}
	if (!positive_ver){
		rot_y = -rot_y;
	}

	glm::mat4 a = glm::rotate(glm::mat4(), rot_y, glm::vec3(0.0,1.0,0.0));
	glm::mat4 b = glm::rotate(glm::mat4(), rot_x, glm::vec3(1.0,0.0,0.0));

	glm::mat4 o = glm::ortho(-scale, scale, -scale, scale, near, far);

	return o*b*a;
}
