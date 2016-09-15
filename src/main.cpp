#include "model.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>

struct Camera
{
	glm::vec3 pos = glm::vec3(.0,.0,1.);
	glm::vec3 front = glm::vec3(.0, .0, -1.);
	glm::vec3 up = glm::vec3(.0,1.,.0);
};

float angle = 0.;
Camera camera;

void key_callback(auto window, auto key, auto scancode, auto action, auto mods) {
	float speed = 0.05f;

	if(key == GLFW_KEY_W)
	    camera.pos += speed * camera.front;
	if(key == GLFW_KEY_S)
	    camera.pos -= speed * camera.front;
	if(key == GLFW_KEY_A)
	    camera.pos -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
	if(key == GLFW_KEY_D)
	    camera.pos += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
	ImGui_ImplGlFw_KeyCallback(window,key,scancode,action,mods);
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
	
	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window)) {
		ImGui_ImplGlfw_NewFrame();

		glfwSetKeyCallback(window, key_callback);

		// Render here
		glClear(GL_COLOR_BUFFER_BIT);
		glShadeModel(GL_SMOOTH);


		tnw::octree::Tree *soct0 = new tnw::octree::Tree(),
						*soct1 = new tnw::octree::Tree(),
						*soct2 = new tnw::octree::Tree(),
						*soct4 = new tnw::octree::Tree(),
						*soct5 = new tnw::octree::Tree();

		std::array<tnw::octree::Tree*, 8> chld;
		chld.fill(nullptr);
		chld[0] = soct0;
		chld[1] = soct1;
		chld[2] = soct2;
		chld[4] = soct4;
		chld[5] = soct5;
		tnw::octree::Tree* oct = new tnw::octree::Tree(chld);

		//RENDERIZAÇÃO MESMO VAI AQUI

		glPushMatrix();

		glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

		glLoadMatrixf(glm::value_ptr(view));

		glRotatef(angle, 0., 1., 0.);

		oct->draw(tnw::octree::BoundingBox(glm::vec3(0.,0.,0.), 0.5)); 

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
