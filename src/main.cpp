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

Scene::Scene() { }
void Scene::addModel(tnw::Model* model) { models.push_back(model); }
std::vector<tnw::Model*> Scene::getModels() {return models;}
void Scene::setSelected(unsigned int s) {selected = s;}
unsigned int Scene::getSelected() {return selected;}
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

	//Camera initalization
	IsometricCamera camera;

	tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(0,0,0), 1);
	// auto oct = std::make_unique<tnw::Octree>(bb);
	std::vector<std::unique_ptr<tnw::Octree>> models;
	// models.push_back(std::move(oct));
	std::vector<std::string> model_names;
	// model_names.push_back("Tree 0");

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
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		glColor3f(0,1,0);
		bb.draw();
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		// oct.draw();
		for (auto& model : models) {
			model->draw();
		}

		glPopMatrix();

		ImGui::SetNextWindowSize(ImVec2(350,560), ImGuiSetCond_FirstUseEver);

		//Interface construction
		ImGui::Begin("Menu");

		if (ImGui::CollapsingHeader("Arquivo")) {
			if (ImGui::Button("Abrir")) {

			}
			ImGui::SameLine();
			if (ImGui::Button("Salvar")) {

			}
		}

		if (ImGui::CollapsingHeader("Objetos")) {
			const char* tree_names[model_names.size()];

			for (unsigned int i = 0; i < model_names.size(); i++){
				
				tree_names[i] = model_names[i].c_str();
			}

			static int curr_item = 0;

			ImGui::PushItemWidth(-1);
			ImGui::ListBox("##cena", &curr_item, tree_names, models.size(), models.size());
			ImGui::PopItemWidth();

			if (ImGui::Button("Translação")) {
				ImGui::OpenPopup("Parâmetros da Translação");
			}
			if (ImGui::BeginPopupModal("Parâmetros da Translação", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				float x,y,z;
				ImGui::InputFloat("x", &x);
				ImGui::InputFloat("y", &y); 
				ImGui::InputFloat("z", &z);

				if (ImGui::Button("OK", ImVec2(120,0))) {
					models[curr_item]->translate(glm::vec3(x,y,z));
					ImGui::CloseCurrentPopup(); 
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("União")) {
				ImGui::OpenPopup("União");
			}

			if (ImGui::BeginPopupModal("União", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				
					static int selected_and = -1;
					ImGui::Combo("selecione a árvore com que operar", &selected_and, tree_names, model_names.size());

				if (ImGui::Button("OK", ImVec2(120,0))) {
					models[curr_item]->bool_and(*models[selected_and]);
					ImGui::CloseCurrentPopup(); 
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Interseção")) {
				ImGui::OpenPopup("Interseção");
			}

			if (ImGui::BeginPopupModal("Interseção", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				
					static int selected_or = -1;
					ImGui::Combo("selecione a árvore com que operar", &selected_or, tree_names, model_names.size());

				if (ImGui::Button("OK", ImVec2(120,0))) {
					models[curr_item]->bool_and(*models[selected_or]);
					ImGui::CloseCurrentPopup(); 
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}			

			if (ImGui::CollapsingHeader("Nova Primitiva")) {
				if (ImGui::Button("Esfera")) {
					ImGui::OpenPopup("Parâmetros da Esfera");
				} 
				if (ImGui::BeginPopupModal("Parâmetros da Esfera", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					float x, y, z, r;
				    ImGui::Text("centro:");
					ImGui::InputFloat("x", &x); 
					ImGui::InputFloat("y", &y); 
					ImGui::InputFloat("z", &z);
					ImGui::Separator();
					ImGui::InputFloat("raio", &r);

				    if (ImGui::Button("OK", ImVec2(120,0))) {
				    	tnw::octree::Sphere s(glm::vec3(x,y,z), r);
				    	auto newoct = std::make_unique<tnw::Octree>(s, bb, 4);
				    	models.push_back(std::move(newoct));
				    	std::stringstream ss;
				    	// ss << "Tree " << model_names.size();
				    	model_names.push_back(ss.str().c_str());
				    	ImGui::CloseCurrentPopup(); 
				    }
				    ImGui::SameLine();
				    if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				    ImGui::EndPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Caixa")) {
					ImGui::OpenPopup("Parâmetros da Caixa");
				}
				if (ImGui::BeginPopupModal("Parâmetros da Caixa", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					float x,y,z,l,h,d;
				    ImGui::Text("centro:");
					ImGui::InputFloat("x", &x); 
					ImGui::InputFloat("y", &y); 
					ImGui::InputFloat("z", &z);

					ImGui::Separator();
					ImGui::InputFloat("largura", &l);
					ImGui::InputFloat("altura", &h);
					ImGui::InputFloat("profundidade", &d);

				    if (ImGui::Button("OK", ImVec2(120,0))) {
				    	tnw::octree::Box bx(glm::vec3(x,y,z), l, h, d);
				    	auto newoct = std::make_unique<tnw::Octree>(bx, bb, 4);
				    	models.push_back(std::move(newoct));
				    	std::stringstream ss;
				    	ss << "Tree " << model_names.size();
				    	model_names.push_back(ss.str().c_str());
				    	ImGui::CloseCurrentPopup(); 
				    }
				    ImGui::SameLine();
				    if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
				    ImGui::EndPopup();
				} 

				ImGui::SameLine();
				if (ImGui::Button("Cilindro")) {
					ImGui::OpenPopup("Parâmetros do Cilindro");
				}
				if (ImGui::BeginPopupModal("Parâmetros do Cilindro", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					float x,y,z,r,h;
					ImGui::Text("centro do lado inferior:");
					ImGui::InputFloat("x", &x);
					ImGui::InputFloat("y", &y);
					ImGui::InputFloat("z", &z);

					ImGui::Separator();
					ImGui::InputFloat("raio", &r);
					ImGui::InputFloat("altura", &h);

					if (ImGui::Button("OK", ImVec2(120,0))) {
						tnw::octree::Cilinder cl(glm::vec3(x,y,z), h, r);
						auto newoct = std::make_unique<tnw::Octree>(cl, bb, 4);
						models.push_back(std::move(newoct));
						std::stringstream ss;
				    	ss << "Tree " << model_names.size();
				    	model_names.push_back(ss.str().c_str());
						ImGui::CloseCurrentPopup(); 
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
					ImGui::EndPopup();
				}

				ImGui::SameLine();
				if (ImGui::Button("Pirâmide Base Quadrada")) {
						ImGui::OpenPopup("Parâmetros da Pirâmide");
				}
				if (ImGui::BeginPopupModal("Parâmetros da Pirâmide", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					float x,y,z,h,l;
					ImGui::Text("centro do lado inferior:");
					ImGui::InputFloat("x", &x);
					ImGui::InputFloat("y", &y);
					ImGui::InputFloat("z", &z);

					ImGui::Separator();
					ImGui::InputFloat("altura", &h);
					ImGui::InputFloat("lado", &l);					

					if (ImGui::Button("OK", ImVec2(120,0))) {
						tnw::octree::SquarePyramid sp({x,y,z}, h, l);
						auto newoct = std::make_unique<tnw::Octree>(sp, bb, 4);
						models.push_back(std::move(newoct));
						std::stringstream ss;
				    	ss << "Tree " << model_names.size();
				    	model_names.push_back(ss.str().c_str());
						ImGui::CloseCurrentPopup(); 
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2(120,0))) { ImGui::CloseCurrentPopup(); }
					ImGui::EndPopup();
				}
			}
		}

		if (ImGui::CollapsingHeader("Câmera")) {
			if (ImGui::Button("Zoom Out")) {
				camera.scale += 1;
			}
			ImGui::SameLine();
			if (ImGui::Button("Zoom In")) {
				camera.scale -= 1;
			}
			if (ImGui::Button("Flip X")) {
				camera.positive_hor = !camera.positive_hor;
			} 
			ImGui::SameLine();
			if (ImGui::Button("Flip Y")) {
				camera.positive_ver = !camera.positive_ver;
			}
		}

		ImGui::End();

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    	glfwSetWindowShouldClose(window, GLFW_TRUE);    	
    } else {
    	ImGui_ImplGlFw_KeyCallback(window, key, scancode, action, mods);
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
