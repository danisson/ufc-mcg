#include "model.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <iostream>

struct Camera
{
	float pitch, yaw, roll;
	int lastxpos, lastypos;
};

Camera camera;

void key_callback(auto window, auto key, auto scancode, auto action, auto mods) {
	
}

tnw::octree::Color circle(const tnw::octree::BoundingBox &bb){
	unsigned int count = 0;
	for (int i = 0; i < 8; ++i)
	{
		if (glm::distance(glm::vec3(0,0,0), bb.getVertice(i)) < 0.4) {
			count++;
		}
	}
	if (count >= 8) { return tnw::octree::Color::black; }
	return tnw::octree::Color::gray;
}

tnw::octree::Color square(const tnw::octree::BoundingBox &bb) {
	unsigned int count = 0;
	float l = 0.5,d = 0.5,h = 0.5;
	glm::vec3 c(0,0,-1), p/*, x(1,0,0), y(0,1,0), z(0,0,1)*/;
	for (int i = 0; i < 8; ++i){
		unsigned int countcoords = 0;
		p = bb.getVertice(i);
		std::cout << "p: " << to_string(p) << "\n";
		if (p[0] >= c[0] - l/2.f && p[0] <= c[0] + l/2.f) {
			countcoords++;
		}
		if (p[1] >= c[1] - h/2.f && p[1] <= c[1] + h/2.f) {
			countcoords++;
		}
		if (p[2] >= c[2] - d/2.f && p[2] <= c[2] + d/2.f) {
			countcoords++;
		}
		std::cout << "count coords: " << countcoords << "\n";
		if (countcoords >= 3) {
			count++;
		}
	}
	std::cout << "count: " << count << "\n";
	if (count >= 8){
		return tnw::octree::Color::black;
	} else if (count > 0) {
		return tnw::octree::Color::gray;
	} else {
		return tnw::octree::Color::gray;
	}
}

tnw::octree::Color cilinder(const tnw::octree::BoundingBox &bb) {
	unsigned int count = 0;
	glm::vec3 p, c(0,0,-1), y(0,1,0);
	float h = 0.5, r = 0.5;
	for (int i = 0; i < 8; ++i)
	{
		p = bb.getVertice(i);
		std::cout << "p: " << to_string(p) << "c: " << to_string(c+(p[1]-c[1])*y) << "\n";
		if ((p[1] >= c[1]) && (p[1] <= c[1]+h) && (glm::distance(p, c+(p[1]-c[1])*y) <= r)){
			count++;
		}
	}
	std::cout << "===\n";
	if (count >= 8){
		return tnw::octree::Color::black;
	} else {
		return tnw::octree::Color::gray;
	}
}

tnw::octree::Color teste_paia(const tnw::octree::BoundingBox &bb){
	if (glm::distance(glm::vec2(bb.getVertice(0)), glm::vec2(0,0)) < 0.25) {
		return tnw::octree::Color::black;
	}
	return tnw::octree::Color::gray;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){

	
	int stateRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (stateRight == GLFW_PRESS) {
		float xoffset = xpos - camera.lastxpos,
			  sensibility = 0.05f;
		
		camera.lastxpos = xpos;
	      

		xoffset *= sensibility;

		camera.yaw += xoffset;
		std::cout << "camera yaw: " << camera.yaw << "\n";

	}

	int stateLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (stateLeft == GLFW_PRESS){
		float yoffset = camera.lastypos - ypos,
			  sensibility = 0.05f;

		camera.lastypos = ypos;
		yoffset *= sensibility;

		camera.pitch += yoffset;
		std::cout << "camera pitch: " << camera.pitch << "\n";
		

	}

}


// void mouse_callback(GLFWwindow* window, double xpos, double ypos){

	
// 	int stateRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
// 	if (stateRight == GLFW_PRESS) {
// 		glm::vec2 currvec = glm::vec2(xpos, ypos), lastvec = glm::vec2(camera.lastxpos, camera.lastypos);
// 		float distance = glm::distance(currvec, lastvec);

// 		distance *= 0.0005f;
// 		camera.yaw += distance;		
// 	}

// 	int stateLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
// 	if (stateLeft == GLFW_PRESS){
// 		glm::vec2 currvec = glm::vec2(xpos, ypos), lastvec = glm::vec2(camera.lastxpos, camera.lastypos);
// 		float distance = glm::distance(currvec, lastvec);

// 		distance *= 0.0005f;
// 		camera.pitch += distance;

// 	}

// }

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
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	//Opções de OpenGL
	glClearColor(1.,1.,1.,1.);
	glEnable(GL_LINE_SMOOTH);

	camera.lastxpos = width / 2; camera.lastypos = height/2;
	tnw::octree::Tree* oct = new tnw::octree::Tree(/*chld*/);
	tnw::octree::Classifier f = cilinder;
	tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(-0.5,-0.5,-0.5), 1);
	oct->classify(f, bb, 4, 0);
	
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		glfwSetKeyCallback(window, key_callback);
		//glfwSetMouseButtonCallback(window, mouse_button_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		// Render here
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);


		// tnw::octree::Tree *soct0 = new tnw::octree::Tree(),
		// 				*soct1 = new tnw::octree::Tree(),
		// 				*soct2 = new tnw::octree::Tree(),
		// 				*soct4 = new tnw::octree::Tree(),
		// 				*soct5 = new tnw::octree::Tree();

		// std::array<tnw::octree::Tree*, 8> chld;
		// chld.fill(nullptr);
		// chld[0] = soct0;
		// chld[1] = soct1;
		// chld[2] = soct2;
		// chld[4] = soct4;
		// chld[5] = soct5;
		//tnw::octree::Tree* oct = new tnw::octree::Tree(/*chld*/);

		//RENDERIZAÇÃO MESMO VAI AQUI

		glPushMatrix();

		glm::mat4 view = glm::yawPitchRoll(camera.yaw, camera.pitch, camera.roll);

		glLoadMatrixf(glm::value_ptr(view));

		oct->draw(bb);
		// tnw::octree::Classifier f = circle;
		// oct->classify(f, tnw::octree::BoundingBox(glm::vec3(-0.5,-0.5,0.), 1), 2, 0); 

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glPopMatrix();

		// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		
		// if (ImGui::Button("Red")){
		// 	glClearColor(1.,0.,0.,1.);
		// }
		// if (ImGui::Button("Green")){
		// 	glClearColor(0.,1.,0.,1.);
		// }
		// if (ImGui::Button("Blue")){
		// 	glClearColor(0.,0.,1.,1.);
		// }

		// ImGui::Text("Hello, world!");
		// ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		// ImGui::Text("Mouse position: %.3f x %.3f y", ImGui::GetMousePos().x, ImGui::GetMousePos().y);

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
