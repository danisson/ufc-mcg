#include "model.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>


struct IsometricCamera
{
	float scale = 1.0, near = 1.0, far = -1.0;
	bool positive_hor = true, positive_ver = true;
};

IsometricCamera camera;

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
	float rot_y = 45.0; 
	float rot_x = glm::degrees(std::asin(std::tan(glm::radians(45.0f))));
	
	if (!positive_ver){
		rot_y = -rot_y;
	}
	if (!positive_hor){
		rot_x = -rot_x;
	}
	glm::mat4 a = glm::rotate(glm::mat4(), rot_y, glm::vec3(0.0,1.0,0.0));
	glm::mat4 b = glm::rotate(glm::mat4(), rot_x, glm::vec3(1.0,0.0,0.0));
	// glm::mat4 b;
	// b[0][0] = sqrt(3);
	// b[0][2] = -sqrt(3);
	// b[1][0] = 1;
	// b[1][1] = 2;
	// b[1][2] = 1;
	// b[2][0] = sqrt(2);
	// b[2][1] = -sqrt(2);
	// b[2][2] = sqrt(2);

	// b *= 1.0/sqrt(6);
	glm::mat4 o = glm::ortho(-scale, scale, -scale, scale, near, far);
	//std::cout<<glm::to_string((sqrt(6))*b*a)<<std::endl;
	//exit(0);
	return o*b*a;
}

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
	glClearColor(1.,1.,1.,1.);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// camera.lastxpos = width / 2; camera.lastypos = height/2;


	tnw::octree::Tree *soct0 = new tnw::octree::Tree(),
	*soct1 = new tnw::octree::Tree(),
	*soct2 = new tnw::octree::Tree(),
	*soct4 = new tnw::octree::Tree(),
	*soct5 = new tnw::octree::Tree();

	std::array<tnw::octree::Tree*, 8> chld;
	chld.fill(nullptr);
	chld[0] = soct0;
	//chld[1] = soct1;
	//chld[2] = soct2;
	//chld[4] = soct4;
	//chld[5] = soct5;
	tnw::octree::Tree* oct = new tnw::octree::Tree(chld);

	tnw::octree::BoundingBox bb = tnw::octree::BoundingBox(glm::vec3(0.,0.,0.), 0.5);

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		glfwSetKeyCallback(window, key_callback);
		//glfwSetMouseButtonCallback(window, mouse_button_callback);
		//glfwSetCursorPosCallback(window, mouse_callback);
		// Render here
		glClear(GL_COLOR_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);


		//RENDERIZAÇÃO MESMO VAI AQUI

		glPushMatrix();

		// glm::mat4 view = glm::yawPitchRoll(camera.yaw, camera.pitch, camera.roll);

		glm::mat4 view = isometric(camera.scale, camera.near, camera.far, camera.positive_hor, camera.positive_ver);

		glLoadMatrixf(glm::value_ptr(view));

		desenharEixos();
		oct->draw(bb); 

		glPopMatrix();

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
